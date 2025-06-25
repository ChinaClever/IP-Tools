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
    connect(this, &Home_MainWid::setDevSig, [&]{mWorkWid->devSetBtn();});

    mLineTabWid = new Home_LineTabWid(ui->tabWidget);
    ui->tabWidget->insertTab(0, mLineTabWid, tr("相监测数据列表"));

    mThTabWid = new Home_ThresholdTabWid(ui->tabWidget);
    ui->tabWidget->insertTab(1, mThTabWid, tr("报警阈值信息"));

    mEnvTabWid = new Home_EnvTabWid(ui->tabWidget);
    ui->tabWidget->insertTab(2, mEnvTabWid, tr("传感器环境状态"));

    mEdit = new QPlainTextEdit(ui->tabWidget);
    ui->tabWidget->insertTab(2, mEnvTabWid, tr("调试日志"));
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}


void Home_MainWid::initWid()
{
    QPalette pl = mEdit->palette();  mId = 1;
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    mEdit->setPalette(pl);
}

void Home_MainWid::onStart()
{
    mId = 1; emit startSig(); mEdit->clear();
    sPdudata *obj = & Core_Object::coreItem.actual.value;
    obj->lineCur.clear(); obj->linePow.clear();
    obj->lineVol.clear();
}


void Home_MainWid::setTextColor(const QString &str)
{
    bool pass = true;
    if(str.contains("err") || str.contains("fail")) pass = false;
    if(str.contains("错误") || str.contains("失败")) pass = false;
    if(str.size() > 2048) pass = true;

    QColor color("black");
    if(!pass) color = QColor("red");
    mEdit->moveCursor(QTextCursor::Start);

    QTextCharFormat fmt;//文本字符格式
    fmt.setForeground(color);// 前景色(即字体色)设为color色
    QTextCursor cursor = mEdit->textCursor();//获取文本光标
    cursor.mergeCharFormat(fmt);//光标后的文字就用该格式显示
    mEdit->mergeCurrentCharFormat(fmt);//textEdit使用当前的字符格式
}


void Home_MainWid::onMsg(const QString &msg)
{
    if(msg.size() > 2048) Core_Object::coreItem.jsonPacket = msg;
    QString str = QString::number(mId++) + "、"+ msg + "\n";
    setTextColor(str); //ui->textEdit->moveCursor(QTextCursor::Start);
    mEdit->insertPlainText(str);
    //ui->textEdit->moveCursor(QTextCursor::Start);
    //ui->textEdit->insertPlainText(str);
}
