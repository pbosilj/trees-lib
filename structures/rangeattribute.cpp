/// \file structures/rangeattribute.cpp
/// \author Petra Bosilj

#include "rangeattribute.h"

#include "../misc/pixels.h"
#include "../misc/commontreedetail.h"

#include "node.h"
#include "imagetree.h"

#include <iostream>

#include <utility>
#include <algorithm>

fl::RangeSettings::RangeSettings() {}
fl::RangeSettings::~RangeSettings() {}

const std::string fl::RangeAttribute::name = "range";

/// \details \copydetails Attribute::Attribute()
fl::RangeAttribute::RangeAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings) {
        this->initSettings();
}

fl::RangeAttribute::~RangeAttribute() {}

/// Calculation of the `RangeAttribute`. The calculation is done efficiently, by
/// storing the maximal and minimal value for each `Node`. These values are retrieved
/// for the child `Node`s and compared to those of the pixels in the current `Node`.
/// Any image element (pixel) position in the `ImageTree` is examined at most once.
///
/// \note The image needs to be associated to the `ImageTree` before
/// this function is called. This can be done by `ImageTree::setImage`.
/// After the calculation the image can be discarded by `ImageTree::unsetImage`.
void fl::RangeAttribute::calculateAttribute(){
    if (! this->myTree->imageSet()){
        std::cerr << "Image not set for the ImageTree, giving up." << std::endl;
        std::exit(-2);
    }
    const cv::Mat &img = this->myTree->image();

    this->minSet = this->maxSet = false;

    const std::vector <std::pair<int, int> > &ownElems = this->myNode->getOwnElements();
    for (int i=0, szi = ownElems.size(); i < szi; ++i){
        double curEl = detail::getCvMatElem(img, ownElems[i].X, ownElems[i].Y);
        if (!this->minSet){
            this->minValue = curEl;
            this->minSet = true;
        }
        else{
            this->minValue = std::min(this->minValue, curEl);
        }

        if (!this->maxSet){
            this->maxValue = curEl;
            this->maxSet = true;
        }
        else{
            this->maxValue = std::max(maxValue, curEl);
        }
    }
    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(RangeAttribute::name, childAttributes);
    for (int i=0, szi = childAttributes.size(); i < szi; ++i){

        RangeAttribute *chat = ((RangeAttribute *)childAttributes[i]);
        chat->value();
        if (this->minSet)
            this->minValue = std::min(this->minValue, chat->minValue);
        else{
            this->minValue = chat->minValue;
            this->minSet = true;
        }
        if (this->maxSet)
            this->maxValue = std::max(this->maxValue, chat->maxValue);
        else{
            this->maxValue = chat->maxValue;
            this->maxSet = true;
        }
    }

    this->attValue = this->maxValue - this->minValue;

    TypedAttribute<double>::calculateAttribute();
}
