/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_mainwid.h"
#include "ui_home_mainwid.h"
#include <QFileDialog>

Home_MainWid::Home_MainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_MainWid)
{
    ui->setupUi(this); //initWid();
    groupBox_background_icon(this); mSsdp = Ssdp_Core::bulid(this);
    connect(mSsdp, &Ssdp_Core::sendMsgSig, this, &Home_MainWid::onDown);
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}

void Home_MainWid::webLogin()
{
    sCfgItem it; it.type = 14;
    QString v = ui->usrEdit->text();
    if(v.size()) {
        it.fc = 1;
        send(it, v);
    }

    v = ui->pwdEdit->text();
    if(v.size()) {
        it.fc =2;
        send(it, v);
    }
}

void Home_MainWid::initWid()
{
    QPalette pl = ui->textEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
}


void Home_MainWid::onDown(const QString &msg)
{
    QString str = QString::number(++mId) + "、"+ msg + "\n";
    ui->textEdit->moveCursor(QTextCursor::Start);
    ui->textEdit->insertPlainText(str);
}

void Home_MainWid::on_findBtn_clicked()
{
    QStringList ips = mSsdp->searchAll();
    QString str = tr("未找到任何目标设备");
    if(ips.size()) {
        str = tr("已找到%1个设备，包含%2个主机，%3个副机\t\n")
                  .arg(ips.size() + mSsdp->getSlaveNum())
                  .arg(ips.size()).arg(mSsdp->getSlaveNum());
        int i=0; foreach(const auto &ip, ips) {
            str += "   " + ip; i++;
            if(i%4==0) str += "\n";
        }
    }
    MsgBox::information(this, str);
}

void Home_MainWid::send(const sCfgItem &it, const QVariant &v)
{
    QString room, ip; //getInput(room, ip);
    if(ui->checkBox->isChecked()) ip = ui->dstEdit->text();
    mSsdp->setCfg(it, v, room, ip);
}

void Home_MainWid::reboot()
{
    sCfgItem it; it.type = 111;
    QString cmd = "reboot";
    if(ui->resetCheck->isChecked()) {
        if(ui->batchCheck->isChecked()) cm_mdelay(465);
        cm_mdelay(655); send(it, cmd);
    }
}

void Home_MainWid::on_startBtn_clicked()
{
    ui->textEdit->clear(); mId = 0;
    if(ui->groupBox_1->isEnabled()) webLogin();
    if(ui->groupBox_2->isEnabled()) devMode();
    if(ui->groupBox_3->isEnabled()) location();
    if(ui->groupBox_4->isEnabled()) netAddr();
    if(ui->groupBox_5->isEnabled()) integrate();
    setDateTime(); setWorkDown(); reboot();
    incrementAddr();
}

void Home_MainWid::devMode()
{
    int v = ui->modeBox->currentIndex();
    sCfgItem it; if(v<3) {
        it.type = 13;
        it.fc = 3;
        send(it, v);
        it.fc = 4;
    } else {
        it.type = 15;
        it.fc = 1;
        send(it, 1);
        it.fc = 2;
    }

    v = ui->addrBox->value(); send(it, v);
}

void Home_MainWid::incrementAddr()
{
    int mode = ui->modeBox->currentIndex();
    if(mode) {
        int v = ui->addrBox->value();
        v +=1; if(mode <3 && v>=10) {
            v = 0; incrementIPAddress();
        } ui->addrBox->setValue(v);
    } else incrementIPAddress();
}

void Home_MainWid::incrementIPAddress()
{
    QString ipAddress = ui->ipEdit->text();
    QHostAddress address(ipAddress);

    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        quint32 ipv4 = address.toIPv4Address();
        ipv4 += 1;
        address.setAddress(ipv4);
    } else if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        Q_IPV6ADDR ipv6 = address.toIPv6Address();
        // 加 1 操作
        for (int i = 15; i >= 0; --i) {
            if (ipv6[i] < 255) {
                ipv6[i] += 1;
                break;
            } else {
                ipv6[i] = 0;
            }
        }
        address.setAddress(ipv6);
    } else {
        // 不支持的地址类型
    }

    ipAddress = address.toString();
    ui->ipEdit->setText(ipAddress.remove("::ffff:"));
}


void Home_MainWid::setDateTime()
{
    sCfgItem it; it.type = 43;  it.fc = 1;
    QDateTime dateTime = QDateTime::currentDateTime();
    QString v = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    if(ui->timeCheckBox->isChecked()) send(it, v);
}

