#include "cv.h"
#include "highgui.h"
#include <stdio.h>
int main(int argc,char **argv ) 
{ 
	CvCapture *capture=0;            //������Ƶ��ȡ�ṹ����Ϊ��Ƶ��ȡ������һ����������ʼֵΪ��
	IplImage *frame, *frame_copy= 0; //�������ڴ洢ͼ��������͵�ͼ��ָ��
	cvNamedWindow("result",1 ); //����һ����Ϊresult�Ĵ��ڣ���ͬ��cvNamedWindow("result" )
	capture=  cvCaptureFromFile("D:\\TestVideo\\123.avi");//��ȡ��Ƶ���������ֵ���ṹָ�����capture,�Ա����ã����ǻ�ȡ��Ƶ
	long  number_of_frames = (long)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT);//��ȡ��Ƶ֡�ĸ���
	printf("numFrames=%d\n",number_of_frames); //������򴰿������֡��

	if(capture ) //�����ȡ��Ƶ�ɹ���ִ��if()���
	{   
		int countFrame=0;  //��¼֡��
		int pic_count=0;
		for(;;)            //��ѭ��
		{
			if(!cvGrabFrame(capture )) //�Ӵ�����ͷ�����ļ���ץȡ֡�� ��ץȡ��֡���ڲ����洢����ץȡ��֡������ѹ���ĸ�ʽ��������ͷ���������壩������û�б����������� ���Ҫȡ�ػ�ȡ��֡����ʹ��cvRetrieveFrame��
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