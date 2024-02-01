#include "main.h"

struct rte_ring *input_rings[2];
void (*forward)(void) = NULL;
struct app_mbuf_array *worker_mbuf;
// struct rte_mbuf *mbuf;

// SP
int SP_priority[2];

// WFQ
int WFQ_weight[2];
struct app_mbuf_array *peek_mbuf[2];
int peek_valid[2];
struct flow_key key;
struct app_fwd_table_item value;

void app_main_loop_forwarding(void)
{
    RTE_LOG(INFO, SWITCH, "Core %u is doing forwarding\n",
            rte_lcore_id());
    app.cpu_freq[rte_lcore_id()] = rte_get_tsc_hz();
    app.fwd_item_valid_time = app.cpu_freq[rte_lcore_id()] / 1000 * VALID_TIME;
    if (forward == NULL)
    {
        if (!strcmp(app.inter_node, "SP"))
        {
            forward = forward_SP;
            for (int i = 0; i < 2; ++i)
                SP_priority[i] = app.SP_priority[i];
        }
        else if (!strcmp(app.inter_node, "WFQ"))
        {
            forward = forward_WFQ;
            for (int i = 0; i < 2; ++i)
            {
                peek_mbuf[i] = rte_malloc_socket(NULL, sizeof(struct app_mbuf_array),
                                                 RTE_CACHE_LINE_SIZE, rte_socket_id());
                if (peek_mbuf[i] == NULL)
                    rte_panic("Worker thread: cannot allocate buffer space\n");
                peek_valid[i] = 0;
                WFQ_weight[i] = app.WFQ_weight[i];
            }
        }
        for (int i = 0; i < 2; ++i)
        {
            input_rings[i] = app.rings_nodes[i];
        }
    }

    worker_mbuf = rte_malloc_socket(NULL, sizeof(struct app_mbuf_array),
                                    RTE_CACHE_LINE_SIZE, rte_socket_id());
    if (worker_mbuf == NULL)
        rte_panic("Worker thread: cannot allocate buffer space\n");

    while (!force_quit)
    {
        if (rte_ring_count(app.rings_tx[app.default_port]) < 20 && get_qlen_bytes(app.default_port) < 10000)
            forward();
    }
}

void forward_SP(void)
{
    uint64_t ts_start = rte_get_tsc_cycles();
    int i;
    for (int priority = 1; priority <= 2; ++priority)
    {
        for (i = 0; i < 2; ++i)
        {
            if (SP_priority[i] == priority)
                break;
        }
        struct rte_ring *ring = input_rings[i];

        int ret = rte_ring_sc_dequeue(
            ring,
            (void **)worker_mbuf->array);
        if (ret == -ENOENT)
            continue;
        packet_enqueue(app.default_port, worker_mbuf->array[0]);
        uint64_t ts_end = rte_get_tsc_cycles();
        app.cyc += (ts_end - ts_start);
        app.forwarding_cyc += (ts_end - ts_start);
        break;
    }
}
// void forward_WFQ(void)
// {
//     uint64_t estimate_departure[2];
//     for (int i = 0; i < 2; ++i)
//     {
//         if (!peek_valid[i])
//         {
//             int ret = rte_ring_sc_dequeue(
//                 input_rings[i],
//                 (void **)&peek_mbuf[i]);
//             if (ret == -ENOENT)
//                 continue;
//             peek_valid[i] = 1;
//         }
//         uint64_t arrival_timestamp=peek_mbuf[i]->timestamp;

//         double bandwidth = WFQ_weight[i] * 1.0 / (WFQ_weight[0] + WFQ_weight[1]) * app.tx_rate_mbps;
//         uint64_t estimate_tx=(uint64_t)(peek_mbuf[i]->mbuf->pkt_len / bandwidth * 8 / 1000000 * app.cpu_freq[rte_lcore_id()]);
//         estimate_departure[i] = arrival_timestamp + estimate_tx;
//         // printf("%lu + %lu = %lu, weight=%d, pkt len=%d\n",arrival_timestamp, estimate_tx, estimate_departure[i],WFQ_weight[i],peek_mbuf[i]->mbuf->pkt_len);

//     }
//     if(peek_valid[0]&&(!peek_valid[1]||estimate_departure[0] <= estimate_departure[1]))
//     {
//         packet_enqueue(app.default_port, peek_mbuf[0]->mbuf);
//         peek_valid[0] = 0;
//     }
//     else if(peek_valid[1]&&(!peek_valid[0]||estimate_departure[1] <= estimate_departure[0]))
//     {
//         packet_enqueue(app.default_port, peek_mbuf[1]->mbuf);
//         peek_valid[1] = 0;
//     }
// }

void forward_WFQ(void)
{
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < WFQ_weight[i]; ++j)
        {
            uint64_t ts_start = rte_get_tsc_cycles();
            int ret = rte_ring_sc_dequeue(
                input_rings[i],
                (void **)worker_mbuf->array);
            if (ret == -ENOENT)
                break;
            packet_enqueue(app.default_port, worker_mbuf->array[0]);
            uint64_t ts_end = rte_get_tsc_cycles();
            app.cyc += (ts_end - ts_start);
            app.forwarding_cyc += (ts_end - ts_start);
        }
    }
}