/// \file algorithms/omegatreealphafilter.tpp
/// \author Petra Bosilj

#ifndef TPP_OMEGATREEALPHAFILTER
#define TPP_OMEGATREEALPHAFILTER

#include "omegatreealphafilter.h"
#include "../structures/boundingspherediameterapprox.h"
#include "predicate.h"

namespace fl{
    template <class Function1, class Function2>
    Node *omegaTreeAlphaFilter(const std::vector <cv::Mat> &img, Function1 alphaDistance, Function2 omegaDistance){
        Node *alphaRoot = alphaTreeDualMax(img, alphaDistance);

        ImageTree *alphaTreeTmp = new ImageTree(alphaRoot,std::make_pair(img[0].rows, img[0].cols));
        alphaTreeTmp->setImages(img);

        alphaTreeTmp->addAttributeToTree<BoundingSphereDiameterApprox>(new BoundingSphereDiameterApproxSettings(4,  omegaDistance));
        alphaTreeTmp->filterTreeByAttributePredicate<BoundingSphereDiameterApprox>(DifferentThanParent<double>());
        alphaTreeTmp->assignAttributeAsLevel<BoundingSphereDiameterApprox>();
        alphaTreeTmp->deleteAttributeFromTree<BoundingSphereDiameterApprox>();
        alphaTreeTmp->unsetImages();
        alphaTreeTmp->disableDelNodesOnDestruct();

        delete alphaTreeTmp;

        alphaRoot->assignGrayLevelRec(detail::maxTreeGrayLvlAssign(img[0])); // shouldn't need at all

        return alphaRoot;
    }
}

#endif
