#ifndef TPP_COMMONTREEDETAIL
#define TPP_COMMONTREEDETAIL

#include "commontreedetail.h"

namespace fl{

    template <class Compare>
    double detail::getCvMatMax(const cv::Mat &img, Compare cmp){
        if (img.type() == CV_8U)
            return *std::max_element(img.begin<uchar>(), img.end<uchar>(), cmp);
        else if (img.type() == CV_32S)
            return *std::max_element(img.begin<int32_t>(), img.end<int32_t>(), cmp);
        else if (img.type() == CV_32F)
            return *std::max_element(img.begin<float>(), img.end<float>(), cmp);
        else if (img.type() == CV_16S)
            return *std::max_element(img.begin<short>(), img.end<short>(), cmp);
        else if (img.type() == CV_16U)
            return *std::max_element(img.begin<unsigned short>(), img.end<unsigned short>(), cmp);
        return *std::max_element(img.begin<uchar>(), img.end<uchar>(), cmp);
    }

    template <class Compare>
    double detail::getCvMatMin(const cv::Mat &img, Compare cmp){
        if (img.type() == CV_8U)
            return *std::min_element(img.begin<uchar>(), img.end<uchar>(), cmp);
        else if (img.type() == CV_32S)
            return *std::min_element(img.begin<int32_t>(), img.end<int32_t>(), cmp);
        else if (img.type() == CV_32F)
            return *std::min_element(img.begin<float>(), img.end<float>(), cmp);
        else if (img.type() == CV_16S)
            return *std::min_element(img.begin<short>(), img.end<short>(), cmp);
        else if (img.type() == CV_16U)
            return *std::min_element(img.begin<unsigned short>(), img.end<unsigned short>(), cmp);
        return *std::min_element(img.begin<uchar>(), img.end<uchar>(), cmp);
    }

}

#endif

