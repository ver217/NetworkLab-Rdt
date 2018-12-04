#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/RdtSender.h"
#include "../include/RdtReceiver.h"
#include "../include/StopWaitRdtSender.h"
#include "../include/StopWaitRdtReceiver.h"
#include "../include/GBNRdtSender.h"
#include "../include/GBNRdtReceiver.h"
#include "../include/SRRdtSender.h"
#include "../include/SRRdtReceiver.h"
#include "../include/TCPSender.h"
#include "../include/TCPReceiver.h"

int main() {
#ifdef GBN
    auto *ps = new GBNRdtSender(4, 3);
    auto * pr = new GBNRdtReceiver(3);
#elif SR
    auto *ps = new SRRdtSender(4, 3);
    auto * pr = new SRRdtReceiver(4, 3);
#elif TCP
    auto *ps = new TCPSender(4, 3);
    auto * pr = new TCPReceiver(4, 3);
#else
    auto *ps = new StopWaitRdtSender();
    auto * pr = new StopWaitRdtReceiver();
#endif
    pns->init();
    pns->setRtdSender(ps);
    pns->setRtdReceiver(pr);
    pns->setInputFile("/home/ver217/c/rdt/rdt/input.txt");
    pns->setOutputFile("/home/ver217/c/rdt/rdt/output.txt");
    pns->start();
    delete ps;
    delete pr;
    delete pUtils;                                  //指向唯一的工具类实例，只在main函数结束前delete
    delete pns;                                     //指向唯一的模拟网络环境类实例，只在main函数结束前delete
    return 0;
}
