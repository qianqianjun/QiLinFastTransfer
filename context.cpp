#include "context.h"

Context::Context(QTcpSocket* socket,QObject *parent) : QObject(parent){
    this->state=TRANSFER_KEY;
    this->socket=socket;
    totalSize=0;finishedSize=0;
    socket->setParent(this);
    socket->setSocketOption(QAbstractSocket::LowDelayOption,1);

    // 连接socket事件和处理函数
    connect(socket,&QTcpSocket::readyRead,this,&Context::receiveSocketData);
    connect(socket,&QTcpSocket::errorOccurred,this,&Context::handleSocketError);
}

void Context::exchange_key(){
    emit printLogMsg("设备握手中……");
    socket->write(crypto.localPublicKey());
}

void Context::sendData(const QByteArray &data)
{
    QByteArray ciphertext=crypto.encrypt(data);
    quint16 size=ciphertext.size();
    // 将数据块的大小信息写进传送数据中
    ciphertext.prepend(static_cast<quint8>(size & 0xFF));
    ciphertext.prepend(static_cast<quint8>((size >>8) & 0xFF));
    // 通过socket发送数据
    socket->write(ciphertext);
}
void Context::receiveSocketData(){
    readBuffer += socket->readAll();
    if (state == TRANSFER_KEY) {
        if (static_cast<quint64>(readBuffer.size()) < crypto.publicKeySize()) {
            emit raiseErrorMsg(QString("握手失败！"));
            return;
        }
        QByteArray publicKey = readBuffer.left(crypto.publicKeySize());
        readBuffer = readBuffer.mid(crypto.publicKeySize());
        try {
            crypto.setRemotePublicKey(publicKey);
        } catch (const std::exception &e) {
            emit raiseErrorMsg(e.what());
            return;
        }
        emit printLogMsg(QString("建立连接中，验证码: %1").arg(crypto.sessionKeyDigest()));
        state = TRANSFER_META;
        transferFileMetaData(); // 这个函数需要在子类中实现
    }
    // 处理接收到的数据
    while (!readBuffer.isEmpty()) {
        if (readBuffer.size() < 2)
            break;

        // 这里决定数据块的大小最大为65536
        quint16 size = static_cast<quint16>(static_cast<quint8>(readBuffer[0])) << 8;
        size |= static_cast<quint8>(readBuffer[1]);
        if (readBuffer.size() < size + 2)
            break;

        QByteArray data = readBuffer.mid(2, size);
        readBuffer = readBuffer.mid(size + 2);

        try {
            data = crypto.decrypt(data);
        } catch (const std::exception &e) {
            emit raiseErrorMsg(e.what());
            return;
        }
        handleReceivedData(data);
    }
}



void Context::handleSocketError()
{
    if(state!=TRANSFER_FINISH)
        emit raiseErrorMsg(socket->errorString());
}
/**
 * 下面是Sender中的相关操作：
*/

/**
 * @brief 创建一个发送端的Context
 * @param socket
 * @param files
 * @param parent
 */
SenderContext::SenderContext(QTcpSocket *socket,
                             const QList<QSharedPointer<QFile>>& fileList,
                             QObject *parent):Context(socket,parent),files(fileList){
    connect(socket,&QTcpSocket::bytesWritten,this,&SenderContext::writeByteToSocket);
    foreach(const QSharedPointer<QFile>& file,files){
        QString filename = QFileInfo(*file).fileName();
        quint64 size = static_cast<quint64>(file->size());
        totalSize += size;
        transferQueue.push_back({filename, size});
        // 文件读指针移动到开头
        file->seek(0);
    }
}

