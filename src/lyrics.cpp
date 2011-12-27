///////////////////////////////////////////////////////////////////////////////
// This file contains the implementation GUI class
///////////////////////////////////////////////////////////////////////////////

#include "lyrics.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>

// Constructor ////////////////////////////////////////////////////////////////
// Initializes main window.
Lyrics::Lyrics():
    _ui(new Ui::Lyrics),
    _downloader(new Downloader(this))
#ifdef Q_OS_SYMBIAN
    ,_dialog(new DownloadDialog(_downloader, this)),
    _menu(new QMenu(this)),
    _downloadAction(new QAction(tr("Download lyrics"), this)),
    _saveAction(new QAction(tr("Save lyrics"), this)),
    _cleanAction(new QAction(tr("Clear interface"), this)),
    _aboutAction(new QAction(tr("About Lyrics"), this)),
    _aboutQtAction(new QAction(tr("About Qt4"), this)),
    _quitAction(new QAction(tr("Quit"), this)),
    _menuAction(new QAction(tr("Menu"), this))
#endif
{
    _ui->setupUi(this);
    this->setWindowTitle(qApp->applicationName() + QString(" v").append(qApp->applicationVersion()));

#ifdef Q_OS_SYMBIAN
    connect(_downloadAction, SIGNAL(triggered()), this, SLOT(Download()));
    connect(_saveAction, SIGNAL(triggered()), this, SLOT(Save()));
    connect(_cleanAction, SIGNAL(triggered()), this, SLOT(Clean()));
    connect(_cleanAction, SIGNAL(triggered()), _dialog, SLOT(Clean()));
    connect(_aboutAction, SIGNAL(triggered()), this, SLOT(About()));
    connect(_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    _menu->addAction(_downloadAction);
    _menu->addAction(_saveAction);
    _menu->addAction(_cleanAction);
    _menu->addAction(_aboutAction);
    _menu->addAction(_aboutQtAction);
    _menuAction->setMenu(_menu);

    this->addAction(_menuAction);
    this->addAction(_quitAction);

    _menuAction->setSoftKeyRole(QAction::PositiveSoftKey);
    _quitAction->setSoftKeyRole(QAction::NegativeSoftKey);
#else
    _ui->saveAction->setShortcut(QKeySequence::Save);
    _ui->quitAction->setShortcut(QKeySequence::Quit);

    _ui->saveAction->setStatusTip(tr("Saves lyrics to file."));
    _ui->cleanAction->setStatusTip(tr("Cleans user interface."));
    _ui->quitAction->setStatusTip(tr("Quits application."));
    _ui->aboutAction->setStatusTip(tr("Shows information about this application."));
    _ui->aboutQtAction->setStatusTip(tr("Shows information about Qt4."));

    connect(_ui->artist, SIGNAL(returnPressed()), this, SLOT(Download()));
    connect(_ui->track, SIGNAL(returnPressed()), this, SLOT(Download()));
    connect(_ui->download, SIGNAL(clicked()), this, SLOT(Download()));
    connect(_ui->saveAction, SIGNAL(triggered()), this, SLOT(Save()));
    connect(_ui->cleanAction, SIGNAL(triggered()), this, SLOT(Clean()));
    connect(_ui->quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(_ui->aboutAction, SIGNAL(triggered()), this, SLOT(About()));
    connect(_ui->aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(_downloader, SIGNAL(StateChanged(Downloader::State)), this, SLOT(StateChanged(Downloader::State)));
#endif

    connect(_downloader, SIGNAL(Downloaded(QString&)), this, SLOT(Downloaded(QString&)));
    connect(_downloader, SIGNAL(Failed(Downloader::Error)), this, SLOT(Failed(Downloader::Error)));
}

// Destructor /////////////////////////////////////////////////////////////////
// Removes ui and http objects.
Lyrics::~Lyrics()
{
    delete _downloader;
    delete _ui;

#ifdef Q_OS_SYMBIAN
    delete _dialog;
    delete _downloadAction;
    delete _saveAction;
    delete _cleanAction;
    delete _aboutAction;
    delete _aboutQtAction;
    delete _quitAction;
    delete _menu;
    delete _menuAction;
#endif
}

// About //////////////////////////////////////////////////////////////////////
// Shows information about this application.
void Lyrics::About()
{
    QMessageBox::about(this, tr("About Lyrics."), tr("Just a simple lyrics downloader. ;)<br><br>"
                                                     "All lyrics are downloaded from <a href=\"http://teksty.org/\">http://teksty.org/</a><br><br>"
                                                     "Thanks to <b>Pavel Fric</b> for Czech translation.<br><br>"
                                                     "<a href=\"http://qt-apps.org/content/show.php?content=129247\">Lyrics @ Qt-apps.org</a><br>"
                                                     "<a href=\"http://sinisblog.wordpress.com/\">http://sinisblog.wordpress.com/</a><br>"
                                                     "<a href=\"mailto:m.siniss@gmail.com\">m.siniss@gmail.com</a>"));
}

// Clean //////////////////////////////////////////////////////////////////////
// Cleans ui.
void Lyrics::Clean()
{
#ifndef Q_OS_SYMBIAN
    _ui->artist->clear();
    _ui->track->clear();
#endif
    _ui->lyrics->clear();
}

// Download ////////////////////////////////////////////////////////////////
// Download() shows _dialog on Symbian or starts new download on PC.
void Lyrics::Download()
{
#ifdef Q_OS_SYMBIAN
    _dialog->show();
#else
    if (_ui->artist->text().isEmpty() || _ui->track->text().isEmpty())
    {
        QMessageBox::warning(this, "*ekhm*", "Give artist's and track's names first.");
        return;
    }
    if (_downloader->GetState() != Downloader::Idle)
        _downloader->Abort();
    QString artist = _ui->artist->text();
    QString track = _ui->track->text();
    _downloader->Download(artist, track);
#endif
}

// Save ///////////////////////////////////////////////////////////////////////
// Saves downloaded lyrics to a specified by the user file.
void Lyrics::Save()
{
    if (_ui->lyrics->toPlainText().isEmpty())
    {
        // Failed. There's no lyrics.
        QMessageBox::warning(this, tr("*ekhm*"), tr("Download some lyrics first."));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Choose file name..."), "", tr("Text files (*.txt);; All files (*.*)"));
    // If user doesn't want to save...
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
    {
        // Failed. Can't write to specified file.
        QMessageBox::critical(this, tr("Failed."), tr("Cannot save lyrics to that file."));
        return;
    }
    file.write(_ui->lyrics->toPlainText().toAscii());
    file.close();
#ifndef Q_OS_SYMBIAN
    this->statusBar()->showMessage(tr("Lyrics saved."), 3000);
#else
    QMessageBox::information(this, tr("Done."), tr("Lyrics saved."));
#endif
}

// Downloaded.
// Puts downloaded lyrics on ui.
void Lyrics::Downloaded(QString &lyrics)
{
    _ui->lyrics->setHtml(lyrics);
}

// Failed.
// Shows error message.
void Lyrics::Failed(Downloader::Error error)
{
    QString message;
    switch (error)
    {
    case Downloader::ArtistNotFound:
        message = tr("Artist not found.");
        break;
    case Downloader::TrackNotFound:
        message = tr("Track not found.");
        break;
    case Downloader::NetworkError:
        message = tr("Network problem detected: ") + _downloader->ErrorString();
        break;
    }
    QMessageBox::critical(this, tr("Failed."), message);
}

// StateChanged.
// Shows information about download progress.
#ifndef Q_OS_SYMBIAN
void Lyrics::StateChanged(Downloader::State state)
{
    QString message;
    switch (state)
    {
    case Downloader::ResolvingArtistName:
        message = tr("Searching artist...");
        break;
    case Downloader::ResolvingTrackName:
        message = tr("Searching track...");
        break;
    case Downloader::DownloadingLyrics:
        message = tr("Downloading lyrics...");
        break;
    case Downloader::Idle:
        message = tr("Idle.");
        break;
    }
    this->statusBar()->showMessage(message);
}

#endif
