/// \file algorithms/maxtreenister.cpp
/// \author Petra Bosilj

#include "maxtreenister.h"

#include "../misc/pixels.h"

#include "../structures/node.h"

namespace fl{
    /// \param img The image used to construct the max-tree.
    ///
    /// \return A `InclusionNode *` (as `Node *) to the root of the max-tree.
    Node *maxTreeNister(const cv::Mat &img){
        return maxTreeNister(img, std::greater<int>(), pxType::regular);
    }
}
