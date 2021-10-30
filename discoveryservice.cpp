#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkInterface>

#include "discoveryservice.h"
#include "settings.h"

DiscoveryService::DiscoveryService(QObject *parent) : QObject(parent)
{
    connect(&socket, &QUdpSocket::readyRead, this, &DiscoveryService::handleDatagrams);
}

void DiscoveryService::bindListen(quint16 serverPort)
{
    this->fileTransferPort = serverPort;
    if (!socket.bind(QHostAddress::Any, DISCOVERY_PORT)) {
        QMessageBox::warning(nullptr, QApplication::applicationName(),
                             QString("无法监听端口 %1.\n你的设备无法被发现")
                             .arg(DISCOVERY_PORT));
    }
    foreach (const QHostAddress &addr, getBroadcastAddr()) {
        sendDatagram(addr, DISCOVERY_PORT);
    }
}

void DiscoveryService::broadcastReq()
{
    QJsonObject obj;
    obj.insert("request", true);
    QByteArray json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    foreach (const QHostAddress &addr, getBroadcastAddr()) {
        socket.writeDatagram(json, addr, DISCOVERY_PORT);
    }
}

void DiscoveryService::sendDatagram(const QHostAddress &addr, quint16 port)
{
    QJsonObject obj;
    obj.insert("request", false);
    obj.insert("device_name", Settings::deviceName());
    obj.insert("device_type", QSysInfo::productType());
    obj.insert("port", Settings::discoverable() ? fileTransferPort : 0);
    socket.writeDatagram(QJsonDocument(obj).toJson(QJsonDocument::Compact), addr, port);
}

bool DiscoveryService::isLocalAddress(const QHostAddress &addr)
{
    /**
     * QHostAddress("127.0.0.1")
     * QHostAddress("::1")
     * QHostAddress("192.168.31.34")
     * QHostAddress("fe80::968f:462f:ba6a:c89b%wlp4s0")
     */
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (addr.isEqual(address))
             return true;
    }
    return false;
}
/**
 * 获取局域网的广播地址，向局域网内的所有主机进行广播。
 * @return  广播地址数组
 */
QList<QHostAddress> DiscoveryService::getBroadcastAddr()
{
    QList<QHostAddress> ret;
    ret.append(QHostAddress::Broadcast);
    foreach (const QNetworkInterface &i, QNetworkInterface::allInterfaces()) {
        if (i.flags() & QNetworkInterface::CanBroadcast) {
            foreach (const QNetworkAddressEntry &e, i.addressEntries()) {
                ret.append(e.broadcast());
            }
        }
    }
    return ret;
}

/**
 * @brief
 * 监听端口，随时响应其他设备的问询请求(request=true)
 * 收到其他包的回应信息，更新在线设备列表(request=false)
 */
void DiscoveryService::handleDatagrams()
{
    //qDebug()<<"socketReadyRead running!";
    while (socket.hasPendingDatagrams()) {
        //qDebug()<<"while running!";
        qint64 size = socket.pendingDatagramSize();
        QByteArray data(size, 0);
        QHostAddress addr;
        quint16 port;
        socket.readDatagram(data.data(), size, &addr, &port);

        if (isLocalAddress(addr))
            continue;

        QJsonDocument json = QJsonDocument::fromJson(data);
        if (!json.isObject())
            continue;
        QJsonObject obj = json.object();
        QJsonValue request = obj.value("request");
        if (!request.isBool())
            continue;
        if (request.toBool()) {
             sendDatagram(addr, port);
            continue;
        }
        QJsonValue deviceName = obj.value("device_name");
        QJsonValue remotePort = obj.value("port");
        if (!deviceName.isString() || !remotePort.isDouble())
            continue;
        QString deviceNameStr = deviceName.toString();
        quint16 remotePortInt = remotePort.toInt();
        emit newHost(deviceNameStr, addr, remotePortInt);
    }
}

void DiscoveryService::leave(){
    foreach (const QHostAddress &addr, getBroadcastAddr()) {
        QJsonObject obj;
        obj.insert("request", false);
        obj.insert("device_name", Settings::deviceName());
        obj.insert("device_type", QSysInfo::productType());
        obj.insert("port", 0);
        socket.writeDatagram(QJsonDocument(obj).toJson(QJsonDocument::Compact), addr, DISCOVERY_PORT);
    }
}
