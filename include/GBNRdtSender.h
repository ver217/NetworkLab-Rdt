#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H
#include "../include/RdtSender.h"
#include "../include/DataStructure.h"
#include <queue>
class GBNRdtSender : public RdtSender {
  private:
    const unsigned int N_BITS;
    const int N;
    int base;
    int nextSeqnum;
    std::queue<Packet> pkts;        //已发送并等待Ack的数据包
  protected:
    Packet make_pkt(int seqNum, char data[Configuration::PAYLOAD_SIZE]);
  public:
    bool getWaitingState();
    bool send(Message &message);                        //发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(Packet &ackPkt);                       //接受确认Ack，将被NetworkServiceSimulator调用
    void timeoutHandler(int seqNum);                    //Timeout handler，将被NetworkServiceSimulator调用
    GBNRdtSender(int n, unsigned int seqnum_bits = 16);
    virtual ~GBNRdtSender();
};

#endif
