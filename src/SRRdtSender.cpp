#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/utils.h"
#include "../include/SRRdtSender.h"

SRRdtSender::SRRdtSender(int n, unsigned int seqnum_bits)
    : SEQ_MAX((seqnum_bits > 0 && seqnum_bits <= 16 && (1 << seqnum_bits) >= 2 * n) ? (1 << seqnum_bits) : (1 << 16)),
      N(n),
      base(0),
      nextSeqnum(0),
      pktds(vector<PacketDocker>(n))  {
}

SRRdtSender::~SRRdtSender() {
}

inline bool SRRdtSender::in_window(int ackNum) {
    if (base == nextSeqnum)
        return false;
    if (base < nextSeqnum)
        return base <= ackNum && ackNum < nextSeqnum;
    return !(nextSeqnum <= ackNum && ackNum < base);
}

bool SRRdtSender::getWaitingState() {
    return nextSeqnum == static_cast<int>((base + N) % SEQ_MAX);
}

bool SRRdtSender::send(Message &message) {
    if (getWaitingState())
        return false;
    Packet pkt = make_data_pkt(nextSeqnum, message.data);
    pktds[nextSeqnum % N] = pkt;
    pUtils->printPacket("发送方发送报文", pkt);
    pns->sendToNetworkLayer(RECEIVER, pkt);
    pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqnum);
    nextSeqnum = (nextSeqnum + 1) % SEQ_MAX;
    return true;
}

void SRRdtSender::receive(Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if (checkSum == ackPkt.checksum && in_window(ackPkt.acknum)) {
        pktds[ackPkt.acknum % N].is_ack = true;
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        pns->stopTimer(SENDER, ackPkt.acknum);
        if (ackPkt.acknum == base) {
            while (base != nextSeqnum && pktds[base % N].is_ack)
                base = (base + 1) % SEQ_MAX;
        }
        cout << "base 向前移动变为" << base << endl;
    } else
        pUtils->printPacket("发送方没有正确收到确认", ackPkt);
}

void SRRdtSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER, seqNum);
    pns->sendToNetworkLayer(RECEIVER, pktds[seqNum % N].pkt);
    pUtils->printPacket("发送方定时器时间到，重发超时的报文", pktds[seqNum % N].pkt);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}
