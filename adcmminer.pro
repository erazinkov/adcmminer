QT       += core gui charts concurrent datavisualization

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calibration.cpp \
    channelmap.cpp \
    chartwidget.cpp \
    controller.cpp \
    datadelegate.cpp \
    decoder.cpp \
    filewatcher.cpp \
    filewatchercontroller.cpp \
    filewatcherworker.cpp \
    flowlayout.cpp \
    histchartwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mousepresseater.cpp \
    processingchartwidget.cpp \
    processingcontroller.cpp \
    histogrammanager.cpp \
    peakfinder.cpp \
    energypeak.cpp \
    chartview.cpp \
    piecewiselinearfunction.cpp \
    processingworker.cpp \
    settings.cpp \
    myspinnerwidget.cpp

INCLUDEPATH += $$system(root-config --incdir)
LIBS += $$system(root-config --libs) -lMinuit -lSpectrum

HEADERS += \
    adcm_df.h \
    calibration.h \
    channelmap.h \
    chartwidget.h \
    constants.h \
    controller.h \
    datadelegate.h \
    decoder.h \
    filewatcher.h \
    filewatchercontroller.h \
    filewatcherworker.h \
    flowlayout.h \
    histchartwidget.h \
    mainwindow.h \
    mousepresseater.h \
    processingchartwidget.h \
    processingcontroller.h \
    processingworker.h \
    utils.h \
    histogrammanager.h \
    peakfinder.h \
    energypeak.h \
    chartview.h \
    piecewiselinearfunction.h \
    settings.h \
    myspinnerwidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
