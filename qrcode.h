#ifndef QRCODE_H
#define QRCODE_H

#include <QWidget>
#include "qrencode/qrencode.h"

class QrcodeWidget : public QWidget
{
    Q_OBJECT
private:
    int _size;
    int _margin;
    QString url;
    QByteArray _str;
public:
    explicit QrcodeWidget(QWidget *parent = nullptr);
    void setUrl(QString url);

protected:
    void paintEvent(QPaintEvent *event);

signals:

};

#endif // QRCODE_H
