#ifndef REMOTE_RECVWID_H
#define REMOTE_RECVWID_H

#include <QWidget>
#include <QUdpSocket>
#include <QHostAddress>

namespace Ui {
class Remote_RecvWid;
}

class Remote_RecvWid : public QWidget
{
    Q_OBJECT

public:
    explicit Remote_RecvWid(QWidget *parent = nullptr);
    ~Remote_RecvWid();

public slots:
    void startSlot();

private:
    void setTextColor(const QString &str);
    void insertText(const QString &dst, const QString &str);
    void upgradeResults(const QString &dst, const QString &str);

private slots:
    void udpRecvSlot();

private:
    Ui::Remote_RecvWid *ui;
    QUdpSocket *udpSocket;
    int mCnt; QStringList mErrIp;
};

#endif // REMOTE_RECVWID_H
