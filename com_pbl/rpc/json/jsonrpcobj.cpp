/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "jsonrpcobj.h"

JsonRpcObj::JsonRpcObj(QObject *parent)
    : QObject{parent}
{

}

JsonRpcObj::~JsonRpcObj()
{
    if(rpc_local) rpc_local->close();
    close();
}

void JsonRpcObj::enableNotification(bool allow)
{
    if(rpc_client) rpc_client->enableReceiveNotification(allow);
    if(rpc_server) rpc_server->enableSendNotification(allow);
}

void JsonRpcObj::close()
{
    if(rpc_client) rpc_client->disconnectFromServer();
    if(rpc_server) rpc_server->close();
}

// "ws://127.0.0.1:6002"
bool JsonRpcObj::startClient(const QString &host, int port, SocketType socket_type)
{
    if(rpc_client) {close(); delete rpc_client;}
    if (socket_type == SocketType::tcp) {
        rpc_client = new jcon::JsonRpcTcpClient(this);
    } else {
        rpc_client = new jcon::JsonRpcWebSocketClient(this);
    }

    initReceiveNotification();
    bool ret = rpc_client->connectToServer(host, port);
    if(ret) enableNotification(true);
    return ret;
}

// 处理不请自来的通知
void JsonRpcObj::initReceiveNotification()
{
    connect(rpc_client, &jcon::JsonRpcClient::notificationReceived,
            this, [](const QString& key, const QVariant& value){
        qDebug() << "Received notification:"
                 << "Key:" << key
                 << "Value:" << value;
    });
}

bool JsonRpcObj::startServer(const QObjectList& services, int port, SocketType socket_type)
{
    if(rpc_server) {close(); delete rpc_server;}
    if (socket_type == SocketType::tcp) {
        rpc_server = new jcon::JsonRpcTcpServer(this);
    } else {
        rpc_server = new jcon::JsonRpcWebSocketServer(this);
    }
    rpc_server->enableSendNotification(true);
    rpc_server->registerServices(services);
    return rpc_server->listen(port);
}

bool JsonRpcObj::startLocalServer(const QObjectList &services)
{
    rpc_local = new jcon::JsonRpcTcpServer(this);
    rpc_local->enableSendNotification(true);
    rpc_local->registerServices(services);
    return rpc_local->listen(QHostAddress::LocalHost, 9224);
}
