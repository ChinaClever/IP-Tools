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
    item = CfgCom::bulid()->item;
    initWid();
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

void Home_DevWid::initWid()
{
    ui->vhBox->setCurrentIndex(item->vh);
    ui->lineNumBox->setCurrentIndex(item->lineNum);
    ui->standardBox->setCurrentIndex(item->standNeutral);
    ui->devspecBox->setCurrentIndex(item->devSpec);
    ui->protocolBox->setCurrentIndex(item->oldProtocol);
    ui->breakerBox->setCurrentIndex(item->isBreaker);
    ui->languageBox->setCurrentIndex(item->language);
    ui->loopNumBox->setValue(item->loopNum);
    ui->typeEdit->setText(item->devType);
    ui->fwEdit->setText(item->fwVer);

    ui->timeBox->setChecked(item->isTimer);
    ui->macBox->setChecked(item->isMac);
    ui->sersorBox->setChecked(item->isSersor);
    ui->linkBox->setChecked(item->isLink);

    ui->lineVolBox->setValue(item->lineVol);
    ui->lineCurBox->setValue(item->lineCur);
    ui->linePowBox->setValue(item->linePow);
    ui->loopVolBox->setValue(item->loopVol);
    ui->loopCurBox->setValue(item->loopCur);
    ui->loopPowBox->setValue(item->loopPow);
}

void Home_DevWid::updateWid()
{
    item->vh = ui->vhBox->currentIndex();
    // it->devSpec = ui->devSpecBox->currentIndex()+1;
    item->lineNum = ui->lineNumBox->currentIndex();
    item->standNeutral = ui->standardBox->currentIndex();
    item->devSpec = ui->devspecBox->currentIndex();
    item->oldProtocol = ui->protocolBox->currentIndex();
    item->isBreaker = ui->breakerBox->currentIndex();
    item->language = ui->languageBox->currentIndex();
    item->loopNum = ui->loopNumBox->value();
    item->devType = ui->typeEdit->text();
    item->fwVer = ui->fwEdit->text();

    item->isTimer = ui->timeBox->isChecked();
    item->isMac = ui->macBox->isChecked();
    item->isSersor = ui->sersorBox->isChecked();
    item->isLink = ui->linkBox->isChecked();
    CfgCom::bulid()->writeParams();

    item->lineVol= ui->lineVolBox->value();
    item->lineCur = ui->lineCurBox->value();
    item->linePow = ui->linePowBox->value();
    item->loopVol = ui->loopVolBox->value();
    item->loopCur = ui->loopCurBox->value();
    item->loopPow = ui->loopPowBox->value();

    CfgCom::bulid()->writeThresholds();
}


void Home_DevWid::updateParams()
{
    sParameter *it = &mIt->param;
    it->vh = ui->vhBox->currentIndex();
    // it->devSpec = ui->devSpecBox->currentIndex()+1;
    it->lineNum = ui->lineNumBox->currentIndex()?3:1;
    it->standNeutral = ui->standardBox->currentIndex();
    it->devSpec = ui->devspecBox->currentIndex();
    it->oldProtocol = ui->protocolBox->currentIndex();
    it->isBreaker = ui->breakerBox->currentIndex();
    it->language = ui->languageBox->currentIndex();
    it->loopNum = ui->loopNumBox->value();

    sVersion *ver = &mIt->ver;
    ver->devType = ui->typeEdit->text();
    ver->fwVer = ui->fwEdit->text();
}

void Home_DevWid::updateThresholds()
{
    mThreshold *it = &mIt->rate;
    it->lineVol= ui->lineVolBox->value();
    it->lineCur = ui->lineCurBox->value();
    it->linePow = ui->linePowBox->value();
    it->loopVol = ui->loopVolBox->value();
    it->loopCur = ui->loopCurBox->value();
    it->loopPow = ui->loopPowBox->value();
}

void Home_DevWid::updateData()
{
    updateParams();
    updateThresholds();
    updateWid();
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


void Home_DevWid::on_loopNumBox_valueChanged(int arg1)
{
    if(!arg1) {ui->loopVolBox->setEnabled(false);
        ui->loopCurBox->setEnabled(false);
        ui->loopPowBox->setEnabled(false);
    } else {ui->loopVolBox->setEnabled(true);
        ui->loopCurBox->setEnabled(true);
        ui->loopPowBox->setEnabled(true);
    }
}


void Home_DevWid::on_tlsBtn_clicked()
{
    sCoreItem *it = &Core_Object::coreItem;
    it->tlsFile = QFileDialog::getOpenFileName(this, "设备证书选择",
                                               QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                               "Pem (*.pem)");
}


void Home_DevWid::on_standardBox_currentIndexChanged(int index)
{
    ui->tlsBtn->setHidden(!index);
}

