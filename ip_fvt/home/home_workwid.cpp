/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QStandardPaths>
#include "dbmacs.h"

Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this);
    Ssdp_Core::bulid(this);
    mCoreThread = new Core_Thread(this);
    mPro=sDataPacket::bulid();
    QTimer::singleShot(450,this,SLOT(initFunSlot()));
    //Core_Http::bulid(this)->initHost("192.168.1.89");
    //Core_Http::bulid(this)->execute("killall cores");
}

Home_WorkWid::~Home_WorkWid()
{
    delete ui;
}

void Home_WorkWid::initLayout()
{
    QPalette pl = ui->textEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
    //ui->userEdit->setPalette(pl); //ui->cntSpin->setPalette(pl);
    //ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //ui->textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QGridLayout *gridLayout = new QGridLayout(this->parentWidget());
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(this);
}

void Home_WorkWid::initFunSlot()
{
    initLayout(); isStart = false;
    QString dir = "usr/data/pdu/cfg";
    FileMgr::build().mkpath(dir);
    timer = new QTimer(this); //timer->start(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutDone()));
    connect(mCoreThread, &Core_Thread::finshSig, this, &Home_WorkWid::finishSlot);
    connect(mCoreThread, &Core_Thread::overSig, this, &Home_WorkWid::updateResult);
    connect(mCoreThread, &Core_Thread::msgSig, this, &Home_WorkWid::insertTextSlot);
    connect(Json_Pack::bulid(this), &Json_Pack::httpSig, this, &Home_WorkWid::insertTextSlot);

}

void Home_WorkWid::logWrite()
{
    sLogItem logIt;
    logIt.dev = ui->devLab->text();
    logIt.user = ui->userEdit->text();
    logIt.sw = ui->fwLab->text();
    logIt.sn = ui->snLab->text();
    if(mResult) logIt.result = tr("通过");
    else logIt.result = tr("失败");
    DbLogs::bulid()->insertItem(logIt);

    if(mResult) {
        sMacItem macIt;
        macIt.dev = ui->devLab->text();
        macIt.user = ui->userEdit->text();
        macIt.sn = ui->snLab->text();
        macIt.mac = ui->macLab->text();
        DbMacs::bulid()->insertItem(macIt);
    }
}

void Home_WorkWid::finishSlot(bool pass, const QString &msg)
{
    QString str = msg + tr(" 配置文件上传");;
    if(pass) {
        mCnt.ok += 1;
        str += tr("成功！");
        mPro->getPro()->uploadPassResult = 1;
    } else {
        mCnt.err += 1;
        str += tr("失败！");
        mPro->getPro()->uploadPassResult = 0;
    } mCnt.all += 1;
    insertTextSlot(str, pass);
    updateCntSlot(); logWrite();
}

void Home_WorkWid::setTextColor(bool pass)
{
    QColor color("black");
    if(!pass) {color = QColor("red"); mResult = pass;}
    ui->textEdit->moveCursor(QTextCursor::Start);

    QTextCharFormat fmt;//文本字符格式
    fmt.setForeground(color);// 前景色(即字体色)设为color色
    QTextCursor cursor = ui->textEdit->textCursor();//获取文本光标
    cursor.mergeCharFormat(fmt);//光标后的文字就用该格式显示
    ui->textEdit->mergeCurrentCharFormat(fmt);//textEdit使用当前的字符格式
}

void Home_WorkWid::insertTextSlot(const QString &msg, bool pass)
{
    QString str = QString::number(mId++) + "、"+ msg + "\n";
    setTextColor(pass); ui->textEdit->insertPlainText(str);
    mPro->getPro()->itemName<<msg;
    mPro->getPro()->uploadPass<<pass;
}

void Home_WorkWid::updateCntSlot()
{
    sCount *cnt = &mCnt;
    ui->okLcd->display(cnt->ok);
    ui->allLcd->display(cnt->all);
    ui->errLcd->display(cnt->err);

    int num = ui->cntSpin->value()-1;
    if(num) ui->cntSpin->setValue(num);

    QString str = "0";
    if(cnt->all) {
        double value = cnt->ok*100.0 / cnt->all;
        str = QString::number(value,'f',0) +"%";
    } ui->passLcd->display(str);
}


