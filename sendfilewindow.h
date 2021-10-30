#ifndef SENDFILEWINDOW_H
#define SENDFILEWINDOW_H
#include <QDialog>
#include <onlinedeviceitem.h>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QList>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTimer>
#include <util.h>
#include <context.h>
#include <progressBarui.h>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QTimer>

namespace Ui {
class sendFileWindow;
}

typedef struct finfo{
    QString fileName;
    QString fileSize;
    finfo(QString name,QString size):fileName(name),fileSize(size){}
}FileInfo;

class SendFileManager:public QObject{
    Q_OBJECT
public:
    QList<FileInfo> fileInfos;
    QVector<DeviceInfo> targetDevices;
    QList<QSharedPointer<QFile>> files;
    int selectedIndex;
    SendFileManager(QVector<DeviceInfo> targetDevices,QObject* parent=nullptr);
    void addFile(const QString& filename);
    void removeFile();
    QString parseSize(qint64 size);
public slots:
    void changeIndex(int row,int col);
};

class SendFileWindow : public QDialog{
    Q_OBJECT
private:
    Ui::sendFileWindow *ui;
    QVector<DeviceInfo> targetDevices;
    SendFileManager* manager;
    QTcpSocket *socket;
    QTimer socketTimeoutTimer;
    QTimer closeTimer;
    QMessageBox hintBox;
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
public:
    explicit SendFileWindow(QVector<DeviceInfo> infos,QWidget *parent = nullptr);
    ~SendFileWindow();
    void initTargetDevice();
    void initSelectedFileArea();
    void renderselectedFiles();
private slots:
    void socketConnected();
    // void socketErrorOccurred();
    void socketTimeout();
    void sendFile();
};
#endif // SENDFILEWINDOW_H