void SenderContext::writeByteToSocket(){
    if(state!=TRANSFER_DATA || socket->bytesToWrite()>0)
        return;

    // 将传送完成的文件从传送队列中删除，开启新传送。
    while(!transferQueue.empty()){
        auto &curFile=transferQueue.front();
        if(curFile.size==0){
            transferQueue.pop_front();
            files.pop_front();
            canPrintLog=true;
        }else{
            if(canPrintLog){
                emit printLogMsg(QString("正在传送文件 %1 ……").arg(curFile.filename));
                canPrintLog=false;
            }
            break;
        }
    }
    // 如果传送队列为空，则正常结束传送，开启下一个设备的传送！
    if(transferQueue.empty()){
        state=TRANSFER_FINISH;
        emit printLogMsg("传输完成！");
        socket->disconnectFromHost();
        // 这里设置在raiseEndMsg之后进行下一个连接的打开。
        QTimer::singleShot(1000,this,&Context::raiseEndSignal);
        return;
    }
    // 这里是传送数据的操作：
    QSharedPointer<QFile> &file=files.front();
    FileMetadata &metaData=transferQueue.front();
    // 注意，这里每次读取的数据块大小不能超过65535，因为数据块的大小采用的是quint16类型存储。
    QByteArray data=file->read(64000);
    sendData(data);
    metaData.size-=data.size();
    finishedSize+=data.size();
    emit updateProgressBar(static_cast<double>(finishedSize) / totalSize);
}

// 传送完公钥后，发送端要传送文件列表等信息给接收端
void SenderContext::transferFileMetaData()
{
    QJsonArray files;
    foreach(FileMetadata data,transferQueue){
        QJsonObject file;
        file.insert("filename",data.filename);
        file.insert("size",static_cast<qint64>(data.size));
        files.append(file);
    }
    QJsonObject package;
    package.insert("device_name",Settings::deviceName());
    package.insert("device_type",QSysInfo::productType());
    package.insert("files",files);
    sendData(QJsonDocument(package).toJson(QJsonDocument::Compact));
}

/**
 * @brief 处理接收到的消息，对Sender来说，要接收的消息只有公钥和接收端是否同意接收文件的结果。
 * 接收公钥的处理在父类中已经实现，这里仅处理在TRANSFER_META阶段传回来的接收端是否同意接收文件的消息。
 * @param data
 */
void SenderContext::handleReceivedData(const QByteArray &data)
{
    if(state==TRANSFER_META){
        try {
            QJsonDocument res=QJsonDocument::fromJson(data);
            QJsonObject resContent=res.object();
            QJsonValue resValue=resContent.value("response");
            // 如果接收端不同意接收文件，直接结束传送。
            if(resValue.toInt()==0){
                emit raiseErrorMsg("接收设备拒绝接收文件！");
                return;
            }
        }  catch (const std::exception& e) {
            emit raiseErrorMsg("文件meta响应解析错误！");
        }
        state=TRANSFER_DATA;
        // 开始发送文件！
        writeByteToSocket();
    }
}

void SenderContext::response(bool accept){
    qDebug()<<"SenderContext::response: "<<accept;
}

/**
 * 下面是 Receiver 中的相关操作：
*/
ReceiverContext::ReceiverContext(QTcpSocket *socket, QObject *parent):Context(socket,parent),savingFile(nullptr){}

/**
 * @brief 根据传输队列和传送进度，做创建新文件，删除已完成文件指针，全部结束传送后断开连接等操作。
 */
void ReceiverContext::prepareReceiveNextFile(){
    QString savePath=Settings::downloadPath();
    while(!transferQueue.empty()){
        FileMetadata &fileInfo=transferQueue.front();
        // 保存文件name加上时间戳，防止文件被覆盖。
        //QString fileName=savePath+QDir::separator()+fileInfo.filename;
        QString timestrap=QDateTime::currentDateTime().toString("yyyy-MM-dd-hh.mm.ss_");
        QString fileName=savePath+QDir::separator() + timestrap+fileInfo.filename;

        if(savingFile){
            savingFile->deleteLater();
            savingFile=nullptr;
        }
        savingFile=new QFile(fileName,this);
        if(!savingFile->open(QIODevice::WriteOnly)){
            emit raiseErrorMsg(QString("无法打开%1，请检查目录权限！").arg(fileName));
            return;
        }
        if(fileInfo.size>0){
            emit printLogMsg(QString("正在接收%1……").arg(fileName));
            break;
        }
        transferQueue.pop_front();
    }
    // 如果传输队列为空，则表示传输已经完成
    if(transferQueue.empty()){
        if(savingFile){
            savingFile->deleteLater();
            savingFile=nullptr;
        }
        state=TRANSFER_FINISH;
        // 打开下载目录，断开连接，窗口五秒后自动关闭
        QDesktopServices::openUrl(QUrl::fromLocalFile(savePath));
        emit printLogMsg("传输完成！");
        socket->disconnectFromHost();
        QTimer::singleShot(5000,this,&Context::raiseEndSignal);
    }
}

