/// \file structures/soilpatternspectra.h
/// \author Petra Bosilj
/// \date 07/10/2019

#ifndef SOILPATTERNSPECTRA_H
#define SOILPATTERNSPECTRA_H


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <vector>
#include <map>

#include "../algorithms/treeconstruction.h"

/// input arguments are positional:
///     1 - path to image
///     2 - tree option: "min, max, tos, alpha, omega, minmax, all"
///     3 - filtering rule: "count, sum, volume, all", count = number of regions, sum = number of pixels (area), volume = number of pixels * contrast
void rTestSoil(int argc, char **argv);

/// \brief Output a granulometric curve for a given image.
void outputGranulometryCurve(const cv::Mat &image, fl::treeType t, const std::vector<int> &rule, std::vector <std::map<double, int> > &hist);

/// \brief Calculate a full image granulometry (with the `AreaAttribute`)
void calculateGranulometry(const fl::ImageTree *tree, std::map<double, int> &hist, const int rule);

/// \brief Determine the output path for the granulometry based on the original image path, the filtering rule and the tree type used.
std::string outputFilePath(const std::string& imagePath, const int rule, const fl::treeType tt);

#endif
