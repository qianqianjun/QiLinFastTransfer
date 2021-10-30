/*
 * @author qianqianjun
 * @date 2021.07
*/
#include "onlinedeviceitem.h"
// OnlineDeviceItem 相关的函数实现
OnlineDeviceItem::OnlineDeviceItem(DeviceInfo info,QWidget *parent) : QWidget(parent),info(info)
{
    QHBoxLayout* layout=new QHBoxLayout(parent);

    this->checkbox=new QCheckBox(this);
    this->checkbox->setMaximumWidth(40);

    this->deviceName=new QLabel(this);
    int fontSize = fontMetrics().horizontalAdvance(info.name);
    if( fontSize >=280 ){
        QString short_str = fontMetrics().elidedText(info.name, Qt::ElideRight, 300);
        this->deviceName->setText(short_str);
    }
    else this->deviceName->setText(info.name);
    this->deviceName->setMinimumWidth(300);
    this->deviceName->setAlignment(Qt::AlignCenter);

    this->ip=new QLabel(info.ip,this);
    this->ip->setMaximumWidth(240);
    this->ip->setAlignment(Qt::AlignCenter);

    this->port=new QLabel(QString::number(info.port),this);
    this->port->setMaximumWidth(100);
    this->port->setAlignment(Qt::AlignCenter);

    this->btn=new QPushButton("传文件",this);
    this->btn->setMaximumWidth(120);

    layout->addWidget(this->checkbox);
    layout->addWidget(this->deviceName);
    layout->addWidget(this->ip);
    layout->addWidget(this->port);
    layout->addWidget(this->btn);
    this->setLayout(layout);
    connect(this->btn,&QPushButton::clicked,this,&OnlineDeviceItem::openTransferWindow);
}
DeviceInfo OnlineDeviceItem::getDeviceInfo(){
    return this->info;
}
void OnlineDeviceItem::openTransferWindow(){
    QVector<DeviceInfo> infos;
    infos.push_back(info);
    SendFileWindow* sendFileWindow=new SendFileWindow(infos,nullptr);
    sendFileWindow->setAttribute(Qt::WA_DeleteOnClose);
    sendFileWindow->show();
}

//  Title 相关的函数实现

Title::Title(QVector<QString> heads,QWidget* parent):QWidget(parent),titles(heads){
    for(int i=0;i<heads.size();i++){
        this->labels.push_back(new QLabel(heads[i],this));
        this->labels[i]->setAlignment(Qt::AlignHCenter);
    }
    this->labels[0]->setMaximumWidth(40);
    this->labels[4]->setMaximumWidth(120);
    this->labels[3]->setMaximumWidth(100);
    this->labels[2]->setMaximumWidth(240);
    this->labels[1]->setMinimumWidth(300);
    QHBoxLayout* layout=new QHBoxLayout(parent);
    for(int i=0;i<labels.size();i++) layout->addWidget(labels[i]);
    this->setLayout(layout);
}

// DeviceManager 相关的函数实现
DeviceManager::DeviceManager(QPushButton* multi_transfer_btn,
                             QPushButton* select_all_btn,
                             QWidget*& topWidget,
                             QObject* parent):QObject(parent),
    multiTransferBtn(multi_transfer_btn),
    selectAllBtn(select_all_btn),topWidget(topWidget){
    connect(this->selectAllBtn,&QToolButton::clicked,this,&DeviceManager::selectAllDevices);
    connect(this->multiTransferBtn,&QToolButton::clicked,this,&DeviceManager::multiTransfer);
}

void DeviceManager::startAsking(DiscoveryService*& discoveryService){
    connect(&discoveryTimer, &QTimer::timeout, discoveryService, &DiscoveryService::broadcastReq);
    discoveryTimer.start(2000);
    discoveryService->broadcastReq();
}

void DeviceManager::freeOldWidget(){
    delete topWidget->layout();
    delete title;
    for(int i=0;i<onlineDeviceItems.size();i++){
        delete onlineDeviceItems[i];
    }
    onlineDeviceItems.clear();
}

