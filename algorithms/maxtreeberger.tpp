/// \file algorithms/maxtreeberger.tpp
/// \author Petra Bosilj

#ifndef TPP_MAXTREEBERGER
#define TPP_MAXTREEBERGER

#include "maxtreeberger.h"
#include "../misc/pixels.h"
#include "../misc/commontreedetail.h"

#include "../structures/node.h"
#include "../structures/inclusionnode.h"

#include <vector>
#include <utility>

#include <iostream>

namespace fl{

    namespace detail{
        pxCoord findRoot(const pxCoord &px, std::vector<std::vector<pxCoord> > &zpar);
        void assignNewNode(std::vector <Node *> &nodes, std::vector<std::vector<int> > &nodeIndices,
                           const pxCoord &coord, const int &value);

        class imgIndexGenerator {
            public:
                imgIndexGenerator(const cv::Mat &img) : img(img), hits(0) {}
                pxCoord operator()();
            private:
             const cv::Mat &img;
             int hits;
             pxCoord countToCoord(int count);
        };

        template <typename Compare>
        class imgIndexComp{
            private:
                const Compare &myComp;
                const cv::Mat &img;
            public:
                imgIndexComp(Compare &cmp, const cv::Mat &img) : myComp(cmp), img(img) { }
                bool operator() (const pxCoord &lhs, const pxCoord &rhs);
        };

        template <typename Compare>
        void sortImgElems(const cv::Mat &img, Compare pxOrder, std::vector <pxCoord> &sorted, const cv::Mat &mask = cv::Mat());
    }

    /// \details \copydetails fl::maxTreeNister(const cv::Mat &img, Compare pxOrder, pxType curType = regular)
    template <typename Compare>
    Node *maxTreeBerger(const cv::Mat &img, Compare pxOrder, pxType curType){

        std::vector <pxCoord> sorted;
        std::vector <std::vector<pxCoord> > parent(img.cols, std::vector<pxCoord>(img.rows));

        detail::sortImgElems(img, pxOrder, sorted);
        detail::maxTreeCore(sorted, curType, parent);
        detail::canonizeTree(sorted, parent, img);
        return detail::makeNodeTree(parent, img)->assignGrayLevelRec(detail::maxTreeGrayLvlAssign(img));
    }

    /// \details \copydetails fl::maxTreeNister(const cv::Mat &img, Compare pxOrder, pxType curType = regular)
    /// \param mask Binary mask image indicating if the pixel at a certain
    /// position should be processed. (0 - no, 1 - yes)
    template <typename Compare>
    Node *maxTreeBerger(const cv::Mat &img, Compare pxOrder, const cv::Mat &mask, pxType curType){

        std::vector <pxCoord> sorted;
        std::vector <std::vector<pxCoord> > parent(img.cols, std::vector<pxCoord>(img.rows, make_pxCoord(-1,-1)));
        for(int i = 0; i < img.cols; ++i){ // unknown pixels in the image, or the ones masked not to be processed.
            for(int j = 0; j < img.rows; ++j){
                if (detail::getCvMatElem(img, i, j) < -9000 || (!mask.empty() && detail::getCvMatElem(mask, i, j) == 0))
                    parent[i][j] = make_pxCoord(-2,-2);
            }
        }

        detail::sortImgElems(img, pxOrder, sorted, mask);
        if (sorted.empty())
            return NULL;

        detail::maxTreeCore(sorted, curType, parent);
        detail::canonizeTree(sorted, parent, img);
        return detail::makeNodeTree(parent, img, mask)->assignGrayLevelRec(detail::maxTreeGrayLvlAssign(img));
    }


    namespace detail{
        template <typename Compare>
        bool imgIndexComp<Compare>::operator() (const pxCoord &lhs, const pxCoord &rhs){
            return myComp(detail::getCvMatElem(img, rhs), detail::getCvMatElem(img, lhs));
        }

        // new implementation - capable of processing masked and unknown pixels at -9999
        template <typename Compare>
        void sortImgElems(const cv::Mat &img, Compare pxOrder, std::vector <pxCoord> &sorted, const cv::Mat &mask){
            sorted.clear();
            sorted.resize(img.cols * img.rows);

            std::generate(sorted.begin(), sorted.end(), (imgIndexGenerator(img)));

            // here remove all that are -9999
            std::multiset<pxCoord, imgIndexComp<Compare> > sortingContainer(imgIndexComp<Compare>(pxOrder, img));
            for (int i=0, szi = sorted.size(); i < szi; ++i){
                if (getCvMatElem(img, sorted[i].X, sorted[i].Y) > -9000 && (mask.empty() || detail::getCvMatElem(mask, sorted[i].X, sorted[i].Y) != 0))
                    sortingContainer.insert(sorted[i]);
            }
            //std::stable_sort(sorted.begin(), sorted.end(), (imgIndexComp<Compare>(pxOrder, img)));
            sorted.assign(sortingContainer.begin(), sortingContainer.end());
        }
    }
}



#endif


