#-------------------------------------------------
#
# Project created by QtCreator 2011-06-08T22:36:36
#
#-------------------------------------------------

QT       += core gui

TARGET = Retriev.er
TEMPLATE = app

target.path += /usr/bin
INSTALLS += target

RC_FILE  = retriever.rc

SOURCES += source/main.cpp \
	source/retriever.cpp \
	source/task.cpp \
	source/log.cpp \
	source/sync.cpp \
	source/options.cpp

HEADERS += header/retriever.h \
	header/task.h \
	header/log.h \
	header/sync.h \
	header/options.h

FORMS   += ui/task.ui \
	ui/retriever.ui \
	ui/options.ui

RESOURCES += resources.qrc

OTHER_FILES += README.md \
    dist/archlinux/PKGBUILD
