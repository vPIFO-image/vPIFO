#pragma once

#include <string>

#define FREQUENCY 200e6


std::string getFlowId(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet);
std::string getSrcIP(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet);
std::string getTraceFileName(std::string inputFileName);
std::string getPFMapFileName(std::string inputFileName);
std::string getRefFileName(std::string inputFileName);
void extractFlowId(const char* pcap_file, const char* flowId_file);

inline long long tsToCycle(const struct timeval* ts) {
    return ts->tv_sec * FREQUENCY + ts->tv_usec * (FREQUENCY / 1e6);
}
