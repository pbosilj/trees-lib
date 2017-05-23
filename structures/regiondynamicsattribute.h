/// \file structures/regiondynamicsattribute.h
/// \author Petra Bosilj

#ifndef REGIONDYNAMICSATTRIBUTE_H
#define REGIONDYNAMICSATTRIBUTE_H

#include "attribute.h"

#include <string>

namespace fl{
/// class RegionDynamicsSettings
///
/// \brief Holder for all the `AttributeSettings` of the `RegionDynamicsAttribute`
    class RegionDynamicsSettings : public AttributeSettings {
        public:
            RegionDynamicsSettings() {}
            ~RegionDynamicsSettings() {}

            virtual RegionDynamicsSettings* clone() const{
                return new RegionDynamicsSettings(*this);
            }
    };

/// \class RegionDynamicsAttribute
///
/// \brief Value of this `Attribute` corresponds to the region dynamics of the region of `Node`.
///
/// This is an increasing attribute, meaning that the value of a `Node` will always be higher or
/// equal than that of it's children `Node`s.
///
/// \note This `Attribute` is calculated based on the `Node::level` of the `Node`s which was assigned
/// during the construction of `ImageTree`. In case of Min and Max-tree it will be equal to the
/// `RangeAttribute` of the `Node` (but does not read pixel values).
    class RegionDynamicsAttribute : public TypedAttribute<double>
    {
        public:
            static const std::string name;

            /// \brief The constructor for `RegionDynamicsAttribute`
            RegionDynamicsAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for the `RegionDynamicsAttribute`
            virtual ~RegionDynamicsAttribute();

            /// \brief Triggers the calculation of the `RegionDynamicsAttribute` for a `Node`.
            virtual void calculateAttribute();
        protected:
        private:
            const double &selfLevel() const;
    };
}

#endif // REGIONDYNAMICSATTRIBUTE_H
