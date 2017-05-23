#include "noncompactnessattribute.h"

#include <cmath>

#include "node.h"
#include "imagetree.h"

const std::string fl::NonCompactnessAttribute::name = "non-compactness";

/// \details \copydetails Attribute::Attribute()
///
/// Ensures that `AreaAttribute` as well as `MomentsAttribute` are added to the `ImageTree`.
fl::NonCompactnessAttribute::NonCompactnessAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings), ownMoments(false), tmp(NULL){
            this->initSettings();

            if (!(this->myNode->isRoot()))
                return;
            this->myTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
            if (!(this->myTree->isAttributeInTree<fl::MomentsAttribute>())){
                this->ownMoments = true;
                this->myTree->addAttributeToTree<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::hu, 1));
            }
}

/// Ensures that the `AreaAttribute` and `MomentsAttribute` are deleted from the `ImageTree`
/// if they were added in the constructor.
fl::NonCompactnessAttribute::~NonCompactnessAttribute(){
    if (this->myNode->isRoot()){
        this->myTree->deleteAttributeFromTree<fl::AreaAttribute>();
        if (this->ownMoments)
            this->myTree->deleteAttributeFromTree<fl::MomentsAttribute>();
    }
}

/// Ensures that the `MomentsSettings` for the internally used `MomentsAttribute`
/// are correctly set. Stores the previous `AttributeSettings` of `MomentsSettings`
void fl::NonCompactnessAttribute::ensureDefaultSettings(){
    this->tmp = ((MomentsSettings *)this->myTree->getAttributeSettings<fl::MomentsAttribute>())->clone();
    this->myTree->changeAttributeSettings<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::hu, 1));
    TypedAttribute<double>::ensureDefaultSettings();
}

/// Reverts the state of `MomentsSettings` in the `ImageTree` to the value stored
/// in `NoncCompactnessAttribute::ensureDefaultSettings()`.
void fl::NonCompactnessAttribute::revertSettingsChanges(){
    if (this->hasStoredSettnigs()){
        this->myTree->changeAttributeSettings<fl::MomentsAttribute>(this->tmp, false);
        delete this->tmp;
        tmp = NULL;
    }
    TypedAttribute<double>::revertSettingsChanges();
}

double &fl::NonCompactnessAttribute::value(){
    double &rv = TypedAttribute<double>::value();
    return rv;
}

/// Calculation of the `NonCompactnessAttribute`. The calculation will be preformed efficiently by
/// using the values of the child `Node`s. Internally, a `MomentsAttribute` and an
/// `AreaAttribute` are used.
void fl::NonCompactnessAttribute::calculateAttribute() {
    int area = ((fl::AreaAttribute *)this->myNode->getAttribute(fl::AreaAttribute::name))->value();
    double hu = ((fl::MomentsAttribute *)this->myNode->getAttribute(fl::MomentsAttribute::name))->value();

    this->attValue = (hu + 1.0/(6.0*double(area)))*2*M_PI;
    TypedAttribute<double>::calculateAttribute();
}
