/// \file structures/objectdetection.h
/// \author Petra Bosilj
/// \date 21/03/2018

#ifndef OBJECTDETECTION_H_INCLUDED
#define OBJECTDETECTION_H_INCLUDED

#include "../structures/imagetree.h"
#include "../structures/node.h"

#include <vector>

namespace fl{

    /// \brief Selects `Node`s from an `ImageTree` likely to correspond to objects.
    void treeObjectDetection(const ImageTree &tree,
                            std::vector <Node *> &selectedObjects,
                            std::vector <Node *> *associatedLeaves = NULL,
                            int delta = 30, int minArea = 150, int maxArea = 4e5, int extinctionThreshold = 10, int minGrowth = 10, int K = 15);
}

#endif // OBJECTDETECTION_H_INCLUDED