/**
 * @brief 父类的纯虚函数，子类必须实现，不过这里确实没用
 */
void ReceiverContext::transferFileMetaData(){
    // 接收端不需要传送metadata
}

/**
 * @brief 接收端处理接收到的数据，包括TRANSFER_META阶段和TRANSFER_DATA阶段的处理操作
 * @param data
 */
void ReceiverContext::handleReceivedData(const QByteArray &data){
    // 处理并解析发送端传过来的metaData信息。
    if(state==TRANSFER_META){
        // 这里传送的时候可能会出现问题，要把操作包含在try块中防止程序崩溃
        QString deviceName;
        try {
            QJsonDocument doc=QJsonDocument::fromJson(data);
            QJsonObject obj=doc.object();
            deviceName=obj.value("device_name").toString();
            QJsonValue filesValue=obj.value("files");
            QJsonArray files=filesValue.toArray();
            foreach(const QJsonValue &file,files){
                QJsonObject fileObj=file.toObject();
                QJsonValue filename=fileObj.value("filename");
                QJsonValue size=fileObj.value("size");
                quint64 IntSize=static_cast<quint64>(size.toDouble());
                totalSize+=IntSize;
                transferQueue.push_back({filename.toString(),IntSize});
            }
        } catch (const std::exception &e) {
            emit raiseErrorMsg(e.what());
            return;
        }
        // 下面的信号和传送的UI界面进行关联，用户的操作将会被记录并传回到response函数中。
        emit metaDataReady(transferQueue,totalSize,deviceName,crypto.sessionKeyDigest());
    }
    // 处理 Sender 发送来的文件数据块。
    if(state==TRANSFER_DATA){
        finishedSize+=data.size();
        emit updateProgressBar(static_cast<double>(finishedSize) / totalSize);
        QByteArray temp=data; // 这个data的size最大为65535，可能包括很多个文件。
        while(temp.size()>0){
            FileMetadata &meta=transferQueue.front();
            // 要取数据块剩余大小以及当前文件剩余未传送大小两者的最小值来决定读取的字节数目
            quint64 sizeToWrite=qMin(meta.size,static_cast<quint64>(temp.size()));
            qint64 writeSize=savingFile->write(temp.left(sizeToWrite));

            meta.size-=writeSize; // 更新当前文件剩余未传的大小
            temp=temp.mid(writeSize); // 删掉已经保存的数据
            // 如果当前文件已经传送完毕，则把该文件从传送队列中移除，并开始新文件的传送。
            if(meta.size==0){
                transferQueue.pop_front();
                prepareReceiveNextFile();
            }
        }
    }
    return;
}
/**
 * @brief 用户操作UI组件，传过来是否接收文件的决定
 * @param accept 是否接收文件
 */
void ReceiverContext::response(bool accept){
    QString savePath=Settings::downloadPath();
    QJsonObject res;
    res.insert("response",static_cast<int>(accept));
    sendData(QJsonDocument(res).toJson(QJsonDocument::Compact));
    if(accept){
        if(!QDir().mkpath(savePath)){
            emit raiseErrorMsg(QString("没有权限创建以下目录：%1……").arg(savePath));
            return;
        }
        if(!QFileInfo(savePath).isWritable()){
            emit raiseErrorMsg(QString("没有权限写入下载目录！"));
            return;
        }
        state=TRANSFER_DATA;
        prepareReceiveNextFile();
    }else{
        connect(socket,&QTcpSocket::bytesWritten,this,&Context::raiseEndSignal);
    }
}
