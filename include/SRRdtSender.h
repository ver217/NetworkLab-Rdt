#pragma once
#include "../include/RdtSender.h"
#include "../include/DataStructure.h"
#include "../include/PacketDocker.h"
#include <vector>

class SRRdtSender : public RdtSender {
  private:
    const unsigned int SEQ_MAX;
    const int N;
    int base;
    int nextSeqnum;
    std::vector<PacketDocker> pktds;        //已发送并等待Ack的数据包
    inline bool in_window(int ackNum);
  public:
    bool getWaitingState();
    bool send(Message &message);                        //发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(Packet &ackPkt);                       //接受确认Ack，将被NetworkServiceSimulator调用
    void timeoutHandler(int seqNum);                    //Timeout handler，将被NetworkServiceSimulator调用
    SRRdtSender(int n = 4, unsigned int seqnum_bits = 16);
    virtual ~SRRdtSender();
};
