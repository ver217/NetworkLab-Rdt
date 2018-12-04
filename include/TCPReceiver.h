#pragma once
#include "../include/Base.h"
#include "../include/RdtReceiver.h"
#include <unordered_map>
class TCPReceiver : public RdtReceiver {
  private:
    const unsigned int SEQ_MAX;
    const int N;
    int base;
    Packet lastAckPkt;              //上次发送的确认报文
    unordered_map<int, Packet> cache;
    inline bool in_window(int seqnum);
  public:
    TCPReceiver(int n = 4, unsigned seqnum_bit = 16);
    virtual ~TCPReceiver();
    
  public:
  
    void receive(Packet &packet);   //接收报文，将被NetworkService调用
};
