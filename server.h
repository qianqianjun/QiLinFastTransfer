#pragma once

#include <QObject>
#include <QTcpServer>
#include <settings.h>
#include <context.h>
#include <progressBarui.h>

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void bindListen();
     quint16 port();
private:
    QTcpServer qTcpServer;
private slots:
    void handleNewConnection();
};
