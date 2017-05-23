/// \file structures/diagonalminimumattribute.h
/// \author Petra Bosilj

#ifndef DIAGONALMINIMUMATTRIBUTE_H
#define DIAGONALMINIMUMATTRIBUTE_H

#include "attribute.h"
#include <string>

namespace fl{

/// \class DiagonalMinimumSettings
///
/// \brief Holder for all the `AttributeSettings` of the `DiagonalMinimumAttribute`
    class DiagonalMinimumSettings : public AttributeSettings {
        public:
            DiagonalMinimumSettings();
            ~DiagonalMinimumSettings();

            virtual DiagonalMinimumSettings* clone() const{
                return new DiagonalMinimumSettings(*this);
            }
    };

/// \class DiagonalMinimumAttribute
///
/// \brief Value of this `Attribute` is the length of the diagonal of the
/// minimum enclosing rectangle with axes-aligned sides of the region held
/// in the `Node`.
///
/// This is an increasing attribute, meaning that the value of a `Node` will
/// always be higher or equal than that of it's children `Node`s.
    class DiagonalMinimumAttribute : public TypedAttribute<double>
    {
        public:
            static const std::string name;

            /// \brief The constructor for `DiagonalMinimumAttribute`.
            DiagonalMinimumAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for `DiagonalMinimumAttribute`.
            virtual ~DiagonalMinimumAttribute();

            /// \brief Triggers the calculation of the `DiagonalMinimumAttribute` value for a `Node`.
            virtual void calculateAttribute();
        protected:
        private:
            int minx, miny, maxx, maxy;
    };
}



#endif // DIAGONALMINIMUMATTRIBUTE_H
