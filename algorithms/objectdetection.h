/// \file structures/objectdetection.h
/// \author Petra Bosilj
/// \date 21/03/2018

#ifndef OBJECTDETECTION_H_INCLUDED
#define OBJECTDETECTION_H_INCLUDED

#include "../structures/imagetree.h"
#include "../structures/node.h"

#include <vector>

namespace fl{

    //void selectFromLeaves(std::vector <fl::Node *> &selected,
    //                          const std::vector <std::pair <int, fl::Node *> > &leafExt,
    //                          std::vector <fl::Node *> *sourceLeaves = NULL);

    void treeObjectDetection(const ImageTree &tree,
                            std::vector <Node *> &selectedObjects,
                            std::vector <Node *> *associatedLeaves = NULL);

}

#endif // OBJECTDETECTION_H_INCLUDED
