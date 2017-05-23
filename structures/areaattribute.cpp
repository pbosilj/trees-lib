/// \file structures/areaattribute.cpp
/// \author Petra Bosilj

#include "areaattribute.h"

#include "node.h"

const std::string fl::AreaAttribute::name = "area";

/// \details \copydetails Attribute::Attribute()
fl::AreaAttribute::AreaAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<int>(baseNode, baseTree, settings, deleteSettings) { this->initSettings(); }

fl::AreaAttribute::~AreaAttribute() { }

/// Calculation of the `AreaAttribute`. The calculation is done efficiently, by re-using
/// the `AreaAttribute` of any child `Node`s in the `ImageTree`.
/// Any image element (pixel) position in the `ImageTree` is retrieved at most once.
void fl::AreaAttribute::calculateAttribute(){
    this->attValue = this->myNode->getOwnElements().size();

    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(AreaAttribute::name, childAttributes);

    for (int i=0, szi = childAttributes.size(); i < szi; ++i){
        this->attValue += ((AreaAttribute *)childAttributes[i])->value();
    }
    TypedAttribute<int>::calculateAttribute();
}
