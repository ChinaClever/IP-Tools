/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_workwid.h"
#include "ui_home_workwid.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDateTime>
#include <QSet>
#include "msgbox.h"
#include "xlsxdocument.h"

static QString excelNow()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

static QString excelCellText(QXlsx::Document &xlsx, int row, int col)
{
    return xlsx.read(row, col).toString().trimmed();
}

static bool excelCellIsIp(const QString &text)
{
    return cm_isIPaddress(text.trimmed());
}

Home_WorkWid::Home_WorkWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_WorkWid)
{
    ui->setupUi(this);
    mExcelArmed = mExcelAbort = mExcelSaved = false;
    ui->excelBtn->setHidden(true);
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
    mExcelArmed = mExcelAbort = mExcelSaved = false;
    ui->targetEdit->setHidden(true);
    ui->excelBtn->setHidden(true);
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
    if(mExcelArmed) {
        QString reason = msg.trimmed();
        if(reason.isEmpty()) reason = tr("未知原因");
        mExcelEnd.insert(host, excelNow());
        mExcelResult.insert(host, tr("升级失败：%1").arg(reason));
    }
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
    if(mExcelArmed) {
        mExcelEnd.insert(msg, excelNow());
        if(pass) mExcelResult.insert(msg, tr("升级完成"));
        else mExcelResult.insert(msg, tr("升级失败：文件上传失败"));
        if(!isStart) saveExcelResult(false);
    }
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
    if(mExcelArmed) saveExcelResult(true);
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
        ret = cm_isIPaddress(str);
        if(ret) ret = cm_pingNet(str);
        if(!ret) MsgBox::critical(this, tr("目标设备IP出错！"));
    } else if(2 == index) {
        if(mExcelFile.isEmpty() || mExcelIps.isEmpty())
            MsgBox::critical(this, tr("请先导入 Excel IP 列表。"));
        else if(!QFile::exists(mExcelFile))
            MsgBox::critical(this, tr("Excel 文件不存在。"));
        else ret = true;
    } else if(3 == index) {
        if(str.isEmpty()) MsgBox::critical(this, tr("机房名称，未指定。")); else ret = true;
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
    case 2: ips = mExcelIps; break;
    case 3: ips = mSsdp->searchRoom(dst); break;
    }

    return ips;
}

void Home_WorkWid::markExcelUpgrade()
{
    QString now = excelNow();
    mExcelAbort = false;
    mExcelSaved = false;
    mExcelArmed = true;
    mExcelStart.clear();
    mExcelEnd.clear();
    mExcelResult.clear();
    for(int i=0; i<mExcelIps.size(); ++i) {
        mExcelStart.insert(mExcelIps.at(i), now);
        mExcelResult.insert(mExcelIps.at(i), tr("升级中"));
    }
}

