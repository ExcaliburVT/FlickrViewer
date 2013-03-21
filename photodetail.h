#ifndef PHOTODETAIL_H
#define PHOTODETAIL_H

#include <QString>
#include <QDate>

class PhotoDetail
{
public:
    PhotoDetail();
    QString Url;
    QString Id;
    QDateTime DateUploaded;
    bool Downloaded;
};

#endif // PHOTODETAIL_H
