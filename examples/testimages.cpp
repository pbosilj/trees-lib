/// \file examples/testimages.cpp
/// \author Petra Bosilj
/// \date 08/10/2019

#include "testimages.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/************** TEST IMAGES *********************/

cv::Mat &testImage1(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(4, 4, CV_8U);
    if (firstCall){
        for (int i=0; i < 4; ++i){
            for (int j=0; j < 2; ++j){
                image.at<uchar>(i,j) = (uchar)255;
            }
            for (int j=3; j < 4; ++j)
                image.at<uchar>(i,j) = (uchar)180;

        }
        image.at<uchar>(0,2) = 180;
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage2(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(70, 110, CV_8U);
    if (firstCall){
        for (int i=0; i < 110; ++i){
            for (int j=0; j < 10;++j){
                image.at<uchar>(0+j, i) = (uchar)255;
                image.at<uchar>(60+j, i) = (uchar)255;

                if (i < 70){
                    image.at<uchar>(i,  0+j) = (uchar)255;
                    image.at<uchar>(i, 100+j) = (uchar)255;
                }
            }
        }
        for (int i=10; i < 100; ++i){
            for (int j=0; j < 10; ++j){
                image.at<uchar>(10+j, i) = (uchar)180;
                image.at<uchar>(50+j, i) = (uchar)180;
                if (i < 60){
                    image.at<uchar>(i, 10+j) = (uchar)180;
                    image.at<uchar>(i, 50+j) = (uchar)180;
                    image.at<uchar>(i, 90+j) = (uchar)180;
                }
            }
        }
        for (int i=20; i < 50; ++i)
            for (int j=20; j < 50; ++j)
                image.at<uchar>(i,j) = (uchar)255;
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage3(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(7, 11, CV_8U);
    if (firstCall){
        for (int i=0; i < 11; ++i){
            image.at<uchar>(0, i) = (uchar)210;
            image.at<uchar>(6, i) = (uchar)210;
            if (i < 7){
                image.at<uchar>(i,  0) = (uchar)210;
                image.at<uchar>(i, 10) = (uchar)210;
            }
        }
        for (int i=1; i < 10; ++i){
            image.at<uchar>(1, i) = (uchar)180;
            image.at<uchar>(5, i) = (uchar)180;
            if (i < 6){
                image.at<uchar>(i, 1) = (uchar)180;
                image.at<uchar>(i, 5) = (uchar)180;
                image.at<uchar>(i, 9) = (uchar)180;
            }
        }
        for (int i=2; i < 5; ++i)
            for (int j=2; j < 5; ++j)
                image.at<uchar>(i,j) = (uchar)200;
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage4(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(9, 9, CV_8U);
    if (firstCall){
        for (int i=1; i < 8; ++i){
            image.at<uchar>(1, i) = (uchar)50;
            image.at<uchar>(7, i) = (uchar)50;
            image.at<uchar>(i, 1) = (uchar)50;
            image.at<uchar>(i, 7) = (uchar)50;
        }
        for (int i=2; i < 7; ++i)
            for (int j=2; j < 7; ++j){
                if (i == 5 && (j==3 || j == 4))
                    image.at<uchar>(i,j) = (uchar)255;
                else if (!(i == 3 && (j==4 || j == 5)))
                    image.at<uchar>(i,j) = (uchar)180;
            }
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage5(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(2, 2, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 255;
        image.at<uchar>(1,0) = 150;
        image.at<uchar>(0,1) = 150;
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage6(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(3, 4, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 1;
        image.at<uchar>(1,0) = 4;
        image.at<uchar>(2,0) = 7;
        image.at<uchar>(0,1) = 3;
        image.at<uchar>(1,1) = 2;
        image.at<uchar>(2,1) = 6;
        image.at<uchar>(0,2) = 7;
        image.at<uchar>(1,2) = 3;
        image.at<uchar>(2,2) = 5;
        image.at<uchar>(0,3) = 2;
        image.at<uchar>(1,3) = 1;
        image.at<uchar>(2,3) = 4;
    }
    return image;
}

cv::Mat &testImage7(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(6, 6, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 6;
        image.at<uchar>(1,0) = 4;
        image.at<uchar>(2,0) = 4;
        image.at<uchar>(3,0) = 8;
        image.at<uchar>(4,0) = 4;
        image.at<uchar>(5,0) = 3;
        image.at<uchar>(0,1) = 7;
        image.at<uchar>(1,1) = 3;
        image.at<uchar>(2,1) = 9;
        image.at<uchar>(3,1) = 8;
        image.at<uchar>(4,1) = 3;
        image.at<uchar>(5,1) = 4;
        image.at<uchar>(0,2) = 8;
        image.at<uchar>(1,2) = 7;
        image.at<uchar>(2,2) = 1;
        image.at<uchar>(3,2) = 1;
        image.at<uchar>(4,2) = 6;
        image.at<uchar>(5,2) = 5;
        image.at<uchar>(0,3) = 6;
        image.at<uchar>(1,3) = 5;
        image.at<uchar>(2,3) = 0;
        image.at<uchar>(3,3) = 2;
        image.at<uchar>(4,3) = 8;
        image.at<uchar>(5,3) = 9;
        image.at<uchar>(0,4) = 8;
        image.at<uchar>(1,4) = 9;
        image.at<uchar>(2,4) = 7;
        image.at<uchar>(3,4) = 6;
        image.at<uchar>(4,4) = 2;
        image.at<uchar>(5,4) = 8;
        image.at<uchar>(0,5) = 9;
        image.at<uchar>(1,5) = 4;
        image.at<uchar>(2,5) = 7;
        image.at<uchar>(3,5) = 3;
        image.at<uchar>(4,5) = 2;
        image.at<uchar>(5,5) = 1;
    }
    return image;
}

cv::Mat &testImage8(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(2, 5, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 3;
        image.at<uchar>(1,0) = 4;

        image.at<uchar>(0,1) = 3;
        image.at<uchar>(1,1) = 1;

        image.at<uchar>(0,2) = 1;
        image.at<uchar>(1,2) = 2;

        image.at<uchar>(0,3) = 4;
        image.at<uchar>(1,3) = 3;

        image.at<uchar>(0,4) = 2;
        image.at<uchar>(1,4) = 1;
    }
    return image;
}

cv::Mat &testImage9(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(5, 6, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 1;
        image.at<uchar>(1,0) = 1;
        image.at<uchar>(2,0) = 1;
        image.at<uchar>(3,0) = 1;
        image.at<uchar>(4,0) = 1;

        image.at<uchar>(0,1) = 1;
        image.at<uchar>(1,1) = 0;
        image.at<uchar>(2,1) = 0;
        image.at<uchar>(3,1) = 0;
        image.at<uchar>(4,1) = 1;

        image.at<uchar>(0,2) = 1;
        image.at<uchar>(1,2) = 0;
        image.at<uchar>(2,2) = 0;
        image.at<uchar>(3,2) = 0;
        image.at<uchar>(4,2) = 1;

        image.at<uchar>(0,3) = 1;
        image.at<uchar>(1,3) = 0;
        image.at<uchar>(2,3) = 0;
        image.at<uchar>(3,3) = 0;
        image.at<uchar>(4,3) = 1;

        image.at<uchar>(0,4) = 1;
        image.at<uchar>(1,4) = 4;
        image.at<uchar>(2,4) = 4;
        image.at<uchar>(3,4) = 4;
        image.at<uchar>(4,4) = 1;

        image.at<uchar>(0,5) = 1;
        image.at<uchar>(1,5) = 1;
        image.at<uchar>(2,5) = 1;
        image.at<uchar>(3,5) = 1;
        image.at<uchar>(4,5) = 1;

    }
    return image;
}

cv::Mat &testImage10(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(5, 4, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = image.at<uchar>(0,1) = image.at<uchar>(0,2) = image.at<uchar>(0,3) = 70;
        image.at<uchar>(4,0) = image.at<uchar>(4,1) = image.at<uchar>(4,2) = image.at<uchar>(4,3) = 70;
        image.at<uchar>(1,0) = image.at<uchar>(1,3) = image.at<uchar>(2,0) = image.at<uchar>(2,3) = image.at<uchar>(3,0) = image.at<uchar>(3,3) = 70;
        image.at<uchar>(1,1) = 66;
        image.at<uchar>(2,1) = 69;
        image.at<uchar>(3,1) = 67;
        image.at<uchar>(1,2) = 59;
        image.at<uchar>(2,2) = 63;
        image.at<uchar>(3,2) = 60;

    }
    return image;
}

cv::Mat &testImage11(void){
    static cv::Mat image = cv::Mat::zeros(1024, 1, CV_8U);
    return image;
}

cv::Mat &testImage12(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(4, 5, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 1;
        image.at<uchar>(1,0) = 1;
        image.at<uchar>(2,0) = 1;
        image.at<uchar>(3,0) = 0;

        image.at<uchar>(0,1) = 4;
        image.at<uchar>(1,1) = 6;
        image.at<uchar>(2,1) = 6;
        image.at<uchar>(3,1) = 0;

        image.at<uchar>(0,2) = 4;
        image.at<uchar>(1,2) = 6;
        image.at<uchar>(2,2) = 6;
        image.at<uchar>(3,2) = 0;

        image.at<uchar>(0,3) = 0;
        image.at<uchar>(1,3) = 0;
        image.at<uchar>(2,3) = 0;
        image.at<uchar>(3,3) = 0;

        image.at<uchar>(0,4) = 5;
        image.at<uchar>(1,4) = 0;
        image.at<uchar>(2,4) = 9;
        image.at<uchar>(3,4) = 2;

    }
    return image;
}
