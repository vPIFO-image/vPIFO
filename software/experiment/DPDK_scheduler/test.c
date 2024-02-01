#include "main.h"

void app_main_loop_test(void)
{
    RTE_LOG(INFO, SWITCH, "Core %u is doing test\n", rte_lcore_id());
    uint64_t last_output_time[APP_MAX_PORTS] = {0};
    app.cpu_freq[rte_lcore_id()] = rte_get_tsc_hz();

    uint64_t output_gap = app.cpu_freq[rte_lcore_id()];
    uint32_t i;
    int ret;
    double irate, orate;
    // double loss_rate;
    double time_in_s;
    struct rte_eth_stats port_stats;
    struct rte_eth_stats port_stats_vector[APP_MAX_PORTS] = {0};
    // FILE *fp = NULL;
    // fp = fopen("test.txt", "w+");
    uint64_t now_time;
    uint64_t base_time;
    base_time = rte_get_tsc_cycles();
    for (i = 0; !force_quit; i = (i + 1) % app.n_ports)
    {
        now_time = rte_get_tsc_cycles();
        // ret = rte_eth_stats_get(i, &port_stats);
        // app.orate[i]=(port_stats.obytes - port_stats_test_vector[i].obytes) * 1.0 / (now_time - last_test_time[i]);
        // port_stats_test_vector[i].obytes=port_stats.obytes;
        if (now_time - last_output_time[i] > output_gap)
        {
            ret = rte_eth_stats_get(i, &port_stats);
            if (ret == 0)
            {
                if(i==0)
                {
                    RTE_LOG(INFO,SWITCH,"qlen: %-4dB, %d,%d,%d,%d,%d,%d,%d,%d, occupied buf %d\n",get_qlen_bytes(1),rte_ring_count(app.rings_flows[0]),rte_ring_count(app.rings_flows[0]),rte_ring_count(app.rings_flows[0]),rte_ring_count(app.rings_flows[0]),rte_ring_count(app.rings_flows[0]),rte_ring_count(app.rings_flows[0]),rte_ring_count(app.rings_nodes[0]),rte_ring_count(app.rings_nodes[1]),get_buff_occu_bytes());
                    RTE_LOG(INFO, SWITCH,"core_freq=%lu, avg_cyc=%f, rx2flows=%f, flows2nodes=%f, forwarding=%f, estimate_throughput=%fGbps\n",app.cpu_freq[rte_lcore_id()],app.cyc*1.0/app.scheduled_packets,app.rx2flows_cyc*1.0/app.scheduled_packets,(app.flows2nodes_cyc[0]+app.flows2nodes_cyc[1])*1.0/app.scheduled_packets,app.forwarding_cyc*1.0/app.scheduled_packets,app.cpu_freq[rte_lcore_id()]/1000000000.0/app.cyc*app.scheduled_packets*1500*8);
                }
                irate = (port_stats.ibytes - port_stats_vector[i].ibytes)  * 8.0 / 1000000;
                orate = (port_stats.obytes - port_stats_vector[i].obytes)  * 8.0 / 1000000;
                // loss_rate = (port_stats.ierrors + port_stats.imissed) * 1.0 / (port_stats.ierrors + port_stats.imissed + port_stats.ibytes) * 100;
                time_in_s = (now_time - base_time) * 1.0 / app.cpu_freq[rte_lcore_id()];
                RTE_LOG(INFO, SWITCH, "Time: %-5fs Port %d: ipkts=%-10ld  opkts=%-10ld  irate=%-10fMbps orate=%-10fMbps\n",
                        time_in_s, i, port_stats.ipackets, port_stats.opackets, irate, orate);
            }
            else
            {
                RTE_LOG(DEBUG, SWITCH, "timestamp=%ld  ERROR\n", now_time);
            }
            port_stats_vector[i].ibytes = port_stats.ibytes;
            // port_stats_vector[i].ipackets = port_stats.ipackets;
            port_stats_vector[i].obytes = port_stats.obytes;
            // port_stats_vector[i].opackets = port_stats.opackets;
            last_output_time[i] = now_time;
        }
    }

}