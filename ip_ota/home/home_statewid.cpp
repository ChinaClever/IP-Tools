/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_statewid.h"
#include "ui_home_statewid.h"


Home_StateWid::Home_StateWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_StateWid)
{
    ui->setupUi(this);  mCnt = 0;
    QTimer::singleShot(1350,this,SLOT(initFunSlot()));
    QGridLayout *gridLayout = new QGridLayout(parent);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(this);
}

Home_StateWid::~Home_StateWid()
{
    delete ui;
}

void Home_StateWid::initFunSlot()
{
    Core_Sender *sender = Core_Sender::bulid(this);
    connect(sender, &Core_Sender::upgradeError, this, &Home_StateWid::upgradeError);
    connect(sender, &Core_Sender::errorMessage, this, &Home_StateWid::insertSlot);
    connect(sender, &Core_Sender::finishSig, this, &Home_StateWid::finishSlot);
    connect(sender, &Core_Sender::throwSig, this, &Home_StateWid::insertSlot);
}

void Home_StateWid::startSlot()
{
    ui->textEdit->clear();  mCnt = 0;
}

void Home_StateWid::upgradeError(const QString &host, const QString &msg)
{
    QString str = host+"\t: "+msg;
    insertSlot(str);
}

void Home_StateWid::setTextColor(const QString &str)
{
    bool pass = true;
    if(str.contains("err") || str.contains("fail")) pass = false;
    if(str.contains("错误") || str.contains("失败")) pass = false;
    if(str.contains("continue")) pass = true;

    QColor color("black");
    if(!pass) color = QColor("red");
    if(str.contains("成功")) color = QColor("green");
    ui->textEdit->moveCursor(QTextCursor::Start);

    QTextCharFormat fmt;//文本字符格式
    fmt.setForeground(color);// 前景色(即字体色)设为color色
    QTextCursor cursor = ui->textEdit->textCursor();//获取文本光标
    cursor.mergeCharFormat(fmt);//光标后的文字就用该格式显示
    ui->textEdit->mergeCurrentCharFormat(fmt);//textEdit使用当前的字符格式
}

void Home_StateWid::insertSlot(const QString &str)
{
    if(str.size()>2) setTextColor(str);
    ui->textEdit->insertPlainText(str+"\n");

    //if(str.size()>2) ui->textEdit->appendPlainText(str);
    //QTextCursor cursor = ui->textEdit->textCursor();
    //cursor.movePosition(QTextCursor::End);
    //ui->textEdit->setTextCursor(cursor);
    if(mCnt++ > 500) ui->textEdit->clear();
}

void Home_StateWid::finishSlot(bool pass, const QString &msg)
{
    QString str;
    if(pass) {
        str = tr(" 升级文件上传成功，等待主机重启设备，重启后有响声！");
    } else {
        str = tr(" 升级失败！");
    } insertSlot(msg+str);
}

