#ifndef DTLS_SERVICE_H
#define DTLS_SERVICE_H
#include <vector>
#include <memory>
#include "dtls_association.h"

class Dtls_Service : public QObject
{
    Q_OBJECT

public:
    Dtls_Service(QObject *parent = nullptr);
    ~Dtls_Service();

    QHostAddress clientHost() {return mClientHost;}
    bool listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 15601);
    bool isListening() const;
    void close();

signals:
    void errorMessage(const QString &message);
    void warningMessage(const QString &message);
    void infoMessage(const QString &message);
    void datagramReceived(const QByteArray &data);

private slots:
    void readyRead();
    void pskRequired(QSslPreSharedKeyAuthenticator *auth);

private:
    void handleNewConnection(const QHostAddress &peerAddress, quint16 peerPort,
                             const QByteArray &clientHello);

    void doHandshake(QDtls *newConnection, const QByteArray &clientHello);
    void decryptDatagram(QDtls *connection, const QByteArray &clientMessage);
    void shutdown();

    bool listening = false;
    QUdpSocket serverSocket;

    QHostAddress mClientHost;
    QSslConfiguration serverConfiguration;
    QDtlsClientVerifier cookieSender;
    std::vector<std::unique_ptr<QDtls>> knownClients;
};

#endif // DTLS_SERVICE_H
