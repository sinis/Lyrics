///////////////////////////////////////////////////////////////////////////////
// This file contains declaration of class that will be responsible
// for downloading lyrics.
///////////////////////////////////////////////////////////////////////////////

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>

class Downloader: public QObject
{
    Q_OBJECT

public:
    Downloader(QObject *parent = 0);
    ~Downloader();

    enum State
    {
        Idle,
        ResolvingArtistName,
        ResolvingTrackName,
        DownloadingLyrics
    };
    enum Error
    {
        ArtistNotFound,
        TrackNotFound,
        NetworkError
    };

    State GetState() const { return _state; }
    QString ErrorString() const;

public slots:
    void Download(const QString &artist, const QString &track);
    void Abort();

signals:
    void StateChanged(Downloader::State state);
    void Failed(Downloader::Error error);
    void Downloaded(QString &lyrics);

private slots:
    void ChangeState(State state);
    void RequestFinished(QNetworkReply *reply);
    void FixString(QString &str);
    bool FindAndCopy(QString &what, const QString &in);

private:
    QNetworkAccessManager *_network;
    QNetworkReply *_current;
    State _state;
    QString _artist;
    QString _track;
    QString _lyrics;
};

#endif // DOWNLOADER_H ////////////////////////////////////////////////////////
