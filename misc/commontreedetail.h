#ifndef COMMONTREEDETAIL_H
#define COMMONTREEDETAIL_H

#include <vector>

#include "pixels.h"

#include <opencv2/imgproc/imgproc.hpp>

namespace fl{
    namespace detail{
        class maxTreeGrayLvlAssign{
            public:
                maxTreeGrayLvlAssign(const cv::Mat &img) : img(img) {}
                int operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const;
            private:
                const cv::Mat &img;
        };

        class alphaTreeGrayLvlAssign{
            public:
                alphaTreeGrayLvlAssign(const cv::Mat &img) : img(img) {}
                int operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const;
            private:
                const cv::Mat &img;
        };

        class alphaTreeGrayLvlHyperAssign{
            public:
                alphaTreeGrayLvlHyperAssign(const std::vector<cv::Mat> &imgs) : imgs(imgs) {}
                std::vector<int> operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chGL, const std::vector<int> &chSz) const;
            private:
                const std::vector<cv::Mat> &imgs;
        };

        class alphaTreeMinLvlAssign{
            public:
                alphaTreeMinLvlAssign(const cv::Mat &img) : img(img) {}
                int operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const;
            private:
                const cv::Mat &img;
        };

        class alphaTreeMinLvlHyperAssign{
            public:
                alphaTreeMinLvlHyperAssign(const std::vector<cv::Mat> &imgs) : imgs(imgs) {}
                std::vector<int> operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chGL, const std::vector<int> &chSz) const;
            private:
                const std::vector<cv::Mat> &imgs;
        };

        class alphaTreeMaxLvlAssign{
            public:
                alphaTreeMaxLvlAssign(const cv::Mat &img) : img(img) {}
                int operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const;
            private:
                const cv::Mat &img;
        };

        class alphaTreeMaxLvlHyperAssign{
            public:
                alphaTreeMaxLvlHyperAssign(const std::vector<cv::Mat> &imgs) : imgs(imgs) {}
                std::vector<int> operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chGL, const std::vector<int> &chSz) const;
            private:
                const std::vector<cv::Mat> &imgs;
        };

        double getCvMatElem(const cv::Mat &img, const std::pair<int, int> &coord);
        double getCvMatElem(const cv::Mat &img, int X, int Y);
        cv::MatConstIterator getCvMatBegin(const cv::Mat &img);
        cv::MatConstIterator getCvMatEnd(const cv::Mat &img);
        double getDerefCvMatConstIterator (cv::MatConstIterator &it, int matType);

        double getCvMatMax(const cv::Mat &img);
        double getCvMatMin(const cv::Mat &img);

        template <class Compare>
        double getCvMatMax(const cv::Mat &img, Compare cmp);

        template <class Compare>
        double getCvMatMin(const cv::Mat &img, Compare cmp);

    }
}

#include "commontreedetail.tpp"

#endif // COMMONTREEDETAIL_H
