/// \file structures/patternspectra2d.cpp
/// \author Petra Bosilj

#include "patternspectra2d.h"

#include "node.h"
#include "imagetree.h"
#include "areaattribute.h"

#include <iostream>

/// This constructor can be used to set up `Binning::linear` and `Binning::logarithmic`.
///
/// \note If used for `Binning::logarithmic` the size of the whole range is considered
/// for the base of the logarithmic `Binning`.
///
/// \param nBins The number of dimensions, or bins, in the `Binning`.
/// \param minValue The minimal value accepted in the histogram.
/// \param maxValue The maximal value accepted in the histogram.
/// \param sc Type of the `Binning::Scale` to be used for the histogram.
/// \remark `Binning::arbitrary` can not be used in this constructor, as it
/// requires setting the bins in the constructor.
fl::Binning::Binning(int nBins, int minValue, int maxValue, Scale sc)
        : nBins(nBins), minValue(minValue), maxValue(maxValue), scale(sc), relativeScale(false) {
    if (maxValue != NODE_VAL && minValue >= maxValue){
        std::cerr << "Mainimal value larger or equal to the maximal value. Releasing raptors" << std::endl;
        std::exit(-3);
    }
    this->range = maxValue - minValue;
    if (sc == logarithmic){
        this->referenceScaleRange = this->range +1;
        this->calcLogB();
    }
    else if (sc == arbitrary) {
        std::cerr << "Initializing arbitrairy binning without limit values. Releasing raptors" << std::endl;
        std::exit(-1);
    }
}

/// This constructor can be used to set up `Binning::logarithmic` when the base of
/// the `Binning` is different than it's range.
///
/// \param nBins The number of dimensions, or bins, in the `Binning`.
/// \param minValue The minimal value accepted in the histogram.
/// \param maxValue The maximal value accepted in the histogram.
/// \param sc Type of the `Binning::Scale` to be used for the histogram.
/// \param referenceScaleRange The value (range) to be used to determine the base of the logarithmic binning.
fl::Binning::Binning(int nBins, int minValue, int maxValue, Scale sc, int referenceScaleRange)
    : nBins(nBins), minValue(minValue), maxValue(maxValue), scale(sc), relativeScale(false), referenceScaleRange(referenceScaleRange){
    if (maxValue != NODE_VAL && minValue >= maxValue){
        std::cerr << "Mainimal value larger or equal to the maximal value. Releasing raptors" << std::endl;
        std::exit(-3);
    }
    this->range = maxValue - minValue;
    this->calcLogB();
    if (sc != logarithmic){
        std::cerr << "referenceScaleRange argument used only in initialization of logarithmic binning. Releasing raptors" << std::endl;
        std::exit(-7);
    }
}

/// This constructor can be used to set up `Binning::arbitrary` when the base of
/// the `Binning` is different than it's range.
///
/// \param nBins The number of dimensions, or bins, in the `Binning`.
/// \param minValue The minimal value accepted in the histogram.
/// \param maxValue The maximal value accepted in the histogram.
/// \param sc Type of the `Binning::Scale` to be used for the histogram.
/// \param lim The array of upper limits of the bins in the histogram, expressed as precentages
/// of the range (e.g.: `{0.0, 0.3, 0.7, 1.0}`).
///     \remark The number of elements in the array should be the same as \p `nBins`. The elements
///     of the array should be increasing. The last element of the array should be `1.0`.
fl::Binning::Binning(int nBins, int minValue, int maxValue, Scale sc, const std::vector<double> &lim)
        : nBins(nBins), minValue(minValue), maxValue(maxValue), scale(sc), relativeScale(false), binUpperLimits(lim) {
    if (maxValue != NODE_VAL && minValue >= maxValue){
        std::cerr << "Mainimal value larger or equal to the maximal value. Releasing raptors" << std::endl;
        std::exit(-3);
    }
    this->range = maxValue - minValue;
    if (sc != arbitrary) {
        std::cerr << "Initializing non arbitrairy binning with limit values. Releasing raptors" << std::endl;
        std::exit(-2);
    }
    else if ((int)lim.size() != nBins) {
        std::cerr << "Initializing arbitrairy binning with wrong number of limit values. Releasing raptors" << std::endl;
        std::exit(-1);
    }
    for (int i=1; i < nBins; ++i){
        if (lim[i] <= lim[i-1] || lim[nBins-1] != 1.0) {
            std::cerr << "Initializing arbitrairy binning with wrong limit values. Releasing raptors" << std::endl;
            std::exit(-4);
        }
    }
}

