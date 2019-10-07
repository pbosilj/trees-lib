#include "treeconstruction.h"

#include <functional>

fl::ImageTree *fl::createTree(fl::treeType t, const cv::Mat &image){
    fl::ImageTree *tree;
    switch (t){
        case maxTree:
            tree = new fl::ImageTree(fl::maxTreeNister(image, std::greater<int>()), // max-tree
                                                    std::make_pair(image.rows, image.cols));
            break;
        case minTree:
            tree = new fl::ImageTree(fl::maxTreeNister(image, std::less<int>()),
                                                    std::make_pair(image.rows, image.cols));
            break;
        case treeOfShapes:
            tree = new fl::ImageTree(fl::tosGeraud(image), std::make_pair(image.rows, image.cols));
            break;
        case alphaTree:
            tree = new fl::ImageTree(fl::alphaTreeDualMax(image), std::make_pair(image.rows, image.cols));
            break;
        case omegaTree:
            tree = new fl::ImageTree(fl::omegaTreeAlphaFilter(image), std::make_pair(image.rows, image.cols));
            break;
        default:
            tree = NULL;
    }
    return tree;
}
