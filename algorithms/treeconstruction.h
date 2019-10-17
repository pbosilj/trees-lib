/// \file algorithms/treeconstruction.cpp
/// \author Petra Bosilj

#ifndef TREECONSTRUCTION_H
#define TREECONSTRUCTION_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <string>


#include "maxtreenister.h"
#include "alphatreedualmax.h"
#include "omegatreealphafilter.h"
#include "tosgeraud.h"

namespace fl{
    /**
    \brief Possble types of image hierarchies.
    **/
    enum class treeType {
        maxTree = 1, minTree = 2, treeOfShapes = 3, alphaTree = 4, omegaTree = 5, minMax = 6
    };

    /**
    \brief Extract a `treeType` from a string.
    **/
    fl::treeType stringToTreeType(std::string s);

    /**
    \brief Constructs an `ImageTree` based on the `treeType` provided.
    **/
    fl::ImageTree *createTree(fl::treeType t, const cv::Mat &image);

    /**
    \brief Constructs an `ImageTree` based on the type of tree provided through a string.
    **/
    fl::ImageTree *createTreeFromString(std::string t, const cv::Mat &image);

}

#endif // TREECONSTRUCTION_H