/// For the given value, determine the bin index. Takes into account the
/// `Binning::Scale` used for this binning.
///
/// \param value The value for which the bin should be determined.
/// \remark Should be between `Binning::minValue` and `Binning::maxValue`
/// \return The index of the correct bin.
/// \remark The indexes are one-valued. The bins `0` and `Binning::maxValue`ss
int fl::Binning::getBin(double value) const{
    if (value >= this->maxValue)
        return nBins+1;
    else if (value < this->minValue)
        return 0;
    value -= this->minValue;
    switch(this->scale) {
        case arbitrary:
            return getArbitraryBinning(value)+1;
            break;
        case logarithmic:
            return getLogarithmicBinning(value)+1;
            break;
        case linear:
        default:
            return getLinearBinning(value)+1;
            break;
    }
}

/// Calls a binary search for on the bin values to determine the bin for
/// a value for `Binning:arbitrary`.
///
/// \param valueShift The value for which to find a bin, already shifted
/// for `Binning::minValue`.
int fl::Binning::getArbitraryBinning(double valueShift) const{
    return this->binSearchArbitrary(valueShift/this->range, 0, this->nBins);
}

int fl::Binning::binSearchArbitrary(double valueScaled, int curMinIndex, int curMaxIndex) const{
    if (curMinIndex >= curMaxIndex)
        return curMinIndex;
    int midBin = (curMinIndex + curMaxIndex) / 2;
    bool ok = true;
    if (midBin > curMinIndex)
        ok = ok&&(valueScaled >= binUpperLimits[midBin-1]);
    if (!ok){
        return binSearchArbitrary(valueScaled, curMinIndex, midBin);
    }

    ok = ok&&(valueScaled < binUpperLimits[midBin]);

    if (!ok){
        return binSearchArbitrary(valueScaled, midBin+1, curMaxIndex);
    }

    return midBin;
}

/// Determines
/// a value for `Binning::linear`.
///
/// \param valueShift The value for which to find a bin, already shifted
/// for `Binning::minValue`.
int fl::Binning::getLinearBinning(double valueShift) const{
    return (int)(valueShift * this->nBins / this->range);
}

/// Determines
/// a value for `Binning::logarithmic`.
///
/// \param valueShift The value for which to find a bin, already shifted
/// for `Binning::minValue` (but not put into range of `Binning:referenceScaleRange`).
int fl::Binning::getLogarithmicBinning(double valueShift) const{
    double scaled = (valueShift+1) * (this->referenceScaleRange) / (this->range +1);
    double rv = log2(scaled) / log2(this->b);
    if (rv < 0.0)
        return -1;
    else
        return (int)rv;
}

/// Calculates the base of the logarithm used in `Binning:logarithmic`
/// based on the `Binning::referenceScaleRange`.
void fl::Binning::calcLogB(){
    this->b = std::pow(this->referenceScaleRange, 1.0/this->nBins);
}


/// Returns the upper limit of the bin \param binIndex as a
/// percentage (the upper limit of the last bin is `1.0`)
///
/// \param binIndex The index of the bin being queried.
/// \return The upper limit of the bin (in percentage form)
double fl::Binning::getUnscaledUpperLimit(int binIndex) const{
    switch(this->scale) {
        case arbitrary:
            return this->binUpperLimits[binIndex-1];
            break;
        case logarithmic:
            return (std::pow(this->b, binIndex) - 1)/(this->referenceScaleRange - 1);
            break;
        case linear:
        default:
            return binIndex / this->nBins;
            break;
    }
}

/// This function initializes any binning parameters depending on the `Node`
/// (region) it is used for (e.g. the `Node` area when the binning has relative
/// size, i.e. when `Binning::relativeScale == true`).
///
/// \note Should only be called by the constructor of `AnyPatternSpectra2D`
/// when associating the spectrum to a specific `Node`.
///
/// \param myNode The `Node` in which a histogram with this `Binning` will
/// be assigned.
void fl::Binning::addLocalInformation(Node *myNode){
    if (this->maxValue == NODE_VAL){
        this->maxValue = ((AreaAttribute *)(myNode->getAttribute(AreaAttribute::name)))->value();
        this->range = this->maxValue - this->minValue;
        if (this->scale == logarithmic && this->referenceScaleRange <= 0){
            this->referenceScaleRange = this->range + 1;
            this->calcLogB();
        }
        this->relativeScale = true;
    }
}

