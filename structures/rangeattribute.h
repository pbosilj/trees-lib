/// \file structures/rangeattribute.h
/// \author Petra Bosilj

#ifndef RANGEATTRIBUTE_H
#define RANGEATTRIBUTE_H

#include "attribute.h"

namespace fl{
/// \class RangeSettings
///
/// \brief Holder for all the `AttributeSettings` of the `RangeAttribute`
    class RangeSettings : public AttributeSettings {
        public:
            RangeSettings();
            ~RangeSettings();

            virtual RangeSettings* clone() const{
                return new RangeSettings(*this);
            }
    };


/// \class RangeAttribute
///
/// \brief Value of this `Attribute` corresponds to the difference between the lowest and highest pixel value.
///
/// This is an increasing attribute, meaning that the value of a `Node` will always be higher or
/// equal than that of it's children `Node`s.
///
/// \note The calculation of this attribute requires reading the pixel values. The corresponding image has to be
/// assigned with `ImageTree::setImage()`.
    class RangeAttribute : public TypedAttribute<double>
    {
        public:
            static const std::string name;

            /// \brief The constructor for `RangeAttribute`.
            RangeAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for the `RangeAttribute`.
            virtual ~RangeAttribute();

            /// \brief Triggers the calculation of the `RangeAttribute` for a `Node`.
            virtual void calculateAttribute();
        protected:
        private:
            bool minSet, maxSet;
            double minValue, maxValue;
    };
}

#endif // RANGEATTRIBUTE_H
