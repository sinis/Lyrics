///////////////////////////////////////////////////////////////////////////////
// This class will be responsoble for GUI.
///////////////////////////////////////////////////////////////////////////////

#ifndef LYRICS_H
#define LYRICS_H

#include <QObject> // For Q_OS_SYMBIAN
#ifdef Q_OS_SYMBIAN
#include <QMenu>
#include <QAction>
#include "ui_lyrics_s60.h"
#include "downloaddialog.h"
#else
#include "ui_lyrics.h"
#endif
#include "downloader.h"

class Lyrics: public
#ifndef Q_OS_SYMBIAN
        QMainWindow
#else
        QWidget
#endif
{
    Q_OBJECT
public:
    Lyrics();
    ~Lyrics();

private slots:
    void Download();
    void Save();
    void Clean();
    void About();
    void Downloaded(QString &lyrics);
    void Failed(Downloader::Error error);
#ifndef Q_OS_SYMBIAN
    void StateChanged(Downloader::State state);
#endif

private:
    Ui::Lyrics *_ui;
    Downloader *_downloader;

#ifdef Q_OS_SYMBIAN
    DownloadDialog *_dialog;

    QMenu *_menu;
    QAction *_downloadAction;
    QAction *_saveAction;
    QAction *_cleanAction;
    QAction *_aboutAction;
    QAction *_aboutQtAction;
    QAction *_quitAction;
    QAction *_menuAction;
#endif
};

#endif // LYRICS_H ////////////////////////////////////////////////////////////
