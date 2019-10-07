#ifndef TREECONSTRUCTION_H
#define TREECONSTRUCTION_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "maxtreenister.h"
#include "alphatreedualmax.h"
#include "omegatreealphafilter.h"
#include "tosgeraud.h"

namespace fl{
    enum treeType { maxTree = 1, minTree = 2, treeOfShapes = 3, alphaTree = 4, omegaTree = 5, minMax = 6 };

    fl::ImageTree *createTree(fl::treeType t, const cv::Mat &image);

}

#endif // TREECONSTRUCTION_H
