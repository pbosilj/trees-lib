/// \file structures/momentsattribute.cpp
/// \author Petra Bosilj

#include "momentsattribute.h"
#include "../misc/pixels.h"
#include "../misc/commontreedetail.h"

#include "node.h"
#include "imagetree.h"

#include <string>
#include <algorithm>

/* MOMENTS SEQUENCES with different NUM_MOMENTSORDER values  <array pos>-<moment of order xy>
 * ========================================================================================================
 * 0-00 1-10 2-20 3-30 4-40 5-01 6-11 7-21 8-31 9-02 10-12 11-22 12-03 13-13 14-04, with NUM_MOMENTSORDER=5.
 * 0-00 1-10 2-20 3-30 4-01 5-11 6-21 7-02 8-12 9-03, with NUM_MOMENTSORDER=4
 * 0-00 1-10 2-20 3-01 4-11 5-02, with NUM_MOMENTSORDER=3
 * 0-00 1-10 2-01, with NUM_MOMENTSORDER=2
 */

const std::string fl::MomentsAttribute::name = "moments";

/// Settings for the `MomentsAttribute`, which will be used in
/// `MomentsHolder::setDefaultCastValue()` to determine the output
/// value of the `MomentsAttribute`.
///
/// \p order The maximal order to be stored within `MomentsHolder`.
/// \p dcv `MomentType` defining the type of the moment (default cast value).
/// \p dp The p order of the moment. This can later be changed, but only so that \p p + \p q <= \p order.
/// \remark Ignored in case of `MomentType::kurtosis`, `MomentType::roundness`, `MomentType::eccentricity`, `MomentType::triangularity`.
/// \p dq The q order of the moment. This can later be changed, but only so that \p p + \p q <= \p order.
/// \remark Ignored in all cases as \p p as well as `MomentType::hu`.
fl::MomentsSettings::MomentsSettings(int order, MomentType dcv, int dp, int dq)
                : order(order), defaultCastValue(dcv), dp(dp), dq(dq) {}

/// Initializes the `MomentsHolder` from `MomentsSettings`.
/// Calls `MomentsHolder::setDefaultCastValue` based on the values given in `MomentsHolder`
void fl::MomentsAttribute::initSettings() {
    TypedAttribute<MomentsHolder>::initSettings();

    MomentsSettings *mys = (MomentsSettings *)(this->getSettings());
    this->attValue.init(mys->order);
    this->attValue.setDefaultCastValue(mys->defaultCastValue, mys->dp, mys->dq);
}

/// Changes the settings of this `MomentsAttribute` as set with new `AttributeSettings` (which have to be
/// of the type `MomentsSettings`).
///
/// \param nsettings A pointer to new `AttributeSettings` (which are `MomentsSettings`).
///
/// \param deleteSettings `false` by default if the `AttributeSettings` are to be reused outside of this
/// function call. Set to `true` if the  `AttributeSettings` were created by the `new` command for this
/// function call.
///
/// \note Will trigger the calculation of `MomentsHolder::setDefaultCastValue()` and a possible recalculation
/// of moments if the new \p order from `MomentsSettings` is larger than the previous \p order.
bool fl::MomentsAttribute::changeSettings(AttributeSettings *nsettings, bool deleteSettings) {

    MomentsSettings *mys = (MomentsSettings *)(this->getSettings());
    MomentsSettings *nys = (MomentsSettings *)nsettings;

    if ((*mys) == (*nys))
        return false;

    if (mys->order < nys->order){ // \<-- ccheck here
        this->vset() = false;
        this->attValue.init(nys->order);
    }
    if (!((nys->defaultCastValue == mys->defaultCastValue) && (nys->dp == mys->dp) && (nys->dq == mys->dq))){
        this->attValue.setDefaultCastValue(nys->defaultCastValue, nys->dp, nys->dq);
    }

    mys->order = std::max(nys->order, mys->order);
    mys->defaultCastValue = nys->defaultCastValue;
    mys->dp = nys->dp;
    mys->dq = nys->dq;

    TypedAttribute<MomentsHolder>::changeSettings(nsettings, deleteSettings); // this will free memory if requested

    return true;
}

/// \details \copydetails Attribute::Attribute()
fl::MomentsAttribute::MomentsAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, bool deleteSettings)
    : TypedAttribute<MomentsHolder>(baseNode, baseTree, settings, deleteSettings), grayCalc(false){
        this->initSettings();
}

fl::MomentsAttribute::~MomentsAttribute() { }

/// Calculation of the `MomentsAttribute`. The calculation will be preformed efficiently by
/// using the values of `MomentsAttribute` of the child `Node`s. Also all the moments values
/// used in the calculation of the requested moment will be stored.
///
/// \note The image needs to be associated to the `ImageTree` if the calculation of the
/// grayscale is required. The function will NOT fail in this case, but will rather only
/// calculate the binary moments. The image can be set by `ImageTree::setImage`.
/// After the calculation the image can be discarded by `ImageTree::unsetImage`.
void fl::MomentsAttribute::calculateAttribute(){
    switch (((fl::MomentsSettings *)(this->getSettings()))->defaultCastValue){
        case MomentType::raw:
        case MomentType::central:
        case MomentType::normal:
            this->grayCalc = this->myTree->imageSet();
        default:
            break;
    }

    const std::vector <pxCoord> &coords = this->myNode->getOwnElements();
    int order = ((MomentsSettings *)(this->getSettings()))->order;
    std::vector <long long> &myRawMoments = this->attValue.rawMoments;
    std::vector <long long> &myBinaryMoments = this->attValue.binaryMoments;

    if (this->grayCalc)
        std::fill(myRawMoments.begin(), myRawMoments.end(), 0);
    std::fill(myBinaryMoments.begin(), myBinaryMoments.end(), 0);

    for (int i=0, szi = coords.size(); i < szi; ++i){
        long long value = 1;
        long long gValue = 0;
        if (this->grayCalc)
            gValue = fl::detail::getCvMatElem(this->myTree->image(), coords[i]);
        for (int q_idx = 0; q_idx < order; ++q_idx){
            long long h = value;
            for (int p_idx = 0; p_idx < order-q_idx; ++p_idx){
                if (this->grayCalc)
                    myRawMoments[this->attValue.momIndex(p_idx,q_idx)] += h*gValue;
                myBinaryMoments[this->attValue.momIndex(p_idx, q_idx)] += h;
                h *= coords[i].X;
            }
            value *= coords[i].Y;
        }
    }
    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(MomentsAttribute::name, childAttributes);

    for (int i=0, szi = childAttributes.size(); i < szi; ++i){
        const std::vector<long long> &childBMoments = ((MomentsAttribute *)childAttributes[i])->value().binaryMoments;
        const std::vector<long long> &childGMoments = ((MomentsAttribute *)childAttributes[i])->value().rawMoments;
        for (int j=0, szj = myBinaryMoments.size(); j < szj; ++j){
            if (this->grayCalc)
                myRawMoments[j] += childGMoments[j];
            myBinaryMoments[j] += childBMoments[j];
        }
    }
    this->attValue.calculateDefaultCastValue();

    TypedAttribute<MomentsHolder>::calculateAttribute();
}

fl::MomentsHolder & fl::MomentsAttribute::value(){
    bool &vset = this->vset();
    switch (((fl::MomentsSettings *)(this->getSettings()))->defaultCastValue){
        case MomentType::raw:
        case MomentType::central:
        case MomentType::normal:
            vset = this->grayCalc;
        default:
            return fl::TypedAttribute<MomentsHolder>::value();
            break;
    }
}
