#include "home_printdlg.h"
#include "ui_home_printdlg.h"

Home_PrintDlg::Home_PrintDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Home_PrintDlg)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    mItem = CfgCom::bulid()->item;
    initFunSlot();
}

Home_PrintDlg::~Home_PrintDlg()
{
    delete ui;
}

void Home_PrintDlg::initFunSlot()
{
//    qDebug()<<mItem->xCodeOffset << " " <<mItem->yCodeOffset<< " " <<mItem->xMacOffset<< " " <<mItem->yMacOffset;
    ui->CodeXlineEdit->setText(QString::number(mItem->xCodeOffset));
    ui->CodeYlineEdit->setText(QString::number(mItem->yCodeOffset));
    ui->MACXlineEdit->setText(QString::number(mItem->xMacOffset));
    ui->MACYlineEdit->setText(QString::number(mItem->yMacOffset));
}

void Home_PrintDlg::on_testPrintBtn_clicked()
{
    QString cmd = "DF:DF:DF:DF:DF:1F";
    mItem->xCodeOffset = ui->CodeXlineEdit->text().toInt();
    mItem->yCodeOffset = ui->CodeYlineEdit->text().toInt();
    mItem->xMacOffset = ui->MACXlineEdit->text().toInt();
    mItem->yMacOffset = ui->MACYlineEdit->text().toInt();
    CfgCom::bulid()->writeOffset();

    bool ret = Printer_BarTender::bulid()->printMAC(cmd , mItem->xCodeOffset , mItem->yCodeOffset, mItem->xMacOffset , mItem->yMacOffset);
    //qDebug()<<ret <<endl;
}

void Home_PrintDlg::on_saveBtn_clicked()
{
    mItem->xCodeOffset = ui->CodeXlineEdit->text().toInt();
    mItem->yCodeOffset = ui->CodeYlineEdit->text().toInt();
    mItem->xMacOffset = ui->MACXlineEdit->text().toInt();
    mItem->yMacOffset = ui->MACYlineEdit->text().toInt();
    CfgCom::bulid()->writeOffset();
    this->close();
}

void Home_PrintDlg::on_closeBtn_clicked()
{
    this->close();
}

// void Home_PrintDlg::on_testPrintInfoBtn_clicked()
// {
//     sBarTend it;
//     it.fw = "V0.0.4-HCD";it.hw = "V0.0.4-HCD";it.pn = "J50002L00ACC090401";
//     bool ret = Printer_BarTender::bulid()->printerInfo(it);
//     qDebug()<<ret <<endl;
// }
