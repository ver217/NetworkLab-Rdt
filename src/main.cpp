#include "../include/Base.h"
#include "../include/Global.h"
#include "../include/RdtSender.h"
#include "../include/RdtReceiver.h"
#include "../include/StopWaitRdtSender.h"
#include "../include/StopWaitRdtReceiver.h"
#include "../include/GBNRdtSender.h"
#include "../include/GBNRdtReceiver.h"


int main() {
    RdtSender *ps = new GBNRdtSender(4, 4);
    RdtReceiver * pr = new GBNRdtReceiver(4);
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
