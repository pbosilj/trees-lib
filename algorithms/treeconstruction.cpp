/// \file algorithms/treeconstruction.cpp
/// \author Petra Bosilj

#include "treeconstruction.h"

#include <functional>

fl::ImageTree *fl::createTree(fl::treeType t, const cv::Mat &image){
    fl::ImageTree *tree;
    switch (t){
        case fl::treeType::maxTree:
            tree = new fl::ImageTree(fl::maxTreeNister(image, std::greater<int>()), // max-tree
                                                    std::make_pair(image.rows, image.cols));
            break;
        case fl::treeType::minTree:
            tree = new fl::ImageTree(fl::maxTreeNister(image, std::less<int>()),
                                                    std::make_pair(image.rows, image.cols));
            break;
        case fl::treeType::treeOfShapes:
            tree = new fl::ImageTree(fl::tosGeraud(image), std::make_pair(image.rows, image.cols));
            break;
        case fl::treeType::alphaTree:
            tree = new fl::ImageTree(fl::alphaTreeDualMax(image), std::make_pair(image.rows, image.cols));
            break;
        case fl::treeType::omegaTree:
            tree = new fl::ImageTree(fl::omegaTreeAlphaFilter(image), std::make_pair(image.rows, image.cols));
            break;
        default:
            std:: cerr << "Incorrect or composite tree type, a hierarchy can not be constructed." << std::endl;
            tree = NULL;
    }
    return tree;
}

fl::treeType fl::stringToTreeType(std::string s){
    if (s == "min")    return fl::treeType::minTree;
    if (s == "max")    return fl::treeType::maxTree;
    if (s == "tos")    return fl::treeType::treeOfShapes;
    if (s == "alpha")  return fl::treeType::alphaTree;
    if (s == "omega")  return fl::treeType::omegaTree;
    if (s == "minmax") return fl::treeType::minMax;

    else{
        std::cerr << "Incorrect tree type. Allowed types are [min, max, tos, alpha, omega, minmax]." << std::endl;
        exit(1);
    }
}

fl::ImageTree *createTreeFromString(std::string t, const cv::Mat &image){
    return createTree(stringToTreeType(t), image);
}