/// Initializes all the necessary settings for fully defining a `PatternSpectra2D`.
///
/// \param firstAttBin `Binning` for the first `Attribute` to be used with `PatternSpectra2D`.
/// \param secondAttBin `Binning` for the second `Attribute` to be used with `PatternSpectra2D`.
/// \param storeValue Turn memoization of `PatternSpectra2D` values in the
/// `ImageTree` on or off. Using memoization might require excess memory.
/// It is not possible to use memoization in all settings, e.g. when
/// `PatternSpectra2D` are calculated locally in every subtree.
/// \param areaNormalize Normalize the `PatternSpectra2D` (locally or globally)
/// if set to `true`. Set to `false` by default.
/// \param cutOff If set to `true`, if it is possible for the given combination
/// of `Attribute`s, force the `PatternSpectra2D` to ignore the `Attribute`
/// combinations which would be impossible due to downscaling (e.g. certain
/// combinations of `AreaAttribute` and `NonCompactnessAttribute` become invalid).
/// Set to `false` by default.
///
/// \note If \p firstAttBin or \p secondAttBin is set to NODE_VAL, the
/// `PatternSpectra2D` are calculated locally for every subtree (the maximum
/// values of the increasing `Attribute` from the subtree is used).
fl::PatternSpectra2DSettings::PatternSpectra2DSettings(const Binning &firstAttBin, const Binning &secondAttBin, bool storeValue, bool areaNormalize, bool cutOff) :
    firstAttBin(firstAttBin), secondAttBin(secondAttBin), storeValue(storeValue),
    areaNormalize(areaNormalize), cutOff(cutOff), areaNorm(1) {
    if ((this->firstAttBin.maxValue == NODE_VAL || this->secondAttBin.maxValue == NODE_VAL) && storeValue){
        std::cerr << "Relative bin sizes with storing the pattern spectra not allowed." << std::endl;
        std::exit(-5);
    }

    upperLimits.clear(); upperLimits.resize(firstAttBin.nBins, 0);
}

/// Initializes all the necessary settings for fully defining a `PatternSpectra2D`
/// based on another copy of `PatterSpectra2DSettings`.
///
/// \param other The `PatterSpectra2DSettings` to be copied.
fl::PatternSpectra2DSettings::PatternSpectra2DSettings (const PatternSpectra2DSettings &other) :
    firstAttBin(other.firstAttBin), secondAttBin(other.secondAttBin),
    storeValue(other.storeValue), areaNormalize(other.areaNormalize),
    cutOff(other.cutOff), areaNorm(other.areaNorm),
    upperLimits(other.upperLimits) {}


/// Initializes `AnyPatternSpectra2D` and associates all the related objects for pattern
/// spectra calculation
///
/// \param baseNode The `Node` in the `ImageTree` for which the pattern spectrum is to
/// be calculated. Has to belong to \p `baseTree`
///
/// \param baseTree The `ImageTree` which is used for the calculation of the pattern
/// spectrum
///
/// \param settings The `PatternSpectra2DSettings` containing the settings which are
/// to be applied during pattern spectrum calculation.
///
/// \param deleteSettings (optional) Ensures that the memory of \p settings is freed after the
/// call to this constructor. If `true`, the \p settings will be deleted, use `false` if settings
/// need to be stored for later processing.
///
/// \note Pattern spectra are not to be created directly, but rather associated to the whole
/// tree through `ImageTree::addPatternSpectra2DToTree` which will then call the appropriate
/// pattern spectra constructors
///
/// \note In order to handle the area normalization, a call to this constructor will associate
/// an `AreaAttribute` to the whole `ImageTree` if called correctly.
fl::AnyPatternSpectra2D::AnyPatternSpectra2D(Node *baseNode, const ImageTree *baseTree, PatternSpectra2DSettings *settings, bool deleteSettings)
        : valueSet(false), myNode(baseNode), myTree(baseTree), useTmp(false){
    this->mySettings = new PatternSpectra2DSettings(*settings);

    if (this->myNode->isRoot()){
        this->myTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
        if (this->mySettings->areaNormalize)
            this->mySettings->areaNorm = ((fl::AreaAttribute *)this->myNode->getAttribute(fl::AreaAttribute::name))->value();
    }

    this->mySettings->firstAttBin .addLocalInformation(myNode);
    this->mySettings->secondAttBin.addLocalInformation(myNode);

    if (deleteSettings)
        delete settings;
}

