#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/GBNRdtSender.h"


GBNRdtSender::GBNRdtSender(int n, unsigned int seqnum_bits)
    : N_BITS(seqnum_bits <= 16 ? (1 << seqnum_bits) : (1 << 16)),
      N(n),
      base(0),
      nextSeqnum(0) {
}


GBNRdtSender::~GBNRdtSender() {
}

Packet GBNRdtSender::make_pkt(int seqNum, char data[Configuration::PAYLOAD_SIZE]) {
    Packet pkt;
    pkt.acknum = -1;
    pkt.seqnum = seqNum;
    memcpy(pkt.payload, data, Configuration::PAYLOAD_SIZE);
    pkt.checksum = pUtils->calculateCheckSum(pkt);
    return pkt;
}

bool GBNRdtSender::getWaitingState() {
    return nextSeqnum == static_cast<int>((base + N) % N_BITS);
}

bool GBNRdtSender::send(Message &message) {
    if (getWaitingState())   //发送方处于等待确认状态
        return false;
    Packet pkt = make_pkt(nextSeqnum, message.data);
    pkts[nextSeqnum % N] = pkt;
    pUtils->printPacket("发送方发送报文", pkt);
    if (base == nextSeqnum)
        pns->startTimer(SENDER, Configuration::TIME_OUT, base);        //启动发送方定时器
    pns->sendToNetworkLayer(RECEIVER, pkt);                              //调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
    nextSeqnum = (nextSeqnum + 1) % N_BITS;
    return true;
}

void GBNRdtSender::receive(Packet &ackPkt) {
    //检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    //如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
    if (checkSum == ackPkt.checksum) {
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        pns->stopTimer(SENDER, base);      //关闭定时器
        base = (ackPkt.acknum + 1) % N_BITS;
        if (base != nextSeqnum)
            pns->startTimer(SENDER, Configuration::TIME_OUT, base);
    }
}

void GBNRdtSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER, seqNum);                                     //首先关闭定时器
    for (int i = base; i < nextSeqnum; i = (i + 1) % N_BITS) {
        pUtils->printPacket("发送方定时器时间到，重发之前的报文", pkts[seqNum % N]);
        pns->sendToNetworkLayer(RECEIVER, pkts[seqNum % N]);          //重新发送数据包
    }
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);           //重新启动发送方定时器
}
