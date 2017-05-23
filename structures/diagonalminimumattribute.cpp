/// \file structures/diagonalminimumattribute.cpp
/// \author Petra Bosilj

#include "diagonalminimumattribute.h"

#include "node.h"

fl::DiagonalMinimumSettings::DiagonalMinimumSettings() {}
fl::DiagonalMinimumSettings::~DiagonalMinimumSettings() {}

const std::string fl::DiagonalMinimumAttribute::name = "diagmin";

/// \details \copydetails Attribute::Attribute()
fl::DiagonalMinimumAttribute::DiagonalMinimumAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings), minx(-1), miny(-1), maxx(0), maxy(0) { this->initSettings(); }

fl::DiagonalMinimumAttribute::~DiagonalMinimumAttribute(){ }

/// Calculation of the `AreaAttribute`. The calculation is done efficiently, by the maximal
/// and minimal x and y coordinated contained in the child `Node`s.
/// Any image element (pixel) position in the `ImageTree` is retrieved at most once.
void fl::DiagonalMinimumAttribute::calculateAttribute(){
    const std::vector <std::pair<int, int> > &ownElems = this->myNode->getOwnElements();
    for (int i=0, szi = ownElems.size(); i < szi; ++i){
        if (ownElems[i].X > this->maxx)
            this->maxx = ownElems[i].X;
        if (this->minx < 0 || ownElems[i].X < this->minx)
            this->minx = ownElems[i].X;
        if (ownElems[i].Y > this->maxy)
            this->maxy = ownElems[i].Y;
        if (this->miny < 0 || ownElems[i].Y < this->miny)
            this->miny = ownElems[i].Y;
    }

    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(DiagonalMinimumAttribute::name, childAttributes);
    for (int i=0, szi = childAttributes.size(); i < szi; ++i){
        DiagonalMinimumAttribute *chat = ((DiagonalMinimumAttribute *)childAttributes[i]);
        chat->value();
        this->maxx = std::max(this->maxx, chat->maxx);
        this->maxy = std::max(this->maxy, chat->maxy);
        this->minx = std::min(this->minx, chat->minx);
        this->miny = std::min(this->miny, chat->miny);
    }

    ++this->maxx;
    ++this->maxy;

    this->attValue = std::sqrt((this->maxx-this->minx)*(this->maxx-this->minx) + (this->maxy-this->miny)*(this->maxy-this->miny));
    TypedAttribute<double>::calculateAttribute();
}

