FlickrViewer
============

Qt Based Flickr Screensaver

In order to use FlickrViewer you must generate your own Flickr API Key.  Go to https://www.flickr.com/services/apps/create/apply/
and select a Non-Commerical Key.  Create a name for the app and flickr will give you a key and secret.

You will need to generate a token, there is documentation on the web on how to do this.  One option is to use the example code found in QtFlickr here:
http://www.qtflickr.com/qtflickr-1.2.zip

The FlickrViewer requires a file named FlickrViewer.ini in the same folder as the exectuable with the following values:

tags: 	Comma delimited list of tags of photos to load.
key:	Flickr assigned key
secret:	Flickr assigned secret
token:	Flickr generated token based on key and secret
userId:	The flickr userId to access photos

slideDuration: Duration in miliseconds
fadeDuration: Duration of the fade between pictures in miliseconds

###########################################################
Example File:
###########################################################

[flickr]
tags="frame,momframe,dadframe"
key = "xxx"
secret = "xxx"
token = "xxx-xxx"
userId = "xxx@N07"

[slideshow]
slideDuration=12000
fadeDuration=3000
