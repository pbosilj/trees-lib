/// \file structures/areaattribute.h
/// \author Petra Bosilj

#ifndef AREAATTRIBUTE_H
#define AREAATTRIBUTE_H

#include "attribute.h"

#include <string>

namespace fl{
/// \class AreaSettings
///
/// \brief Holder for all the `AttributeSettings` of the `AreaAttribute`
    class AreaSettings : public AttributeSettings {
        public:
            AreaSettings() {} ///< The constructor of `AreaSettings`.
            ~AreaSettings() {} ///< The destructor of `AreaSettings`

            virtual AreaSettings* clone() const{
                return new AreaSettings(*this);
            }
    };

/// \class AreaAttribute
///
/// \brief Value of this `Attribute` corresponds to the area (number of pixels) of the `Node`.
///
/// This is an increasing attribute, meaning that the value of a `Node` will always be higher or
/// equal than that of it's children `Node`s.
    class AreaAttribute : public TypedAttribute<int>{
        public:
            static const std::string name;

            /// \brief The constructor for `AreaAttribute`.
            AreaAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for `AreaAttribute`.
            virtual ~AreaAttribute();

            /// \brief Triggers the calculation of the `AreaAttribute` value for a `Node`.
            virtual void calculateAttribute();
        protected:
        private:

    };
}
#endif // AREAATTRIBUTE_H
