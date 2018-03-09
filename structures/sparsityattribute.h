/// \file structures/sparsityattribute.h
/// \author Petra Bosilj

#ifndef SPARSITYATTRIBUTE_H
#define SPARSITYATTRIBUTE_H

#include "attribute.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


namespace fl{

    /// \class SparsitySettings
    ///
    /// \brief Holder for all the `AttributeSettings` of the `SparsityAttribute`
    class SparsitySettings : public AttributeSettings {
        public:
            SparsitySettings() {} ///< The constructor of `SparsitySettings`.
            ~SparsitySettings() {} ///< The destructor of `SparsitySettings`

            virtual SparsitySettings* clone() const{
                return new SparsitySettings(*this);
            }
    };

/// \class SparsityAttribute
///
/// \brief Value of this `Attribute` describes the sparsity of the region.
///
/// This is not an increasing attribute. It is calculated as the ratio of the
/// area of the region and convex hull of the region.
    class SparsityAttribute : public TypedAttribute<double>{
        public:
            static const std::string name;

            /// \brief The constructor for `SparsityAttribute`.
            SparsityAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings = false);

            /// \brief The destructor for `SparsityAttribute`.
            virtual ~SparsityAttribute();

            /// \brief Triggers the calculation of the `SparsityAttribute` value for a `Node`.
            virtual void calculateAttribute();
        protected:
            std::vector <cv::Point2f> convex_hull;
            int area; // do this better
        private:
    };
}

#endif // SPARSITYATTRIBUTE_H
