#include "cv.h"
#include "highgui.h"
#include <stdio.h>
int main(int argc,char **argv ) 
{ 
	CvCapture *capture=0;            //定义视频获取结构，作为视频获取函数的一个参数，初始值为零
	IplImage *frame, *frame_copy= 0; //定义用于存储图像各种类型的图像指针
	cvNamedWindow("result",1 ); //创建一个名为result的窗口，等同于cvNamedWindow("result" )
	capture=  cvCaptureFromFile("D:\\TestVideo\\123.avi");//获取视频句柄，并赋值给结构指针变量capture,以备后用，就是获取视频
	long  number_of_frames = (long)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT);//获取视频帧的个数
	printf("numFrames=%d\n",number_of_frames); //在命令框窗口输出总帧数

	if(capture ) //如果获取视频成功，执行if()语句
	{   
		int countFrame=0;  //记录帧数
		int pic_count=0;
		for(;;)            //死循环
		{
			if(!cvGrabFrame(capture )) //从从摄像头或者文件中抓取帧。 被抓取的帧在内部被存储，被抓取的帧可能是压缩的格式（由摄像头／驱动定义），所以没有被公开出来。 如果要取回获取的帧，请使用cvRetrieveFrame。
				break;
			frame=cvRetrieveFrame(capture );
			if(!frame )
				break;
			//frame_copy=cvQueryFrame(capture);
			char imageName[100];
			if((countFrame%25)==0)
			{
			  sprintf(imageName,"D:\\TestVideo\\%d.jpg",pic_count++);
			  cvSaveImage(imageName,frame);
			}
			countFrame++;
			  cvShowImage("result",frame ); 
			// Sleep(66.9);
			if(cvWaitKey(100)==27 ) 
				break; 
		}
		cvReleaseImage(&frame_copy );
		cvReleaseCapture(&capture );  
	} 
	cvDestroyWindow("result");  
	return 0;
}