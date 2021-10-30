#pragma once

#include <QString>
#include <QApplication>
#include <QDir>
#include <QHostInfo>
#include <QSettings>
#include <QStandardPaths>
class Settings {
public:
    static QString deviceName();
    static QString downloadPath();
    static bool discoverable();
    static quint16 serverPort();
    static quint16 DiscoveryPort();
    static quint16 WebPort();
    static void setDeviceName(const QString &deviceName);
    static void setDownloadPath(const QString &downloadPath);
    static void setDiscoverable(bool discoverable);
    static void setServerPort(quint16 serverPort);
    static void setDiscoveryPort(quint16 discoveryPort);
    static void setWebPort(quint16 webPort);
};
