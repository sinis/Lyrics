# -------------------------------------------------
# Project created by QtCreator 2010-08-24T14:53:06
# -------------------------------------------------
QT += network xml
TARGET = Lyrics
TEMPLATE = app

SOURCES += src/main.cpp \
    src/lyrics.cpp \
    src/downloader.cpp

symbian:SOURCES += src/downloaddialog.cpp

HEADERS += src/lyrics.h \
    src/downloader.h

symbian:HEADERS += src/downloaddialog.h

FORMS += ui/lyrics.ui \

symbian:FORMS += ui/downloaddialog.ui \
                 ui/lyrics_s60.ui

symbian:ICON = res/icon.svg

TRANSLATIONS += res/lyrics_pl.ts \
                res/lyrics_cs.ts

win32:RC_FILE = res/res.rc
win32:QMAKE_LFLAGS += -static-libgcc
RESOURCES += res/res.qrc
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp
