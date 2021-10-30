#pragma once
#include <QMenu>
#include <QSystemTrayIcon>
#include "discoveryservice.h"
#include "server.h"
#include "mainui.h"
#include "templateengine/templatecache.h"
#include "httpserver/staticfilecontroller.h"
#include "httpserver/httplistener.h"
using namespace stefanfrings;
class QiLinTray : public QSystemTrayIcon {
    Q_OBJECT
public:
    explicit QiLinTray(QApplication*& a,QObject *parent = nullptr);
public slots:
    void showOnlineDeviceWindow();
    void showSettingWindow();
    void onWindowClose();
private:
    QApplication* app;
    QMenu menu;
    Server fileTransferServer;
    DiscoveryService discoveryService;
    MainUI* mainui;
    bool haveUi;
private slots:
    void openDownloadFolder(); // 点击下载目录执行操作
    void exitApplication(); // 点击退出执行操作
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
};
