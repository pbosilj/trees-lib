/// \file algorithms/maxtreeberger.h
/// \author Petra Bosilj

#ifndef MAXTREEBERGER_H
#define MAXTREEBERGER_H

#include "../structures/node.h"
#include "../structures/inclusionnode.h"
#include "../structures/imagetree.h"

#include "../misc/pixels.h"
#include "../misc/commontreedetail.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <utility>
#include <functional>
#include <algorithm>

#include <stack>
#include <set>
#include <vector>

#include <iostream>

namespace fl{

    namespace detail{
        void maxTreeCore(const std::vector<fl::pxCoord> &sorted, fl::pxType curType, std::vector<std::vector<fl::pxCoord> > &parent);
        void canonizeTree(const std::vector<fl::pxCoord> &sorted, std::vector<std::vector<fl::pxCoord> > &parent, const cv::Mat &img);
        fl::Node* makeNodeTree(const std::vector<std::vector<fl::pxCoord> > &parent, const cv::Mat &img);
    }

    /// \brief Constructs the max-tree using the algorithm
    /// from: Ch. Berger, Th. Geraud, R. Levillain, N. Widynski, A. Baillard, E. Bertin:
    /// "Effective Component Tree Computation with Application to Pattern Recognition in Astronomical Imaging"
    /// (2007)
    Node *maxTreeBerger(const cv::Mat &img);

    /// \brief \copybrief maxTreeBerger(const cv::Mat &img)
    template <typename Compare>
    Node *maxTreeBerger(const cv::Mat &img, Compare pxOrder, pxType curType = regular);
}

#include "maxtreeberger.tpp"

#endif // MAXTREEBERGER_H
