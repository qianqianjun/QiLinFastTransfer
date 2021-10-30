#include "util.h"

util::util()
{

}
/**
 * @brief getSettings
 * @param group
 * @return
 */



QSettings* getSettings(QString group,QObject* parent){
    if(group=="listener"){
        QSettings* set=new QSettings(parent);
        set->beginGroup("listener");
        set->setValue("cleanupInterval",60000);
        qlonglong value=5368709120;
        set->setValue("maxMultiPartSize",value);
        set->setValue("maxRequestSize",16000);
        set->setValue("maxThreads",100);
        set->setValue("minThreads",4);
        set->setValue("port",Settings::WebPort());
        set->setValue("readTimeout",60000);
        return set;
    }
    if(group=="static"){
        QSettings* set=new QSettings(parent);
        set->beginGroup("static");
        set->setValue("cacheSize",1000000);
        set->setValue("cacheTime",60000);
        set->setValue("encoding","UTF-8");
        set->setValue("maxAge",60000);
        set->setValue("maxCachedFileSize",65536);
        return set;
    }
    if(group=="templates"){
        QString binDir=QCoreApplication::applicationDirPath();
        QSettings* set=new QSettings(parent);
        set->beginGroup("templates");
        set->setValue("cacheSize",1000000);
        set->setValue("cacheTime",60000);
        set->setValue("encoding","UTF-8");
        set->setValue("path",binDir+"/etc/templates");
        set->setValue("suffix",".tpl");
        return set;
    }
    qDebug()<<"can not find the settings!";
    return new QSettings(parent);
}

QString searchConfigFile(QString fileName)
{
    QString binDir=QCoreApplication::applicationDirPath();
    QString curDir=QDir::currentPath();
    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/etc");
    searchList.append(binDir+"/../etc");
    qDebug()<<"QString searchConfigFile(QString fileName) not use!";
    searchList.append("/Users/xueyucheng/Qt/QtProjects/QinLinTransfer/etc");
    searchList.append(QDir::rootPath()+"etc/opt");
    searchList.append(QDir::rootPath()+"etc");

    foreach (QString dir, searchList)
    {
        QFile file(dir+"/"+fileName);
        if (file.exists())
        {
            fileName=QDir(file.fileName()).canonicalPath();
            qDebug("Using config file %s",qPrintable(fileName));
            return fileName;
        }
    }

    // not found
    foreach (QString dir, searchList)
    {
        qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
    }
    qFatal("Cannot find config file %s",qPrintable(fileName));
    return nullptr;
}

QStringList getLocalHostIP()
{
  QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
  QStringList result;
  foreach(QHostAddress address, AddressList){
      if(address.protocol() == QAbstractSocket::IPv4Protocol &&
         address != QHostAddress::Null &&
         address != QHostAddress::LocalHost){
          result.push_back(address.toString());
      }
  }
  return result;
}
