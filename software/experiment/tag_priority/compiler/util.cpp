#include <map>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <pcap.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include "SP.h"

const int IP_OFFSET = 2;

std::string getFlowId(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet) {
    struct ip* ip_header = (struct ip*)(packet + IP_OFFSET);

    int protocol = ip_header->ip_p;

    char src_ip_str[INET_ADDRSTRLEN];
    char dst_ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip_str, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip_str, INET_ADDRSTRLEN);

    std::ostringstream result;
    result << src_ip_str << ", ";
    result << dst_ip_str << ", ";

    if (protocol == IPPROTO_TCP) {
        struct tcphdr* tcp_header = (struct tcphdr*)(packet + IP_OFFSET + ip_header->ip_hl * 4);

        uint16_t src_port = ntohs(tcp_header->th_sport);
        uint16_t dst_port = ntohs(tcp_header->th_dport);

        result << src_port << ", ";
        result << dst_port << ", ";
        result << "TCP";
    } else if (protocol == IPPROTO_UDP) {
        struct udphdr* udp_header = (struct udphdr*)(packet + IP_OFFSET + ip_header->ip_hl * 4);

        uint16_t src_port = ntohs(udp_header->uh_sport);
        uint16_t dst_port = ntohs(udp_header->uh_dport);

        result << src_port << ", ";
        result << dst_port << ", ";
        result << "UDP";
    } else {
        result << "Unsupported protocol";
    }

    return result.str();
}

std::string getSrcIP(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet) {
    struct ip* ip_header = (struct ip*)(packet + IP_OFFSET);

    int protocol = ip_header->ip_p;

    char src_ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip_str, INET_ADDRSTRLEN);

    std::ostringstream result;
    result << src_ip_str;

    return result.str();
}

std::string getTraceFileName(std::string inputFileName){
    size_t dotPos = inputFileName.find(".output");

    assert(dotPos != std::string::npos);

    std::string outputFileName = inputFileName;
    outputFileName.replace(dotPos, 7, ".trace");

    return outputFileName;
}

std::string getRefFileName(std::string inputFileName){
    size_t dotPos = inputFileName.find(".output");

    assert(dotPos != std::string::npos);

    std::string outputFileName = inputFileName;
    outputFileName.replace(dotPos, 7, ".ref");

    return outputFileName;
}

std::string getPFMapFileName(std::string inputFileName){
    size_t dotPos = inputFileName.find(".output");

    assert(dotPos != std::string::npos);

    std::string outputFileName = inputFileName;
    outputFileName.replace(dotPos, 7, ".pfmap");

    return outputFileName;
}

void extractFlowIdHandler(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet) {
    static std::set<std::string> flowIdSet;

    std::string flowId = getFlowId(user, pkthdr, packet);

    if(flowIdSet.insert(flowId).second){
        std::cout << flowId << std::endl;
    }
}

void extractFlowId(const char* pcap_file, const char* flowId_file){
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap_handle;

    pcap_handle = pcap_open_offline(pcap_file, errbuf);
    if (pcap_handle == nullptr) {
        std::cerr << "Error opening pcap file: " << errbuf << std::endl;
        return;
    }

    freopen(flowId_file, "w", stdout);
    if (pcap_loop(pcap_handle, 0, extractFlowIdHandler, nullptr) < 0) {
        std::cerr << "Error in pcap_loop: " << pcap_geterr(pcap_handle) << std::endl;
        pcap_close(pcap_handle);
        return;
    }
    fclose(stdout);

    pcap_close(pcap_handle);
    
    return;
}
