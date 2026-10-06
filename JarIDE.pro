# ============================================================
#  Jar-IDE  (Jar Python-IDE)
#  A lightweight, animated Python IDE built with C++ / Qt5.
#  Target: Windows 7 ~ Windows 11 (Qt 5.14 MinGW 7.3 64-bit)
# ============================================================

QT       += core gui widgets network

CONFIG   += c++17
CONFIG   -= app_bundle

TARGET    = JarIDE
TEMPLATE  = app

# ---- Win7 compatibility: no Win10-only effects required ----
DEFINES += QT_DEPRECATED_WARNINGS

# ---- Allow large files / modern API on MinGW ----
unix:!macx: LIBS += -ldl

SOURCES += \
    src/main.cpp \
    src/appstyle.cpp \
    src/pythonchecker.cpp \
    src/welcomewindow.cpp \
    src/jroproject.cpp \
    src/codeeditor.cpp \
    src/pythonhighlighter.cpp \
    src/pythonrunner.cpp \
    src/mainwindow.cpp

HEADERS += \
    src/appstyle.h \
    src/pythonchecker.h \
    src/welcomewindow.h \
    src/jroproject.h \
    src/codeeditor.h \
    src/pythonhighlighter.h \
    src/pythonrunner.h \
    src/mainwindow.h

RESOURCES += \
    RES/resources.qrc

# ---- Output directories ----
DESTDIR     = bin
OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
RCC_DIR     = build/rcc
UI_DIR      = build/ui
