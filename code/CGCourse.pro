QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    baseobject.cpp \
    camera.cpp \
    drawer.cpp \
    drawmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    photon.cpp \
    polygon.cpp \
    polygonalmodel.cpp \
    primitives.cpp \
    renderingwidget.cpp \
    scene.cpp \
    scenemanager.cpp \
    scenewidget.cpp \
    sphere.cpp \
    test_main.cpp\
    test_camera.cpp\
    thinlens.cpp

HEADERS += \
    baseobject.h \
    camera.h \
    drawer.h \
    drawmanager.h \
    light.h \
    mainwindow.h \
    photon.h \
    polygon.h \
    polygonalmodel.h \
    primitives.h \
    renderingwidget.h \
    scene.h \
    scenemanager.h \
    scenewidget.h \
    sphere.h \
    thinlens.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    CGCourse.pro.user

FORMS += \
    scenewidget.ui
