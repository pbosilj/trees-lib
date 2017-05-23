/// \file structures/momentsattribute.h
/// \author Petra Bosilj

#ifndef MOMENTSATTRIBUTE_H
#define MOMENTSATTRIBUTE_H

#include "attribute.h"
#include "momentsholder.h"

namespace fl{
/// \class MomentsSettings
///
/// \brief Holder for all the `AttributeSettings` of the `MomentsAttribute`
    class MomentsSettings : public AttributeSettings {
        public:
            /// \brief Constructor for the `MomentsSettings`.
            MomentsSettings(int order, MomentType dcv, int dp=-1, int dq=-1);
            ~MomentsSettings() { }

            virtual MomentsSettings* clone() const{
                return new MomentsSettings(*this);
            }
            int order;
            MomentType defaultCastValue;
            int dp, dq;

            /// \brief A comparison operator for `MomentsSettings`
            bool operator==(const MomentsSettings& other) const{
                return (this->order == other.order) && (this->defaultCastValue == other.defaultCastValue) &&
                    (this->dp == other.dp) && (this->dq == other.dq);
            }
    };


/// \class MomentsAttribute
///
/// \brief Value of this `Attribute` corresponds to the selected moment the region of `Node`.
///
/// This is not an increasing `Attribute` and it's value will depend on the shape of the region.
///
/// \note This `Attribute` uses the value type `MomentsHolder`. While it can be cast into double
/// or printed via `std::ostream& operator<<` as a single value, a single `MomentsAttribute` can
/// be used to retrieve many different moments. This is achieved by changing the default return
/// value by `MomentsAttribute::changeSettings()` which will not trigger the recalculation of
/// the moments if the \p order attribute in the `MomentsSettings()` is unchanged.
/// Different moments can also be obtained by handing the `MomentsAttribute::value()` directly as
/// `MomentsHolder` and using the following functions:
///     - `MomentsHolder::getBinaryMoment()`
///     - `MomentsHolder::getRawMoment()`
///     - `MomentsHolder::getCentralMoment()`
///     - `MomentsHolder::getNormCentralMoment()`
///     - `MomentsHolder::getKurtosis()`
///     - `MomentsHolder::getRoundness()`
///     - `MomentsHolder::getTriangularity()`
///     - `MomentsHolder::getHuInvariant()`
///
/// \note Grayscale moments currently only implemented for integer type images.
    class MomentsAttribute : public TypedAttribute<MomentsHolder>{
        public:
            static const std::string name;

            /// \brief The constructor for `MomentsAttribute`.
            MomentsAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, bool deleteSettings = false);

            /// \brief The destructor for `MomentsAttribute`.
            virtual ~MomentsAttribute();

            /// \brief Triggers the calculation of the `MomentsAttribute` value for a `Node`.
            virtual void calculateAttribute();

            MomentsHolder &value();
        protected:

            /// \brief Initializes any of the internal variables dependent of the `MomentsSettings`. Is called from the constructor.
            virtual void initSettings();

            /// \brief Changes the `MomentsSettings` for this `MomentsAttribute`.
            virtual bool changeSettings(AttributeSettings *nsettings, bool deleteSettings = false);
        private:
            bool grayCalc;
    };
}

#endif // MOMENTSATTRIBUTE_H
