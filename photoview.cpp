
/******************************************************************************
 *   Copyright (C) 2009 by Evgeni Gordejev   *
 *   evgeni.gordejev@gmail.com   *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Library Lesser General Public License as   *
 *   published by the Free Software Foundation; either version 2 of the       *
 *   License, or (at your option) any later version.                          *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Library Lesser General Public *
 *   License along with this program; if not, write to the                    *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include <photoview.h>
#include <QPaintEvent>
#include <QPainter>
#include <QNetworkAccessManager>
#include <QtCore>


PhotoView::PhotoView (QWidget * parent )
    : QWidget ( parent )
{

    QString iniFile = QCoreApplication::applicationDirPath() + "/FlickrViewer.ini";
    QSettings settings(iniFile, QSettings::IniFormat);
    int slideDuration = settings.value("slideshow/slideDuration").toInt();

    photoPath = QCoreApplication::applicationDirPath() + "/photos/";

    photoDirectory = QDir(photoPath);

    //timer used to change photos
    timer = new QTimer ( this );
    connect ( timer, SIGNAL ( timeout() ), this, SLOT ( nextPhoto() ) );
    timer->start ( slideDuration );

    opacity = 1.0;
    nextPhoto();
}




PhotoView::~PhotoView()
{
    delete timer;
}

void PhotoView::mousePressEvent (QMouseEvent * event)
{
    timer->stop();
    qApp->quit();
}

void PhotoView::keyPressEvent(QKeyEvent * event)
{
    timer->stop();
    qApp->quit();
}


void PhotoView::paintEvent ( QPaintEvent * event )
{
    //painting widget content
    QPainter painter ( this );
    painter.fillRect ( event->rect(),QColor ( 0,0,0 ) );

    if ( !currentPhoto.isEmpty()  )
    {
        painter.setOpacity(opacity);

        //adjusting photo to the center
        QImage image = QImage(photoPath + currentPhoto);

        image = image.scaled(this->width(),this->height(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (this->width()/2) - (image.width()/2);
        int y = (this->height()/2)  - (image.height()/2);

        painter.drawImage( x, y,image);
		
    }
}

QStringList PhotoView::AvailablePhotos()
{
    QStringList filters;
    filters << "*.jpg";
    return photoDirectory.entryList(filters,QDir::Files);
}

int PhotoView::randInt(int low, int high)
    {
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
    }

void PhotoView::nextPhoto()
{
    if ( AvailablePhotos().count() == 0 )
        return;

	currentPhoto = AvailablePhotos().at(randInt(0,AvailablePhotos().count()-1));
	update();
}
