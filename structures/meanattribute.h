/// \file structures/rangeattribute.h
/// \author Petra Bosilj

#ifndef MEANATTRIBUTE_H
#define MEANATTRIBUTE_H

#include "attribute.h"

namespace fl{
/// \class MeanSettings
///
/// \brief Holder for all the `AttributeSettings` of the `MeanAttribute`
    class MeanSettings : public AttributeSettings {
        public:
            MeanSettings();
            ~MeanSettings();

            virtual MeanSettings* clone() const{
                return new MeanSettings(*this);
            }
    };


/// \class MeanAttribute
///
/// \brief Value of this `Attribute` is the mean gray level of the region.
///
///
/// \note The calculation of this attribute requires reading the pixel values. The corresponding image has to be
/// assigned with `ImageTree::setImage()`.
    class MeanAttribute : public TypedAttribute<double>
    {
        public:
            static const std::string name;

            /// \brief The constructor for `RangeAttribute`.
            MeanAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for the `RangeAttribute`.
            virtual ~MeanAttribute();

            /// \brief Triggers the calculation of the `RangeAttribute` for a `Node`.
            virtual void calculateAttribute();
        protected:
        private:
            int N;
    };
}


#endif // MEANATTRIBUTE_H
