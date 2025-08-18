/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_setwid.h"
#include "ui_home_setwid.h"
#include "printer/printer_bartender.h""
Home_SetWid::Home_SetWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_SetWid)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    mHttp = Core_Http::bulid(this);
    mPro=sDataPacket::bulid();
    mIt = &Core_Object::coreItem.desire;
    item = CfgCom::bulid()->item;
    initWid();
}

Home_SetWid::~Home_SetWid()
{
    delete ui;
}

void Home_SetWid::on_eleClearBtn_clicked()
{
    Core_Thread::bulid(this)->clearAllEle();
    MsgBox::information(this, tr("电能已清除"));
}

void Home_SetWid::on_factoryBtn_clicked()
{
    Core_Thread::bulid(this)->factoryRestore();
    MsgBox::information(this, tr("设备已进行出厂设置"));
}

void Home_SetWid::on_logBtn_clicked()
{
    Core_Thread::bulid(this)->clearLogs();
    MsgBox::information(this, tr("设备日志已清除，重启后生效"));
}

void Home_SetWid::on_cascadeBtn_clicked()
{
    Core_Thread::bulid(this)->enCascade(1);
    MsgBox::information(this, tr("设备级联功能已开启"));
}

void Home_SetWid::on_rtuEnBtn_clicked()
{
    Core_Thread::bulid(this)->rtuSet(1);
    Core_Thread::bulid(this)->reset();
    MsgBox::information(this, tr("Modbus-RTU功能已开启，设备已重启"));
}

void Home_SetWid::on_rtuDisenBtn_clicked()
{
    Core_Thread::bulid(this)->rtuSet(0);
    Core_Thread::bulid(this)->reset();
    MsgBox::information(this, tr("Modbus-RTU功能已关闭，设备已重启"));
}

void Home_SetWid::initWid()
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

    ui->macBox->setChecked(item->isMac);
    ui->eleBox->setChecked(item->isEle);
    ui->linkBox->setChecked(item->isLink);
    ui->timeBox->setChecked(item->isTimer);
    ui->sersor->setChecked(item->isSersor);
    ui->printFw->setChecked(item->fwPrint);
    ui->printSN->setChecked(item->snPrint);
    ui->printMac->setChecked(item->macPrint);

    ui->lineVolBox->setValue(item->lineVol);
    ui->lineCurBox->setValue(item->lineCur);
    ui->linePowBox->setValue(item->linePow);
    ui->loopVolBox->setValue(item->loopVol);
    ui->loopCurBox->setValue(item->loopCur);
    ui->loopPowBox->setValue(item->loopPow);
}

void Home_SetWid::updateWid()
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
    item->isSersor = ui->sersor->isChecked();
    item->isLink = ui->linkBox->isChecked();
    item->isMac = ui->macBox->isChecked();
    item->isEle = ui->eleBox->isChecked();

    item->fwPrint = ui->printFw->isChecked();
    item->snPrint = ui->printSN->isChecked();
    item->macPrint = ui->printMac->isChecked();
    CfgCom::bulid()->writeParams();

    item->lineVol= ui->lineVolBox->value();
    item->lineCur = ui->lineCurBox->value();
    item->linePow = ui->linePowBox->value();
    item->loopVol = ui->loopVolBox->value();
    item->loopCur = ui->loopCurBox->value();
    item->loopPow = ui->loopPowBox->value();

    CfgCom::bulid()->writeThresholds();
}


void Home_SetWid::updateParams()
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

void Home_SetWid::updateThresholds()
{
    mThreshold *it = &mIt->rate;
    it->lineVol= ui->lineVolBox->value();
    it->lineCur = ui->lineCurBox->value();
    it->linePow = ui->linePowBox->value();
    it->loopVol = ui->loopVolBox->value();
    it->loopCur = ui->loopCurBox->value();
    it->loopPow = ui->loopPowBox->value();
}

void Home_SetWid::updateData()
{
    updateParams();
    updateThresholds();
    updateWid();
}

void Home_SetWid::on_lineVolBox_valueChanged(int arg1)
{
    double v = arg1 * ui->lineCurBox->value() / 1000.0;
    ui->linePowBox->setValue(v);
}

void Home_SetWid::on_lineCurBox_valueChanged(int arg1)
{
    double v = arg1 * ui->lineVolBox->value() / 1000.0;
    ui->linePowBox->setValue(v);
}

void Home_SetWid::on_loopVolBox_valueChanged(int arg1)
{
    double v = arg1 * ui->loopCurBox->value() / 1000.0;
    ui->loopPowBox->setValue(v);
}

void Home_SetWid::on_loopCurBox_valueChanged(int arg1)
{
    double v = arg1 * ui->loopVolBox->value() / 1000.0;
    ui->loopPowBox->setValue(v);
}


void Home_SetWid::on_loopNumBox_valueChanged(int arg1)
{
    if(!arg1) {ui->loopVolBox->setEnabled(false);
        ui->loopCurBox->setEnabled(false);
        ui->loopPowBox->setEnabled(false);
    } else {ui->loopVolBox->setEnabled(true);
        ui->loopCurBox->setEnabled(true);
        ui->loopPowBox->setEnabled(true);
    }
}


void Home_SetWid::on_tlsBtn_clicked()
{
    sCoreItem *it = &Core_Object::coreItem;
    it->tlsFile = QFileDialog::getOpenFileName(this, "设备证书选择",
                                               QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                               "Pem (*.pem)");
}


void Home_SetWid::on_standardBox_currentIndexChanged(int index)
{
    ui->tlsBtn->setHidden(!index);
}


void Home_SetWid::on_printBtn_clicked()
{

    QString mac = Core_Object::coreItem.actual.mac;
    QString fw = ui->fwEdit->text();

    bool macPrint = ui->printMac->isChecked();
    bool fwPrint = ui->printFw->isChecked();

    qDebug()<<macPrint<<' '<<fwPrint;
    qDebug() << fw << "  " << mac;

    if (!macPrint && !fwPrint) {
        QMessageBox::warning(this, "提示", "请至少勾选要打印的内容（MAC 或 FW）！");
        return;
    }

    if ((macPrint && mac.trimmed().isEmpty()) || (fwPrint && fw.trimmed().isEmpty())) {
        QMessageBox::warning(this, "提示", "MAC 或 FW 数据为空，请先进行检测！");
        return;
    }

    sBarTend it;
    it.fw = fw;
    // 添加必要字段

//    if (macPrint)
//        Printer_BarTender::bulid()->printMAC(mac);

//    if (fwPrint)
//        Printer_BarTender::bulid()->printerInfo(it);
}



