///////////////////////////////////////////////////////////////////////////////
// Project: Lyrics
// Author: Sinis
// Created: 24.08.2010
// Description: Lyrics is a simple application that downloads lyrics
//  of a specified by the user song.
///////////////////////////////////////////////////////////////////////////////

#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include "lyrics.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Lyrics");
    app.setApplicationVersion("0.5.1");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString(":/lyrics_") + locale);
    app.installTranslator(&translator);

    Lyrics lyrics;
#ifndef Q_OS_SYMBIAN
    lyrics.adjustSize();
    lyrics.show();
#else
    lyrics.showMaximized();
#endif
    return app.exec();
}
