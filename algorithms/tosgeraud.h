#ifndef TOSGERAUD_H_INCLUDED
#define TOSGERAUD_H_INCLUDED

#include "../structures/node.h"
#include "../structures/inclusionnode.h"
#include "../structures/imagetree.h"

#include "../misc/commontreedetail.h"

#include <opencv2/imgproc/imgproc.hpp>


namespace fl{
    /// \brief Constructs the tree of shapes using the algorithm from:
    /// Th. Geraud, E. Carlinet, S. Crozet, L. Najman:
    /// "A quasi-linear algorithm to compute the tree of shapes of nD images"
    /// (2013)
    Node *tosGeraud(const cv::Mat &img);
}


#endif // TOSGERAUD_H_INCLUDED