QString Home_WorkWid::getTime()
{
    QTime t(0,0,0,0);
    t = t.addSecs(startTime.secsTo(QTime::currentTime()));
    return  tr("%1").arg(t.toString("mm:ss"));
}

void Home_WorkWid::updateTime()
{
    QString str = getTime();
    QString style = "background-color:yellow; color:rgb(0, 0, 0);";
    style += "font:100 34pt \"微软雅黑\";";

    ui->timeLab->setText(str);
    ui->timeLab->setStyleSheet(style);
}

void Home_WorkWid::setWidEnabled(bool en)
{
    ui->groupBox->setEnabled(en);
}


void Home_WorkWid::updateResult()
{
    QString style; mId = 1;
    QString str = tr("---");
    if(mResult) {
        str = tr("成功"); style = "background-color:green; color:rgb(255, 255, 255);";
    } else {
        str = tr("失败"); style = "background-color:red; color:rgb(255, 255, 255);";
    } style += "font:100 34pt \"微软雅黑\";";

    isStart = false;
    setWidEnabled(true);
    ui->timeLab->setText(str);
    ui->timeLab->setStyleSheet(style);
    ui->startBtn->setText(tr("开 始"));
    //QTimer::singleShot(450,this,SLOT(updateCntSlot()));
    str = QTime::currentTime().toString("hh:mm:ss");
    ui->endLab->setText(str);
    timer->stop();
}

bool Home_WorkWid::initMac()
{
    bool ret = true;
    sMac *it = MacAddr::bulid()->macItem;
    int res =  MacAddr::bulid()->macCnt(it->mac, it->endMac);
    if(res <= it->cntMac) {
        if(res < 1) {
            MsgBox::critical(this, tr("MAC地址已用完，无法继续使用")); ret = false;
        } else {
            QString str = tr("剩余MAC地址，仅有%1个，请向领导反馈").arg(res);
            MsgBox::critical(this, str);
        }
    }

    return ret;
}


bool Home_WorkWid::initUser()
{
    QString str = ui->userEdit->text();
    if(str.isEmpty()){MsgBox::critical(this, tr("请先填写工单号！")); return false;}
    mPro->getPro()->pn = ui->userEdit->text();

    int cnt = ui->cntSpin->value();
    if(cnt < 1) {MsgBox::critical(this, tr("请先填写订单剩余数量！")); return false;}
    return true;
}

bool Home_WorkWid::initHost()
{
    bool ret = false;
    if(ui->adCheckBox->isChecked()) {
        Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
        QStringList ips = ssdp->searchAll();
        if(1 == ips.size()) {
            ret = true; mCoreThread->setIps(ips);
            Core_Http::bulid(this)->initHost(ips.first());
        }else if(ips.size()){
            QString str = tr("目标设备太多\t\n");
            int i=0; foreach(const auto &ip, ips) {
                str += "   " + ip; i++;
                if(i%4==0) str += "\n";
            } MsgBox::critical(this,str);
        } else {
            QString str = tr("未找到任何目标设备");
            MsgBox::critical(this,str);
        }
    } else ret = true;

    return ret;
}

bool Home_WorkWid::inputCheck()
{
    bool ret = true;
    QString str = ui->ipEdit->text();
    if(ui->adCheckBox->isChecked()) {
        //ret = initHost();
    } else {
        ret = cm_isIPaddress(str); if(!ret) cout << str;
        QStringList ips; ips << str;
        if(ret) Core_Http::bulid(this)->initHost(str);
        if(ret) {ret = cm_pingNet(str); mCoreThread->setIps(ips); }
        if(!ret) MsgBox::critical(this, tr("目标设备IP出错！"));
    }

    return ret;
}


void Home_WorkWid::initData()
{
    QStringList ips;
    mId = 1; mResult = true;
    if(!ui->adCheckBox->isChecked()) {
        ips << ui->ipEdit->text();
        //mCoreThread->setIps(ips);
    }
}


