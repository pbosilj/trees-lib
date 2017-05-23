/// \file algorithms/maxtreenister.h
/// \author Petra Bosilj

#ifndef MAXTREENISTER_H
#define MAXTREENISTER_H

#include "../structures/node.h"
#include "../structures/inclusionnode.h"
#include "../structures/imagetree.h"

#include "../misc/pixels.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <utility>
#include <functional>

#include <iostream>

namespace fl{
    /// \brief Constructs the max-tree using algorithm from:
    /// D. Nister, H. Stewenius: "Linear time
    /// maximally stable extremal regions" (2008).
    Node *maxTreeNister(const cv::Mat &img);

    /// \brief \copybrief maxTreeNister(const cv::Mat &img)
    template <typename Compare>
    Node *maxTreeNister(const cv::Mat &img, Compare pxOrder, pxType curType = regular);


}

#include "maxtreenister.tpp"

#endif // MAXTREENISTER_H
