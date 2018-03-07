#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QLabel>

#include <assert.h>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <func.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),"",tr("Image File(*.bmp *.jpg *.jpeg *.png)"));
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    std::string name = code->fromUnicode(filename).data();
    image = imread(name);
    if(!image.data)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Image data is null!"));
        msgBox.exec();
    }
    else
    {
        //image������ͼ��
        Mat input = imread(name);
        cvtColor(image,image,CV_BGR2RGB);
        img = QImage((const unsigned char*)(image.data),image.cols,image.rows, image.cols*image.channels(),  QImage::Format_RGB888);
        img = img.scaled(ui->input_image->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        //ui->input_image->setScaledContents(true);
        ui->input_image->setAlignment(Qt::AlignCenter);
        ui->input_image->clear();
        ui->input_image->setPixmap(QPixmap::fromImage(img));
        ui->input_image->resize(ui->input_image->pixmap()->size());
        size300(input);
        //�ҶȻ�
        cvtColor(input, input, CV_BGR2GRAY);
        //��ֵ�˲�
        medianBlur(input, input,3);
        //��ֵ��
        binary(input);
        img = cvMat2QImage(input);
        img = img.scaled(ui->ori_binarization->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        //ui->input_image->setScaledContents(true);
        ui->ori_binarization->setAlignment(Qt::AlignCenter);
        ui->ori_binarization->clear();
        ui->ori_binarization->setPixmap(QPixmap::fromImage(img));
        ui->ori_binarization->resize(ui->ori_binarization->pixmap()->size());
        //�ж��Ƿ�Ϊ��ɫ����
        if (double(countNonZero(input)) / double(input.rows) / double(input.cols) > 0.5)
        {
            input = reverse_pic(input);
        }
        //�ж��ʻ���ϸ
        Mat temp = input(Rect(round(input.cols*0.1),round(input.rows*0.1),round(input.cols*0.8),round(input.rows*0.8)));
        int sum_point = countNonZero(temp);
        if(double(sum_point) / double(temp.rows) / double(temp.cols) < 0.28 )
        {
            Mat element = getStructuringElement(MORPH_RECT,Size(3,3));
            dilate(input,input,element);
        }
        input = add_frame(input);
        //�������
        vector<vector<Point>> contours;
        Mat tmpImage = input(cv::Rect(0, 0, round(input.cols*0.15), input.rows));
        contours = bwareaopen(tmpImage, 30);
        tmpImage = input(cv::Rect(round(input.cols*0.15), 0, input.cols - round(input.cols*0.15), input.rows));
        contours = bwareaopen(tmpImage, 275);
        input = reverse_pic(input);
        //�ͷ�contours
        vector<vector<Point>>().swap(contours);
        Mat H = H_Shadwo(input);
        Mat V = V_Shadwo(input,0);
        //�ü�ͼ��
        //ˮƽ����ü�
        int H1 = H1_cut_value(H, input);
        int H2 = H2_cut_value(H, input);
        input = input(cv::Rect(0, H1, input.cols,H2 - H1));
        V = V_Shadwo(input,0,"�ü���Ĵ�ֱͶӰ");
        //��ֱ����ü�
        int W1 = W1_cut_value(V);
        int W2 = W2_cut_value(V);
        input = input(Rect(W1, 0, W2 - W1, input.rows));
        //��ʾͼƬ
        img = cvMat2QImage(input);
        img = img.scaled(ui->binarization->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        //ui->input_image->setScaledContents(true);
        ui->binarization->setAlignment(Qt::AlignCenter);
        ui->binarization->clear();
        ui->binarization->setPixmap(QPixmap::fromImage(img));
        ui->binarization->resize(ui->binarization->pixmap()->size());
        //�ַ��ָ�
        vector<Mat> character(7);
        V = V_Shadwo(input);
        //14���ָ���
        vector<int> W = seg_character(V,input);
        if (W[14] == 0)
        {
            for (int i = 0; i < 7; i++)
            {
                if (W[i * 2 + 1] - W[i * 2] > round(input.cols*0.135))
                {
                    W[14] += 1;
                }
            }
        }
        //����ָ����λ��Ϊ0������ָ�
        if (W[14] != 0)
        {
            W = force_seg(V, W);
        }
        for (int i = 0; i < 7; i++)
        {
            character[i] = input(Rect(W[i * 2], 0, W[i * 2 + 1] - W[i * 2], input.rows));
        }
        //�������ñ���
        vector<int>().swap(W);
        input.release();
        tmpImage.release();
        H.release();
        V.release();
        //����ʶ��
        vector<QString> result = recognition(character);
        //ѭ����label����ʾͼƬ
        for(int i = 0;i < 7;i++)
        {
            img = cvMat2QImage(character[i]);
            QLabel *label = findChild<QLabel*>("c"+QString::number(i+1));
            img = img.scaled(label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
            label->setAlignment(Qt::AlignCenter);
            label->clear();
            label->setPixmap(QPixmap::fromImage(img));
            label->resize(ui->binarization->pixmap()->size());
        }
        QString ans = result[0];
        for (int i = 1; i < 7; i++)
        {
            ans += result[i];
        }
        ui->ans->setText(ans);
    }
}

