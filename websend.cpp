#include "websend.h"
#include "ui_websend.h"
WebSend::WebSend(WebServer*& webServer,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::websend),havaQrcode(false),webServer(webServer)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowTitle("发送文件");
    ui->lineEdit->setEnabled(false);

    QStringList ips=getLocalHostIP();
    for(int i=0;i<ips.size();i++){
        ui->ip_combox->addItem(ips[i]);
    }
}

void WebSend::removeOld()
{
    if(havaQrcode){
        delete ui->qrcode_area->layout();
        delete qrcodeWidget;
    }
}

WebSend::~WebSend()
{
    delete ui;
}

void WebSend::on_pushButton_clicked()
{
    QString path=QFileDialog::getOpenFileName(this,"打开文件","./");
    if(!path.isEmpty()){
        ui->lineEdit->setText(path);
        filePath=path;
    }
}


void WebSend::on_pushButton_2_clicked()
{
    if(filePath.isEmpty()){
        QMessageBox::critical(this,"错误","请选择文件");
        return;
    }
    removeOld();
    havaQrcode=true;
    QVBoxLayout *layout=new QVBoxLayout(this);
    qrcodeWidget=new QrcodeWidget(this);
    QString url=webServer->openSender(ui->ip_combox->currentText(),Settings::WebPort(),filePath);
    qrcodeWidget->setUrl(url);
    layout->addWidget(qrcodeWidget);
    ui->qrcode_area->setLayout(layout);
    update();
}
