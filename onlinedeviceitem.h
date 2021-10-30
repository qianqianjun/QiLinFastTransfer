#ifndef ONLINEDEVICEITEM_H
#define ONLINEDEVICEITEM_H
/*
 * @author qianqianjun
 * @date 2021.07
*/
#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <sendfilewindow.h>
#include <discoveryservice.h>
#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <util.h>

class OnlineDeviceItem : public QWidget{
    Q_OBJECT
private:
    QLabel* deviceName;
    QLabel* ip;
    QLabel* port;
    DeviceInfo info;
public:
    QCheckBox * checkbox;
    QPushButton* btn;
    explicit OnlineDeviceItem(DeviceInfo info,QWidget *parent = nullptr);
    DeviceInfo getDeviceInfo();
    void openTransferWindow();
};


class Title:public QWidget{
    Q_OBJECT
private:
    QVector<QString> titles;
    QVector<QLabel*> labels;
public:
    explicit Title(QVector<QString> heads,QWidget* parent=nullptr);
};

class DeviceManager:public QObject{
    Q_OBJECT
private:
    QVector<DeviceInfo> onlineInfos;
    QVector<DeviceInfo> selectedDevices;
    QVector<OnlineDeviceItem*> onlineDeviceItems;
    Title* title;
    QPushButton* multiTransferBtn;
    QPushButton* selectAllBtn;
    QWidget* topWidget;
    QTimer discoveryTimer;
public:
    DeviceManager(QPushButton* mult_transfer_btn,QPushButton* select_all_btn,QWidget*& topWidget,
                  QObject* parent=nullptr);
    QVector<DeviceInfo> getSelectedDevices();
    void addConnectionAction();
    void updateDeviceList(const QString &deviceName, const QHostAddress &addr, quint16 port);
    void freeOldWidget();
    void startAsking(DiscoveryService*& discoveryService);
    void renderOnlinePage();
public slots:
    void updateSelectedDevice();
    void selectAllDevices();
    void multiTransfer();
    void openTransferWindow(QVector<DeviceInfo> devices);
};

#endif // ONLINEDEVICEITEM_H
