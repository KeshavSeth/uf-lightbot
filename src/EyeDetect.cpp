/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 */

#include <iostream>

#include "HaarCascadeObjectDetector.h"
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int
main(int argc, char *argv[])
{
  CvCapture* capture = NULL;

  double t = 0.0;
  Mat frame, frameCopy;

  Mat gray;
  string fname;

  unsigned int kmod = 0;

  ObjectDetector *faceDetector = new HaarCascadeObjectDetector("haarcascade_frontalface_alt.xml");
  ObjectDetector *eyeDetector = new HaarCascadeObjectDetector("haarcascade_mcs_lefteye.xml");
  ((HaarCascadeObjectDetector*)eyeDetector)->setImageScale(0.7);

  capture = cvCaptureFromCAM(0);

  if( capture ) {
    cout << "In capture ..." << endl;
    vector<Rect> faces;
  for(;;) {
    kmod++;
    IplImage* iplImg = cvQueryFrame( capture );
    frame = iplImg;

    if( frame.empty() )
        break;
    
    if( iplImg->origin == IPL_ORIGIN_TL )
      frame.copyTo( frameCopy );
    else 
      flip( frame, frameCopy, 0 );

      cvtColor(frameCopy, gray, CV_BGR2GRAY);
    if(kmod%2 == 0) {
      cvNamedWindow("result", 1);
        faces.clear();
      t = (double)cvGetTickCount();
      faceDetector->detect(gray, faces);
      t = (double)cvGetTickCount() - t;
      cout<<"execution time = "<<(t/((double)cvGetTickFrequency()*1000.0))<<" ms"<<endl;
    }

      vector<Rect>::const_iterator it;
      for(it = faces.begin(); it != faces.end(); it++) {
        rectangle(frameCopy, Point(it->x, it->y), Point(it->x+it->width, it->y+it->height), CV_RGB(255, 0 , 0));
     
        Rect eyeROI(it->x, it->y,cvRound(it->width/2.0), cvRound(it->height/2.0)); 
        rectangle(frameCopy, Point(eyeROI.x, eyeROI.y), Point(eyeROI.x + eyeROI.width, eyeROI.y + eyeROI.height), CV_RGB(0, 255, 0));
        vector<Rect> eyes;
        eyeDetector->detect(gray, eyes, eyeROI);

        vector<Rect>::const_iterator eyes_it;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        //if(lips.empty()) { cout<<"empty"; } else { cout<<"lips found"<<endl; }
        cvNamedWindow("eyes", 1);
        for(eyes_it = eyes.begin(); eyes_it != eyes.end(); eyes_it++) {
          rectangle(frameCopy, Point(eyes_it->x, eyes_it->y), Point(eyes_it->x+eyes_it->width, eyes_it->y+eyes_it->height), CV_RGB(0, 0 , 255 ));
          Mat edge_detected = gray(*eyes_it);
equalizeHist( edge_detected,edge_detected );
         //GaussianBlur( edge_detected, edge_detected, Size(9, 9), 2, 2 );
         vector<Vec3f> circles;

         HoughCircles(edge_detected, circles, CV_HOUGH_GRADIENT, 1, edge_detected.rows/4, 50, 100 );
    for( size_t i = 0; i < circles.size(); i++ )
    {
         cout<<"found a circle"<<endl;
         Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
         int radius = cvRound(circles[i][2]);
         // draw the circle center
         circle( edge_detected, center, 3, Scalar(0,255,0), -1, 8, 0 );
         // draw the circle outline
         circle( edge_detected, center, radius, Scalar(0,0,255), 3, 8, 0 );
    }

          imshow("eyes", edge_detected);
        }
       
      }  
      imshow("result", frameCopy);

     if( waitKey( 10 ) >= 0 )
         goto _cleanup_;
    }
  }
 

_cleanup_:
  cvReleaseCapture( &capture );
  delete faceDetector;
  delete eyeDetector;
  return 0;
}

