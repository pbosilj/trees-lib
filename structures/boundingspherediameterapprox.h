#ifndef BOUNDINGSPHEREDIAMETERAPPROX_H
#define BOUNDINGSPHEREDIAMETERAPPROX_H

#include "attribute.h"

#include <functional>

namespace fl{

/// \class BoundingSphereDiameterApproxSettings
///
/// \brief Holder for all the `AttributeSettings` of the `BoundingShpereDiameterApproxAttribute`
    class BoundingSphereDiameterApproxSettings : public AttributeSettings {
        public:
            ~BoundingSphereDiameterApproxSettings() {}

            /// \brief The class constructor for `BoundingSphereDiameterApproxSettings`.
            ///
            /// Sets the number of dimensions and the distance function to be used.
            ///
            /// \param nDim Number of dimensions of the multichannel image with which to operate.
            /// \param distance A distance function to determine a distance between two vector elements
            /// (pixels of a multichannel image). This function will return a double and compare two
            /// `std::vector<double *>` pointing to the values of a pixel.
            BoundingSphereDiameterApproxSettings(int nDim, const std::function<double(std::vector<double *>, std::vector<double *>)> &distance)
                : nDim(nDim), distance(distance) {}

            virtual BoundingSphereDiameterApproxSettings* clone() const{
                return new BoundingSphereDiameterApproxSettings(*this);
            }

            int nDim;
            std::function<double(std::vector<double *>, std::vector<double *>)> distance;
    };


/// \class BoundingSphereDiameterApprox
///
/// \brief Value of this `Attribute` is the approximation of the diameter of
/// bounding sphere of the pixel values for the `ImageTree` multivariate data (multiple
/// images set). From:
/// S. Lefevre, L. Chapel, F. Merciol: "Hyperspectral image classification from
/// multiscale description with constrained connectivity and machine learning" (2014)
///
/// \note This is an advanced case of `RangeAttribute` approximation for multivariate data.
///
/// \note The calculation of this attribute requires reading the pixel values of a multichannel
/// image. The corresponding images (representing a single multichannel image) has to be
/// assigned with `ImageTree::setImages()`.
///
/// This is an increasing attribute, meaning that the value of a `Node` will always be higher or
/// equal than that of it's children `Node`s.
    class BoundingSphereDiameterApprox : public TypedAttribute<double>
    {
        public:
            static const std::string name;

            /// \brief The constructor for `BoundingSphereDiameterApprox`.
            BoundingSphereDiameterApprox(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for `BoundingSphereDiameterApprox`.
            virtual ~BoundingSphereDiameterApprox();

            /// \brief Triggers the calculation of the `BoundingSphereDiameterApprox` value for a `Node`.
            virtual void calculateAttribute();
        protected:

            /// \brief Initialize the radius and center values with the set number of dimensions.
            virtual void initSettings();

            /// \brief Changes the `AttributeSettings` for this `Attribute`.
            virtual bool changeSettings(AttributeSettings *nsettings, bool deleteSettings = false);
        private:

            /// \brief Estimated radius of the bounding box
            double radius;

            /// \brief Estimated center of the bounding box.
            std::vector <double *> center;
    };

}
#endif // BOUNDINGSPHEREDIAMETERAPPROX_H
