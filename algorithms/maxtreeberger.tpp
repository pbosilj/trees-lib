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
        void sortImgElems(const cv::Mat &img, Compare pxOrder, std::vector <pxCoord> &sorted);
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

    namespace detail{
        template <typename Compare>
        bool imgIndexComp<Compare>::operator() (const pxCoord &lhs, const pxCoord &rhs){
            return myComp(detail::getCvMatElem(img, rhs), detail::getCvMatElem(img, lhs));
        }

        template <typename Compare>
        void sortImgElems(const cv::Mat &img, Compare pxOrder, std::vector <pxCoord> &sorted){
            sorted.clear();
            sorted.resize(img.cols * img.rows);

            std::generate(sorted.begin(), sorted.end(), (imgIndexGenerator(img)));
            std::stable_sort(sorted.begin(), sorted.end(), (imgIndexComp<Compare>(pxOrder, img)));
        }
    }
}



#endif


