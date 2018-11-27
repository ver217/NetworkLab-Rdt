#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/utils.h"
#include "../include/GBNRdtReceiver.h"


GBNRdtReceiver::GBNRdtReceiver(unsigned seqnum_bit): SEQ_MAX((seqnum_bit > 0 && seqnum_bit <= 16) ? (1 << seqnum_bit) : (1 << 16)), expectSequenceNumberRcvd(0) {
    lastAckPkt = make_ack_pkt(-1);
}


GBNRdtReceiver::~GBNRdtReceiver() {
}

void GBNRdtReceiver::receive(Packet &packet) {
    //检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(packet);
    //如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
    if (checkSum == packet.checksum && expectSequenceNumberRcvd == packet.seqnum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);
        //取出Message，向上递交给应用层
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        lastAckPkt = make_ack_pkt(packet.seqnum);
        pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        expectSequenceNumberRcvd = (expectSequenceNumberRcvd + 1) % SEQ_MAX;
    } else {
        if (checkSum != packet.checksum)
            pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
        else
            pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
        pUtils->printPacket("接收方重新发送上次的确认报文", lastAckPkt);
    }
    pns->sendToNetworkLayer(SENDER, lastAckPkt);    //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
}
