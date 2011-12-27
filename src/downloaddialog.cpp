///////////////////////////////////////////////////////////////////////////////
// Implementation of DownloadDialog.
///////////////////////////////////////////////////////////////////////////////

#include "downloaddialog.h"
#include <QMessageBox>

// Constructor.
DownloadDialog::DownloadDialog(Downloader *downloader, QWidget *parent):
    QDialog(parent),
    _ui(new Ui::DownloadDialog),
    _downloader(downloader),
    _downloadAction(new QAction(tr("Download"), this)),
    _abortAction(new QAction(tr("Abort"), this)),
    _hideAction(new QAction(tr("Hide"), this))
{
    _ui->setupUi(this);
    this->addAction(_downloadAction);
    this->addAction(_abortAction);
    this->addAction(_hideAction);
    connect(_downloadAction, SIGNAL(triggered()), this, SLOT(Download()));
    connect(_abortAction, SIGNAL(triggered()), _downloader, SLOT(Abort()));
    connect(_hideAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(_downloader, SIGNAL(StateChanged(Downloader::State)), this, SLOT(StateChanged(Downloader::State)));
    connect(_downloader, SIGNAL(Downloaded(QString&)), this, SLOT(hide()));
    _downloadAction->setSoftKeyRole(QAction::PositiveSoftKey);
    _hideAction->setSoftKeyRole(QAction::NegativeSoftKey);
}

// Destructor.
DownloadDialog::~DownloadDialog()
{
    delete _ui;
    delete _downloadAction;
    delete _abortAction;
    delete _hideAction;
}

// Clean.
// This slot is invoked in Lyrics by _cleanAction. Clean() removes content
// of _ui->artist and _ui->track.
void DownloadDialog::Clean()
{
    _ui->artist->clear();
    _ui->track->clear();
}

// Download.
// Takes artist's and track's names from _ui and invokes _downloader.
void DownloadDialog::Download()
{
    QString artist = _ui->artist->text();
    QString track = _ui->track->text();

    if (artist.isEmpty() || track.isEmpty())
    {
        QMessageBox::warning(this, "*ekhm*", "Give artist's and track's names first.");
        return;
    }

    _downloader->Download(artist, track);
}

// StateChanged.
// This slot is invoked after every _downloader's state change.
// It changes _ui->state and replaces soft keys.
void DownloadDialog::StateChanged(Downloader::State state)
{
    QString message;
    switch (state)
    {
    case Downloader::ResolvingArtistName:
        message = tr("Searching artist...");
        _downloadAction->setSoftKeyRole(QAction::NoSoftKey);
        _abortAction->setSoftKeyRole(QAction::PositiveSoftKey);
        break;
    case Downloader::ResolvingTrackName:
        message = tr("Searching track...");
        break;
    case Downloader::DownloadingLyrics:
        message = tr("Downloading lyrics...");
        break;
    case Downloader::Idle:
        message = tr("Idle.");
        _abortAction->setSoftKeyRole(QAction::NoSoftKey);
        _downloadAction->setSoftKeyRole(QAction::PositiveSoftKey);
        break;
    }
    _ui->state->setText(message);
}