void Home_MainWid::location()
{
    sCfgItem it; it.type = 11;
    QString v = ui->roomEdit->text();
    if(v.size()) {
        it.fc = 1;
        send(it, v);
    }

    v = ui->locatEdit->text();
    if(v.size()) {
        it.fc = 2;
        send(it, v);
    }
}

void Home_MainWid::netAddr()
{
    sCfgItem it; it.type = 41; it.fc = 1;
    int id = ui->dhcpBox->currentIndex();
    send(it, id); if(id == 0) {
        QString v = ui->ipEdit->text();
        if(v.size()) {it.fc = 2; send(it, v);}

        v = ui->maskEdit->text();
        if(v.size()) {it.fc = 3; send(it, v);}

        v = ui->gwEdit->text();
        if(v.size()) {it.fc = 4; send(it, v);}

        v = ui->dnsEdit->text();
        if(v.size()) {it.fc = 6; send(it, v);}
    }
}

void Home_MainWid::integrate()
{
    sCfgItem it;
    it.type = 16; it.fc = 1;
    int v = ui->snmpBox->currentIndex();
    send(it, v);

    it.type = 15; it.fc = 11;
    v = ui->modeBox->currentIndex();
    send(it, v);

    it.type = 17; it.fc = 1;
    v = ui->rpcBox->currentIndex();
    send(it, v);

    it.type = 18;
    v = ui->pushBox->currentIndex();
    if(v == 1) {
        it.fc = 2; send(it, ui->hostEdit->text());
        it.fc = 3; send(it, ui->portEdit->text());
        it.fc = 1; v = 1;
    } else v = 0;
    send(it, v);

    v = ui->ctrlBox->currentIndex();
    it.fc = 8; send(it, v);
}

QByteArray Home_MainWid::readFile(const QString &fn)
{
    QFile file(fn); QByteArray res;
    bool ret = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(ret) res = file.readAll();
    else cout << file.errorString();
    file.close();
    return res;
}

bool Home_MainWid::checkFile(const QByteArray &msg)
{
    QJsonObject obj;
    QJsonParseError jsonerror; bool ret = false;
    QJsonDocument doc = QJsonDocument::fromJson(msg, &jsonerror);
    if (!doc.isNull() && jsonerror.error == QJsonParseError::NoError) {
        if(doc.isObject()) {
            obj = doc.object();
            ret = true;
        }
    }
    return ret;
}


void Home_MainWid::setWorkDown()
{
    QString dir = ui->pathEdit->text();
    QString room, ip; //getInput(room, ip);
    if(ui->checkBox->isChecked()) ip = ui->dstEdit->text();
    if(dir.size() && ui->batchCheck->isChecked()) {
        QStringList ls = File::entryList(dir);
        foreach(const auto &f, ls) {
            QString fn = dir + "/" + f;
            QByteArray res = readFile(fn);
            if(checkFile(res)) mSsdp->setJson(res, room, ip);
        }
    }
}

void Home_MainWid::on_dhcpBox_currentIndexChanged(int index)
{
    ui->ipEdit->setDisabled(index);
    ui->maskEdit->setDisabled(index);
    ui->gwEdit->setDisabled(index);
    ui->dnsEdit->setDisabled(index);
}

void Home_MainWid::on_pushBox_currentIndexChanged(int index)
{
    bool en = false; if(index == 1) en = true;
    ui->hostEdit->setEnabled(index);
    ui->portEdit->setEnabled(en);
}


void Home_MainWid::on_modeBox_currentIndexChanged(int index)
{
    ui->addrBox->setEnabled(index);
}


void Home_MainWid::on_checkBox_stateChanged(int arg1)
{
    ui->dstEdit->setEnabled(arg1);
}

void Home_MainWid::on_batchCheck_clicked(bool checked)
{
    ui->pathEdit->setEnabled(checked);
    ui->selectBtn->setEnabled(checked);
}

void Home_MainWid::on_selectBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->pathEdit->setText(dir);
}

void Home_MainWid::setEnableWid(bool en)
{
    ui->groupBox_1->setEnabled(en);
    ui->groupBox_2->setEnabled(en);
    ui->groupBox_3->setEnabled(en);
    ui->groupBox_4->setEnabled(en);
    ui->groupBox_5->setEnabled(en);
}


void Home_MainWid::on_rangeBox_currentIndexChanged(int index)
{
    if(index) {
        setEnableWid(false); switch (index) {
        case 1: ui->groupBox_1->setEnabled(true); break;
        case 2: ui->groupBox_2->setEnabled(true); break;
        case 3: ui->groupBox_3->setEnabled(true); break;
        case 4: ui->groupBox_4->setEnabled(true); break;
        case 5: ui->groupBox_5->setEnabled(true); break;
        }
    } else setEnableWid(true);
}

