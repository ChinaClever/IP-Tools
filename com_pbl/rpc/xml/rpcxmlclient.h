#ifndef RPCXMLCLIENT_H
#define RPCXMLCLIENT_H

#include "maia/maiaXmlRpcClient.h"

class RpcXmlClient : public QObject
{
    Q_OBJECT
public:
    explicit RpcXmlClient(QObject *parent = nullptr);

public slots:
    void doClient();

private slots:
    void testResponse(QVariant &);
    void testFault(int, const QString &);
    void handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    MaiaXmlRpcClient *rpc;
};

#endif // RPCXMLCLIENT_H