void DeviceManager::addConnectionAction(){
    for(int i=0;i<this->onlineDeviceItems.size();i++){
        connect(this->onlineDeviceItems[i]->checkbox,&QCheckBox::clicked,this,&DeviceManager::updateSelectedDevice);
    }
}

void DeviceManager::updateSelectedDevice(){
    this->selectedDevices.clear();
    for(int i=0;i<onlineDeviceItems.size();i++){
        if(onlineDeviceItems[i]->checkbox->checkState()==Qt::Checked){
            this->selectedDevices.push_back(onlineDeviceItems[i]->getDeviceInfo());
        }
    }
}

QVector<DeviceInfo> DeviceManager::getSelectedDevices(){
    return this->selectedDevices;
}

void DeviceManager::selectAllDevices(){
    for(int i=0;i<this->onlineDeviceItems.size();i++){
        if(this->onlineDeviceItems[i]->checkbox->checkState()!=Qt::Checked){
            this->onlineDeviceItems[i]->checkbox->setCheckState(Qt::Checked);
        }
    }
    updateSelectedDevice();
}

void DeviceManager::multiTransfer(){
    if(this->selectedDevices.size()>0){
        openTransferWindow(this->selectedDevices);
    }else{
        QMessageBox::critical(nullptr, QApplication::applicationName(),
                             QString("请先选择要传输的设备！"));
    }
}

void DeviceManager::openTransferWindow(QVector<DeviceInfo> devices){
    SendFileWindow* sendFileWindow=new SendFileWindow(devices,nullptr);
    sendFileWindow->show();
}

QString ipv4Parser(QString str){
    if(str.startsWith("::ffff:")){
        QList<QString> arr=str.split("ffff:");
        return arr[arr.length()-1];
    }
    return str;
}
void DeviceManager::updateDeviceList(const QString &deviceName, const QHostAddress &addr, quint16 port){
    if(port==0){
        for(int i=0;i<onlineInfos.size();i++){
            if(onlineInfos[i].addr.isEqual(addr)){
                onlineInfos.remove(i);
                break;
            }
        }
        //检查一下是否刚好在选中的列表中，如果有的话，马上删了。
        for(int i=0;i<selectedDevices.size();i++){
            if(selectedDevices[i].addr.isEqual(addr)){
                selectedDevices.remove(i);
            }
        }
    }else{
        bool add=true;
        for(int i=0;i<onlineInfos.size();i++){
            if(onlineInfos[i].addr.isEqual(addr)){
                onlineInfos[i].name=deviceName;
                onlineInfos[i].port=port;
                add=false;
                break;
            }
        }
        if(add)
            onlineInfos.push_back(DeviceInfo(deviceName,ipv4Parser(addr.toString()),port,addr));
    }
    // 下面是更新UI的操作
    freeOldWidget();
    renderOnlinePage();
}

void DeviceManager::renderOnlinePage(){
    QVBoxLayout* layout=new QVBoxLayout();
    QVector<QString> heads;
    heads<<"选择"<<"设备名称"<<"Ip地址"<<"端口号"<<"操作";
    title=new Title(heads,topWidget);
    layout->addWidget(title);
    // 下面是添加设备的功能
    for(int i=0;i<onlineInfos.size();i++){
        OnlineDeviceItem* item=new OnlineDeviceItem(onlineInfos[i],topWidget);
        onlineDeviceItems.push_back(item);
        layout->addWidget(item);
    }
    addConnectionAction();
    layout->addStretch();
    topWidget->setLayout(layout);

    for(int i=0;i<this->onlineDeviceItems.size();i++){
        for(int j=0;j<this->selectedDevices.size();j++){
            if(this->onlineDeviceItems[i]->getDeviceInfo().ip==this->selectedDevices[j].ip){
                this->onlineDeviceItems[i]->checkbox->setCheckState(Qt::Checked);
                break;
            }
        }
    }
}
