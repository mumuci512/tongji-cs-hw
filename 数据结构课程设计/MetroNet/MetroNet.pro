QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    colorpickerbutton.cpp \
    main.cpp \
    mainwindow.cpp \
    metromapwidget.cpp \
    selectablelinelabel.cpp

HEADERS += \
    colorpickerbutton.h \
    mainwindow.h \
    metromapwidget.h \
    selectablelinelabel.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32-msvc* {
#    QMAKE_CXXFLAGS += /utf-8
#    QMAKE_CFLAGS += /utf-8
#    # 确保Qt使用UTF-8字符串
#    DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
#}

## 设置Qt的文本编码
#QT += core
#CONFIG += c++11
#DEFINES += QT_DEPRECATED_WARNINGS
