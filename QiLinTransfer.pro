QT += core gui widgets network

CONFIG += c++11

SOURCES += \
    context.cpp \
    crypto.cpp \
    discoveryservice.cpp \
    main.cpp \
    mainui.cpp \
    navigater.cpp \
    onlinedeviceitem.cpp \
    progressBarui.cpp \
    qilintray.cpp \
    qrcode.cpp \
    qrencode/bitstream.c \
    qrencode/mask.c \
    qrencode/mmask.c \
    qrencode/mqrspec.c \
    qrencode/qrencode.c \
    qrencode/qrinput.c \
    qrencode/qrspec.c \
    qrencode/rsecc.c \
    qrencode/split.c \
    sendfilewindow.cpp \
    server.cpp \
    settings.cpp \
    webreceive.cpp \
    websend.cpp \
    util.cpp \
    # start of httpserver and templeteengine
    httpserver/httpglobal.cpp \
    httpserver/httplistener.cpp \
    httpserver/httpconnectionhandler.cpp \
    httpserver/httpconnectionhandlerpool.cpp \
    httpserver/httprequest.cpp \
    httpserver/httpresponse.cpp \
    httpserver/httpcookie.cpp \
    httpserver/httprequesthandler.cpp \
    httpserver/httpsession.cpp \
    httpserver/httpsessionstore.cpp \
    httpserver/staticfilecontroller.cpp \
    templateengine/template.cpp \
    templateengine/templateloader.cpp \
    templateengine/templatecache.cpp \
    webserver.cpp
    # end of httpserver and templeteengine

HEADERS += \
    context.h \
    crypto.h \
    discoveryservice.h \
    mainui.h \
    navigater.h \
    onlinedeviceitem.h \
    progressBarui.h \
    qilintray.h \
    qrcode.h \
    qrencode/bitstream.h \
    qrencode/config.h \
    qrencode/mask.h \
    qrencode/mmask.h \
    qrencode/mqrspec.h \
    qrencode/qrencode.h \
    qrencode/qrencode_inner.h \
    qrencode/qrinput.h \
    qrencode/qrspec.h \
    qrencode/rsecc.h \
    qrencode/split.h \
    sendfilewindow.h \
    server.h \
    settings.h \
    webreceive.h \
    websend.h \
    util.h \
    # start of httpserver and templeteengine
    httpserver/httpglobal.h \
    httpserver/httplistener.h \
    httpserver/httpconnectionhandler.h \
    httpserver/httpconnectionhandlerpool.h \
    httpserver/httprequest.h \
    httpserver/httpresponse.h \
    httpserver/httpcookie.h \
    httpserver/httprequesthandler.h \
    httpserver/httpsession.h \
    httpserver/httpsessionstore.h \
    httpserver/staticfilecontroller.h\
    templateengine/templateglobal.h \
    templateengine/template.h \
    templateengine/templateloader.h \
    templateengine/templatecache.h \
    webserver.h
    # end of httpserver and templeteengine

FORMS += \
    mainui.ui \
    progressBarui.ui \
    sendfilewindow.ui \
    webreceive.ui \
    websend.ui

RESOURCES += \
    icons.qrc \
    image.qrc

RC_ICONS = icons/app.ico
ICON = icons/app.icns

unix {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lsodium

    PREFIX = $$(PREFIX)
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }

    binary.path = $$PREFIX/bin
    binary.files = $$OUT_PWD/QiLinTransfer
    binary.extra = cp "$$OUT_PWD/QiLinTransfer" "$$OUT_PWD/QiLinTransfer"
    binary.CONFIG = no_check_exist executable

    icon.path = $$PREFIX/share/icons/hicolor/scalable/apps
    icon.files = $$OUT_PWD/QiLinTransfer.svg
    icon.extra = cp "$$PWD/icons/app.svg" "$$OUT_PWD/QiLinTransfer.svg"
    icon.CONFIG = no_check_exist 

    desktop.path = $$PREFIX/share/applications
    desktop.files = $$OUT_PWD/QiLinTransfer.desktop
    desktop.extra = \
        cp "$$PWD/../misc/QiLinTransfer.desktop" "$$OUT_PWD/QiLinTransfer.desktop" && \
        sed -i 's/Exec=QiLinTransfer/Exec=QiLinTransfer/g' "$$OUT_PWD/QiLinTransfer.desktop" && \
        sed -i 's/Icon=QiLinTransfer/Icon=QiLinTransfer/g' "$$OUT_PWD/QiLinTransfer.desktop"
    desktop.CONFIG = no_check_exist 

    INSTALLS += binary icon desktop
}

QMAKE_INFO_PLIST = Info.plist

DISTFILES += \
    etc/templates/upload.tpl \
    image/4baa28a7a5c4097ccba3ad471ba529c0.png \
    image/ee529b5f885f3e0c0574cace9080536e.png

OTHER_FILES += etc/* etc/static/*

