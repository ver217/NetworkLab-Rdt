#pragma once
#include "../include/DataStructure.h"

struct PacketDocker {
    Packet pkt;
    bool is_ack;
    PacketDocker() : is_ack(false) {}
    PacketDocker(const Packet& pkt) : pkt(pkt), is_ack(false) {}
};
