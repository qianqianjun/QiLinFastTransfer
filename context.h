#ifndef CONTEXT_H
#define CONTEXT_H
/**
 * @author: write by qianqinajun
 * @date: 2021.06
* */

#include <QObject>
#include <QTcpSocket>
#include <util.h>
#include <crypto.h>
#include <QVector>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QDesktopServices>

class Context : public QObject
{
    Q_OBJECT
private slots:
    void receiveSocketData();
    void handleSocketError();
protected:
    State state;
    QTcpSocket* socket;
    Crypto crypto;
    QByteArray readBuffer;
    QVector<FileMetadata> transferQueue;
    quint64 totalSize;
    quint64 finishedSize;
    virtual void handleReceivedData(const QByteArray &data)=0;
    virtual void transferFileMetaData()=0;
public:
    explicit Context(QTcpSocket* socket,QObject *parent = nullptr);
    void exchange_key();
    void sendData(const QByteArray& data);
    virtual void response(bool accept)=0;
    // 下面的这些信号都和UI界面的提示相关，在UI类中被连接到对应的槽
signals:
    void printLogMsg(const QString& msg);
    void updateProgressBar(double progress);
    void raiseErrorMsg(const QString& msg);
    void raiseEndSignal();
    void metaDataReady(const QVector<FileMetadata> &metadata,quint64 totalSize,
                       const QString& deviceName,const QString &keyDigit);
};

// sender的相关定义
class SenderContext:public Context{
    Q_OBJECT
private:
    QList<QSharedPointer<QFile>> files;
    bool canPrintLog;
private slots:
    void writeByteToSocket();
protected:
    void transferFileMetaData();
    void handleReceivedData(const QByteArray& data);
public:
    explicit SenderContext(QTcpSocket* socket,
                           const QList<QSharedPointer<QFile>>& fileList,
                           QObject *parent = nullptr);
    void response(bool accept);
};

// receiver的相关定义
class ReceiverContext:public Context{
    Q_OBJECT
private:
    QFile* savingFile;
    void prepareReceiveNextFile();
protected:
    void transferFileMetaData();
    void handleReceivedData(const QByteArray& data);
public:
    explicit ReceiverContext(QTcpSocket* socket,QObject *parent = nullptr);
    void response(bool accept);
};

#endif // CONTEXT_H
