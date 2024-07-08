/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QFileDialog>
#include "msgbox.h"

Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this);
    QTimer::singleShot(5,this,SLOT(initFunSlot()));
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
    ui->targetEdit->setHidden(true);
    mSsdp = Ssdp_Core::bulid(this);
    mSender = Core_Sender::bulid(this);
    connect(mSender, &Core_Sender::upgradeError, this, &Home_WorkWid::upgradeError);
    connect(mSender, &Core_Sender::infoMessage, this, &Home_WorkWid::insertTextSlot);
    connect(mSender, &Core_Sender::finishSig, this, &Home_WorkWid::finishSlot);
    connect(mSender, &Core_Sender::overSig, this, &Home_WorkWid::updateResult);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutDone()));
    // QTimer::singleShot(450,this,SLOT(updateCntSlot()));
}


void Home_WorkWid::upgradeError(const QString &host, const QString &msg)
{
    QString str = host + tr(" 升级失败 ") ;
    mCnt.okCnt -= 1; mCnt.errCnt += 1;
    insertTextSlot(false, str);
    if(!isStart) updateResult();
    MsgBox::critical(this, str+msg);
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

void Home_WorkWid::insertTextSlot(bool pass, const QString &msg)
{
    setTextColor(pass);
    if(!(mId%500)) ui->textEdit->clear();
    QString str = QString::number(mId++) + "、"+ msg + "\n";
    ui->textEdit->insertPlainText(str);
}

void Home_WorkWid::finishSlot(bool pass, const QString &msg)
{
    QString str;
    if(pass) {
        mCnt.okCnt += 1;
        str = tr(" 文件上传成功！");
    } else {
        mCnt.errCnt += 1;
        str = tr(" 文件上传失败！");
    }
    insertTextSlot(pass, msg+str);
}

void Home_WorkWid::initCnt(int dst, int slave)
{
    sCount *cnt = &(mCnt);
    cnt->dstCnt = dst;
    cnt->slaveCnt = slave;
    cnt->allCnt = dst + slave;
    cnt->okCnt = cnt->errCnt = 0;
}

void Home_WorkWid::updateCntSlot()
{
    sCount *cnt = &mCnt;
    ui->okLcd->display(cnt->okCnt);
    ui->allLcd->display(cnt->allCnt);
    ui->errLcd->display(cnt->errCnt);

    //ui->cntSpin->setValue(mItem->cnts.cnt);
    //if(mItem->cnts.cnt < 1)mItem->user.clear();
    //ui->userEdit->setText(mItem->user);

    QString str = "0";
    if(cnt->dstCnt) {
        double value = cnt->okCnt*100.0 / cnt->dstCnt;
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
    ui->searchBtn->setEnabled(en);
    ui->groupBox_2->setEnabled(en);
}

void Home_WorkWid::updateResult()
{
    QString style; //mId = 1;
    QString str = tr("---");
    if(mResult && mCnt.dstCnt) {
        str = tr("成功"); style = "background-color:green; color:rgb(255, 255, 255);";
    } else {
        str = tr("失败"); style = "background-color:red; color:rgb(255, 255, 255);";
    } style += "font:100 34pt \"微软雅黑\";";

    isStart = false;
    setWidEnabled(true);
    ui->timeLab->setText(str);
    ui->timeLab->setStyleSheet(style);
    ui->startBtn->setText(tr("开 始"));
    QTimer::singleShot(450,this,SLOT(updateCntSlot()));
    str = QTime::currentTime().toString("hh:mm:ss");
    ui->endLab->setText(str);
    mSender->stopRun();
    timer->stop();
}


void Home_WorkWid::timeoutDone()
{
    updateTime();
    updateCntSlot();
}

bool Home_WorkWid::readRsaSig(const QString &name)
{
    QString fn = name;
    fn = fn.replace(".zip", ".sig");
    QFile file(fn); QString md5, sig;
    if(file.open(QIODevice::ReadOnly)) {
        md5 = file.readLine();
        sig = file.readLine();
        md5 = md5.left(md5.size() - 2);
        qDebug() << md5.size() << sig.size();
    } else {
        MsgBox::critical(this, tr("校验文件不存在 %1").arg(fn));
        file.close(); return false;
    } file.close();

    mFileIt.md5.clear(); mFileIt.sig.clear();
    if(md5.size() == 32) mFileIt.md5 = md5;
    else {MsgBox::critical(this, tr("MD5校验码格式不对:%1%2").arg(md5.size()).arg(md5.data())); return false;}

    if(sig.size() > 256) mFileIt.sig = sig;
    else {MsgBox::critical(this, tr("签名信息格式不对:%1").arg(sig.data())); return false;}
    return true;
}

void Home_WorkWid::initData(sOtaFile &it)
{
    mId = 1; mResult = true;
    it.file = ui->fnLab->text().split("/").last();
    it.size = File::fileSize(ui->fnLab->text());

    //QString md = it.file.split(".").last();
    //if(md.size() != 32) it.md5 = md;
    //else it.md5 = File::md5(ui->fnLab->text());

    it.fc = ui->crcBox->currentIndex();
    ui->mdLab->setText(it.md5);

    QString suffix; double rate = 1;
    if(it.size > 1024*1024) {rate = 1024*1024; suffix="Mb";}
    else if(it.size > 1024) {rate = 1024; suffix="Kb";}
    QString str = QString::number(it.size/rate, 'f', 1);
    ui->sizeLab->setText(str + suffix); it.path = "/upload/";
    //it.dev = ui->modeBox->currentText();   
    //it.path = "/home/lzy/work/upload/";
}

bool Home_WorkWid::inputCheck()
{    
    bool ret = false;
    QString str = ui->targetEdit->text();
    int index = ui->searchBox->currentIndex();
    if(1 == index) {
        if(str.isEmpty()) MsgBox::critical(this, tr("机房名称，未指定。")); else ret = true;
    } else if(2 == index) {
        ret = cm_isIPaddress(str);
        if(ret) ret = cm_pingNet(str);
        if(!ret) MsgBox::critical(this, tr("目标设备IP出错！"));
    } else ret = true;

    return ret;
}

bool Home_WorkWid::initWid()
{
    bool ret = inputCheck();
    if(ret && QFile::exists(ui->fnLab->text())) {
        setWidEnabled(false);
        mId = 1; mResult = true;
        ui->startBtn->setText(tr("终 止"));
        startTime = QTime::currentTime(); emit startSig();
        QString str = startTime.toString("hh:mm:ss");
        ui->startLab->setText(str);
        ui->endLab->setText("---");
        ui->textEdit->clear();
        isStart = true;
    }

    return ret;
}

QStringList Home_WorkWid::getIpList()
{
    QStringList ips; QString dst = ui->targetEdit->text();
    switch (ui->searchBox->currentIndex()) {
    case 0: ips = mSsdp->searchAll(); break;
    case 1: ips = mSsdp->searchDev(dst); break;
    case 2: ips = mSsdp->searchRoom(dst); break;
    }

    return ips;
}

void Home_WorkWid::on_startBtn_clicked()
{
    if(isStart == false) {
        if(initWid()) {
            timer->start(500);
            QStringList ips = getIpList();
            initCnt(ips.size(), mSsdp->getSlaveNum());
            mSender->sendFile(ips, ui->fnLab->text(), mFileIt);
        }
    } else {
        bool ret = MsgBox::question(this, tr("确定需要提前结束？"));
        if(ret) { mResult = false; updateResult(); }
    }
}

bool Home_WorkWid::fileCrc(const QString &fn)
{
    //int index = ui->crcBox->currentIndex();
    //bool ret = true; if(index == 0) {
    //    QString str = fn.split(".").last();
    //    if(str.size() == 32) index = 1;
    //} if(index == 1) ret = File::CheckMd5(fn);
    int index = ui->crcBox->currentIndex();
    bool ret = true; if(index == 0) {
        ret = readRsaSig(fn); if(ret) {
            QString md5 = File::md5(fn);
            ret = md5 == mFileIt.md5 ? true:false;
            if(!ret) MsgBox::critical(this, tr("你选择的文件，未能通过检验。"));
        }
    } else {
        mFileIt.md5 = File::md5(fn);
    }

    if(ret) {
        int size = File::fileSize(fn) / (1024*1024);
        if(size > 18) {MsgBox::critical(this, tr("你选择的文件太大，无法进行批量升级。")); ret=false;}
    }

    return ret;
}

void Home_WorkWid::on_imgBtn_clicked()
{
    QString str = tr("请选择");  // QString dir = QDir::home().absolutePath();
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString fn = QFileDialog::getOpenFileName(this, tr("选择烧录文件"), dir, "升级包文件(*.zip)");
    if(fn.isEmpty()) return;

    bool en = fileCrc(fn);
    if(en){
        str = tr("已选择");
        ui->fnLab->setText(fn);
        QTimer::singleShot(5,this,SLOT(updateWidSlot()));
    } else ui->fnLab->clear();

    ui->imgBtn->setText(str);
    ui->startBtn->setEnabled(en);
}


void Home_WorkWid::on_searchBtn_clicked()
{
    if(!inputCheck()) return;
    QStringList res = getIpList();
    if(res.size()) {
        QString str = tr("已找到%1个设备，包含%2个主机，%3个副机")
                .arg(res.size() + mSsdp->getSlaveNum())
                .arg(res.size()).arg(mSsdp->getSlaveNum());
        if(res.size() + mSsdp->getSlaveNum() < 50) {
            for(int i=0; i<res.size(); i++) {
                if(i%5) str += "\t"; else str += "\n";
                str += res.at(i);
            }
        } MsgBox::information(this, str);
    } else {
        MsgBox::critical(this, tr("未找到任何目标设备"));
    }
}


void Home_WorkWid::on_searchBox_currentIndexChanged(int index)
{
    ui->targetEdit->setHidden(!index);
}

