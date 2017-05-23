/// \file structures/attribute.cpp
/// \author Petra Bosilj

#include "attribute.h"

namespace fl{

    AttributeSettings::AttributeSettings() {}
    AttributeSettings::AttributeSettings(const AttributeSettings &other){}
    AttributeSettings::~AttributeSettings() { }

    const std::string Attribute::name = "default";

    /// This constructor will ensure that the memory reserved through
    /// `AttributeSettings *` used in construction will be freed (allows the use of
    /// `new` in the constructor).
    ///
    /// \param baseNode The pointer to the `Node` to which this copy of `Attribute` is assigned.
    /// \param baseTree The pointer to the `ImageTree` to which the `Node` \p baseNode belongs to.
    /// \param settings The pointer to the `AttributeSettings` holding the necessary settings for
    /// this `Attribute`. A copy of these settings is assigned to the `Attribute`.
    /// \param deleteSettings (optional) Ensures that the memory of \p settings is freed after the
    /// call to this constructor. If `true`, the \p settings will be deleted, use `false` if settings
    /// need to be stored for later processing.
    Attribute::Attribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, bool deleteSettings)
            : valueSet(false), myNode(baseNode), myTree(baseTree){
        mySettings = settings->clone();
        if (deleteSettings)
            delete settings;
    }

    /// Ensures that the memory reserved for `AttributeSettings` of this attribute is
    /// freed upon object destruction.
    Attribute::~Attribute(){
        delete this->mySettings;
    }

    /// Needs to be overridden by every concrete `Attribute` implemented to ensure
    /// correct computation of the `Attribute` value.
    void Attribute::calculateAttribute(){
        this->valueSet = true;
    }
}
