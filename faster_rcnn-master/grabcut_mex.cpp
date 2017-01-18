#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "mex.h"
#include <iostream>
using namespace cv;
using namespace std;
#define PI 3.14159265358979323846 //����Բ����

static string cls_name[34] = {
	"apple", "ellipsoid", //������
	"banana", "unknown", //����
	"bk", "",
	"bread", "column", //����
	"bun", "unknown",

	"coin", "",
	"doughnut", "torus", //Բ����
	"egg", "ellipsoid",
	"lemon", "ellipsoid",
	"mongo", "unknown",

	"orange", "ellipsoid",
	"pear", "unknown",
	"plum", "ellipsoid",
	"qiwi", "ellipsoid",
	"sachima", "column",

	"tomato", "ellipsoid" };
double CalVolume(Mat, double, Mat, double, string);

void exit_with_help()
{
	mexPrintf(
		"Usage: [imageMatrix] = DenseTrack('imageFile.jpg');\n"
		);
}

static void fake_answer(mxArray *plhs[])
{
	plhs[0] = mxCreateDoubleMatrix(0, 0, mxREAL);
}

void RGB2Gray(char *filename, mxArray *plhs[])
{
	// read the image
	Mat image = imread(filename);
	if (image.empty()) {
		mexPrintf("can't open input file %s\n", filename);
		fake_answer(plhs);
		return;
	}

	// convert it to gray format
	Mat gray;
	if (image.channels() == 3)
		cvtColor(image, gray, CV_RGB2GRAY);
	else
		image.copyTo(gray);

	// convert the result to Matlab-supported format for returning
	int rows = gray.rows;
	int cols = gray.cols;
	plhs[0] = mxCreateDoubleMatrix(rows, cols, mxREAL);
	double *imgMat;
	imgMat = mxGetPr(plhs[0]);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			*(imgMat + i + j * rows) = (int)gray.at<uchar>(i, j);

	return;
}

int char2int(char str[])
{
	int num = 0;
	for (int i = 0; i <strlen(str);i++)
	{
		int temp = str[i] - '0';
		num = num * 10 + temp;
	}
	return num;
}
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	//mexPrintf("nrhs_num %d\n", nrhs);
	if (nrhs == 7)
	{
		char top_filename[1024],side_filename[1024],shape[1024];
		mxGetString(prhs[0], top_filename, mxGetN(prhs[0]) + 1);//����ͼ�ļ���
		//mexPrintf("open input file %s\n", top_filename);
		mxGetString(prhs[2], side_filename, mxGetN(prhs[2]) + 1);//����ͼ�ļ���
		//mexPrintf("open input file %s\n", side_filename);
		mxGetString(prhs[4], shape, mxGetN(prhs[4]) + 1);//����ͼ�ļ���
		string s = shape;
		//mexPrintf("open input file %s\n", s);
		double *r,*r1;
		r = mxGetPr(prhs[1]);
		r1 = mxGetPr(prhs[3]);

		double *top_pixel=mxGetPr(prhs[5]);
		//mexPrintf("top_pixel %f\n", *top_pixel);
		double *side_pixel=mxGetPr(prhs[6]);
		//mexPrintf("side_pixel %f\n", *side_pixel);
		//mexPrintf("len %f--- \n", *(r+1));//+1?-1?
		//mexPrintf("len %f--- \n", *(r1 + 1));//+1?-1?
		if (top_filename == NULL||side_filename == NULL)
		{
			mexPrintf("Error: top or side filename is NULL\n");
			exit_with_help();
			return;
		}
		Mat top = imread(top_filename);
		Mat side = imread(side_filename);
		if (top.empty()||side.empty()) {
			mexPrintf("can't open input file %s or %s\n", top_filename,side_filename);
			fake_answer(plhs);
			return;
		}
		//mexPrintf("imread \n");//+1?-1?
		Mat top_mask(top.size(), CV_8UC1), side_mask(side.size(), CV_8UC1);
		Mat bgdModel, fgdModel;
		Rect rect(*r, *(r + 1), *(r + 2), *(r + 3)), rect1(*r1, *(r1 + 1), *(r1 + 2), *(r1 + 3));//(x,y,length,width)

		(top_mask(rect)).setTo(Scalar(GC_PR_FGD));
		grabCut(top, top_mask, rect, bgdModel, fgdModel, 10, GC_INIT_WITH_RECT);
		//mexPrintf("top grabcut \n");//+1?-1?
		(side_mask(rect1)).setTo(Scalar(GC_PR_FGD));
		grabCut(side, side_mask, rect1, bgdModel, fgdModel, 10, GC_INIT_WITH_RECT);
		//mexPrintf("side grabcut \n");//+1?-1?
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		double *volume =mxGetPr(plhs[0]);
		//imwrite("top.jpg", top_mask(rect));
		//imwrite("side.jpg", side_mask(rect1));
		*volume = CalVolume(top_mask(rect), *top_pixel, side_mask(rect1), *side_pixel, s);
		//mexPrintf("volume %f--- \n", *volume);//+1?-1?
		//int row_start = *r;//opencv�е�row��col��matlab��ͼ��Ľṹ��һ���ģ�x=width=rows��y=length=cols
		//int col_start = *(r + 1);
		//int rows = *(r+2);
		//int cols = *(r+3);
		//plhs[0] = mxCreateDoubleMatrix(rows, cols, mxREAL);
		//double *imgMat;
		//imgMat = mxGetPr(plhs[0]);
		//for (int i = 0; i < rows; i++)
		//	for (int j = 0; j < cols; j++)
		//		*(imgMat + i + j * rows) = (int)mask.at<uchar>(i+row_start, j+col_start);
		return;
		//RGB2Gray(filename, plhs);
	}
	else
	{
		exit_with_help();
		fake_answer(plhs);
		return;
	}
}


