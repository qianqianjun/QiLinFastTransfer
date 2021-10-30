#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>
#include <navigater.h>
#include <QLayout>
#include <QPushButton>
#include <onlinedeviceitem.h>
#include <QSpacerItem>
#include <QCloseEvent>
#include <QNetworkInterface>
#include <discoveryservice.h>
#include <settings.h>
#include <webserver.h>
namespace Ui {
class MainUI;
}

class MainUI : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainUI(DiscoveryService* discoverService,QWidget *parent = nullptr);
    ~MainUI();
    void setPageIndex(int index);
signals:
    void closeWindow();
public slots:
    void settingSave();
    void settingCancle();
    void chooseDownloadDir();
    void openTransferWindow();
    void openReceiverWindow();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Navigater* navigater;
    Ui::MainUI *ui;
    DeviceManager* manager;
    DiscoveryService* discoverService;
    WebServer* webServer;

    void initWebPage();
    void initialLeftMenu();
    void initUserInfo();
    void initSettingPage();
};

#endif // MAINUI_H
