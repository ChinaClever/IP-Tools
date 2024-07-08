#ifndef DTLS_ASSOCIATION_H
#define DTLS_ASSOCIATION_H

#include <QtNetwork>
#include <QtCore>
#include "print.h"

class Dtls_Association : public QObject
{
    Q_OBJECT

public:
    Dtls_Association(const QString &address, quint16 port=15601);
    ~Dtls_Association();

    bool startHandshake();
    bool waitForConnected();
    bool writeData(const QByteArray &array);
    QString getName() {return name;}

signals:
    void errorMessage(const QString &message);
    void warningMessage(const QString &message);
    void infoMessage(const QString &message);
    void serverResponse(const QString &clientInfo, const QByteArray &plainText);

private slots:
    void udpSocketConnected();
    void readyRead();
    void handshakeTimeout();
    void pskRequired(QSslPreSharedKeyAuthenticator *auth);

private:
    QString name;
    QUdpSocket socket;
    QDtls crypto;
    bool isRecved;
    Q_DISABLE_COPY(Dtls_Association)
};

#endif // DTLS_ASSOCIATION_H