/// Ensures the proper destruction of `AnyPatternSpectra2D`.
///
/// \note The pattern spectra are not to be deleted directly and this destructor should
/// not be called directly. In order to remove pattern spectra from a tree, the function
/// `ImageTree::deletePatternSpectra2DFromTree` should be used.
///
/// \note If the pattern spectra are deleted through the appropriate functions, this
/// destructor will also delete any additional `AreaAttribute`s from the `ImageTree`.
fl::AnyPatternSpectra2D::~AnyPatternSpectra2D(){
    if (this->myNode->isRoot()){
        this->myTree->deleteAttributeFromTree<fl::AreaAttribute>();
    }
    delete this->mySettings;
}

/// Obtain the filled out histogram that is the pattern spectrum. Internally, the function
/// `calculatePatternSpectra2D` is called.
///
/// \param pathDown If the pattern spectrum is calculated from the parent (ancestor), allows passing
/// an ancestral path down to the original node. Only the values of the `Node`s on the path down are
/// calculated into the pattern spectrum.
///
/// \note \p pathDown would end with the `Node` for which the descriptor is being calculated. All the
/// child `Node`s from this point down are calculated into the pattern spectrum.
///
/// \return A reference to the filled out histogram holding the pattern spectrum.
const std::vector<std::vector<double> > &fl::AnyPatternSpectra2D::getPatternSpectraMatrix(std::vector<Node *> *pathDown){
    return getPatternSpectraMatrix(true, NULL, pathDown);
}

/// \note If this function is called by the user (e.g. indirect call by `fl::AnyPatternSpectra2D::getPatternSpectraMatrix(std::vector<Node *> *pathDown)` )
/// no new \p settings are ever called (i.e. the \p settings are `NULL`).
const std::vector<std::vector<double> > &fl::AnyPatternSpectra2D::getPatternSpectraMatrix(bool userCall, PatternSpectra2DSettings *settings, std::vector<Node *> *pathDown){
    if (pathDown != NULL){
        pathDown->pop_back();
        if (pathDown->empty())
            pathDown = NULL;
    }

    // was this for calculation of children values recursively in a single call
    if (this->valueSet && !(this->mySettings->storeValue)){ // keep the "value" but use the "tmp" for this calculation
        this->useTmp = true;
    }
    if (!(this->mySettings->storeValue) || !(this->valueSet)){ // if value not stored (not ever or not yet)
        bool useRelativeScale = (this->mySettings->firstAttBin.relativeScale || this->mySettings->secondAttBin.relativeScale);
        if (userCall && this->mySettings->areaNormalize){
            if (useRelativeScale){ // normalizing relatively, with value of the active Node
                this->mySettings->areaNorm = ((fl::AreaAttribute *)this->myNode->getAttribute(fl::AreaAttribute::name))->value();
            }
            else{ // normalizing with root value
                this->mySettings->areaNorm = ((fl::AreaAttribute *)this->myTree->root()->getAttribute(fl::AreaAttribute::name))->value();
            }
        }
        if (userCall && useRelativeScale){
            this->calculatePatternSpectra2D(this->mySettings, pathDown);
        }
        else // if (setting == NULL -> null is processed || settings != NULL -> real settings are sent)
            this->calculatePatternSpectra2D(settings, pathDown);
        //else // ACTUALLY NOT NEEDED, COULD HAVE PUT EVERYTHING IN A SINGLE ELSE WITH (settings) F-CALL
        //    this->calculatePatternSpectra2D();
    }

    return (this->useTmp)?(this->tmp):(this->ps);
}

/// Resets the values stored in the spectrum and clears the memory.
/// In case it was only a recursive call, empties the temporary placeholder.
void fl::AnyPatternSpectra2D::resetPatternSpectraMatrix(void){
    if (this->useTmp){
        this->tmp.clear();
        this->useTmp = false;
    }
    else{
        this->ps.clear();
        this->valueSet = false;
    }
}

void fl::AnyPatternSpectra2D::calculatePatternSpectra2D(PatternSpectra2DSettings *baseSettings, std::vector<Node *> *pathDown){
    this->valueSet = true;
}
