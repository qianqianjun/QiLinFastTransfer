#include "webreceive.h"
#include "ui_webreceive.h"

WebReceive::WebReceive(WebServer*& webServer,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::webreceive),havaQrcode(false),webServer(webServer)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowTitle("接收文件");
    QStringList ips=getLocalHostIP();
    for(int i=0;i<ips.size();i++){
        ui->ips_combox->addItem(ips[i]);
    }
}

void WebReceive::removeOld()
{
    if(havaQrcode){
        delete ui->qrcode_area->layout();
        delete qrcodeWidget;
    }
}

WebReceive::~WebReceive()
{
    delete ui;
}


void WebReceive::on_pushButton_clicked()
{
    removeOld();
    havaQrcode=true;
    QVBoxLayout *layout=new QVBoxLayout(this);
    qrcodeWidget=new QrcodeWidget(this);

    QString url=webServer->openReceiver(ui->ips_combox->currentText(),Settings::WebPort());
    qrcodeWidget->setUrl(url);

    layout->addWidget(qrcodeWidget);
    ui->qrcode_area->setLayout(layout);
    update();
}

