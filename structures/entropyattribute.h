/// \file structures/entropyattribute.h
/// \author Petra Bosilj

#ifndef ENTROPYATTRIBUTE_H
#define ENTROPYATTRIBUTE_H

#include "attribute.h"
#include <string>

namespace fl{
/// \class EntropySettings
///
/// \brief Holder for all the `AttributeSettings` of the `EntropyAttribute`
    class EntropySettings : public AttributeSettings {
        public:
            EntropySettings();
            ~EntropySettings();

            virtual EntropySettings* clone() const{
                return new EntropySettings(*this);
            }
    };

/// \class EntropyAttribute
///
/// \brief Value of this `Attribute` corresponds to the Shannon entropy of the `Node` (increased by 1).
///
/// \note All attributes are implemented such that their minimal value is always 1 for the purpose of using
/// `Binning::logarithmic` for `PatternSpectra2D`.
///
/// \note The calculation of this attribute requires reading the pixel values. The corresponding image has to be
/// assigned with `ImageTree::setImage()`.
///
/// This a scale invariant attribute which is not increasing.
    class EntropyAttribute : public TypedAttribute<double>
    {
        public:
            static const std::string name;

            /// \brief The constructor for `EntropyAttribute`.
            EntropyAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for `EntropyAttribute`.
            virtual ~EntropyAttribute();

            /// \brief Triggers the calculation of the `EntropyAttribute` value for a `Node`.
            virtual void calculateAttribute();
        protected:
        private:
            std::vector<int> hist;
    };
}

#endif // ENTROPYATTRIBUTE_H
