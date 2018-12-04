#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/utils.h"
#include "../include/SRRdtReceiver.h"
#include <algorithm>

SRRdtReceiver::SRRdtReceiver(int n, unsigned seqnum_bits)
    : SEQ_MAX((seqnum_bits > 0 && seqnum_bits <= 16 && (1 << seqnum_bits) >= 2 * n) ? (1 << seqnum_bits) : (1 << 16)),
      N(n),
      base(0),
      acks(vector<Packet>(n)) {
}

SRRdtReceiver::~SRRdtReceiver() {
}

inline bool SRRdtReceiver::in_window(int seqnum) {
    int end = (base + N - 1) % SEQ_MAX;
    if (base < end)
        return base <= seqnum && seqnum <= end;
    else
        return !(end < seqnum && seqnum < base);
}

inline bool SRRdtReceiver::in_prev_window(int seqnum) {
    int start = (base - N) % SEQ_MAX;
    if (start < 0)
        start += SEQ_MAX;
    int end = (base - 1) % SEQ_MAX;
    if (end < 0)
        end += SEQ_MAX;
    if (start < end)
        return start <= seqnum && seqnum <= end;
    else
        return !(end < seqnum && seqnum < start);
}

void SRRdtReceiver::receive(Packet &packet) {
    //检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(packet);
    //如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
    if (checkSum == packet.checksum && in_window(packet.seqnum)) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);
        //取出Message，向上递交给应用层
        if (cache.find(packet.seqnum) == cache.end())
            cache[packet.seqnum] = packet;
        if (packet.seqnum == base) {
            unordered_map<int, Packet>::const_iterator iter;
            while (!cache.empty() && (iter = cache.find(base)) != cache.end()) {
                Message msg;
                memcpy(msg.data, iter->second.payload, sizeof(iter->second.payload));
                pns->delivertoAppLayer(RECEIVER, msg);
                cache.erase(iter);
                base = (base + 1) % SEQ_MAX;
            }
        }
        Packet ackPkt = make_ack_pkt(packet.seqnum);
        acks[packet.seqnum % N] = ackPkt;
        pUtils->printPacket("接收方发送确认报文", ackPkt);
        pns->sendToNetworkLayer(SENDER, ackPkt);    //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
    } else if (checkSum == packet.checksum && in_prev_window(packet.seqnum)) {
        pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
        pUtils->printPacket("接收方重新发送上次的确认报文", acks[packet.seqnum % N]);
        pns->sendToNetworkLayer(SENDER, acks[packet.seqnum % N]);
    } else
        pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
}
