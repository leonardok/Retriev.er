#-------------------------------------------------
#
# Project created by QtCreator 2011-06-08T22:36:36
#
#-------------------------------------------------

QT       += core gui

TARGET = retriev.er
TEMPLATE = app

target.path += /usr/bin
INSTALLS += target

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

RC_FILE  = retriever.rc

SOURCES += source/main.cpp \
	source/retriever.cpp \
	source/task.cpp \
	source/log.cpp \
	source/sync.cpp \
	source/options.cpp \
	source/monitor.cpp

HEADERS += header/retriever.h \
	header/task.h \
	header/log.h \
	header/sync.h \
	header/options.h \
	header/monitor.h

FORMS   += ui/task.ui \
	ui/retriever.ui \
	ui/options.ui \
	ui/about.ui

RESOURCES += resources.qrc

OTHER_FILES += README.md \
    dist/archlinux/PKGBUILD \
    Changelog
