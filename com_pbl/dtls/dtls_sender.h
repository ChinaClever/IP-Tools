#ifndef DTLS_SENDER_H
#define DTLS_SENDER_H

#include "dtls_recver.h"

class Dtls_Sender : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Dtls_Sender(QObject *parent = nullptr);
    bool sendFile(const QString &ip, const QString &fn, const sOtaFile &it);
    void sendData(const QString &ip, const sOtaFile &it, const QByteArray &data);
    static void setRunState(bool run) {gRunState=run;}

signals:
    void errorMessage(const QString &);
    void subProgress(const QString &,int);
    void infoMessage(bool,const QString &);
    void finishSig(bool, const QString &);
    void throwSig(const QString &);

public slots:
    void run() override;
    void throwMessage(const QString &message);

private:
    bool workDown(const QString &host);
    bool writeData(Dtls_Association *dtls);
    void startNewConnection(const QString &address);

private:
    QString mHost;
//    CThread *mThread;
    QByteArray mHead;
    QByteArray mArray;
    Dtls_Association *mDtls;
    static bool gRunState;
};

#endif // DTLS_SENDER_H
