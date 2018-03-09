/// \file structures/yextentattribute.h
/// \author Petra Bosilj

#ifndef YEXTENTATTRIBUTE_H
#define YEXTENTATTRIBUTE_H

#include "attribute.h"
#include <string>

namespace fl{

/// \class YExtentSettings
///
/// \brief Holder for all the `AttributeSettings` of the `YExtentAttribute`
    class YExtentSettings : public AttributeSettings {
        public:
            YExtentSettings();
            ~YExtentSettings();

            virtual YExtentSettings* clone() const{
                return new YExtentSettings(*this);
            }
    };

/// \class YExtentAttribute
///
/// \brief Value of this `Attribute` is the height of the axes-aligned
/// bounding box of the region held in the `Node`.
/// minimum enclosing rectangle with axes-aligned sides of the region held
/// in the `Node`.
///
/// This is an increasing attribute, meaning that the value of a `Node` will
/// always be higher or equal than that of it's children `Node`s.
    class YExtentAttribute : public TypedAttribute<int>
    {
        public:
            static const std::string name;

            /// \brief The constructor for `YExtentAttribute`.
            YExtentAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for `YExtentAttribute`.
            virtual ~YExtentAttribute();

            /// \brief Triggers the calculation of the `YExtentAttribute` value for a `Node`.
            virtual void calculateAttribute();
        protected:
        private:
            int miny, maxy;
    };
}



#endif // YEXTENTATTRIBUTE_H