bool Home_WorkWid::initWid()
{
    mPro->init();
    bool ret = initMac();
    if(ret) ret = initUser();
    if(ret) ret = inputCheck();
    //if(ret) ret = updateWid();
    if(ret) {
        initData();
        setWidEnabled(false);
        ui->startBtn->setText(tr("终 止"));
        QTimer::singleShot(2100,this,&Home_WorkWid::updateWid);
        startTime = QTime::currentTime(); emit startSig();
        QString str = startTime.toString("hh:mm:ss");
        ui->startLab->setText(str);
        ui->endLab->setText("---");
        ui->textEdit->clear();
        isStart = true;
    }

    return ret;
}

void Home_WorkWid::writeSnMac(const QString &sn, const QString &mac)
{
    QString dir = "usr/data/pdu/cfg/"; QFile file(dir + "mac.conf");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        file.write(mac.toLatin1());
    } file.close();

    file.setFileName(dir + "sn.conf");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        file.write(sn.toLatin1());
    } file.close();
}


bool Home_WorkWid::updateWid()
{
    QString dir = "usr/data/pdu";
    Cfg_App cfg(dir, this); sAppVerIt it;
    bool ret = cfg.app_unpack(it);
    if(!ret || it.sn.isEmpty()) MsgBox::critical(this, tr("版本信息读取错误"));
    //it.sn = mCoreThread->createSn();
    //cfg.app_serialNumber(it.sn);

    QString str = it.sn;
    if(str.isEmpty()) str = "--- ---";
    ui->snLab->setText(str);
    mPro->getPro()->productSN = str;

    str = it.dev;
    if(str.isEmpty()) str = "--- ---";
    ui->devLab->setText(str);
    mPro->getPro()->productType = str;

    str = it.hw;
    if(str.isEmpty()) str = "--- ---";
    ui->hwLab->setText(str);

    str = it.ver;
    if(str.isEmpty()) str = "--- ---";
    ui->fwLab->setText(str);
    mPro->getPro()->softwareVersion = str;

    str = mCoreThread->m_mac;
    if(str.isEmpty()) str = "--- ---";
    ui->macLab->setText(str);
    mPro->getPro()->macAddress = str;
    //writeSnMac(it.sn, str);

    return ret;
}



void Home_WorkWid::timeoutDone()
{
    updateTime();
}


void Home_WorkWid::on_startBtn_clicked()
{
    mPro->getPro()->testStartTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    if(isStart == false) {
        if(initWid()) {
            timer->start(500);
            //mCoreThread->run();
            //mCoreThread->start();
            emit mCoreThread->startSig();
        }
    } else {
        bool ret = MsgBox::question(this, tr("确定需要提前结束？"));
        if(ret) { mResult = false; updateResult(); }
    }
}



void Home_WorkWid::on_findBtn_clicked()
{
    Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
    QStringList ips = ssdp->searchAll();
    QString str = tr("未找到任何目标设备");
    if(ips.size()) {
        str = tr("已找到%1个设备，包含%2个主机，%3个副机\t\n")
                .arg(ips.size() + ssdp->getSlaveNum())
                .arg(ips.size()).arg(ssdp->getSlaveNum());
        int i=0; foreach(const auto &ip, ips) {
            str += "   " + ip; i++;
            if(i%4==0) str += "\n";
        }
    }
    MsgBox::information(this, str);
}


void Home_WorkWid::on_downBtn_clicked()
{
    QString str = tr("请确认下载设备的配置文件?");
    if(MsgBox::question(this, str)) {
        if(!inputCheck() || !initHost()) return;
        FileMgr::build().delFileOrDir("usr/data/pdu");
        QStringList fs = mCoreThread->getFs(); emit startSig();
        cm_mdelay(10); fs.removeLast(); fs.removeLast();
        Core_Http::bulid(this)->downFile(fs);
    } ui->startBtn->setEnabled(true);
}


void Home_WorkWid::on_adCheckBox_clicked(bool checked)
{
    ui->ipEdit->setDisabled(checked);
}
void Home_WorkWid::on_userEdit_selectionChanged()
{
    ui->userEdit->setClearButtonEnabled(1);
}
