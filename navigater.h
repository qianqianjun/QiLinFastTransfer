#ifndef NAVIGATER_H
#define NAVIGATER_H

#include <QObject>
#include <QLabel>
#include <QString>
#include <QWidget>
#include <QVector>
#include <QStackedWidget>
#include <QDebug>
#include <QMouseEvent>

class NavigaterLabel:public QLabel{
    Q_OBJECT
public:
    NavigaterLabel(int index,QString text="",QWidget* parent=nullptr,Qt::WindowFlags f=Qt::WindowFlags());
    ~NavigaterLabel();
protected:
    void mousePressEvent(QMouseEvent* event);
private:
    int index;
signals:
    void clicked(int index);
};

class Navigater:public QObject{
    Q_OBJECT
public:
    Navigater(QStackedWidget* stackedWidget,QVector<NavigaterLabel*> menus,QObject* parent=nullptr);
private:
    QVector<NavigaterLabel*> labels;
    int currentIndex;
    QStackedWidget* pages;
public slots:
    void selected(int index);
};

#endif // NAVIGATER_H
