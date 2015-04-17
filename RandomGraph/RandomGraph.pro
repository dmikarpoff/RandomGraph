#-------------------------------------------------
#
# Project created by QtCreator 2014-11-12T23:17:51
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RandomGraph
TEMPLATE = app
LIBS += -LC:\DevTools\OGDF\_release -lOGDF -lpsapi
INCLUDEPATH += C:\DevTools\OGDF

SOURCES += main.cpp\
        mainwindow.cpp \
        RandomGraph.cpp \
        VertexWindow.cpp \
        EdgeWindow.cpp \
        graphlayouter.cpp \
        graphestimator.cpp \
        tester.cpp \
        bignumber.cpp

HEADERS  += mainwindow.h \
        RandomGraph.h \
        VertexWindow.h \
    EdgeWindow.h \
    graphlayouter.h \
    graphestimator.h \
    tester.h \
    bignumber.h

FORMS += \
    mainwindow.ui

LIBS_DYN_LINK = C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/QT5CORE.DLL \
                C:/DevTools/Qt/Qt5.3.2/Tools/mingw482_32/bin/libwinpthread-1.dll \
                C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/icuin52.dll \
                C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/icuuc52.dll \
                C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/icudt52.dll \
                C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/QT5GUI.DLL \
                C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/QT5WIDGETS.DLL \
                C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/QT5XML.DLL \
                C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/libgcc_s_dw2-1.dll \
                C:/DevTools/Qt/Qt5.3.2/5.3/mingw482_32/bin/libstdc++-6.dll

CONFIG(release, debug|release): DESTDIR = $${OUT_PWD}/release
CONFIG(debug, debug|release): DESTDIR = $${OUT_PWD}/debug

extra_libs.files = $${LIBS_DYN_LINK}
extra_libs.path = $$DESTDIR

INSTALLS += extra_libs
