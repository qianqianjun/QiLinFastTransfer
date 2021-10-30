#pragma once

#include <QObject>
#include <QUdpSocket>

class DiscoveryService : public QObject {
    Q_OBJECT
public:
    explicit DiscoveryService(QObject *parent = nullptr);
    void bindListen(quint16 serverPort);
public slots:
    void broadcastReq();
    void leave();
private:
    enum {
        DISCOVERY_PORT = 52637
    };
    QUdpSocket socket;
    quint16 fileTransferPort;
    void sendDatagram(const QHostAddress &addr, quint16 port);
    bool isLocalAddress(const QHostAddress &addr);
    QList<QHostAddress> getBroadcastAddr();
private slots:
    void handleDatagrams();
signals:
    void newHost(const QString &deviceName, const QHostAddress &addr, quint16 port);
};
