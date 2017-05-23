/// \file structures/patternspectra2d.h
/// \author Petra Bosilj

#ifndef PATTERNSPECTRA2D_H
#define PATTERNSPECTRA2D_H

#include <string>
#include <vector>
#include <utility>
#include <cmath>

#include "attribute.h"

#include <cstdlib>
#include <iostream>

/// \brief A value to be used as `Binning` upper limit if the limit
/// should be read from the `Node` every time variably (no preset
/// fixed maximum).
#define NODE_VAL -1

/*  combinations of parameters, and the output for a Node which is NOT root:
 * ================================================================================================================================================================================ ||
 * (firstAttBin.max == NODE_VAL || secondAttBin.max == NODE_VAL)  |  storeValue   |  areaNormalize    |                            OUTPUT                                           ||
 * -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- ||
 *                             true                               |     true      |        X          |   RAPTORS                  RAPTORS                          RAPTORS         ||
 *                             true                               |     false     |       true        |   local PS (local max)   , locally normalized (node area) , no memoization  ||
 *                             true                               |     false     |       false       |   local PS (local max)   , not normalized                 , no memoization  ||
 *                             false                              |     true      |       true        |   partial PS (global max), globally normalized (root area), memoization     ||
 *                             false                              |     true      |       false       |   partial PS (global max), not normalized                 , memoization     ||
 *                             false                              |     false     |       true        |   partial PS (global max), globally normalized (root area), no memoization  ||
 *                             false                              |     false     |       false       |   partial PS (global max), not normalized                 , no memoization  ||
 * ================================================================================================================================================================================ ||
 */

namespace fl{
    class ImageTree;
    class Node;

    /// \class Binning
    ///
    /// \brief Used to encode histogram `Binning` for one dimension of `PatternSpectra2D`.
    class Binning{
        public:
            /// Possible `Scale`s to use in a `Binning`.
            enum Scale {
                linear, ///< Linear scale, all bins of the same width.
                logarithmic, ///< Logarithmic scale, smaller bins for smaller values.
                arbitrary, ///< Arbitrary user-determined bins.
            };

            /// \brief Class construction for `Binning::linear` and `Binning::logarithmic` scales.
            Binning(int nBins, int minValue, int maxValue, Scale sc);
            /// \brief Constructor with all parameters for `Binning::logarithmic` scale.
            Binning(int nBins, int minValue, int maxValue, Scale sc, int referenceScaleRange);
            /// \brief Class constructor for `Binning::arbitrary`.
            Binning(int nBins, int minValue, int maxValue, Scale sc, const std::vector<double> &lim);
            /// \brief Class destructor.
            ~Binning() {}

            /// \brief Determine the bin index for a value.
            int getBin(double value) const;

            int nBins; ///< The number of bins for a 1D `Binning`.
            int minValue; ///< The minimal attribute value accepted by this `Binning`.
            int maxValue; ///< The maximal attribute value accepted by this `Binning`.
            Scale scale; ///< A scale to use in this `Binning`. The choices are `Binning::linear`, `Binning::logarithmic` and `Binning::arbitrary`.

            /// \brief An indicator value signifying if the scale of calculation
            /// is same for all the nodes of the tree or not.
            ///
            /// This value is `true` if a predefined size should be used for all the
            /// `PatternSpectra2D` calculated from the tree. The value is `false`
            /// if the area of each `Node` (region) is taken as the reference size when
            /// calculating the `PatternSpectra2D` for that `Node`.
            bool relativeScale;

            /// \brief Get the reference scale used with `Binning::logarithmic`.
            ///
            /// If different from `Binning::maxValue - Binning::minValue`,
            /// it corresponds to the length used to determine the base of the `Binning`.
            ///
            /// \return The reference range used for determining the base of the `Binning`.
            /// This determines the scale the descriptor is using.
            const int &getReferenceScaleRange() const {return this->referenceScaleRange;}

            /// \brief Get the logarithm based used with `Binning::logarithmic`.
            ///
            /// The `Binning` is set such that a value of `1` from the range
            /// `[1, Binning::getReferenceScaledRange()]` (i.e. after scaling to
            /// the desired range) is the lower attribute value limit for the first
            /// bin.
            ///
            /// \return The logarithm base of the `Binning` (for `Binning::logarithmic`).
            /// This determines the scale the descriptor is using.
            const double &getLogarithmBase() const {return this->b;}

            /// \brief Return an upper limit of a bin with a certain index,
            /// expressed as a percentage.
            double getUnscaledUpperLimit(int binIndex) const;

        protected:

            friend class AnyPatternSpectra2D;

            /// \brief Associate the information specific to a `Node` which
            /// is locally used for the `PatternSpectra2D`.
            void addLocalInformation(Node *myNode);

        private:
            int range, referenceScaleRange;
            double b;
            int getLinearBinning(double valueShift) const;
            int getLogarithmicBinning(double valueShift) const;
            int getArbitraryBinning(double valueShift) const;

