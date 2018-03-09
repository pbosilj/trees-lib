/// \file structures/meanattribute.cpp
/// \author Petra Bosilj

#include "meanattribute.h"

#include "../misc/pixels.h"
#include "../misc/commontreedetail.h"

#include "node.h"
#include "imagetree.h"

#include <iostream>

#include <utility>
#include <algorithm>

fl::MeanSettings::MeanSettings() {}
fl::MeanSettings::~MeanSettings() {}

const std::string fl::MeanAttribute::name = "mean";

/// \details \copydetails Attribute::Attribute()
fl::MeanAttribute::MeanAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings) {
        this->initSettings();
}

fl::MeanAttribute::~MeanAttribute() {}

/// Calculation of the `MeanAttribute`. The calculation is done efficiently, by
/// storing the attribute value from the children `Node`s as well as their size.
/// Any image element (pixel) position in the `ImageTree` is examined at most once.
///
/// \note The image needs to be associated to the `ImageTree` before
/// this function is called. This can be done by `ImageTree::setImage`.
/// After the calculation the image can be discarded by `ImageTree::unsetImage`.
void fl::MeanAttribute::calculateAttribute(){
    if (! this->myTree->imageSet()){
        std::cerr << "Image not set for the ImageTree, giving up." << std::endl;
        std::exit(-2);
    }
    const cv::Mat &img = this->myTree->image();

    const std::vector <std::pair<int, int> > &ownElems = this->myNode->getOwnElements();
    double sum;
    for (int i=0, szi = ownElems.size(); i < szi; ++i){
        double curEl = detail::getCvMatElem(img, ownElems[i].X, ownElems[i].Y);
        sum += curEl;
    }

    sum *= ownElems.size();
    this->N = ownElems.size();

    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(MeanAttribute::name, childAttributes);
    for (int i=0, szi = childAttributes.size(); i < szi; ++i){

        MeanAttribute *chat = ((MeanAttribute *)childAttributes[i]);
        chat->value();

        sum += chat->attValue;
        this->N += chat->N;
    }

    this->attValue = sum / N;

    TypedAttribute<double>::calculateAttribute();
}
