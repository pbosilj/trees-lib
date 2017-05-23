/// \file structures/valuedeviationattribute.h
/// \author Petra Bosilj


#ifndef VALUEDEVIATIONATTRIBUTE_H
#define VALUEDEVIATIONATTRIBUTE_H

#include "attribute.h"

namespace fl{

/// class ValueDeviationSettings
///
/// \brief Holder for all the `AttributeSettings` of the `ValueDeviationAttribute`
    class ValueDeviationSettings : public AttributeSettings {
        public:
            ValueDeviationSettings();
            ~ValueDeviationSettings();

            virtual ValueDeviationSettings* clone() const{
                return new ValueDeviationSettings(*this);
            }
    };

/// \class ValueDeviationAttribute
///
/// \brief Value of this `Attribute` corresponds to the standard deviation from the mean graylevel value
/// of the `Node` (increased by 1).
/// \note All attributes are implemented such that their minimal value is always 1 for the purpose of using
/// `Binning::logarithmic` for `PatternSpectra2D`.
///
/// This is not an increasing attribute.
    class ValueDeviationAttribute : public TypedAttribute<double>
    {
        public:
            static const std::string name;

            /// \brief The constructor for `ValueDeviationAttribute`.
            ValueDeviationAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for `ValueDeviationAttribute`.
            virtual ~ValueDeviationAttribute();

            /// \brief Triggers the calculation of the `ValueDeviationAttribute` for a `Node`.
            virtual void calculateAttribute();
        protected:
        private:
            double mean;
    };

}

#endif // VALUEDEVIATIONATTRIBUTE_H
