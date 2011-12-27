///////////////////////////////////////////////////////////////////////////////
// Implementation of Downloader class.
///////////////////////////////////////////////////////////////////////////////

#include "downloader.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDomDocument>

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

    QString requestUrl = QString("http://lyrics.wikia.com/api.php?func=getArtist&artist=") + _artist + "&fmt=text";

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
    QDomDocument xml;
    QDomNode node;

    switch (_state)
    {
    case ResolvingArtistName:
        // If data is empty - artist does not exist in the db.
        if (data.isEmpty())
        {
            emit Failed(ArtistNotFound);
            ChangeState(Idle);
            return;
        }
        url.setUrl(QString("http://lyrics.wikia.com/api.php?func=getSong&artist=") + _artist +
                   QString("&song=") + _track + "&fmt=xml");

        request.setUrl(url);
        _network->get(request);
        ChangeState(ResolvingTrackName);
        break;

    case ResolvingTrackName:
        //  If lyrics tag is set to "Not found" - no such track in the db.
        xml.setContent(data);
        node = xml.elementsByTagName("lyrics").at(0);
        if (node.isNull() || node.toElement().text() == "Not found")
        {
            emit Failed(TrackNotFound);
            ChangeState(Idle);
            return;
        }

        node = xml.elementsByTagName("url").at(0);

        url.setEncodedUrl((QByteArray)node.toElement().text().toStdString().c_str(), QUrl::TolerantMode);

        request.setUrl(url);
        _network->get(request);
        ChangeState(DownloadingLyrics);
        break;

    case DownloadingLyrics:
        pos = data.indexOf("<div class='lyricbox'>");
        data.remove(0, pos);
        pos = data.indexOf("</div>");
        data.remove(0, pos+6);
        pos = data.indexOf("\n");
        data.remove(pos, data.length()-pos);
        _lyrics = data;
        qDebug(_lyrics.toStdString().c_str());
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
    str.replace(QChar(' '), QChar('_'));
    str.replace(QChar('\''), QString("%27"));
    str.replace(QChar('&'), QString("%26"));

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
