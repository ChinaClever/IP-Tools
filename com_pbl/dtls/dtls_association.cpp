/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "dtls_association.h"

Dtls_Association::Dtls_Association(const QString &address, quint16 port)
    : name(address), crypto(QSslSocket::SslClientMode)
{
    auto configuration = QSslConfiguration::defaultDtlsConfiguration();
    configuration.setPeerVerifyMode(QSslSocket::VerifyNone);
    configuration.setProtocol(QSsl::DtlsV1_2OrLater);
    crypto.setDtlsConfiguration(configuration);
    crypto.setPeer(QHostAddress(address), port);

    connect(&socket, &QAbstractSocket::connected, this, &Dtls_Association::udpSocketConnected);
    connect(&crypto, &QDtls::handshakeTimeout, this, &Dtls_Association::handshakeTimeout);
    connect(&crypto, &QDtls::pskRequired, this, &Dtls_Association::pskRequired);
    connect(&socket, &QUdpSocket::readyRead, this, &Dtls_Association::readyRead);
    socket.connectToHost(address, port);
}

Dtls_Association::~Dtls_Association()
{
    if (crypto.isConnectionEncrypted())
        crypto.shutdown(&socket);
    socket.close();
}

bool Dtls_Association::startHandshake()
{
    if (socket.state() != QAbstractSocket::ConnectedState) {
        emit infoMessage(tr("%1: connecting UDP socket first ...").arg(name));
        return crypto.isConnectionEncrypted();
    }

    if (!crypto.doHandshake(&socket)) {
        emit errorMessage(tr("%1: failed to start a handshake - %2")
                          .arg(name, crypto.dtlsErrorString()));
    } else {
        emit infoMessage(tr("%1: starting a handshake").arg(name));

    }

    return waitForConnected();
}

bool Dtls_Association::waitForConnected()
{
    for(int i=0; i<1000; ++i) if(crypto.isConnectionEncrypted()) break; else cm_mdelay(1);
    return socket.waitForConnected(1000);
}

void Dtls_Association::udpSocketConnected()
{
    emit infoMessage(tr("%1: UDP socket is now in ConnectedState, continue with handshake ...").arg(name));
    cm_mdelay(3); startHandshake();
}

void Dtls_Association::readyRead()
{
    if (socket.pendingDatagramSize() <= 0) {
        emit warningMessage(tr("%1: spurious read notification?").arg(name));
        return;
    }

    QByteArray dgram(socket.pendingDatagramSize(), Qt::Uninitialized);
    const qint64 bytesRead = socket.readDatagram(dgram.data(), dgram.size());
    if (bytesRead <= 0) {
        emit warningMessage(tr("%1: spurious read notification?").arg(name));
        return;
    }

    dgram.resize(bytesRead);
    if (crypto.isConnectionEncrypted()) {
        const QByteArray plainText = crypto.decryptDatagram(&socket, dgram);
        if (plainText.size()) {
            if(plainText.contains("ACK")) isRecved = true;
            emit serverResponse(name, plainText);
            return;
        }

        if (crypto.dtlsError() == QDtlsError::RemoteClosedConnectionError) {
            emit errorMessage(tr("%1: shutdown alert received").arg(name));
            socket.close();
            return;
        }

        emit warningMessage(tr("%1: zero-length datagram received?").arg(name));
    } else {
        if (!crypto.doHandshake(&socket, dgram)) {
            emit errorMessage(tr("%1: handshake error - %2").arg(name, crypto.dtlsErrorString()));
            return;
        }
    }
}

void Dtls_Association::handshakeTimeout()
{
    emit warningMessage(tr("%1: handshake timeout, trying to re-transmit").arg(name));
    if (!crypto.handleTimeout(&socket))
        emit errorMessage(tr("%1: failed to re-transmit - %2").arg(name, crypto.dtlsErrorString()));
}

void Dtls_Association::pskRequired(QSslPreSharedKeyAuthenticator *auth)
{
    Q_ASSERT(auth);

    emit infoMessage(tr("%1: providing pre-shared key ...").arg(name));
    auth->setIdentity(name.toLatin1());
    auth->setPreSharedKey(QByteArrayLiteral("\x1a\x2b\x3c\x4d\x5e\x6f"));
}

bool Dtls_Association::writeData(const QByteArray &array)
{
    isRecved = false; cm_mdelay(3);
    if (waitForConnected()) {
        //qDebug() << array.size() << qCompress(array).size();
        const qint64 written = crypto.writeDatagramEncrypted(&socket, qCompress(array));
        if (written > 0) {
            for(int i=0; i<1000; ++i) if(isRecved) break; else cm_mdelay(1);
        } else emit errorMessage(tr("%1: failed to send a ping - %2")
                                 .arg(name, crypto.dtlsErrorString()));
    }

    return isRecved;
}

