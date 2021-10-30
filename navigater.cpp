#include "navigater.h"

NavigaterLabel::~NavigaterLabel(){}

NavigaterLabel::NavigaterLabel(int index,QString text,QWidget* parent,Qt::WindowFlags f):QLabel(text,parent,f),index(index){}
void NavigaterLabel::mousePressEvent(QMouseEvent *event){
    event->accept();
    emit clicked(this->index);
}

Navigater::Navigater(QStackedWidget* stackedWidget,QVector<NavigaterLabel*> menus,QObject* parent):QObject(parent),labels(menus),currentIndex(0),pages(stackedWidget){
    for(int i=0;i<this->labels.size();i++){
        this->labels[i]->setMargin(5);
        connect(this->labels[i],&NavigaterLabel::clicked,this,&Navigater::selected);
    }
    pages->setCurrentIndex(0);
    labels[0]->setStyleSheet("QLabel{background-color:rgb(17,150,238);color:rgb(255,255,255);}");
}

void Navigater::selected(int index){
    if(currentIndex!=index){
        currentIndex=index;
        for(int i=0;i<labels.size();i++){
            labels[i]->setStyleSheet("QLabel{background-color:rgb(255,255,255);color:rgb(0,0,0);}");
        }
        labels[index]->setStyleSheet("QLabel{background-color:rgb(17,150,238);color:rgb(255,255,255);}");
        pages->setCurrentIndex(index);
    }
}
