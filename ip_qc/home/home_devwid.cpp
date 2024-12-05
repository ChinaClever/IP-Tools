/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_devwid.h"
#include "ui_home_devwid.h"

Home_DevWid::Home_DevWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_DevWid)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    mHttp = Core_Http::bulid(this);
    mPro=sDataPacket::bulid();
    mIt = &Core_Object::coreItem.desire;
}

Home_DevWid::~Home_DevWid()
{
    delete ui;
}

void Home_DevWid::on_eleClearBtn_clicked()
{
    Core_Thread::bulid(this)->clearAllEle();
    MsgBox::information(this, tr("电能已清除"));
}

void Home_DevWid::on_factoryBtn_clicked()
{
    Core_Thread::bulid(this)->factoryRestore();
    MsgBox::information(this, tr("设备已进行出厂设置"));
}

void Home_DevWid::on_logBtn_clicked()
{
    Core_Thread::bulid(this)->clearLogs();
    MsgBox::information(this, tr("设备日志已清除，重启后生效"));
}

void Home_DevWid::on_openAllBtn_clicked()
{
    Core_Thread::bulid(this)->relayCtrl(1);
}

void Home_DevWid::on_closeAllBtn_clicked()
{
    Core_Thread::bulid(this)->relayCtrl(0);
}

void Home_DevWid::on_enDelayBtn_clicked()
{
    Core_Thread::bulid(this)->relayDelay(1);
    MsgBox::information(this, tr("顺序上下电延时功能已开启"));
}

void Home_DevWid::on_disDelayBtn_clicked()
{
    Core_Thread::bulid(this)->relayDelay(0);
    MsgBox::information(this, tr("顺序上下电延时功能已清除"));
}

void Home_DevWid::on_cascadeBtn_clicked()
{
    Core_Thread::bulid(this)->enCascade(1);
    MsgBox::information(this, tr("设备级联功能已开启"));
}

void Home_DevWid::on_rtuEnBtn_clicked()
{
    Core_Thread::bulid(this)->rtuSet(1);
    MsgBox::information(this, tr("Modbus-RTU功能已开启"));
}

void Home_DevWid::on_rtuDisenBtn_clicked()
{
    Core_Thread::bulid(this)->rtuSet(0);
    MsgBox::information(this, tr("Modbus-RTU功能已关闭"));
}

void Home_DevWid::on_bosEnBtn_clicked()
{
    Core_Thread::bulid(this)->boxSet(1);
    MsgBox::information(this, tr("传感器盒子功能已开启"));
}

void Home_DevWid::on_boxDisenBtn_clicked()
{
    Core_Thread::bulid(this)->boxSet(0);
    MsgBox::information(this, tr("传感器盒子功能已关闭"));
}

void Home_DevWid::updateParams()
{
    sParameter *it = &mIt->param;
    it->vh = ui->vhBox->currentIndex();
    it->devSpec = ui->devSpecBox->currentIndex()+1;
    it->lineNum = ui->lineNumBox->currentIndex()?3:1;
    it->standNeutral = ui->standardBox->currentIndex();
    it->sensorBoxEn = ui->sensorBox->currentIndex();
    it->isBreaker = ui->breakerBox->currentIndex();
    it->language = ui->languageBox->currentIndex();
    it->loopNum = ui->loopNumBox->value();

    sVersion *ver = &mIt->ver;
    ver->devType = ui->typeEdit->text();
    mPro->getPro()->productType = ui->typeEdit->text();
    ver->fwVer = ui->fwEdit->text();
}

void Home_DevWid::updateThresholds()
{
    sThreshold *it = &mIt->rate;
    it->lineVol[0] = ui->lineVolBox->value();
    it->lineCur[0] = ui->lineCurBox->value();
    it->linePow[0] = ui->linePowBox->value();
    it->loopVol[0] = ui->loopVolBox->value();
    it->loopCur[0] = ui->loopCurBox->value();
    it->loopPow[0] = ui->loopPowBox->value();
}

void Home_DevWid::updateData()
{
    // updateParams();
    // updateThresholds();
}

void Home_DevWid::on_lineVolBox_valueChanged(int arg1)
{
    double v = arg1 * ui->lineCurBox->value() / 1000.0;
    ui->linePowBox->setValue(v);
}

void Home_DevWid::on_lineCurBox_valueChanged(int arg1)
{
    double v = arg1 * ui->lineVolBox->value() / 1000.0;
    ui->linePowBox->setValue(v);
}

void Home_DevWid::on_loopVolBox_valueChanged(int arg1)
{
    double v = arg1 * ui->loopCurBox->value() / 1000.0;
    ui->loopPowBox->setValue(v);
}

void Home_DevWid::on_loopCurBox_valueChanged(int arg1)
{
    double v = arg1 * ui->loopVolBox->value() / 1000.0;
    ui->loopPowBox->setValue(v);
}



















