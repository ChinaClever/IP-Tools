/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_mainwid.h"
#include "ui_home_mainwid.h"

Home_MainWid::Home_MainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_MainWid)
{
    ui->setupUi(this); //initWid();
    groupBox_background_icon(this);
    mWorkWid = new Home_WorkWid(ui->workWid);
    Core_Http *http = Core_Http::bulid(this);
    connect(http, &Core_Http::httpSig, this, &Home_MainWid::onMsg);
    connect(mWorkWid, &Home_WorkWid::startSig, this, &Home_MainWid::onStart);
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}


void Home_MainWid::initWid()
{
    QPalette pl = ui->textEdit->palette();  mId = 1;
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
}

void Home_MainWid::onStart()
{    
    ui->textEdit->clear(); mId = 1;
}


void Home_MainWid::setTextColor(const QString &str)
{
    bool pass = true;
    if(str.contains("err") || str.contains("fail")) pass = false;
    if(str.contains("错误") || str.contains("失败")) pass = false;

    QColor color("black");
    if(!pass) color = QColor("red");
    ui->textEdit->moveCursor(QTextCursor::Start);

    QTextCharFormat fmt;//文本字符格式
    fmt.setForeground(color);// 前景色(即字体色)设为color色
    QTextCursor cursor = ui->textEdit->textCursor();//获取文本光标
    cursor.mergeCharFormat(fmt);//光标后的文字就用该格式显示
    ui->textEdit->mergeCurrentCharFormat(fmt);//textEdit使用当前的字符格式
}


void Home_MainWid::onMsg(const QString &msg)
{    
    QString str = QString::number(mId++) + "、"+ msg + "\n";
    setTextColor(str); //ui->textEdit->moveCursor(QTextCursor::Start);
    ui->textEdit->insertPlainText(str);

    if(msg.contains("Download failed: Url")) {
        QString str = tr("文件下载错误：\n") + msg;
        MsgBox::critical(this, str); mId = 0;
    } else if(mId == 10 && msg.contains("logo.png")) {
        QString str = tr("配置文件下载成功\n");
        MsgBox::information(this, str);
    }
}
