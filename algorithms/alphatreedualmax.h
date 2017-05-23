#ifndef ALPHATREEDUALMAX_H_INCLUDED
#define ALPHATREEDUALMAX_H_INCLUDED

#include "maxtreenister.h"

#include "../structures/node.h"

#include <opencv2/imgproc/imgproc.hpp>


namespace fl{
    /// \brief Constructs the alpha-tree as a min-tree
    /// on image edges instead of pixels.
    Node *alphaTreeDualMax(const cv::Mat &img);

    template <class Function>
    Node *alphaTreeDualMax(const std::vector <cv::Mat> &img, Function distance);
}

#include "alphatreedualmax.tpp"

#endif // ALPHATREEDUALMAX_H_INCLUDED
