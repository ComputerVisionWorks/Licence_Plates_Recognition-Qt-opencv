#ifndef FUNC_H
#define FUNC_H

#include <QString>
#include <QImage>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "mainwindow.h"

using namespace cv;
using namespace std;
//����ͼ���С
void size300(Mat &src);
//ȡ��
Mat reverse_pic(Mat src);
//��ֵ��
void binary(Mat &src);
//�����߿�Ӱ��
Mat add_frame(Mat src);
//ȥ��С��������룺ԭͼ����ȥС���������
void bwareaopen(Mat &src, int n);
//��ֱͶӰ
Mat V_Shadwo(Mat &src,int a = 0,string str = "��ֱͶӰ");
//��ˮƽͶӰ
Mat H_Shadwo(Mat &src,int a = 0,string str = "ˮƽͶӰ");
//ˮƽ�ü�H1(���룺��ֱͶӰMat��ԭͼ�����ˮƽ�ü���һ��y����)
int H1_cut_value(Mat &H,Mat &input);
//ˮƽ�ü�H2(���룺��ֱͶӰMat��ԭͼ�����ˮƽ�ü��ڶ���y����)
int H2_cut_value(Mat &H,Mat &input);
//��ֱ�ü�W1
int W1_cut_value(Mat &V);
//��ֱ�ü�W2
int W2_cut_value(Mat &V);
//�ַ��ָ�
vector<int> seg_character(Mat &V,Mat &input);
//ǿ�Ʒָ�
vector<int> force_seg(Mat &V,vector<int> W);
//�ַ�ʶ��
vector<QString> recognition(vector<Mat> &character);
//�ַ�ȥ�ڱ�(��ֱ�ü�)
void character_cut_H(Mat &src,int k);
//�ַ�ȥ�ڱ�(ˮƽ�ü�)
void character_cut_V(Mat &src,int k);
//QImage��Matת��
QImage cvMat2QImage(const cv::Mat& mat);
//����ƥ��
int featureMatch(Mat &src,int index);

#endif // FUNC_H
