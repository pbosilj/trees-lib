#ifndef TPP_ATTRIBUTE
#define TPP_ATTRIBUTE

#include "attribute.h"

namespace fl{
    template <typename AttType>
    const std::string TypedAttribute<AttType>::name = "default typed";


    /// \details \copydetails Attribute::Attribute()
    template <typename AttType>
    TypedAttribute<AttType>::TypedAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, bool deleteSettings)
        : Attribute(baseNode, baseTree, settings, deleteSettings), storedSettings(0) {
    }


    /// Returns the value of this `TypedAttribute`. If the value is not calculated,
    /// calculates the value.
    ///
    /// \return The value of this `TypedAttribute`.
    ///
    /// \note In case the value is not calculated, the following functions are called:
    /// - TypedAttribute::ensureDefaultSettings()
    /// - TypedAttribute::calculateAttribute()
    /// - TypedAttribute::revertSettingsChanges()
    template <typename AttType>
    AttType &TypedAttribute<AttType>::value(){
        if (!this->valueSet){
            this->ensureDefaultSettings();
            this->calculateAttribute();
            this->revertSettingsChanges();
        }
        AttType &retval = this->attValue;
        return retval;
    }

    template <typename AttType>
    void TypedAttribute<AttType>::calculateAttribute(){
        Attribute::calculateAttribute();
    }
}

#endif //TPP_ATTRIBUTE
