/// \file structures/yexstentattribute.cpp
/// \author Petra Bosilj

#include "yextentattribute.h"

#include "node.h"

fl::YExtentSettings::YExtentSettings() {}
fl::YExtentSettings::~YExtentSettings() {}

const std::string fl::YExtentAttribute::name = "yextent";

/// \details \copydetails Attribute::Attribute()
fl::YExtentAttribute::YExtentAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<int>(baseNode, baseTree, settings, deleteSettings), miny(-1), maxy(-1) { this->initSettings(); }

fl::YExtentAttribute::~YExtentAttribute(){ }

/// Calculation of the `YExtentAttribute`. The calculation is done efficiently, by the maximal
/// and minimal x and y coordinated contained in the child `Node`s.
/// Any image element (pixel) position in the `ImageTree` is retrieved at most once.
void fl::YExtentAttribute::calculateAttribute(){
    const std::vector <pxCoord> &ownElems = this->myNode->getOwnElements();
    for (int i=0, szi = ownElems.size(); i < szi; ++i){

        if (ownElems[i].Y > this->maxy)
            this->maxy = ownElems[i].Y;
        if (this->miny < 0 || ownElems[i].Y < this->miny)
            this->miny = ownElems[i].Y;
    }

    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(YExtentAttribute::name, childAttributes);
    for (int i=0, szi = childAttributes.size(); i < szi; ++i){
        YExtentAttribute *chat = ((YExtentAttribute *)childAttributes[i]);
        chat->value();
        this->maxy = std::max(this->maxy, chat->maxy);
        this->miny = std::min(this->miny, chat->miny);
    }

    this->attValue = (this->maxy-this->miny +1);
    TypedAttribute<int>::calculateAttribute();
}


