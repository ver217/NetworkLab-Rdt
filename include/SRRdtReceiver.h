#pragma once
#include "../include/Base.h"
#include "../include/RdtReceiver.h"
#include "../include/PacketDocker.h"
#include <vector>
#include <unordered_map>

class SRRdtReceiver : public RdtReceiver {
  private:
    const unsigned int SEQ_MAX;
    const int N;
    int base;
    unordered_map<int, Packet> cache;
    vector<Packet> acks;
    inline bool in_window(int seqnum);
    inline bool in_prev_window(int seqnum);
  public:
    SRRdtReceiver(int n = 4, unsigned seqnum_bits = 16);
    virtual ~SRRdtReceiver();
    void receive(Packet &packet);   //接收报文，将被NetworkService调用
};
