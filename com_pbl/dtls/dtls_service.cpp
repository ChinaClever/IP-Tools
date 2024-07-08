/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "dtls_service.h"

namespace {

QString peer_info(const QHostAddress &address, quint16 port)
{
    const static QString info = QStringLiteral("(%1:%2)");
    return info.arg(address.toString()).arg(port);
}

QString connection_info(QDtls *connection)
{
    QString info(Dtls_Service::tr("Session cipher: "));
    info += connection->sessionCipher().name();

    info += Dtls_Service::tr("; session protocol: ");
    switch (connection->sessionProtocol()) {
    case QSsl::DtlsV1_0:
        info += Dtls_Service::tr("DTLS 1.0.");
        break;
    case QSsl::DtlsV1_2:
        info += Dtls_Service::tr("DTLS 1.2.");
        break;
    case QSsl::DtlsV1_2OrLater:
        info += Dtls_Service::tr("DTLS 1.2 or later.");
        break;
    default:
        info += Dtls_Service::tr("Unknown protocol.");
    }

    return info;
}

} // unnamed namespace


Dtls_Service::Dtls_Service(QObject *parent) : QObject{parent}
{
    connect(&serverSocket, &QAbstractSocket::readyRead, this, &Dtls_Service::readyRead);
    serverConfiguration = QSslConfiguration::defaultDtlsConfiguration();
    serverConfiguration.setPreSharedKeyIdentityHint("DTLS service");
    serverConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);    
    serverConfiguration.setProtocol(QSsl::DtlsV1_2OrLater);
}

Dtls_Service::~Dtls_Service()
{
    shutdown();
}

bool Dtls_Service::listen(const QHostAddress &address, quint16 port)
{
    if (address != serverSocket.localAddress() || port != serverSocket.localPort()) {
        shutdown();
        listening = serverSocket.bind(address, port);
        if (!listening) emit errorMessage(serverSocket.errorString());
    } else {
        listening = true;
    }

    return listening;
}

bool Dtls_Service::isListening() const
{
    return listening;
}

void Dtls_Service::close()
{
    listening = false;
}

void Dtls_Service::readyRead()
{
    const qint64 bytesToRead = serverSocket.pendingDatagramSize();
    if (bytesToRead <= 0) {
        emit warningMessage(tr("A spurious read notification"));
        return;
    }

    QByteArray dgram(bytesToRead, Qt::Uninitialized);
    QHostAddress peerAddress;
    quint16 peerPort = 0;
    const qint64 bytesRead = serverSocket.readDatagram(dgram.data(), dgram.size(),
                                                       &peerAddress, &peerPort);
    if (bytesRead <= 0) {
        emit warningMessage(tr("Failed to read a datagram: ") + serverSocket.errorString());
        return;
    }

    dgram.resize(bytesRead);
    if (peerAddress.isNull() || !peerPort) {
        emit warningMessage(tr("Failed to extract peer info (address, port)"));
        return;
    }

    const auto client = std::find_if(knownClients.begin(), knownClients.end(),
                                     [&](const std::unique_ptr<QDtls> &connection){
        return connection->peerAddress() == peerAddress
                && connection->peerPort() == peerPort;
    });

    if (client == knownClients.end())
        return handleNewConnection(peerAddress, peerPort, dgram);

    if ((*client)->isConnectionEncrypted()) {
        decryptDatagram(client->get(), dgram);
        if ((*client)->dtlsError() == QDtlsError::RemoteClosedConnectionError)
            knownClients.erase(client);
        return;
    }

    doHandshake(client->get(), dgram);
}

void Dtls_Service::pskRequired(QSslPreSharedKeyAuthenticator *auth)
{
    Q_ASSERT(auth);

    emit infoMessage(tr("PSK callback, received a client's identity: '%1'")
                     .arg(QString::fromLatin1(auth->identity())));
    auth->setPreSharedKey(QByteArrayLiteral("\x1a\x2b\x3c\x4d\x5e\x6f"));
}

void Dtls_Service::handleNewConnection(const QHostAddress &peerAddress,
                                       quint16 peerPort, const QByteArray &clientHello)
{
    if (!listening)
        return;

    mClientHost = peerAddress;
    const QString peerInfo = peer_info(peerAddress, peerPort);
    if (cookieSender.verifyClient(&serverSocket, clientHello, peerAddress, peerPort)) {
        emit infoMessage(peerInfo + tr(": verified, starting a handshake"));
        std::unique_ptr<QDtls> newConnection{new QDtls{QSslSocket::SslServerMode}};
        newConnection->setDtlsConfiguration(serverConfiguration);
        newConnection->setPeer(peerAddress, peerPort);
        newConnection->connect(newConnection.get(), &QDtls::pskRequired,
                               this, &Dtls_Service::pskRequired);
        knownClients.push_back(std::move(newConnection));
        doHandshake(knownClients.back().get(), clientHello);
    } else if (cookieSender.dtlsError() != QDtlsError::NoError) {
        emit errorMessage(tr("DTLS error: ") + cookieSender.dtlsErrorString());
    } else {
        emit infoMessage(peerInfo + tr(": not verified yet"));
    }
}

void Dtls_Service::doHandshake(QDtls *newConnection, const QByteArray &clientHello)
{
    const bool result = newConnection->doHandshake(&serverSocket, clientHello);
    if (!result) {
        emit errorMessage(newConnection->dtlsErrorString());
        return;
    }

    const QString peerInfo = peer_info(newConnection->peerAddress(),
                                       newConnection->peerPort());
    switch (newConnection->handshakeState()) {
    case QDtls::HandshakeInProgress:
        emit infoMessage(peerInfo + tr(": handshake is in progress ..."));
        break;
    case QDtls::HandshakeComplete:
        emit infoMessage(tr("Connection with %1 encrypted. %2")
                         .arg(peerInfo, connection_info(newConnection)));
        break;
    default:
        Q_UNREACHABLE();
    }
}

void Dtls_Service::decryptDatagram(QDtls *connection, const QByteArray &clientMessage)
{
    Q_ASSERT(connection->isConnectionEncrypted());

    const QString peerInfo = peer_info(connection->peerAddress(), connection->peerPort());
    const QByteArray dgram = connection->decryptDatagram(&serverSocket, clientMessage);
    if (dgram.size()) {
        emit datagramReceived(qUncompress(dgram));
        connection->writeDatagramEncrypted(&serverSocket, tr("to %1: ACK").arg(peerInfo).toLatin1());
    } else if (connection->dtlsError() == QDtlsError::NoError) {
        emit warningMessage(peerInfo + ": " + tr("0 byte dgram, could be a re-connect attempt?"));
    } else {
        emit errorMessage(peerInfo + ": " + connection->dtlsErrorString());
    }
}

void Dtls_Service::shutdown()
{
    for (const auto &connection : knownClients)
        connection->shutdown(&serverSocket);

    serverSocket.close();
}
