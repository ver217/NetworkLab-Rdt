#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/utils.h"
#include "../include/TCPSender.h"

TCPSender::TCPSender(int n, unsigned int seqnum_bits)
    : SEQ_MAX((seqnum_bits > 0 && seqnum_bits <= 16) ? (1 << seqnum_bits) : (1 << 16)),
      N(n),
      base(0),
      nextSeqnum(0),
      pkts(vector<Packet>(n))  {
}

TCPSender::~TCPSender() {
}

inline bool TCPSender::in_window(int ackNum) {
    if (base == nextSeqnum)
        return false;
    if (base < nextSeqnum)
        return base <= ackNum && ackNum < nextSeqnum;
    return !(nextSeqnum <= ackNum && ackNum < base);
}

bool TCPSender::getWaitingState() {
    return nextSeqnum == static_cast<int>((base + N) % SEQ_MAX);
}

bool TCPSender::send(Message &message) {
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

void TCPSender::receive(Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if (checkSum == ackPkt.checksum && in_window(ackPkt.acknum)) {
        base = (ackPkt.acknum  + 1) % SEQ_MAX;
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        pns->stopTimer(SENDER, 0);
        cout << "base 向前移动变为" << base << endl;
        if (base != nextSeqnum) {
            pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
            cout << "当前窗口不为空，打印出当前窗口内容" << endl;
            for (int i = base; i != nextSeqnum; i = (i + 1) % SEQ_MAX)
                pUtils->printPacket("当前窗口的内容", pkts[i % N]);
        } else
            cout << "当前窗口为空，所有包已经确认" << endl;
    } else if (checkSum == ackPkt.checksum) {
        if (ack_cnt.find(ackPkt.acknum) != ack_cnt.end())
            ++ack_cnt[ackPkt.acknum];
        else
            ack_cnt[ackPkt.acknum] = 1;
        if (ack_cnt[ackPkt.acknum] == 3) {
            pns->sendToNetworkLayer(RECEIVER, pkts[base % N]);
            pUtils->printPacket("收到冗余ACK，快速重传报文", pkts[base % N]);
            ack_cnt[ackPkt.acknum] = 0;
        }
    } else
        pUtils->printPacket("发送方没有正确收到确认", ackPkt);
}

void TCPSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER, 0);
    pns->sendToNetworkLayer(RECEIVER, pkts[base % N]);
    pUtils->printPacket("发送方定时器时间到，重发最早未被确认的报文", pkts[base % N]);
    pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
}
