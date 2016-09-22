#include "stdio.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

int main(int argc, char* argv[])
{
    CvCapture* capture1 = cvCreateCameraCapture( 0 );
   // CvCapture* capture2 = cvCreateCameraCapture( 1 );

    double w = 20, h = 400;
    cvSetCaptureProperty ( capture1, CV_CAP_PROP_FRAME_WIDTH,  w );  
    cvSetCaptureProperty ( capture1, CV_CAP_PROP_FRAME_HEIGHT, h );
   // cvSetCaptureProperty ( capture2, CV_CAP_PROP_FRAME_WIDTH,  w );  
  //  cvSetCaptureProperty ( capture2, CV_CAP_PROP_FRAME_HEIGHT, h );

    cvNamedWindow( "Camera_1", CV_WINDOW_AUTOSIZE );
   // cvNamedWindow( "Camera_2", CV_WINDOW_AUTOSIZE );

    IplImage* frame1;
  //  IplImage* frame2;

    int n = 2;
    while(1)
    {
        frame1 = cvQueryFrame( capture1 );
        if( !frame1 ) break;
        cvShowImage( "Camera_1", frame1 );

     //   frame2 = cvQueryFrame( capture2 );
    //    if( !frame2 ) break;
     //   cvShowImage( "Camera_2", frame2 );

        int key = cvWaitKey(30);
        if( key == 27 ) break;
    }
     cvReleaseCapture( &capture1 );
     //cvReleaseCapture( &capture2 );
     cvDestroyWindow( "Camera_1" );
   //  cvDestroyWindow( "Camera_2" );
	 return 0;
}