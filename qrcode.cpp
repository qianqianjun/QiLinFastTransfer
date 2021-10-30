#include "qrcode.h"
#include <QString>
#include <QPainter>
#include <QBrush>
#include <QDebug>

QrcodeWidget::QrcodeWidget(QWidget *parent) : QWidget(parent)
{

}

void QrcodeWidget::setUrl(QString url)
{
    this->url=url;
    update();
}

void QrcodeWidget::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    QBrush brush(Qt::black);
    paint.setBrush(brush);
    QRcode* mqrcode=QRcode_encodeString(this->url.toUtf8().data(),0,QR_ECLEVEL_Q,QR_MODE_8,true);
    if(mqrcode!=NULL){
        this->_size=(this->width()-50)/mqrcode->width;
        this->_margin=(this->width()/2)-(mqrcode->width*_size)/2;
        unsigned char* poin=mqrcode->data;
        for(int x=0;x<mqrcode->width;x++){
            for(int y=0;y<mqrcode->width;y++){
                if(*poin &1){
                    QRectF r(x*_size+_margin,y*_size+_margin,_size,_size);
                    paint.drawRect(r);
                }
                poin++;
            }
        }
    }
}
