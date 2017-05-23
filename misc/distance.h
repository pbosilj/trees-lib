#ifndef DISTANCE_H
#define DISTANCE_H

#include <vector>
#include <opencv2/core/core.hpp>

namespace fl{

    /// \class distance
    ///
    /// \brief An interface for designing distance functors for multispectral images.
    ///
    /// Provides a way to compare two pixels represented by `std::vector` of a multispectral
    /// image.
    ///
    /// \tparam RT return type of the functors: it is predicted that they will return either
    /// integral or floating-point values.
    /// \tparam IT input type of the functors: type of each single component of the multispectral
    /// image (expecting either integral or floating-point values).
    template <typename RT, typename IT>
    class distance{
        public:
            /// \brief A distance constructor.
            distance() {}

            /// \brief A distance destructor.
            virtual ~distance();

            /// \brief The operator calculating the distance between two elements of a multispectral image.
            ///
            /// \param x The first multispectral pixel (image element) to be compared.
            /// \param y The second multispectral pixel (image element) to be compared.
            /// \return The distance between \p x and \p y.
            virtual RT operator()(std::vector <IT *> x, std::vector <IT *> y) const = 0;
    };

    /// \class distanceL1Integral
    ///
    /// \brief L1 distance functor (integral values).
    ///
    /// L1 distance functor operating on multispectral images holding integral values.
    /// \note It is expected that the differences between consecutive values will fall into the
    /// `integer` range.
    ///
    /// \tparam TI Input type of the functor. Type of each single component of the multispectral
    /// image. Has to be of integral type (`short`, `int`, etc.).
    template <typename TI>
    // where TI is an integral type
    class distanceL1Integral : public distance <int, TI>{
        public:
            /// \brief distanceL1Integral constructor.
            distanceL1Integral() {}

            /// \brief distanceL1Integral destructor.
            virtual ~distanceL1Integral() {}

            /// \brief Calculates the L1 distance between two elements of a multispectral image.
            int operator()(std::vector <TI *> x, std::vector <TI *> y) const;
    };

    /// \class distanceL1NonIntegral
    ///
    /// \brief L1 distance functor (non-integral values).
    ///
    /// L1 distance functor operating on multispectral images holding non-integral (floating point)
    /// values.
    /// \tparam TNI Input type of the functor. Type of each single component of the multispectral
    /// image. Should not be an integral type (see `distanceL1Integral`).
    template <typename TNI>
    // where TNI is a non-integral type
    class distanceL1NonIntegral : public distance <double, TNI>{
        public:

            /// \brief distanceL1NonIntegral constructor.
            distanceL1NonIntegral() {}

            /// \brief distanceL1NonIntegral destructor.
            virtual ~distanceL1NonIntegral() {}

            /// \brief Calculates the L1 distance between two elements of a multispectral image.
            double operator()(std::vector <TNI *> x, std::vector <TNI *> y) const;
    };

    /// \class distanceL2square
    ///
    /// \brief L2 distance squared functor (integral values).
    ///
    /// L2 distance squared functor operating on multispectral images holding integral values.
    /// \note It is expected that the differences between consecutive values will fall into the
    /// `integer` range.
    ///
    /// \tparam TI Input type of the functor. Type of each single component of the multispectral
    /// image. Has to be of integral type (`short`, `int`, etc.).
    template <typename TI>
    class distanceL2square : public distance <int, TI>{
        public:

            /// \brief distanceL2square constructor.
            distanceL2square() {}

            /// \brief distanceL2square destructor.
            virtual ~distanceL2square() {}

            /// \brief Calculates the L2 distance squared between two elements of a multispectral image.
            int operator()(std::vector <TI *> x, std::vector <TI *> y) const;
    };

    /// \class distanceL2
    ///
    /// \brief L2 distance functor (any values).
    ///
    /// L2 distance functor operating on multispectral images holding any values.
    /// \tparam T Input type of the functor.
    template <typename T>
    class distanceL2 : public distance<double, T>{
        public:

            /// \brief distanceL2 constructor.
            distanceL2() {}

            /// \brief distanceL2 destructor.
            virtual ~distanceL2() {};

            /// \brief Calculates the L2 distance between two elements of a multispectral image.
            double operator()(std::vector<T *> x, std::vector <T *> y) const;
    };

    /// \class distanceSAM
    ///
    /// \brief SAM distance functor.
    ///
    /// SAM angular distance functor for comparing the elements of mulsipectral images.
    /// \tparam T Input type of the functor.
    template <typename T>
    class distanceSAM : public distance<double, T>{
        public:

            /// \brief distanceSAM constructor.
            distanceSAM() {}

            /// \brief distanceSAM destructor.
            virtual ~distanceSAM() {}

            /// \brief Calculates the SAM angular distance between two elements of a multispectral image.
            double operator()(std::vector <T *> x, std::vector <T *> y) const;
    };
}

#include "distance.tpp"

#endif
