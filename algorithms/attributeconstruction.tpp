/// \file algorithms/attributeconstruction.tpp
/// \author Petra Bosilj
/// \date 15/10/2019

#ifndef TPP_ATTRIBUTECONSTRUCTION
#define TPP_ATTRIBUTECONSTRUCTION

#include "../structures/areaattribute.h"
#include "../structures/diagonalminimumattribute.h"
#include "../structures/entropyattribute.h"
#include "../structures/meanattribute.h"
#include "../structures/momentsattribute.h"
#include "../structures/momentsholder.h"
#include "../structures/noncompactnessattribute.h"
#include "../structures/rangeattribute.h"
#include "../structures/regiondynamicsattribute.h"
#include "../structures/sparsityattribute.h"
#include "../structures/valuedeviationattribute.h"
#include "../structures/yextentattribute.h"
#include "../structures/boundingspherediameterapprox.h"
#include "../misc/distance.h"

template <typename SomeAttribute>
fl::AttributeSettings *fl::getSettingsForAttributeType(void){
    if (SomeAttribute::name == fl::AreaAttribute::name)
        return new fl::AreaSettings();
    if (SomeAttribute::name == fl::BoundingSphereDiameterApprox::name)
        return new fl::BoundingSphereDiameterApproxSettings(4,  fl::distanceL1Integral<double>());
    if (SomeAttribute::name == fl::EntropyAttribute::name)
        return new fl::EntropySettings();
    if (SomeAttribute::name == fl::DiagonalMinimumAttribute::name)
        return new fl::DiagonalMinimumSettings();
    if (SomeAttribute::name == fl::MeanAttribute::name)
        return new fl::MeanSettings();
    if (SomeAttribute::name == fl::NonCompactnessAttribute::name)
        return new fl::NonCompactnessSettings();
    if (SomeAttribute::name == fl::RangeAttribute::name)
        return new fl::RangeSettings();
    if (SomeAttribute::name == fl::RegionDynamicsAttribute::name)
        return new fl::RegionDynamicsSettings();
    if (SomeAttribute::name == fl::SparsityAttribute::name)
        return new fl::SparsitySettings();
    if (SomeAttribute::name == fl::ValueDeviationAttribute::name)
        return new fl::ValueDeviationSettings();
    if (SomeAttribute::name == fl::YExtentAttribute::name)
        return new fl::YExtentSettings();
    if (SomeAttribute::name == fl::MomentsAttribute::name) // default choice is the first moment invariant of Hu
        return new fl::MomentsSettings(5, fl::MomentType::hu, 1);
    else
        return NULL;
}

#endif
