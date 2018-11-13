/// \file structures/noncompactnessattribute.h
/// \author Petra Bosilj

#ifndef NONCOMPACTNESSATTRIBUTE_H
#define NONCOMPACTNESSATTRIBUTE_H

#include "attribute.h"

#include "areaattribute.h"
#include "momentsattribute.h"

#include <string>

namespace fl{
/// \class NonCompactnessSettings
///
/// \brief Holder for all the `AttributeSettings` of the `NonCompactnessAttribute`
    class NonCompactnessSettings : public AttributeSettings{
        public:
            NonCompactnessSettings() {}
            ~NonCompactnessSettings() {}

            virtual NonCompactnessSettings* clone() const{
                return new NonCompactnessSettings(*this);
            }
    };

/// \class NonCompactnessAttribute
///
/// \brief Value of this `Attribute` corresponds to the selected moment the region of `Node`.
///
/// The value of this `Attribute` describes the elongation of the object, and is a corrected
/// second invariant of Hu (corrected for the discrete case of images).
/// This is not an increasing `Attribute` and it's value will depend on the shape of the region.
///
/// \note This attribute internally uses `MomentsAttribute` as well as `AreaAttribute`
/// \remark Actually using `AreaAttribute` could be avoided by reading the value directly from
/// `MomentsAttribute`.
    class NonCompactnessAttribute : public TypedAttribute<double>{
        public:
            static const std::string name;

            /// \brief The constructor for `NonCompactnessAttribute`.
            NonCompactnessAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for `NonCompactnessAttribute`.
            virtual ~NonCompactnessAttribute();

            /// \brief Triggers the calculation of the `NonCompactnessAttribute` value for a `Node`.
            virtual void calculateAttribute();
            double &value();

            /// \copybrief Attribute::ensureDefaultSettings()
            virtual void ensureDefaultSettings();

            /// \copybrief Attribute::revertSettingsChanges()
            virtual void revertSettingsChanges();
        protected:
        private:
            bool ownMoments;
            std::vector<fl::MomentsSettings *>tmp;
    };

}

#endif // NONCOMPACTNESSATTRIBUTE_H

