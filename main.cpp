#include <stdexcept>

#include <QApplication>
#include <QMessageBox>

#include "qilintray.h"

#include<QNetworkInterface>
#include <QDebug>
#include <QUdpSocket>
#include <mainui.h>
void test(){
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("麒麟快传");
    a.setOrganizationDomain("www.xjtu.edu.cn");
    a.setApplicationName("麒麟快传");
    a.setApplicationVersion("1.0.0");

    a.setQuitOnLastWindowClosed(false);

    try {
        if (!QSystemTrayIcon::isSystemTrayAvailable())
            throw std::runtime_error(a.translate("Main", "您的系统不支持托盘运行模式！")
                                     .toUtf8().toStdString());
        QApplication* app=&a;
        QiLinTray t(app);
        t.show();
        return a.exec();
    } catch (const std::exception &e) {
        // 窗口的父组件，窗口的标题，窗口显示的内容。
        QMessageBox::critical(nullptr, QApplication::applicationName(), e.what());
        return 1;
    }
}
