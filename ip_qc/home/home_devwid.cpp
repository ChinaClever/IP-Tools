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
    it->boardNum = ui->boardNumBox->value();
    it->loopNum = ui->loopNumBox->value();
    it->outputNum = ui->outputNumBox->value();

    sVersion *ver = &mIt->ver;
    ver->devType = ui->typeEdit->text();
    mPro->getPro()->productType = ui->typeEdit->text();
    ver->fwVer = ui->fwEdit->text();
    ver->loopOutlets.clear();
    ver->loopOutlets << ui->opBox_1->value();
    ver->loopOutlets << ui->opBox_2->value();
    ver->loopOutlets << ui->opBox_3->value();
    ver->loopOutlets << ui->opBox_4->value();
    ver->loopOutlets << ui->opBox_5->value();
    ver->loopOutlets << ui->opBox_6->value();
}

void Home_DevWid::updateThresholds()
{
    sThreshold *it = &mIt->rate;
    it->lineVol = ui->lineVolBox->value();
    it->lineCur = ui->lineCurBox->value();
    it->linePow = ui->linePowBox->value();
    it->loopVol = ui->loopVolBox->value();
    it->loopCur = ui->loopCurBox->value();
    it->loopPow = ui->loopPowBox->value();
}

void Home_DevWid::updateOutlets()
{
    QList<QCheckBox *> box = checkBoxFound();
    mIt->rate.ops.clear(); foreach (auto it, box) {
        if(it->isChecked()) {
            QString name = it->objectName().remove("op_");
            mIt->rate.ops << name.toInt();
        }
    }
}

void Home_DevWid::updateData()
{
    updateParams();
    updateOutlets();
    updateThresholds();
}

QList<QCheckBox *> Home_DevWid::checkBoxFound()
{
    QList<QCheckBox *> box;
    for (int i = 0; i < ui->checkWid->children().size(); i++) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(ui->checkWid->children()[i]);
        if (cb) box << cb;
    }
    return box;
}

void Home_DevWid::on_uncheckBtn_clicked()
{
    QList<QCheckBox *> box = checkBoxFound();
    foreach (auto it, box) it->setChecked(false);
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



