double CalVolume(Mat top, double top_ratio, Mat side, double side_ratio, string shape)//�����Ȳ���mat����ʽ���������ܲ���Ҫ����ά������
{
	double volume = 0.0;
	//int m = top.rows;
	//int n = side.rows;
	double sA = 0;//����ͼ��ر������ڼ������
	int LA_MAX = 0;//����ͼ�е�������������Ŀ
	int LB_MAX = 0;//����ͼ�е�������������Ŀ
	double sB = 0;//������������
	double sAE = 0;
	//for (int i = 0; i <side.rows; i++)//ͳ�Ʋ���ͼside��ʳ��ռ�õ����ظ���
	//{
	//	for (int j = 0; j < side.cols;j++)
	//	//int num = side.at<uchar>(i,0);
	//	cout << int(int(side.at<uchar>(i, j)))  << "  ";
	//}
	//cout << 100 - GC_PR_FGD << endl;
	//�޸�ϵ��
	//aA = side_ratio*(double)side.cols/top.rows;
	//cout << aA << endl;

	//�����崦��
	//������,������ֻ�õ��˲���ͼ��û�õ�����ͼ�����Բ���Ҫ���б���ϵ��������
	if (shape == "ellipsoid")
	{
		double sum = 0;
		for (int i = 0; i <side.rows; i++)//ͳ�Ʋ���ͼside��ʳ��ռ�õ����ظ���
		{
			int food_pixel_num = 0;
			for (int j = 0; j <side.cols; j++)
			{
				if (int(side.at<uchar>(i, j)) == GC_PR_FGD)//�����grabcut�ָ��õ���ǰ������
					food_pixel_num++;//�����˿��ܴ��ڵ�ʳ���пյ�״�������˴��пյĲ��ֲ�������
			}
			if (food_pixel_num)
			{
				sum += pow(food_pixel_num, 2);
				//cout << food_pixel_num << endl;
			}
		}
		volume = sum*pow(side_ratio, 3)*PI / 4;
		return volume;
	}




	//
	//���崦��
	//
	if (shape == "column")//����
	{
		//����ͼ����
		int heightA = top.rows;//���ڽ�ȡ��ʱ��ʳ����ÿһ�б�Ȼ�����أ�����ʳ��ĸ���ʵ�ǲ���Ҫ��ģ����ǣ��Է���һ
		for (int i = 0; i <top.rows; i++)//ͳ�Ƹ���ͼtop��ʳ��ռ�õ����ظ���
		{
			int food_pixel_num = 0;
			int start = 0;
			while (int(top.at<uchar>(i, start)) != GC_PR_FGD&&start<top.cols)
				start++;
			if (start < top.cols)
			{
				int end = top.cols - 1;
				while (int(top.at<uchar>(i, end)) != GC_PR_FGD&&end >= start)
					end--;
				if (end >= start)
				{
					sA += end - start + 1;
					LA_MAX = max(LA_MAX, end - start + 1);//���ʳ��ռ�õ����س���(���пղ���)
				}
				else
				{
					heightA--;
				}
			}
		}
		//����ͼ����
		int widthB = side.cols;//���ڽ�ȡ��ʱ��ʳ����ÿһ�б�Ȼ�����أ�����ʳ��ĸ���ʵ�ǲ���Ҫ��ģ����ǣ��Է���һ
		int heightB = side.rows;
		for (int i = 0; i <side.rows; i++)//ͳ�Ƹ���ͼtop��ʳ��ռ�õ����ظ���
		{
			int start = 0;
			while (int(side.at<uchar>(i, start)) != GC_PR_FGD&&start<side.cols)
				start++;
			if (start < side.cols)
			{
				int end = side.cols - 1;
				while (int(side.at<uchar>(i, end)) != GC_PR_FGD&&end >= start)
					end--;
				if (end >= start)
				{
					LB_MAX = max(LB_MAX, end - start + 1);//���ʳ��ռ�õ����س���(���пղ���)
				}
				else
					heightB--;

			}
		}
		double mean_height = 0;
		int rows = 0;
		for (int j = 0; j <side.cols; j++)//ͳ�Ƹ���ͼtop��ʳ��ռ�õ����ظ���
		{
			int start = 0;
			while (int(side.at<uchar>(start,j)) != GC_PR_FGD&&start<side.rows)
				start++;
			if (start < side.rows)
			{
				int end = side.rows - 1;
				while (int(side.at<uchar>(end,j)) != GC_PR_FGD&&end >= start)
					end--;
				if (end >= start)
				{
					mean_height =mean_height + end - start + 1;//���ʳ��ռ�õ����س���(���пղ���)
					rows++;
				}

			}
		}
		//��������ͼ����ϵ��
		top_ratio = min(top_ratio, side_ratio*(double)LB_MAX / heightA);
		//�����������
		volume = (sA*top_ratio*top_ratio) * (mean_height/rows * side_ratio);
		return volume;
	}

	//������״����
	//����������״ʱ����Ҫͳ�����ظ���




	//���ڸ���״Ҳ��Ҫ�õ�����ͼ������Ҳ��Ƚ���
	if (shape == "unknown")
	{
		//ͨ�ù�ʽ
		//����ͼ����
		int heightA = top.rows;//���ڽ�ȡ��ʱ��ʳ����ÿһ�б�Ȼ�����أ�����ʳ��ĸ���ʵ�ǲ���Ҫ��ģ����ǣ��Է���һ
		for (int i = 0; i <top.rows; i++)//ͳ�Ƹ���ͼtop��ʳ��ռ�õ����ظ���
		{
			int j = 0;
			for (; j < top.cols; j++)
			{
				if (int(top.at<uchar>(i, j)) == GC_PR_FGD)
					sA++;
			}
			//if (j == top.cols)
			//	heightA--;

			//int start = 0;
			//while (int(top.at<uchar>(i, start)) != GC_PR_FGD&&start<top.cols)
			//	start++;
			//if (start < top.cols)
			//{
			//	int end = top.cols - 1;
			//	while (int(top.at<uchar>(i, end)) != GC_PR_FGD&&end >= start)
			//		end--;
			//	sA += end - start + 1;
			//}
			//else
			//	heightA--;
		}

		//����ͼ����
		for (int i = 0; i <side.rows; i++)//ͳ�Ʋ���ͼside��ʳ��ռ�õ����ظ���
		{
			int food_pixel_num = 0;
			for (int j=0; j < side.cols; j++)
			{
				
				if (int(side.at<uchar>(i, j)) == GC_PR_FGD)
					food_pixel_num++;
			}
			sB += pow(food_pixel_num, 2);
			LB_MAX = max(LB_MAX, food_pixel_num);

			//int start = 0;
			//while (int(side.at<uchar>(i, start)) != GC_PR_FGD&&start<side.cols)
			//	start++;
			//if (start < side.cols)
			//{
			//	int end = side.cols - 1;
			//	while (int(side.at<uchar>(i, end)) != GC_PR_FGD&&end >= start)
			//		end--;
			//	sB += pow((end - start + 1), 2);
			//	LB_MAX = max(LB_MAX, end - start + 1);
			//}
		}
		//�޸ı���ϵ��
		top_ratio = min(top_ratio, side_ratio*(double)LB_MAX / heightA);
		//mexPrintf("top_pixel %f;   sA: %d; sB %d;LB_MAX:%d\n", top_ratio, sA, sB,LB_MAX);
		//cout << sA << "  " << sB << "  " << top_ratio << endl;
		//�������
		volume = sA  * pow(top_ratio, 2) * sB*side_ratio / pow(LB_MAX, 2);
		return volume;
	}

	//���ڸ���״Ҳ��Ҫ�õ�����ͼ������Ҳ��Ƚ���
	if (shape == "grape")
	{
		//used for grape
		//����ͼ����
		int heightA = top.rows;//���ڽ�ȡ��ʱ��ʳ����ÿһ�б�Ȼ�����أ�����ʳ��ĸ���ʵ�ǲ���Ҫ��ģ����ǣ��Է���һ
		for (int i = 0; i <top.rows; i++)//ͳ�Ƹ���ͼtop��ʳ��ռ�õ����ظ���
		{
			int j = 0;
			for (; j < top.cols; j++)
			{
				if (int(top.at<uchar>(i, j)) == GC_PR_FGD)
					sA++;
			}
			//if (j == top.cols)
			//	heightA--;

			//int start = 0;
			//while (int(top.at<uchar>(i, start)) != GC_PR_FGD&&start<top.cols)
			//	start++;
			//if (start < top.cols)
			//{
			//	int end = top.cols - 1;
			//	while (int(top.at<uchar>(i, end)) != GC_PR_FGD&&end >= start)
			//		end--;
			//	sA += end - start + 1;
			//}
			//else
			//	heightA--;
		}

		//����ͼ����
		for (int i = 0; i <side.rows; i++)//ͳ�Ʋ���ͼside��ʳ��ռ�õ����ظ���
		{
			int food_pixel_num = 0;
			for (int j = 0; j < side.cols; j++)
			{
				if (int(side.at<uchar>(i, j)) == GC_PR_FGD)
					food_pixel_num++;
			}
			sB += pow(food_pixel_num*0.9, 2);
			LB_MAX = max(LB_MAX, food_pixel_num);

			//int start = 0;
			//while (int(side.at<uchar>(i, start)) != GC_PR_FGD&&start<side.cols)
			//	start++;
			//if (start < side.cols)
			//{
			//	int end = side.cols - 1;
			//	while (int(side.at<uchar>(i, end)) != GC_PR_FGD&&end >= start)
			//		end--;
			//	sB += pow((end - start + 1), 2);
			//	LB_MAX = max(LB_MAX, end - start + 1);
			//}
		}
		//�޸ı���ϵ��
		top_ratio = min(top_ratio, side_ratio*(double)LB_MAX / heightA);
		//mexPrintf("top_pixel %f;   sA: %f; sB %f;LB_MAX:%d\n", top_ratio, sA, sB,LB_MAX);
		//cout << sA << "  " << sB << "  " << top_ratio << endl;
		//�������
		volume = sA  * pow(top_ratio, 2) * sB*side_ratio / pow(LB_MAX, 2);
		return volume;
	}

	if (shape == "torus")//̥�壬������Ѹ������
	{
		//����ͼ����
		int heightA = top.rows;
		for (int i = 0; i < top.rows; i++)//ͳ�Ƹ���ͼA��ʳ��ռ�õ����ظ���
		{
			int food_pixel_num = 0;
			int start = 0;
			while (int(top.at<uchar>(i, start)) != GC_PR_FGD&&start < top.cols)
				start++;
			if (start < top.cols)
			{
				int end = top.cols - 1;
				while (int(top.at<uchar>(i, end)) != GC_PR_FGD&&end >= start)
					end--;
				if (end >= start)
				{
					for (int j = start; j <= end; j++)
					{
						if (top.at<uchar>(i, j) == GC_PR_FGD)
							sA++;
						else
							sAE++;
					}
				}
				else
					heightA--;
			}
		}

		//����ͼ����
		int widthB = side.cols;
		int heightB = side.rows;
		for (int i = 0; i <side.rows; i++)//ͳ�Ʋ���ͼside��ʳ��ռ�õ����ظ���
		{
			int food_pixel_num = 0;
			int start = 0;
			while (int(side.at<uchar>(i, start)) != GC_PR_FGD&&start<side.cols)
				start++;
			if (start < side.cols)
			{
				int end = side.cols - 1;
				while (int(side.at<uchar>(i, end)) != GC_PR_FGD&&end >= start)
					end--;
				if (end >= start)
					LB_MAX = max(LB_MAX, end - start + 1);
				else
					heightB--;
			}
		}
		//�޸ı���ϵ��
		top_ratio = min(top_ratio, side_ratio*(double)LB_MAX / heightA);
		//mexPrintf("top_pixel %f;   sA: %d; sAE %d;LB_MAX:%d\n", top_ratio, sA, sAE, widthB);
		//�������
		volume = pow(PI, 1.5) * pow(heightB, 2) * top_ratio * pow(side_ratio, 2) * (sqrt(sA + sAE) + sqrt(sAE)) / 4;
		return volume;
	}

	return 0;
}
//int main(int argc, char** argv)
//{
//
//	string filename = "E:\\ECUSTFDD\\banana\\banana001-1.jpg";
//	Mat image = imread(filename, 1);
//	if (image.empty())
//	{
//		cout << "\n Durn, couldn't read image filename " << filename << endl;
//		return 1;
//	}
//
//	Mat mask(image.size(), CV_8UC1);
//	Mat bgdModel, fgdModel;
//	Rect rect(297, 128, 445, 293);//(x,y,length,width)
//	(mask(rect)).setTo(Scalar(GC_PR_FGD));
//	grabCut(image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);
//
//	Mat img = mask(rect);
//	double volume = CalVolume(img, 1, img, 1, "column");
//	double volume2 = CalVolume(img, 1, img, 1, "unknown");
//	double volume3 = CalVolume(img, 1, img, 1, "torus");
//	double volume4 = CalVolume(img, 1, img, 1, "ellipsoid");
//	printf("result:%f;%f;%f;%f;", volume, volume2, volume3, volume4);
//	getchar();
//	return 0;
//}