/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QStandardPaths>


Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this);
    Ssdp_Core::bulid(this);
    mCoreThread = Core_Thread::bulid(this);
    QTimer::singleShot(450,this,SLOT(initFunSlot()));
    mPro=sDataPacket::bulid();
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
    timer = new QTimer(this); //timer->start(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutDone()));
    connect(mCoreThread, &Core_Thread::finshSig, this, &Home_WorkWid::finishSlot);
    connect(mCoreThread, &Core_Thread::overSig, this, &Home_WorkWid::updateResult);
    connect(mCoreThread, &Core_Thread::msgSig, this, &Home_WorkWid::insertTextSlot);
    connect(Json_Pack::bulid(this), &Json_Pack::httpSig, this, &Home_WorkWid::insertTextSlot);
    //QTimer::singleShot(450,this,SLOT(updateCntSlot()));
}

void Home_WorkWid::logWrite()
{
    sLogItem logIt;
    logIt.dev = "MPro";
    logIt.mac = ui->macLab->text();
    logIt.user = ui->userEdit->text();
    logIt.sw = ui->fwLab->text();
    logIt.sn = ui->snLab->text();
    if(mResult) logIt.result = tr("通过");
    else logIt.result = tr("失败");
    DbLogs::bulid()->insertItem(logIt);
}

void Home_WorkWid::finishSlot(bool pass, const QString &msg)
{
    QString str = msg + tr(" 检查结果 ");
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

    // mPro->init();
    QString str = QString::number(mId++) + "、"+ msg + "\n";
    setTextColor(pass); ui->textEdit->insertPlainText(str);
    mPro->getPro()->itemName<<msg;
    mPro->getPro()->uploadPass<<pass;
    // setTextColor(mPro->updatePro(str,pass)); ui->textEdit->insertPlainText(mPro->getPro()->itemName);
}

void Home_WorkWid::updateCntSlot()
{
    sCount *cnt = &mCnt;
    ui->okLcd->display(cnt->ok);
    ui->allLcd->display(cnt->all);
    ui->errLcd->display(cnt->err);

    int num = ui->cntSpin->value()-1;
    if(num) ui->cntSpin->setValue(num);

    QString str = "0"; if(cnt->all) {
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


bool Home_WorkWid::initUser()
{
    QString str = ui->userEdit->text();
    if(str.isEmpty()){MsgBox::critical(this, tr("请先填写工单号！")); return false;}
    mPro->getPro()->pn = ui->userEdit->text();

    int cnt = ui->cntSpin->value();
    if(cnt < 1) {MsgBox::critical(this, tr("请先填写订单剩余数量！")); return false;}
    return true;
}


bool Home_WorkWid::inputCheck()
{
    bool ret = true;
    QString str = ui->ipEdit->text();
    if(ui->auCheckBox->isChecked()) {
#if 0
        Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
        QStringList ips = ssdp->searchAll();
        if(ips.size()) { //cout << ips.size() << ips;
            ret = true; mCoreThread->setIps(ips);
            Core_Http::bulid(this)->initHost(ips.first());
        } else {
            QString str = tr("未找到任何目标设备");
            MsgBox::critical(this,str);
        }
#endif
    } else {
        ret = cm_isIPaddress(str);
        QStringList ips; ips << str;
        if(ret) Core_Http::bulid(this)->initHost(str);
        if(ret) {ret = cm_pingNet(str); mCoreThread->setIps(ips);}
        if(!ret) MsgBox::critical(this, tr("目标设备IP出错！"));
    }

    return ret;
}


void Home_WorkWid::initData()
{
    QStringList ips;
    mId = 1; mResult = true;
    //if(!ui->auCheckBox->isChecked()) {
    //    ips << ui->ipEdit->text();
    //} mCoreThread->setIps(ips);

    sCoreItem *it = &(Core_Object::coreItem);
    it->jsonPacket.clear();
    it->datetime.clear();
    //it->ip = ips.first();
    it->mac.clear();
    it->sn.clear();

    memset(&(it->actual.param), 0, sizeof(sParameter));
    it->actual.rate.lineVol = 0;
    it->actual.rate.lineCur = 0;
    it->actual.rate.linePow = 0;
    it->actual.rate.loopVol = 0;
    it->actual.rate.loopCur = 0;
    it->actual.rate.loopPow = 0;
    it->actual.rate.ops.clear();
}


bool Home_WorkWid::initWid()
{
    mPro->init();
    bool ret = initUser();
    if(ret) ret = inputCheck();
    //if(ret) ret = updateWid();
    if(ret) {
        initData();
        setWidEnabled(false);
        ui->startBtn->setText(tr("终 止"));
        QTimer::singleShot(1715,this,SLOT(updateWidSlot()));
        startTime = QTime::currentTime(); emit startSig();
        QString str = startTime.toString("hh:mm:ss");
        ui->startLab->setText(str);
        ui->endLab->setText("---");
        ui->textEdit->clear();
        isStart = true;
    }

    return ret;
}

void Home_WorkWid::updateWidSlot()
{
    updateWid();
}

bool Home_WorkWid::updateWid()
{
    sCoreItem *it = &Core_Object::coreItem;

    QString str = it->sn;
    if(str.isEmpty()) str = "--- ---";
    ui->snLab->setText(str);
    mPro->getPro()->productSN = str;

    str = it->actual.ver.fwVer;
    if(str.isEmpty()) str = "--- ---";
    ui->fwLab->setText(str);
    mPro->getPro()->softwareVersion = str;

    str = it->mac;
    if(str.isEmpty()) str = "--- ---";
    ui->macLab->setText(str);
    mPro->getPro()->macAddress = str;
    return true;
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
            mCoreThread->start();
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

void Home_WorkWid::on_auCheckBox_clicked(bool checked)
{
    ui->ipEdit->setDisabled(checked);
}


void Home_WorkWid::devSetBtn()
{
    ui->startBtn->setEnabled(true);
}

void Home_WorkWid::on_logoBtn_clicked()
{
    sCoreItem *it = &Core_Object::coreItem;
    it->logo = QFileDialog::getOpenFileName(this, "设备Logo图片选择",
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    "Images (*.png *.xpm *.jpg)");

    QString str = it->logo;
    if(str.isEmpty()) str = "--- ---";
    ui->logoLab->setText(str);
}

void Home_WorkWid::on_ipEdit_textChanged(const QString &arg1)
{
    Core_Http::bulid()->initHost(arg1);
}

void Home_WorkWid::on_userEdit_selectionChanged()
{
    ui->userEdit->setClearButtonEnabled(1);
}

