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

    mEditTabWid = new QPlainTextEdit(ui->tabWidget);
    ui->tabWidget->insertTab(0, mEditTabWid, tr("调试日志"));

    mLineTabWid = new Home_LineTabWid(ui->tabWidget);
    ui->tabWidget->insertTab(1, mLineTabWid, tr("相监测数据列表"));
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}


void Home_MainWid::initWid()
{
    QPalette pl = mEditTabWid->palette();  mId = 1;
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    mEditTabWid->setPalette(pl);
}

void Home_MainWid::onStart()
{    
    mEditTabWid->clear(); mId = 1;
    sThreshold *it = &Core_Object::coreItem.actual.value;
    for(int i=0; i<3; ++i) it->lineVol[i] = 0;
}


void Home_MainWid::setTextColor(const QString &str)
{
    bool pass = true;
    if(str.contains("err") || str.contains("fail")) pass = false;
    if(str.contains("错误") || str.contains("失败")) pass = false;

    QColor color("black");
    if(!pass) color = QColor("red");
    mEditTabWid->moveCursor(QTextCursor::Start);

    QTextCharFormat fmt;//文本字符格式
    fmt.setForeground(color);// 前景色(即字体色)设为color色
    QTextCursor cursor = mEditTabWid->textCursor();//获取文本光标
    cursor.mergeCharFormat(fmt);//光标后的文字就用该格式显示
    mEditTabWid->mergeCurrentCharFormat(fmt);//textEdit使用当前的字符格式
}


void Home_MainWid::onMsg(const QString &msg)
{
    if(msg.contains("Download failed: Url") && msg.contains(".pem")) return;
    if(msg.size() > 2048) Core_Object::coreItem.jsonPacket = msg;
    QString str = QString::number(mId++) + "、"+ msg + "\n";
    setTextColor(str); //mEditTabWid->moveCursor(QTextCursor::Start);
    mEditTabWid->insertPlainText(str);

    if(msg.contains("Download failed: Url")) {
        QString str = tr("文件下载错误：\n") + msg;
        MsgBox::critical(this, str); mId = 0;
    } else if(msg.contains("Download completed:") && msg.contains("cfg.ini")) {
        QString str = tr("配置文件下载成功\n");
        MsgBox::information(this, str);
    }
}
