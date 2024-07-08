/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "rpcxmlclient.h"

RpcXmlClient::RpcXmlClient(QObject *parent)
    : QObject{parent}
{
    //	rpc = new MaiaXmlRpcClient(QUrl("http://phpxmlrpc.sourceforge.net/server.php"), this);
    //	rpc = new MaiaXmlRpcClient(QUrl("https://rpc.gandi.net/xmlrpc/2.0/"), this);
    //  rpc = new MaiaXmlRpcClient(QUrl("http://localhost:8082/RPC2"), this);
    // rpc = new MaiaXmlRpcClient(QUrl("http://localhost:8082"), this);
    rpc = new MaiaXmlRpcClient(QUrl("http://192.168.1.32:6082"), this);

    QSslConfiguration config = rpc->sslConfiguration();
    config.setProtocol(QSsl::AnyProtocol);
    rpc->setSslConfiguration(config);

    connect(rpc, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)),
            this, SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));
    doClient();
}


void RpcXmlClient::doClient() {
    QVariantList args; args << 0<< 0;
    rpc->call("pduMetaData", args,
              this, SLOT(testResponse(QVariant &)),
              this, SLOT(testFault(int, const QString &)));
}


void RpcXmlClient::testResponse(QVariant &arg) {
    qDebug() << arg.toString();
}

void RpcXmlClient::testFault(int error, const QString &message) {
    qDebug() << "EEE:" << error << "-" << message;
}


void RpcXmlClient::handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors) {
    qDebug() << "SSL Error:" << errors;
    reply->ignoreSslErrors(); // don't do this in real code! Fix your certs!
}
