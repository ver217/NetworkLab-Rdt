#pragma once
#include "Base.h"
#include "Global.h"
#include "DataStructure.h"

Packet make_pkt(int seqNum, int ackNum, char data[Configuration::PAYLOAD_SIZE]);
Packet make_data_pkt(int seqNum, char data[Configuration::PAYLOAD_SIZE]);
Packet make_ack_pkt(int ackNum);

class BitMap {
    unsigned int bitmap;
  public:
    BitMap();
    void set(int n);
    bool has(int n);
    void remove(int n);
    void clear();
};
