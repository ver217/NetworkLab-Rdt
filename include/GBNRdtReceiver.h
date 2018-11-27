#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "../include/RdtReceiver.h"
class GBNRdtReceiver : public RdtReceiver {
  private:
    const unsigned int N_BITS;
    int expectSequenceNumberRcvd;   // 期待收到的下一个报文序号
    Packet lastAckPkt;              //上次发送的确认报文
  public:
    GBNRdtReceiver(unsigned seqnum_bit = 16);
    virtual ~GBNRdtReceiver();
    
  public:
  
    void receive(Packet &packet);   //接收报文，将被NetworkService调用
};

#endif
