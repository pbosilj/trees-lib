/// \file structures/regionclassification.h
/// \author Petra Bosilj
/// \date 22/03/2018

#ifndef REGIONCLASSIFICATION_H_INCLUDED
#define REGIONCLASSIFICATION_H_INCLUDED

#include "../structures/node.h"

#include <vector>

namespace fl{

    /// \brief Prompts the user to manually classify `Node`s into `weed`, `crop`, and `mixed` class.

            /// \brief Given a list of `Node`s (output of segmentation step), it prompts the user
        /// to assign one of the three possible classes to the selected regions.
        /// (producing ground truth).
        ///
        /// TODO: write full description
        /// TODO: implement as external function instead of `ImageTree` method?
    void manualRegionClassification(std::vector <Node *> &toMark, const cv::Mat &originalImage, const cv::Mat &rgbImg, std::vector <int> &classSizes);
}

#endif // REGIONCLASSIFICATION_H_INCLUDED

