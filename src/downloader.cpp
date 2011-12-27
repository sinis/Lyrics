///////////////////////////////////////////////////////////////////////////////
// Implementation of Downloader class.
///////////////////////////////////////////////////////////////////////////////

#include "downloader.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

// Constructor.
Downloader::Downloader(QObject *parent):
    QObject(parent),
    _network(new QNetworkAccessManager(this)),
    _current(0),
    _state(Idle)
{
    connect(_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(RequestFinished(QNetworkReply*)));
}

// Destructor.
Downloader::~Downloader()
{
    delete _network;
}

// Download()
// If downloader is idling Download() starts new request.
void Downloader::Download(const QString &artist, const QString &track)
{
    if (_state != Idle)
        return;

    _artist = artist;
    _track = track;
    FixString(_artist);
    FixString(_track);

    ChangeState(ResolvingArtistName);

    QString requestUrl = QString("http://teksty.org/") + _artist[0].toLower();
    if (_artist[0].toLower() == 'i')
        requestUrl += '/';
    QNetworkRequest request;
    QUrl url(requestUrl);
    request.setUrl(url);
    _current = _network->get(request);
}

// Abort()
// If downloader isn't idling Abort() stops its work and sets _state to Idle.
void Downloader::Abort()
{
    if (_state == Idle)
        return;
    _current->abort();
    ChangeState(Idle);
}

// ChangeState()
// This slot sets new state and emits signal StateChanged().
void Downloader::ChangeState(State state)
{
    if (_state == state)
        return;
    _state = state;
    emit StateChanged(_state);
}

// RequestFinished()
// It receives every reply and checks if any error occured (if so - emits
// Failed()). Depending on _state RequestFinished() sends next request
// (to resolve track name or download lyrics) or emits signal Downloaded().
void Downloader::RequestFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::OperationCanceledError)
    {
        // User aborted request.
        return;
    }
    else if (reply->error() != QNetworkReply::NoError)
    {
        emit Failed(NetworkError);
        ChangeState(Idle);
        return;
    }

    QString data = reply->readAll();
    QUrl url;
    QNetworkRequest request;
    int pos;

    switch (_state)
    {
    case ResolvingArtistName:
        if (!FindAndCopy(_artist.prepend('/'), data))
        {
            emit Failed(ArtistNotFound);
            ChangeState(Idle);
            return;
        }
        url.setUrl(QString("http://teksty.org/") + _artist);
        request.setUrl(url);
        _network->get(request);
        ChangeState(ResolvingTrackName);
        break;

    case ResolvingTrackName:
        if (!FindAndCopy(_track.prepend(','), data))
        {
            emit Failed(TrackNotFound);
            ChangeState(Idle);
            return;
        }
        // Need to cut "teksty-piosenek" from artist.
        _artist.chop(_artist.length() - _artist.indexOf(',') - 1);
        url.setUrl(QString("http://teksty.org/") + _artist + _track);
        request.setUrl(url);
        _network->get(request);
        ChangeState(DownloadingLyrics);
        break;

    case DownloadingLyrics:
        pos = data.indexOf("<div class=\"songText\" id=\"songContent\">");
        data.remove(0, pos+39);
        pos = data.indexOf("</div>");
        data.remove(pos, data.length()-pos);
        _lyrics = data;
        ChangeState(Idle);
        emit Downloaded(_lyrics);
        break;
    default: break;
    }
}

// FixString
// It fixes string so string can be added to url.
void Downloader::FixString(QString &str)
{
    str.replace(QChar(' '), QChar('-'));
    str.replace(QChar('\''), QString(""));

    // Code below is dedicated for polish letters.
    str.replace("ą", "a", Qt::CaseInsensitive);
    str.replace("ć", "c", Qt::CaseInsensitive);
    str.replace("ę", "e", Qt::CaseInsensitive);
    str.replace("ł", "l", Qt::CaseInsensitive);
    str.replace("ń", "n", Qt::CaseInsensitive);
    str.replace("ó", "o", Qt::CaseInsensitive);
    str.replace("ś", "s", Qt::CaseInsensitive);
    str.replace("ź", "z", Qt::CaseInsensitive);
    str.replace("ż", "z", Qt::CaseInsensitive);
}

// FindAndCopy
// This method tries to find 'what' in 'in'. It is needed to find out
// if artist and track exists. If FindAndCopy() find a proper string
// it copies that string to 'in'.
bool Downloader::FindAndCopy(QString &what, const QString &in)
{
    int pos = in.indexOf(what, 0, Qt::CaseInsensitive);
    int length;

    if (pos == -1)
    {
        // Not found.
        return false;
    }

    length = in.indexOf('\"', pos) - pos;
    what = in.mid(pos+1, length-1);
    return true;
}

// ErrorString.
// Returns error message if available.
QString Downloader::ErrorString() const
{
    if (_current)
        return _current->errorString();
    return "";
}
