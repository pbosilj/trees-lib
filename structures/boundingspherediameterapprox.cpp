/// \file structures/boundingspherediameterapprox.cpp
/// \author Petra Bosilj

#include "boundingspherediameterapprox.h"
#include "areaattribute.h"
#include "node.h"
#include "imagetree.h"

#include "../misc/commontreedetail.h"
#include "../misc/pixels.h"

const std::string fl::BoundingSphereDiameterApprox::name = "boundingshpere";

/// \details \copydetails Attribute::Attribute()
fl::BoundingSphereDiameterApprox::BoundingSphereDiameterApprox(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
: TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings){

    this->initSettings();
    if (!(this->myNode->isRoot()))
        return;
    this->myTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());

}

fl::BoundingSphereDiameterApprox::~BoundingSphereDiameterApprox()
{
    BoundingSphereDiameterApproxSettings *mys = (BoundingSphereDiameterApproxSettings *)(this->getSettings());
    for (int i=0; i < mys->nDim; ++i)
        delete this->center[i];
    //dtor
    if (this->myNode->isRoot())
        this->myTree->deleteAttributeFromTree<AreaAttribute>();
}

/// Initializes the dimensions of the bounding box center with the number of dimensions provided by
/// `BoundingSphereDiameterApproxSettings`.
void fl::BoundingSphereDiameterApprox::initSettings() {
    TypedAttribute<double>::initSettings();

    BoundingSphereDiameterApproxSettings *mys = (BoundingSphereDiameterApproxSettings *)(this->getSettings());
    this->attValue = 0;
    this->center.reserve(mys->nDim);
}

/// Changes the settings of this `BoundingSphereDiameterApprox` as set with new `AttributeSettings` (which have to be
/// of the type `BoundingSphereDiameterApproxSettings`).
///
/// \param nsettings A pointer to new `AttributeSettings` (which are `BoundingSphereDiameterApproxSettings`).
///
/// \param deleteSettings `false` by default if the `AttributeSettings` are to be reused outside of this
/// function call. Set to `true` if the  `AttributeSettings` were created by the `new` command for this
/// function call.
///
/// \note Will trigger the calculation of `BoundingSphereDiameterApprox::initSettings()`.
bool fl::BoundingSphereDiameterApprox::changeSettings(AttributeSettings *nsettings, bool deleteSettings) {
    BoundingSphereDiameterApproxSettings *mys = (BoundingSphereDiameterApproxSettings *)(this->getSettings());
    BoundingSphereDiameterApproxSettings *nys = (BoundingSphereDiameterApproxSettings *)nsettings;

    if (mys->nDim != nys->nDim){
        for (int i=0; i < mys->nDim; ++i)
            delete this->center[i];
        this->center.resize(0);
        mys->nDim = nys->nDim;
        this->initSettings();
    }
    else{
        for (int i=0; i < mys->nDim; ++i, *(this->center[i]) = 0);
    }
    mys->distance = nys->distance;

    TypedAttribute<double>::changeSettings(nsettings, deleteSettings);

    return true;

}

/// Calculation of the `RangeAttribute`. The calculation is done efficiently, by
/// re-using the values of the child `Node`s.
/// Any image element (pixel) position in the `ImageTree` is examined at most once.
///
/// \note A multivariate image needs to be associated to the `ImageTree` before
/// this function is called. This can be done by `ImageTree::setImages` (one image
/// for each dimension of the multivariate image).
/// After the calculation the image can be discarded by `ImageTree::unsetImages`.
void fl::BoundingSphereDiameterApprox::calculateAttribute(){

    if (! this->myTree->imagesSet()){
        std::cerr << "Image not set for the ImageTree, giving up." << std::endl;
        std::exit(-2);
    }

    BoundingSphereDiameterApproxSettings *mys = (BoundingSphereDiameterApproxSettings *)(this->getSettings());
    const std::vector<cv::Mat> &imgs = this->myTree->images();

    int totalArea = 0;

    const std::vector <std::pair<int, int> > &ownElems = this->myNode->getOwnElements();
    for (int i=0, szi = ownElems.size(); i < szi; ++i){
        for (int j=0, szj = imgs.size(); j < szj; ++j){
            if (i == 0)
                this->center.emplace_back(new double(detail::getCvMatElem(imgs[j], ownElems[i].X, ownElems[i].Y)));
            else
                *(this->center[j]) += detail::getCvMatElem(imgs[j], ownElems[i].X, ownElems[i].Y);
        }
    }

    totalArea += ownElems.size();

    std::vector <Attribute *> childBB;
    this->myNode->getChildrenAttributes(BoundingSphereDiameterApprox::name, childBB);
    std::vector <Attribute *> childSizes;
    this->myNode->getChildrenAttributes(AreaAttribute::name, childSizes);

    for (int i=0, szi = childBB.size(); i < szi; ++i){

        BoundingSphereDiameterApprox *cBB = ((BoundingSphereDiameterApprox *)childBB[i]);
        cBB->value();
        AreaAttribute *cArea = ((AreaAttribute *)childSizes[i]);
        for (int j=0, szj = imgs.size(); j < szj; ++j){
            if ((int)this->center.size() < szj)
                this->center.emplace_back(new double(*(cBB->center[j])*cArea->value()));
            else
                *(this->center[j]) += *(cBB->center[j])*cArea->value();
        }
        totalArea += cArea->value();
    }
    for (int j=0, szj = this->center.size(); j < szj; ++j)
        *(this->center[j]) /= totalArea;

    double nr = 0;
    for (int i=0, szi = childBB.size(); i < szi; ++i){
        BoundingSphereDiameterApprox *cBB = ((BoundingSphereDiameterApprox *)childBB[i]);
        double distNow = cBB->value()/2 + mys->distance(cBB->center, this->center);
        if (distNow > nr)
            nr = distNow;
    }

    this->attValue = nr * 2;

    TypedAttribute<double>::calculateAttribute();
}
