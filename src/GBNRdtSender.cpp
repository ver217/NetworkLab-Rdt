#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/utils.h"
#include "../include/GBNRdtSender.h"

GBNRdtSender::GBNRdtSender(int n, unsigned int seqnum_bits)
    : SEQ_MAX((seqnum_bits > 0 && seqnum_bits <= 16) ? (1 << seqnum_bits) : (1 << 16)),
      N(n),
      base(0),
      nextSeqnum(0),
      pkts(vector<Packet>(n))  {
}

GBNRdtSender::~GBNRdtSender() {
}

inline bool GBNRdtSender::in_window(int ackNum) {
    if (base == nextSeqnum)
        return false;
    if (base < nextSeqnum)
        return base <= ackNum && ackNum < nextSeqnum;
    return !(nextSeqnum <= ackNum && ackNum < base);
}

bool GBNRdtSender::getWaitingState() {
    return nextSeqnum == static_cast<int>((base + N) % SEQ_MAX);
}

bool GBNRdtSender::send(Message &message) {
    if (getWaitingState())
        return false;
    Packet pkt = make_data_pkt(nextSeqnum, message.data);
    pkts[nextSeqnum % N] = pkt;
    pUtils->printPacket("发送方发送报文", pkt);
    pns->sendToNetworkLayer(RECEIVER, pkt);
    if (base == nextSeqnum)
        pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
    nextSeqnum = (nextSeqnum + 1) % SEQ_MAX;
    return true;
}

void GBNRdtSender::receive(Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if (checkSum == ackPkt.checksum && in_window(ackPkt.acknum)) {
        base = (ackPkt.acknum  + 1) % SEQ_MAX;
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        pns->stopTimer(SENDER, 0);
        cout << "base 向前移动变为" << base << endl;
        if (base != nextSeqnum)
            pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
    } else
        pUtils->printPacket("发送方没有正确收到确认", ackPkt);
}

void GBNRdtSender::timeoutHandler(int seqNum) {
    cout << "发送方定时器时间到，重发最多前N个报文" << endl;
    pns->stopTimer(SENDER, 0);
    for (int i = base; i != nextSeqnum; i = (i + 1) % SEQ_MAX) {
        pns->sendToNetworkLayer(RECEIVER, pkts[i % N]);
        pUtils->printPacket("发之前的报文", pkts[i % N]);
    }
    pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
}
