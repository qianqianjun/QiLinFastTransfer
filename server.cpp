#include <stdexcept>
#include "server.h"

Server::Server(QObject *parent) : QObject(parent) {}

void Server::bindListen(){
    quint16 port = Settings::serverPort();
    if (!qTcpServer.listen(QHostAddress::Any, port))
        throw std::runtime_error(QString("无法监听 %1 端口").arg(port).toUtf8().toStdString());
    connect(&qTcpServer, &QTcpServer::newConnection, this, &Server::handleNewConnection);
}

quint16 Server::port(){
    return qTcpServer.serverPort();
}

/**
 * 每当有一个新的TCP连接，则执行下面的功能
 * 该功能由start函数设置。
 */
void Server::handleNewConnection()
{
    while (qTcpServer.hasPendingConnections()) { // 检查连接队列中是否有等待的连接。
        // 从等待队列中取出连接，（socket）
        ReceiverContext* receiver = new ReceiverContext(qTcpServer.nextPendingConnection(),nullptr);
        ProgressBarUI* progressBar=new ProgressBarUI(receiver,nullptr);
        progressBar->setAttribute(Qt::WA_DeleteOnClose);
    }
}
