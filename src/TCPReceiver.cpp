#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/utils.h"
#include "../include/TCPReceiver.h"


TCPReceiver::TCPReceiver(int n, unsigned seqnum_bit)
    : SEQ_MAX((seqnum_bit > 0 && seqnum_bit <= 16) ? (1 << seqnum_bit) : (1 << 16)),
      N(n),
      base(0),
      lastAckPkt(make_ack_pkt(-1)) {
}


TCPReceiver::~TCPReceiver() {
}

inline bool TCPReceiver::in_window(int seqnum) {
    int end = (base + N - 1) % SEQ_MAX;
    if (base < end)
        return base <= seqnum && seqnum <= end;
    else
        return !(end < seqnum && seqnum < base);
}

void TCPReceiver::receive(Packet &packet) {
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
                lastAckPkt = make_ack_pkt(base);
                base = (base + 1) % SEQ_MAX;
            }
            pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        } else {
            pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
            pUtils->printPacket("接收方重新发送上次的确认报文", lastAckPkt);
        }
    } else if (checkSum != packet.checksum) {
        pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
        pUtils->printPacket("接收方重新发送上次的确认报文", lastAckPkt);
    }
    pns->sendToNetworkLayer(SENDER, lastAckPkt);    //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
}
