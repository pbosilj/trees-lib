#include "regiondynamicsattribute.h"

#include "node.h"

#include <cstdlib>
#include <algorithm>

const std::string fl::RegionDynamicsAttribute::name = "regiondynamics";

fl::RegionDynamicsAttribute::RegionDynamicsAttribute(const Node *baseNode, const ImageTree *baseTree,
        AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings) { this->initSettings(); }

fl::RegionDynamicsAttribute::~RegionDynamicsAttribute(){ }

const double & fl::RegionDynamicsAttribute::selfLevel() const {
    return this->myNode->level();
}

/// Calculation of the `RegionDynamicsAttribute`. The calculation is done efficiently, by
/// using the values of the child `Node`s.
void fl::RegionDynamicsAttribute::calculateAttribute(){
    this->attValue = 0.0;

    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(RegionDynamicsAttribute::name, childAttributes);

    for (int i=0, szi = childAttributes.size(); i < szi; ++i){
        double curr = ((RegionDynamicsAttribute *)childAttributes[i])->value()
                    + std::abs(this->selfLevel() - ((RegionDynamicsAttribute *)childAttributes[i])->selfLevel());
        this->attValue = std::max(this->attValue, curr);
    }
    TypedAttribute<double>::calculateAttribute();
}
