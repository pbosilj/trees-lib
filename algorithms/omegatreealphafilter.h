/// \file examples/omegatreealphafilter.h
/// \author Petra Bosilj

#ifndef OMEGATREEALPHAFILTER_H_INCLUDED
#define OMEGATREEALPHAFILTER_H_INCLUDED

#include "alphatreedualmax.h"

#include "../structures/node.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <vector>


namespace fl{
    Node *omegaTreeAlphaFilter(const cv::Mat &img);
    template <class Function1, class Function2>
    Node *omegaTreeAlphaFilter(const std::vector <cv::Mat> &img, Function1 alphaDistance, Function2 omegaDistance);
}

#include "omegatreealphafilter.tpp"

#endif // OMEGATREEALPHAFILTER_H_INCLUDED
