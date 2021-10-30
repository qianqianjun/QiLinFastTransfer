#include "webserver.h"


RequestMapper::RequestMapper(QObject *parent):HttpRequestHandler(parent){
    QSettings* templeteSettings=getSettings("templates",this);
    templateCache=new TemplateCache(templeteSettings,this);
    QSettings* fileSettings=getSettings("static",this);
    staticFileController=new StaticFileController(fileSettings,this);
}

void RequestMapper::setStaicFileController(StaticFileController*& controller){
    delete staticFileController;
    staticFileController=controller;
}
void RequestMapper::service(HttpRequest &request, HttpResponse &response){
    QByteArray path=request.getPath();
    if (path.startsWith("/file")){
        FileUploadController(templateCache).service(request, response);
    }else{
        staticFileController->service(request, response);
    }
}

FileUploadController::FileUploadController(TemplateCache*& templateCache, QObject* parent):HttpRequestHandler(parent),templateCache(templateCache){

}

void FileUploadController::service(HttpRequest &request, HttpResponse &response)
{
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t=templateCache->getTemplate("upload",request.getHeader("Accept-Language"));
    t.enableWarnings();
    if (request.getParameter("action")=="show")
    {
        QTemporaryFile* file=request.getUploadedFile("data");
        QString fileName=request.getParameter("filename");
        if (file){
            QString timestrap=QDateTime::currentDateTime().toString("yyyy-MM-dd-hh.mm.ss_");
            QString storeName=Settings::downloadPath()+QDir::separator() + timestrap+fileName;
            QFile* storeFile=new QFile(storeName,this);

            if(storeFile->open(QIODevice::WriteOnly)){
                while (!file->atEnd() && !file->error()){
                    QByteArray buffer=file->read(65536);
                    storeFile->write(buffer);
                }
            }
            storeFile->close();
            storeFile->deleteLater();
            QDesktopServices::openUrl(QUrl::fromLocalFile(Settings::downloadPath()));

            QJsonObject responseText;
            responseText.insert("ok", true);
            responseText.insert("msg", QString("文件已保存在%1%2").arg(storeName).arg("！"));
            response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
        }else{
            QJsonObject responseText;
            responseText.insert("ok", false);
            responseText.insert("msg","文件上传失败！");
            response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
        }
    }
    else{
        response.write(t.toUtf8(),true);
    }
}


WebServer::WebServer(QObject *parent):QObject(parent)
{
    // request mapper
    mapper=new RequestMapper(this);
    // listener
    QSettings* listenSetings=getSettings("listener",this);
    listener=new HttpListener(listenSetings,mapper,this);
}

QString WebServer::openSender(QString ip,qint16 port,QString filePath)
{
    QFileInfo fileInfo(filePath);
    QString staticPath=fileInfo.absolutePath();
    QString resource=fileInfo.fileName();

    QSettings* fileSettings=getSettings("static",this);
    fileSettings->setValue("path",staticPath);
    StaticFileController* staticFileController=new StaticFileController(fileSettings,this);
    mapper->setStaicFileController(staticFileController);
    return QString("http://%1:%2/%3").arg(ip).arg(port).arg(resource);
}

QString WebServer::openReceiver(QString ip,qint16 port)
{
    return QString("http://%1:%2/file").arg(ip).arg(port);
}

WebServer::~WebServer()
{
    delete mapper;
    delete listener;
}
