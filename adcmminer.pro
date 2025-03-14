QT       += core gui charts concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calibration.cpp \
    channelmap.cpp \
    controller.cpp \
    datadelegate.cpp \
    decoder.cpp \
    filewatcher.cpp \
    main.cpp \
    mainwidget.cpp \
    mainwindow.cpp \
    mousepresseater.cpp \
    worker.cpp

INCLUDEPATH += $$system(root-config --incdir)
LIBS += $$system(root-config --libs) -lMinuit -lSpectrum

HEADERS += \
    adcm_df.h \
    calibration.h \
    channelmap.h \
    controller.h \
    datadelegate.h \
    decoder.h \
    filewatcher.h \
    mainwidget.h \
    mainwindow.h \
    mousepresseater.h \
    worker.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
