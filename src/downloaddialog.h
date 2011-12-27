///////////////////////////////////////////////////////////////////////////////
// This class will be used on Symbian. It will take artist's and track's names
// from user and will invoke Downloader::Download().
///////////////////////////////////////////////////////////////////////////////

#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>
#include <QAction>
#include "ui_downloaddialog.h"
#include "downloader.h"

class DownloadDialog: public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDialog(Downloader *downloader, QWidget *parent = 0);
    ~DownloadDialog();

public slots:
    void Clean();
    void Download();
    void StateChanged(Downloader::State state);

private:
    Ui::DownloadDialog *_ui;
    Downloader *_downloader;

    QAction *_downloadAction;
    QAction *_abortAction;
    QAction *_hideAction;
};

#endif // DOWNLOADDIALOG_H ////////////////////////////////////////////////////
