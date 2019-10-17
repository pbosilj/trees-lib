/// \file algorithms/attributeconstruction.h
/// \author Petra Bosilj
/// \date 15/10/2019


#ifndef ATTRIBUTECONSTRUCTION_H
#define ATTRIBUTECONSTRUCTION_H

#include "../structures/attribute.h"

namespace fl{
    template <typename SomeAttribute>
    fl::AttributeSettings *getSettingsForAttributeType(void);
}

#include "attributeconstruction.tpp"

#endif
