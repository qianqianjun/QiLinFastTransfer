#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <QHostAddress>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QSettings>
#include <settings.h>
typedef struct data{
    QString name;
    QString ip;
    quint16 port;
    QHostAddress addr;
    data(QString name,QString ip,qint16 port,QHostAddress addr):name(name),ip(ip),port(port),addr(addr){}
}DeviceInfo;

QString searchConfigFile(QString fileName);
QStringList getLocalHostIP();
QSettings* getSettings(QString group,QObject* parent);
class util
{
public:
    util();
};

typedef enum S {
    TRANSFER_KEY,
    TRANSFER_META,
    TRANSFER_DATA,
    TRANSFER_FINISH
}State;

typedef struct metadata{
    QString filename;
    quint64 size;
} FileMetadata;


#endif // UTIL_H
