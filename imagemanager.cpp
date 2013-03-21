#include "imagemanager.h"
#include <./QtFlickr/qtflickr.h>
#include <QNetworkReply>
#include <exif.h>


ImageManager::ImageManager(QWidget * parent )
    : QWidget ( parent )
{

    QString iniFile = QCoreApplication::applicationDirPath() + "/FlickrViewer.ini";
    QSettings settings(iniFile, QSettings::IniFormat);

    key = settings.value("flickr/key").toString();
    secret = settings.value("flickr/secret").toString();
    token = settings.value("flickr/token").toString();
    userId = settings.value("flickr/userId").toString();

    networkAccessManager = new QNetworkAccessManager ( this );
    photoPath = QCoreApplication::applicationDirPath() + "/photos/";

    qtFlickr = new QtFlickr (   key,
                                secret,
                                this );
    qtFlickr->setToken(token);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());


    // connecting listner to the requestFinished() signal
    connect ( qtFlickr,
              SIGNAL ( requestFinished ( int, QtfResponse, QtfError, void* ) ),
              this,
              SLOT ( requestFinished ( int, QtfResponse, QtfError, void* ) ) );

    checkUpdateTimer = new QTimer ( this );
    connect ( checkUpdateTimer, SIGNAL ( timeout() ), this, SLOT ( GetPhotos() ) );
    checkUpdateTimer->start ( 60000*10 );
    GetPhotos();


}



ImageManager::~ImageManager()
{
    delete qtFlickr;
    delete networkAccessManager;
}

void ImageManager::GetPhotos()
{
    if(photosToDownload.count() == 0)
    {
        photos.clear();
        GetPhotos("1");
    }
}

void ImageManager::GetPhotos(QString page)
{

    try
    {
        QString iniFile = QCoreApplication::applicationDirPath() + "/FlickrViewer.ini";
        QSettings settings(iniFile, QSettings::IniFormat);
        QString tags = settings.value("flickr/tags").toString();


        QtfMethod method;
        method.method = "flickr.photos.search";
        method.args.insert("page",page);
        method.args.insert("per_page","500");
        method.args.insert( "user_id", userId );
        method.args.insert( "tags", tags);
        method.args.insert( "extras", "url_o,date_upload" );
        QtfRequest request;
        request.requests.insert( "photo", "url_o,id,dateupload" );
        request.requests.insert( "photos", "page,pages" );
        qtFlickr->get ( method,request );
    }
    catch (std::exception & e)
    {
        qCritical() << "Exception: " << e.what() << "\n";
    }
}

void ImageManager::requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData )
{
    if ( !err.code )
    {
        QList<QtfTag> list = data.tags.values("photo");
        for ( int i = 0; i < list.size(); ++i )
        {
            QString Id = list.at(i).attrs.value("id");
            if(!photos.contains(Id))
            {
                PhotoDetail detail;
                detail.Url = list.at(i).attrs.value("url_o");
                detail.Id = Id;
                detail.DateUploaded = QDateTime::fromTime_t(list.at ( i ).attrs.value("dateupload").toLong());
                photos.insert(Id, detail);
            }
        }
        QList<QtfTag> pageList = data.tags.values("photos");
        int totalPages = pageList[0].attrs.value("pages").toInt();
        int currentPage = pageList[0].attrs.value("page").toInt();
        if(totalPages > currentPage)
        {
            currentPage++;
            GetPhotos(QString::number(currentPage));
            return;
        }
    }
    else
    {
        qCritical() <<err.message;
        return;
    }

    QHashIterator<QString, PhotoDetail> photo(photos);

    while (photo.hasNext()) {
        photo.next();

        if(QFile::exists(photoPath + photo.key() + ".jpg"))
        {
            if(QFileInfo(photoPath + photo.key() + ".jpg").lastModified() > photo.value().DateUploaded)
            {
                photos[photo.key()].Downloaded = true;
            }
        }
        else
        {
            if(!photo.value().Url.isEmpty())
            {
                photosToDownload.enqueue(photo.value());
            }
        }
    }

    //Check to delete old files
    QDir photoDirectory = QDir(photoPath);
    QStringList filters;
    filters << "*.jpg";
    QStringList allFiles = photoDirectory.entryList(filters,QDir::Files);
    foreach(QString file, allFiles)
    {
        QString fileName = file.split(".")[0];
        if(!photos.contains(fileName))
        {
            qDebug() << "Deleting: " + file;
            QFile::remove(photoPath + file);
        }
    }

    qDebug() << "Total Files: " + QString::number(photos.count());

    CheckDownloadQueue();
}

void ImageManager::CheckDownloadQueue()
{
    if(photosToDownload.count() > 0)
    {

        PhotoDetail photoDetail = photosToDownload.head();
        qDebug() << "Attempting: " + photoDetail.Url;

        QNetworkRequest request;
        request.setUrl(QUrl(photosToDownload.head().Url));
        QNetworkReply *reply = networkAccessManager->get (request);

        connect(reply, SIGNAL(finished()),
            this, SLOT(onRequestCompleted()));
    }
}

void ImageManager::onRequestCompleted()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QByteArray imageData = reply->readAll();

    if(!QDir(photoPath).exists())
    {
        QDir().mkdir(photoPath);
    }

    QImage image;
    //getting image from received binary data
    if ( image.loadFromData(imageData))
    {
        int orientation = -1000;



        Exif *exifData = new Exif();
        exifData->readJpegSections(imageData,&orientation);


        PhotoDetail photoDetail = photosToDownload.dequeue();
        QString path = photoPath + photoDetail.Id + ".jpg";


        QTransform myTransform;
        switch (orientation)
        {
            case 3:
                myTransform.rotate(180);
                image = image.transformed(myTransform);
                break;
            case 6:
                myTransform.rotate(90);
                image = image.transformed(myTransform);
                break;
            case 8:
                myTransform.rotate(270);
                image = image.transformed(myTransform);
                break;
        }
        image.save(path);

        qDebug() << "Saved: " + photoDetail.Id + ".jpg";
        photos[photoDetail.Id].Downloaded = true;
    }
    else
    {
        //failed to download, try later
        PhotoDetail photoDetail = photosToDownload.dequeue();
        qDebug() << "Failed: " + photoDetail.Url;
    }
    CheckDownloadQueue();
}


