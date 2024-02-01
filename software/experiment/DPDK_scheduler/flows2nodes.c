#include "main.h"

void app_main_loop_flows2nodes(int nodeid)
{
    app.cpu_freq[rte_lcore_id()] = rte_get_tsc_hz();
    struct flows2nodes_context context;
    RTE_LOG(INFO, SWITCH, "Core %u is doing flows2nodes %d\n",
            rte_lcore_id(), nodeid);
    context.nodeid = nodeid;
    if (!strcmp(app.intra_node, "SP"))
    {
        context.flows2nodes = flows2nodes_SP;
        for (int i = 0; i < 3; ++i)
            context.SP_priority[i] = app.SP_priority[2 + nodeid * 3 + i];
    }
    else if (!strcmp(app.intra_node, "WFQ"))
    {
        context.flows2nodes = flows2nodes_WFQ;
        for (int i = 0; i < 3; ++i)
        {
            context.peek_mbuf[i] = rte_malloc_socket(NULL, sizeof(struct app_mbuf_array),
                                                     RTE_CACHE_LINE_SIZE, rte_socket_id());
            if (context.peek_mbuf[i] == NULL)
                rte_panic("Worker thread: cannot allocate buffer space\n");
            context.peek_valid[i] = 0;
            context.WFQ_weight[i] = app.WFQ_weight[i + 2 + nodeid * 3];
        }
    }
    else if (!strcmp(app.intra_node, "pFabric"))
    {
        context.flows2nodes = flows2nodes_pFabric;
        for (int i = 0; i < 3; ++i)
        {
            context.pFabric_size[i] = app.pFabric_size[i + nodeid * 3];
        }
    }
    else
    {
        context.flows2nodes = 0;
    }

    for (int i = 0; i < 3; ++i)
    {
        context.input_rings[i] = app.rings_flows[i + nodeid * 3];
    }
    context.output_ring = app.rings_nodes[nodeid];

    context.worker_mbuf = rte_malloc_socket(NULL, sizeof(struct app_mbuf_array),
                                            RTE_CACHE_LINE_SIZE, rte_socket_id());
    if (context.worker_mbuf == NULL)
        rte_panic("Worker thread: cannot allocate buffer space\n");

    while (!force_quit)
    {
        if (rte_ring_count(app.rings_nodes[nodeid]) < 20)
            context.flows2nodes(&context);
    }
}

void flows2nodes_SP(struct flows2nodes_context *context)
{
    uint64_t ts_start = rte_get_tsc_cycles();
    int i;
    for (int priority = 1; priority <= 3; ++priority)
    {
        for (i = 0; i < 3; ++i)
        {
            if (context->SP_priority[i] == priority)
                break;
        }
        struct rte_ring *ring = context->input_rings[i];

        int ret = rte_ring_sc_dequeue(
            ring,
            (void **)context->worker_mbuf->array);
        if (ret == -ENOENT)
            continue;
        rte_ring_sp_enqueue(context->output_ring, context->worker_mbuf->array[0]);
        RTE_LOG(DEBUG, SWITCH, "%s: enqueue packet to %s\n", __func__, context->output_ring->name);
        uint64_t ts_end = rte_get_tsc_cycles();
        app.cyc += (ts_end - ts_start);
        app.flows2nodes_cyc[context->nodeid] += (ts_end - ts_start);
        break;
    }
}
// void flows2nodes_WFQ(struct flows2nodes_context *context)
// {
//     uint64_t estimate_departure[3];
//     for (int i = 0; i < 3; ++i)
//     {
//         if (!context->peek_valid[i])
//         {
//             int ret = rte_ring_sc_dequeue(
//                 context->input_rings[i],
//                 (void **)context->peek_mbuf[i]->array);
//             if (ret == -ENOENT)
//                 continue;
//             context->peek_valid[i] = 1;
//         }
//         // uint64_t arrival_timestamp=context->peek_mbuf[i]->timestamp;
//         // double bandwidth = context->WFQ_weight[i] * 1.0 / (context->WFQ_weight[0] + context->WFQ_weight[1]) * app.tx_rate_mbps;
//         // uint64_t estimate_tx=(uint64_t)(context->peek_mbuf[i]->mbuf->pkt_len / bandwidth * 8 / 1000000 * app.cpu_freq[rte_lcore_id()]);
//         // estimate_departure[i] = arrival_timestamp + estimate_tx;
//         // printf("%s:%lu + %lu = %lu, weight=%d, pkt len=%d\n",__func__,arrival_timestamp, estimate_tx, estimate_departure[i],context->WFQ_weight[i],context->peek_mbuf[i]->mbuf->pkt_len);
//         // rte_hash_del_key(app.fwd_hash, &key);
//     }
//     int ring_id = 0;
//     uint64_t min_departure = UINT64_MAX;
//     for (int i = 0; i < 3; ++i)
//     {
//         if (context->peek_valid[i] && estimate_departure[i] < min_departure)
//         {
//             min_departure = estimate_departure[i];
//             ring_id = i;
//         }
//     }
//     if (context->peek_valid[0] || context->peek_valid[1] || context->peek_valid[2])
//     {
//         rte_ring_sp_enqueue(context->output_ring, context->peek_mbuf[ring_id]);
//         context->peek_valid[ring_id] = 0;
//     }
// }

void flows2nodes_WFQ(struct flows2nodes_context *context)
{
    
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < context->WFQ_weight[i]; ++j)
        {
            uint64_t ts_start = rte_get_tsc_cycles();
            int ret = rte_ring_sc_dequeue(
                context->input_rings[i],
                (void **)&context->worker_mbuf->array);
            if (ret == -ENOENT)
                break;
            rte_ring_sp_enqueue(context->output_ring, context->worker_mbuf->array[0]);
            uint64_t ts_end = rte_get_tsc_cycles();
            app.cyc += (ts_end - ts_start);
            app.flows2nodes_cyc[context->nodeid] += (ts_end - ts_start);
        }
    }
}

void flows2nodes_pFabric(struct flows2nodes_context *context)
{
    uint64_t ts_start = rte_get_tsc_cycles();
    int ring = 0;
    int min_size = INT_MAX;
    for (int i = 0; i < 3; ++i)
    {
        if (context->pFabric_size[i] < min_size)
        {
            ring = i;
            min_size = context->pFabric_size[i];
        }
    }
    for (int i = 0; i < 2; ++i)
    {
        int ret = rte_ring_sc_dequeue(
            context->input_rings[ring],
            (void **)context->worker_mbuf->array);
        if (ret == -ENOENT)
        {
            ring = (ring + 1) % 3;
            continue;
        }
        context->pFabric_size[ring] -= context->worker_mbuf->array[0]->pkt_len;
        rte_ring_sp_enqueue(context->output_ring, context->worker_mbuf->array[0]);
        uint64_t ts_end = rte_get_tsc_cycles();
        app.cyc += (ts_end - ts_start);
        app.flows2nodes_cyc[context->nodeid] += (ts_end - ts_start);
        break;
    }
}