/// \file structures/valuedeviationattribute.cpp
/// \author Petra Bosilj

#include "valuedeviationattribute.h"

#include "../misc/pixels.h"
#include "../misc/commontreedetail.h"

#include "node.h"
#include "imagetree.h"

#include <iostream>

#include <utility>
#include <algorithm>

const std::string ValueDeviationAttribute::name = "value-deviation";

fl::ValueDeviationSettings::ValueDeviationSettings() {}
fl::ValueDeviationSettings::~ValueDeviationSettings() {}

/// \details \copydetails Attribute::Attribute()
fl::ValueDeviationAttribute::ValueDeviationAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings) {
        this->initSettings();
}

fl::ValueDeviationAttribute::~ValueDeviationAttribute(){}

/// Calculation of the `ValueDeviationAttribute`. The calculation is done efficiently, by re-using
/// the mean and variance calculated for the child values of the `ValueDeviationAttribute`.
/// Any image element (pixel) position in the `ImageTree` is retrieved at most once.
///
/// \note The image needs to be associated to the `ImageTree` before
/// this function is called. This can be done by `ImageTree::setImage`.
/// After the calculation the image can be discarded by `ImageTree::unsetImage`.
void fl::ValueDeviationAttribute::calculateAttribute(){
    if (! this->myTree->imageSet()){
        std::cerr << "Image not set for the ImageTree, giving up." << std::endl;
        std::exit(-2);
    }
    const cv::Mat &img = this->myTree->image();

    double localMean = 0, localVariance = 0;

    const std::vector <std::pair<int, int> > &ownElems = this->myNode->getOwnElements();
    std::vector <int> elemValues;

    for (int i=0, szi = ownElems.size(); i < szi; ++i){
        int curEl = detail::getCvMatElem(img, ownElems[i].X, ownElems[i].Y);
        elemValues.push_back(curEl);
        localMean += curEl;
    }
    localMean /= elemValues.size();
    for (int i=0, szi = elemValues.size(); i < szi; ++i){
        localVariance += (localMean - elemValues[i])*(localMean - elemValues[i]);
    }

    localVariance /= ownElems.size();

    int totalElems = ownElems.size();

    double totalMean = localMean*ownElems.size();

    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(ValueDeviationAttribute::name, childAttributes);
    for (int i=0, szi = childAttributes.size(); i < szi; ++i){

        ValueDeviationAttribute *chat = ((ValueDeviationAttribute *)childAttributes[i]);
        chat->value();
        int childSize = chat->myNode->getOwnElements().size();
        totalMean  += childSize * (chat->mean);
        totalElems += childSize;
    }

    totalMean /= totalElems;

    double s1 = ownElems.size() * localVariance;

    double s2 = ownElems.size() * (localMean - totalMean)*(localMean - totalMean);

    for (int i=0, szi = childAttributes.size(); i < szi; ++i){

        ValueDeviationAttribute *chat = ((ValueDeviationAttribute *)childAttributes[i]);
        chat->value();
        int childSize = chat->myNode->getOwnElements().size();
        s1 += childSize * (chat->attValue-1)* (chat->attValue-1);
        s2 += childSize * (chat->mean - totalMean)*(chat->mean - totalMean);
    }

    this->mean = totalMean;

    this->attValue = (s1 + s2) / totalElems;

    this->attValue = std::sqrt(this->attValue)+1;

    TypedAttribute<double>::calculateAttribute();
}
