#include <opencv/cv.h>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


Mat integralImg(Mat img)
{
	Mat *toReturn = new Mat(img.rows,img.cols,CV_32FC1);

	//faccio si che in ogni elemento ci sia la somma dei valori contenuti in img
	for(int i=0;i<img.rows;i++)
	{

		for(int j=0;j<img.cols;j++)
		{
			if(j==0)
				toReturn->at<float>(i,j) = (float)img.at<uchar>(i,j);
			else
				toReturn->at<float>(i,j) = toReturn->at<float>(i,j-1) + (float)img.at<uchar>(i,j); 

		}
	}


	for(int j=0;j<img.cols;j++)
	{

		for(int i=1;i<img.rows;i++)
		{
			
				toReturn->at<float>(i,j) = toReturn->at<float>(i-1,j) + toReturn->at<float>(i,j);  

		}
	}




	Mat *normalized = new Mat(img.rows,img.cols,CV_32FC1);
	normalize(*toReturn,*normalized,0,1,NORM_MINMAX);
	imshow("test2",*normalized);
	waitKey(0);
	return *normalized;
}

int main(int argc,char *argv[]){




	Mat left = imread("C:/Users/davide/Desktop/homework_embedded2/Tsukuba_L.png",0);
	Mat right = imread("C:/Users/davide/Desktop/homework_embedded2/Tsukuba_R.png",0);

	int dispRange = 15;

	vector<Mat> integrali;

	//calcolo piani disparità
	for(int k=0;k<=dispRange;k++)
	{
		Mat piano(left.rows,left.cols,CV_8UC1);;
		for(int i=0;i<left.rows;i++)
		{

			for(int j=0;j<left.cols;j++)
			{

				if(j-k < 0)
					continue;
				piano.at<uchar>(i,j) = (uchar)abs(left.at<uchar>(i,j) - right.at<uchar>(i,j-k));


			}
		}
		integrali.push_back( integralImg(piano) );

	}

	//calcolo per ciascun pixel la disparity in base all'indice (tra 0 e range) che minimizza la distanza tra le patch, sfruttando le immagini integrali
	
	Mat disparityMap(left.rows,left.cols,CV_8UC1);
	
	for(int i=2;i<left.rows-1;i++)
	{

		for(int j=2;j<left.cols-1;j++)
		{

			float minDist = 1000000.0f;
			int indMin = -1;

			for(int k=0;k<=dispRange;k++)
			{
				float dist;
				Mat pianoK = integrali[k];
				//uso le proprietà delle immagini integrali
				dist = pianoK.at<float>(i+1,j+1);
				dist -= pianoK.at<float>(i-2,j+1);
				dist -= pianoK.at<float>(i+1,j-2);
				dist += pianoK.at<float>(i-2,j-2);

				if(dist<minDist)
				{
					minDist = dist;
					indMin = k;
				}
			}

				disparityMap.at<uchar>(i,j) = (unsigned char)( 255 * ((float)indMin)/(dispRange+1)    );

		}
	}

	imshow("disparity",disparityMap);
	waitKey(0);


}
