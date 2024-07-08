#ifndef JSONRPCOBJ_H
#define JSONRPCOBJ_H

#include "jcon/json_rpc_tcp_client.h"
#include "jcon/json_rpc_tcp_server.h"
#include "jcon/json_rpc_websocket_client.h"
#include "jcon/json_rpc_websocket_server.h"

enum class SocketType {tcp, websocket};
class JsonRpcObj : public QObject
{
    Q_OBJECT
public:
    explicit JsonRpcObj(QObject *parent = nullptr);
    ~JsonRpcObj();
    bool startLocalServer(const QObjectList &services);
    bool startServer(const QObjectList &services, int port=6002,
                     SocketType socket_type = SocketType::tcp);

    bool startClient(const QString& host, int port=6002,
                     SocketType socket_type = SocketType::tcp);

    void enableNotification(bool allow=true);
    void close();

private:
    void initReceiveNotification();

protected:
    jcon::JsonRpcClient* rpc_client = nullptr;
private:
    jcon::JsonRpcServer* rpc_local = nullptr;
    jcon::JsonRpcServer* rpc_server = nullptr;
};

#endif // JSONRPCOBJ_H