void Home_WorkWid::on_startBtn_clicked()
{
    if(isStart == false) {
        if(initWid()) {
            timer->start(500);
            QStringList ips = getIpList();
            int slave = ui->searchBox->currentIndex() == 2 ? 0 : mSsdp->getSlaveNum();
            if(ui->searchBox->currentIndex() == 2) markExcelUpgrade();
            else mExcelArmed = false;
            initCnt(ips.size(), slave);
            mSender->sendFile(ips, ui->fnLab->text(), mFileIt);
        }
    } else {
        bool ret = MsgBox::question(this, tr("确定需要提前结束？"));
        if(ret) { mExcelAbort = true; mResult = false; updateResult(); }
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
        QString str;
        int index = ui->searchBox->currentIndex();
        if(index == 2) {
            str = tr("已导入%1个IP，点击开始后按该列表升级").arg(res.size());
        } else {
            str = tr("已找到%1个设备，包含%2个主机，%3个副机")
                    .arg(res.size() + mSsdp->getSlaveNum())
                    .arg(res.size()).arg(mSsdp->getSlaveNum());
        }
        int total = index == 2 ? res.size() : res.size() + mSsdp->getSlaveNum();
        if(total < 50) {
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
    bool excel = index == 2;
    ui->excelBtn->setHidden(!excel);
    ui->targetEdit->setHidden(index == 0);
    ui->targetEdit->setReadOnly(excel);
    if(excel) {
        ui->targetEdit->setText(QFileInfo(mExcelFile).fileName());
        ui->targetEdit->setToolTip(mExcelFile);
        ui->targetEdit->setPlaceholderText(tr("请导入 Excel 文件"));
    } else {
        if(!mExcelFile.isEmpty() && ui->targetEdit->text() == QFileInfo(mExcelFile).fileName())
            ui->targetEdit->clear();
        ui->targetEdit->setToolTip(QString());
        if(index == 1) ui->targetEdit->setPlaceholderText(tr("请输入设备IP"));
        else if(index == 3) ui->targetEdit->setPlaceholderText(tr("请输入机房名称"));
        else ui->targetEdit->setPlaceholderText(QString());
    }
}


bool Home_WorkWid::loadExcelIps(const QString &fn)
{
    QXlsx::Document xlsx(fn);
    if(!xlsx.load()) {
        MsgBox::critical(this, tr("无法打开 Excel 文件。"));
        return false;
    }
    QStringList sheets = xlsx.sheetNames();
    if(!sheets.isEmpty()) xlsx.selectSheet(sheets.first());

    QXlsx::CellRange dim = xlsx.dimension();
    mExcelIps.clear();
    if(dim.isValid()) {
        QSet<QString> seen;
        for(int r=dim.firstRow(); r<=dim.lastRow(); ++r) {
            for(int c=dim.firstColumn(); c<=dim.lastColumn(); ++c) {
                QString text = excelCellText(xlsx, r, c);
                if(!excelCellIsIp(text)) continue;
                if(!seen.contains(text)) {
                    seen.insert(text);
                    mExcelIps << text;
                }
                break;
            }
        }
    }
    if(mExcelIps.isEmpty()) {
        MsgBox::critical(this, tr("Excel 中未找到 IP 地址。"));
        return false;
    }

    mExcelFile = fn;
    ui->targetEdit->setText(QFileInfo(fn).fileName());
    ui->targetEdit->setToolTip(fn);
    MsgBox::information(this, tr("已导入 %1 个 IP，可开始升级。").arg(mExcelIps.size()));
    return true;
}

static void excelShiftDown(QXlsx::Document &xlsx, int lastRow, int lastCol)
{
    for(int r=lastRow; r>=1; --r) {
        for(int c=1; c<=lastCol; ++c) {
            QVariant value = xlsx.read(r, c);
            if(!value.isNull()) xlsx.write(r + 1, c, value);
        }
    }
    for(int c=1; c<=lastCol; ++c)
        xlsx.write(1, c, QVariant());
}

static bool excelResultCols(QXlsx::Document &xlsx, int &colStart, int &colEnd, int &colResult)
{
    QXlsx::CellRange dim = xlsx.dimension();
    if(!dim.isValid()) return false;
    int lastCol = dim.lastColumn();
    colStart = colEnd = colResult = 0;
    for(int c=1; c<=lastCol; ++c) {
        QString text = excelCellText(xlsx, 1, c);
        if(text == QStringLiteral("升级开始时间")) colStart = c;
        else if(text == QStringLiteral("升级完成时间")) colEnd = c;
        else if(text == QStringLiteral("升级结果")) colResult = c;
    }
    if(colStart && colEnd && colResult) return true;

    bool rowHasIp = false;
    for(int c=dim.firstColumn(); c<=lastCol; ++c) {
        if(excelCellIsIp(excelCellText(xlsx, 1, c))) { rowHasIp = true; break; }
    }
    if(rowHasIp) excelShiftDown(xlsx, dim.lastRow(), lastCol);

    colStart = lastCol + 1;
    colEnd = lastCol + 2;
    colResult = lastCol + 3;
    xlsx.write(1, colStart, QStringLiteral("升级开始时间"));
    xlsx.write(1, colEnd, QStringLiteral("升级完成时间"));
    xlsx.write(1, colResult, QStringLiteral("升级结果"));
    xlsx.setColumnWidth(colStart, colEnd, 22);
    xlsx.setColumnWidth(colResult, 48);
    return true;
}

void Home_WorkWid::saveExcelResult(bool finalize)
{
    if(!mExcelArmed || mExcelFile.isEmpty()) return;
    if(finalize) {
        QString now = excelNow();
        for(int i=0; i<mExcelIps.size(); ++i) {
            const QString ip = mExcelIps.at(i);
            if(mExcelResult.value(ip) != tr("升级中")) continue;
            mExcelEnd.insert(ip, now);
            if(mExcelAbort) mExcelResult.insert(ip, tr("升级失败：已终止"));
            else mExcelResult.insert(ip, tr("升级失败：未完成"));
        }
    }

    QXlsx::Document xlsx(mExcelFile);
    if(!xlsx.load()) {
        MsgBox::critical(this, tr("无法写回 Excel：%1").arg(mExcelFile));
        return;
    }
    QStringList sheets = xlsx.sheetNames();
    if(!sheets.isEmpty()) xlsx.selectSheet(sheets.first());

    int colStart = 0, colEnd = 0, colResult = 0;
    if(!excelResultCols(xlsx, colStart, colEnd, colResult)) {
        MsgBox::critical(this, tr("Excel 中没有可写入的数据。"));
        return;
    }

    QXlsx::CellRange dim = xlsx.dimension();
    for(int r=dim.firstRow(); r<=dim.lastRow(); ++r) {
        for(int c=dim.firstColumn(); c<=dim.lastColumn(); ++c) {
            QString text = excelCellText(xlsx, r, c);
            if(!excelCellIsIp(text) || !mExcelResult.contains(text)) continue;
            xlsx.write(r, colStart, mExcelStart.value(text));
            xlsx.write(r, colEnd, mExcelEnd.value(text));
            xlsx.write(r, colResult, mExcelResult.value(text));
            break;
        }
    }

    if(!xlsx.save()) {
        MsgBox::critical(this, tr("升级结果写入 Excel 失败，请确认文件未被占用。"));
        return;
    }
    if(!mExcelSaved) {
        mExcelSaved = true;
        insertTextSlot(true, tr("升级结果已写入 %1").arg(QFileInfo(mExcelFile).fileName()));
    }
}

void Home_WorkWid::on_excelBtn_clicked()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString fn = QFileDialog::getOpenFileName(this, tr("选择IP列表"), dir, tr("Excel 文件(*.xlsx)"));
    if(fn.isEmpty()) return;
    loadExcelIps(fn);
}

