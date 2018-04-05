#include "commontreedetail.h"

#include <iostream>

namespace fl{
    int detail::maxTreeGrayLvlAssign::operator() (double myLvl, const std::vector <pxCoord> &/*ownElems*/, const std::vector<int> &/*chGL*/, const std::vector<int> &/*chSz*/) const{
        return myLvl;
    }

    int detail::alphaTreeGrayLvlAssign::operator() (double /*myLvl*/, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const{
        if (chGL.empty()){
            return (int)detail::getCvMatElem(this->img, ownElems.front().X, ownElems.front().Y);
        }

        long long total=0L, areatot=0L;

        for (int i=0, szi = chGL.size(); i < szi; total+=chSz[i]*chGL[i], areatot+=chSz[i], ++i );

        return (int)(total/areatot);
    }

    std::vector<int> detail::alphaTreeGrayLvlHyperAssign::operator() (double /*myLvl*/, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chHGL, const std::vector<int> &chSz) const{
        if (chHGL.empty()){
            std::vector<int> rValue;
            for (int i=0, szi=imgs.size(); i < szi; ++i)
                rValue.emplace_back((int)detail::getCvMatElem(this->imgs[i], ownElems.front().X, ownElems.front().Y));
            return rValue;
        }

        long long areatot=0L;
        std::vector <long long> total(chHGL[0].size(), 0L);

        for (int i=0, szi = chHGL.size(); i < szi; ++i ){
            for (int j=0, szj = chHGL[i].size(); j < szj; ++j)
                total[j]+=chSz[i]*chHGL[i][j];
            areatot+=chSz[i];
        }
        std::vector <int> rValue;
        for (int j=0, szj = chHGL[0].size(); j < szj; ++j)
            rValue.push_back((int)(total[j]/areatot));

        return rValue;
    }

    int detail::alphaTreeMinLvlAssign::operator() (double /*myLvl*/, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &/*chSz*/) const{
        if (chGL.empty()){
            return (int)detail::getCvMatElem(this->img, ownElems.front().X, ownElems.front().Y);
        }

        int minV = chGL[0];

        for (int i=1, szi = chGL.size(); i < szi; minV = std::min(minV, chGL[i]), ++i );

        return minV;
    }
    std::vector<int> detail::alphaTreeMinLvlHyperAssign::operator() (double /*myLvl*/, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chHGL, const std::vector<int> &/*chSz*/) const{
        if (chHGL.empty()){
            std::vector<int> rValue;
            for (int i=0, szi=imgs.size(); i < szi; ++i)
                rValue.emplace_back((int)detail::getCvMatElem(this->imgs[i], ownElems.front().X, ownElems.front().Y));
            return rValue;
        }

        std::vector<int> minV = chHGL[0];

        for (int i=1, szi = chHGL.size(); i < szi; ++i ){
            for (int j=0, szj = chHGL[i].size(); j < szj; ++j)
                minV[j] = std::min(minV[j], chHGL[i][j]);
        }

        return minV;
    }

    int detail::alphaTreeMaxLvlAssign::operator() (double /*myLvl*/, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &/*chSz*/) const{
        if (chGL.empty()){
            return (int)detail::getCvMatElem(this->img, ownElems.front().X, ownElems.front().Y);
        }

        int maxV = chGL[0];

        for (int i=1, szi = chGL.size(); i < szi; maxV = std::max(maxV, chGL[i]), ++i );

        return maxV;
    }

    std::vector<int> detail::alphaTreeMaxLvlHyperAssign::operator() (double /*myLvl*/, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chHGL, const std::vector<int> &/*chSz*/) const{
        if (chHGL.empty()){
            std::vector<int> rValue;
            for (int i=0, szi=imgs.size(); i < szi; ++i)
                rValue.emplace_back((int)detail::getCvMatElem(this->imgs[i], ownElems.front().X, ownElems.front().Y));
            return rValue;
        }

        std::vector<int> minV = chHGL[0];

        for (int i=1, szi = chHGL.size(); i < szi; ++i ){
            for (int j=0, szj = chHGL[i].size(); j < szj; ++j)
                minV[j] = std::max(minV[j], chHGL[i][j]);
        }

        return minV;
    }

    double detail::getCvMatElem(const cv::Mat &img, const std::pair<int, int> &coord){
        return getCvMatElem(img, coord.first, coord.second);
    }
    double detail::getCvMatElem(const cv::Mat &img, int X, int Y){
        if (img.type() == CV_8U)
            return img.at<uchar>(Y, X);
        else if (img.type() == CV_32S)
            return img.at<int32_t>(Y, X);
        else if (img.type() == CV_32F)
            return img.at<float>(Y, X);
        else if (img.type() == CV_16S)
            return img.at<short>(Y, X);
        else if (img.type() == CV_16U)
            return img.at<unsigned short>(Y, X);

        return img.at<uchar>(Y, X);
    }
    double detail::getCvMatMax(const cv::Mat &img){
        if (img.type() == CV_8U)
            return *std::max_element(img.begin<uchar>(), img.end<uchar>());
        else if (img.type() == CV_32S)
            return *std::max_element(img.begin<int32_t>(), img.end<int32_t>());
        else if (img.type() == CV_32F)
            return *std::max_element(img.begin<float>(), img.end<float>());
        else if (img.type() == CV_16S)
            return *std::max_element(img.begin<short>(), img.end<short>());
        else if (img.type() == CV_16U)
            return *std::max_element(img.begin<unsigned short>(), img.end<unsigned short>());
        return *std::max_element(img.begin<uchar>(), img.end<uchar>());
    }

    double detail::getCvMatMin(const cv::Mat &img){
        if (img.type() == CV_8U)
            return *std::min_element(img.begin<uchar>(), img.end<uchar>());
        else if (img.type() == CV_32S)
            return *std::min_element(img.begin<int32_t>(), img.end<int32_t>());
        else if (img.type() == CV_32F)
            return *std::min_element(img.begin<float>(), img.end<float>());
        else if (img.type() == CV_16S)
            return *std::min_element(img.begin<short>(), img.end<short>());
        else if (img.type() == CV_16U)
            return *std::min_element(img.begin<unsigned short>(), img.end<unsigned short>());
        return *std::min_element(img.begin<uchar>(), img.end<uchar>());
    }
    cv::MatConstIterator detail::getCvMatBegin(const cv::Mat &img){
        if (img.type() == CV_8U)
            return img.begin<uchar>();
        else if (img.type() == CV_32S)
            return img.begin<int32_t>();
        else if (img.type() == CV_32F)
            return img.begin<float>();
        else if (img.type() == CV_16S)
            return img.begin<short>();
        else if (img.type() == CV_16U)
            return img.begin<ushort>();
        return img.begin<uchar>();
    }
    cv::MatConstIterator detail::getCvMatEnd(const cv::Mat &img){
        if (img.type() == CV_8U)
            return img.end<uchar>();
        else if (img.type() == CV_32S)
            return img.end<int32_t>();
        else if (img.type() == CV_32F)
            return img.end<float>();
        else if (img.type() == CV_16S)
            return img.end<short>();
        else if (img.type() == CV_16U)
            return img.end<ushort>();
        return img.end<uchar>();
    }
    double detail::getDerefCvMatConstIterator(cv::MatConstIterator &it, int matType){
        if (matType == CV_8U)
            return (double)(*(uchar *)(*it));
        else if (matType == CV_32S)
            return (double)(*(int32_t *)(*it));
        else if (matType == CV_32F)
            return (double)(*(float *)(*it));
        else if (matType == CV_16S)
            return (double)(*(short *)(*it));
        else if (matType == CV_16U)
            return (double)(*(ushort *)(*it));
        return (double)(*(uchar *)(*it));
    }
}
