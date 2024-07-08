#ifndef DATAPACKET_H
#define DATAPACKET_H
#include <QtCore>
#include <QColor>

#define LINE_NUM  3
#define SENOR_NUM 1
#define NAME_SIZE 32
#define DEV_NUM 3
#define ARRAY_SIZE 255 //一包数据最长
#define PACK_ARRAY_SIZE LINE_NUM

// 倍率定义
#define COM_RATE_VOL	1.0    // 电压
#define COM_RATE_CUR	10.0    // 电流
#define COM_RATE_POW	1000.0  // 功率
#define COM_RATE_ELE	10.0    // 电能
#define COM_RATE_PF     100.0   // 功率因数
#define COM_RATE_TEM	1.0    // 温度
#define COM_RATE_HUM	1.0    // 湿度


/**
 * 数据单元：包括当前值，阈值，临界值，告警状态等
 */
struct sDataUnit
{
    sDataUnit() {size=0;}

    ushort size;
    ushort value[PACK_ARRAY_SIZE]; // 值
    ushort min[PACK_ARRAY_SIZE]; // 最小值
    ushort max[PACK_ARRAY_SIZE]; // 最大值
    uchar alarm[PACK_ARRAY_SIZE]; // 报警值 0表示未报警  1表示已报警 2表示已记录
    uchar status[PACK_ARRAY_SIZE];
};



/**
 * 数据对象：包括电流，电压，功率，电能，开关状态，插接位名称
 */
struct sObjData
{
    sObjData() {size=0;}
    int size;
    sDataUnit vol; // 电压
    sDataUnit cur; // 电流

    ushort pow[PACK_ARRAY_SIZE]; // 功率
    uint ele[PACK_ARRAY_SIZE]; // 电能

    uchar pf[PACK_ARRAY_SIZE]; // 功率因数
    uchar sw[PACK_ARRAY_SIZE]; // 开关状态 0 表示未启用

    ushort hz[PACK_ARRAY_SIZE]; // 电压频率
    ushort aPow[PACK_ARRAY_SIZE]; // 视在功率值

    uint powStatus[PACK_ARRAY_SIZE]; // 功率
    uint ratedCur[PACK_ARRAY_SIZE]; // 额定电流
    uint wave[PACK_ARRAY_SIZE]; // 谐波值
    uint tem[PACK_ARRAY_SIZE];
    uchar delay[PACK_ARRAY_SIZE];

    uchar pl[PACK_ARRAY_SIZE]; // 负载百分比
    uint curThd[PACK_ARRAY_SIZE]; // 电流谐波含量
    uint volThd[PACK_ARRAY_SIZE]; // 电压谐波含量
};



/**
 * 环境数据结构体
 */
struct sEnvData
{
    sEnvData() {size=0;}
    uchar size;
    sDataUnit tem; // 温度
    sDataUnit hum; // 湿度
};


struct sDevType
{
    sDevType() {}

    uint devId;
    uchar devType; // 1 ZPDU执行板类型  2 MPDU执行板类型   3 RPDU执行板类型
    // 4 SI-PDU  5 IP-PDU  6 BM-PDU
    uchar screen;//0 液晶 1 断码屏 2 数码管

    uchar ac; // 1 交流 2 直流
    uchar series; // 1 A系列  2 B系列  3 C系列  4 D系列1
    uchar lines;
    uchar outputs;

    uchar specs; // 1 互感器  2锰铜
    uchar version; // 版本号

    uchar ip[18];
    QString sn;
    QString dev_type;
};



/**
 * 设备数据结构体：
 */
struct sDevData
{
    sDevData() {id=1; en=0;}

    uchar id, en;  // 设备号
    sDevType devType; //设备类型
    uchar offLine; //离线标志 > 0在线

    sObjData line; // 相数据
    sEnvData env; // 环境数据

    uchar hz; // 电压频率
    ushort br;  // 00	表示波特率9600(00默认9600，01为4800，02为9600，03为19200，04为38400)
    ushort version;
    ushort reserve;
};


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
    QString macAddress;
    QString softwareVersion;
    // QString clientName;
    QString testTime;
    QString testStartTime;
    QString testEndTime;
    QStringList no, itemName;
    QList<bool> uploadPass;
    uchar uploadPassResult;
    QString pn;

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
