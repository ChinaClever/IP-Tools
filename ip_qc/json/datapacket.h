#ifndef DATAPACKET_H
#define DATAPACKET_H
#include <QtCore>
#include <QColor>

// #define LINE_NUM  3
// #define SENOR_NUM 1
// #define NAME_SIZE 32
// #define DEV_NUM 3
// #define ARRAY_SIZE 255 //一包数据最长
// #define PACK_ARRAY_SIZE LINE_NUM

// 倍率定义
#define COM_RATE_VOL	10.0    // 电压
#define COM_RATE_CUR	100.0    // 电流
#define COM_RATE_POW	1000.0  // 功率
#define COM_RATE_ELE	10.0    // 电能
#define COM_RATE_PF     100.0   // 功率因数
#define COM_RATE_TEM	10.0    // 温度
#define COM_RATE_HUM	1.0    // 湿度



enum {
    Test_Fun, // 功能
    Test_Start, // 开始
    Test_Ading,
    Test_vert, // 验证
    Test_Over, // 终止
    Sendok,    //发送成功
    Sendfail,  //发送失败
    Test_End, // 完成
    Collect_Start, // 数据采集
    Test_Info=0,
    Test_Pass=1,
    Test_Fail=2,
};




struct sProgress
{
    sProgress() {step=0;}

    uchar step; // 步骤
    QString time;
    QList<bool> pass, itPass;
    QStringList status, item;

    uchar result;    // 最终结果
    QTime startTime;

    QString softwareType;
    QString companyName;
    QString protocolVersion;

    QString productType;
    QString productSN;
    QString moduleSn;
    QString macAddress;
    QString softwareVersion;
    // QString clientName;
    QString testTime;
    QString testStartTime;
    QString testEndTime;
    QStringList no, itemName,testRequest,testStep,testItem;

    QList<bool> uploadPass;
    uchar uploadPassResult;
    QString pn,orderNum;

};


/**
 * 数据包
 */
class sDataPacket
{
    sDataPacket();
public:
    static sDataPacket *bulid();

    void init();
    sProgress *getPro() {return pro;}
    bool delay(int s=1);

protected:

    sProgress *pro;
};



#endif // DATAPACKET_H