            int binSearchArbitrary(double valueScaled, int curMinIndex, int curMaxIndex) const;

            void calcLogB();
            const std::vector <double> binUpperLimits;

    };

/// \class PatternSpectra2DSettings
///
/// \brief Interface to hold all the settings required to set up
/// and define all `PatternSpectra2D` parameters.
    class PatternSpectra2DSettings{
        public:

            /// \brief Class constructor for `PatternSpectra2DSettings`.
            PatternSpectra2DSettings(const Binning &firstAttBin, const Binning &secondAttBin, bool storeValue, bool areaNormalize = false, bool cutOff = false);

            /// \brief Copy constructor for `PatternSpectra2DSettings`.
            PatternSpectra2DSettings (const PatternSpectra2DSettings &other);

            Binning firstAttBin; ///< `Binning` for the first `Attribute` to be used with `PatternSpectra2D`.
            Binning secondAttBin; ///< `Binning` for the second `Attribute` to be used with `PatternSpectra2D`.
        private:

            bool storeValue, areaNormalize, cutOff;
            int areaNorm;
            std::vector <double> upperLimits;

            friend class AnyPatternSpectra2D;

            template <typename AT1, typename AT2>
            friend class PatternSpectra2D;
    };

/// \class AnyPatternSpectra2D
///
/// \brief Abstract `PatternSpectra2D`, virtual interface not specifying
/// the attributes used to build th Pattern Spectrum.
    class AnyPatternSpectra2D{
        public:

            /// \brief The default constructor for `AnyPatternSpectra2D`.
            AnyPatternSpectra2D(Node *baseNode, const ImageTree *baseTree, PatternSpectra2DSettings *settings, bool deleteSettings = true);

            /// \brief The destructor for `AnyPatternSpectra2D`.
            virtual ~AnyPatternSpectra2D(void) = 0;

            /// \brief Gets the reference to the filled out matrix (histogram)
            /// holding the `PatternSpectra2D` values.
            virtual const std::vector<std::vector<double> > &getPatternSpectraMatrix(std::vector<Node *> *pathDown = NULL);

            /// \brief Reset the values stored in the pattern spectrum.
            void resetPatternSpectraMatrix(void);
        protected:
            virtual const std::vector<std::vector<double> > &getPatternSpectraMatrix(bool userCall, PatternSpectra2DSettings *settings, std::vector<Node *> *pathDown = NULL);
            bool valueSet; ///< Indicator variable signifying if the pattern spectrum is calculated or not.
            Node *myNode; ///< The `Node` associated to this spectrum. Should be from `myTree`
            const ImageTree *myTree; ///< The `ImageTree` associated to this spectrum.
            bool useTmp; ///< Indicator variable signifying whether to use a temporary matrix (internal calculation) or not.
            std::vector <std::vector <double> > ps; ///< The matrix holding the pattern spectrum.
            std::vector <std::vector <double> > tmp; ///< The matrix holding the intermediate spectrum in recursive calls.

            /// \brief Gets the reference to the filled out matrix. Handles internal recursive calls differently from
            /// user-made calls.
            virtual void calculatePatternSpectra2D(PatternSpectra2DSettings *baseSettings = NULL, std::vector<Node *> *pathDown = NULL) = 0;
            PatternSpectra2DSettings *mySettings; ///< The `PatternSpectra2DSettings` associated to this spectrum.
        private:
    };

/// \class PatternSpectra2D
///
/// \brief A concrete implementation of a pattern spectrum. The attributes to be used are specified
/// by the specific `Attribute` classes in the template argument.
///
/// \tparam AT1 First attribute to be used for `PatternSpectra2D` calculation. Should be increasing.
/// \tparam AT2 Second attribute to be used for `PatternSpectra2D` calculation. Can be increasing or not.
    // where AT1, AT2 are attributes
    template <typename AT1, typename AT2>
    class PatternSpectra2D : public AnyPatternSpectra2D{
        public:
            /// \brief Default constructor for `PatternSpectra2D`
            PatternSpectra2D(Node *baseNode, const ImageTree *baseTree, PatternSpectra2DSettings *settings, bool deleteSettings = true);

            /// \brief Destructor for `PatternSpectra2D`
            ~PatternSpectra2D(void) {}

            /// \copydoc AnyPatternSpectra2D::getPatternSpectraMatrix()
            virtual const std::vector<std::vector<double> > &getPatternSpectraMatrix(std::vector<Node *> *pathDown = NULL);
        protected:
            void calculatePatternSpectra2D(PatternSpectra2DSettings *baseSettings = NULL, std::vector<Node *> *pathDown = NULL);
            const std::vector<std::vector<double> > &getPatternSpectraMatrix(bool userCall, PatternSpectra2DSettings *settings, std::vector<Node *> *pathDown = NULL);
        private:
    };
}

#include "patternspectra2d.tpp"

#endif // PATTERNSPECTRA2D_H
