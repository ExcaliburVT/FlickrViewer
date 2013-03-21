#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QMainWindow>
#include <./QtFlickr/qtflickr.h>
#include <QNetworkReply>
#include <photodetail.h>


class ImageManager : public QWidget
{
    Q_OBJECT
public:
    ImageManager(QWidget * parent = 0);
    ~ImageManager();

private slots:
    void GetPhotos();
    void requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData );
    void onRequestCompleted();

private:
    void GetPhotos(QString page);
    void CheckDownloadQueue();
    QImage RotateImage(const QImage &src, unsigned short orientation);

    QtFlickr *qtFlickr;
    QHash<QString, PhotoDetail> photos;
    QString key;
    QString secret;
    QString token;
    QString userId;
    QQueue<PhotoDetail> photosToDownload;
    QNetworkAccessManager *networkAccessManager;
    QString photoPath;
    QTimer *checkUpdateTimer;

};

#endif // IMAGEMANAGER_H
