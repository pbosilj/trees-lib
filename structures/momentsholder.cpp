/// \file structures/momentsholder.cpp
/// \author Petra Bosilj

#include "momentsholder.h"

#include <cmath>
#include <cstdio>

#include <iostream>

#define XMean first
#define YMean second

/* MOMENTS SEQUENCES with different NUM_MOMENTSORDER values  <array pos>-<moment of order xy>
 * ========================================================================================================
 * 0-00 1-10 2-20 3-30 4-40 5-01 6-11 7-21 8-31 9-02 10-12 11-22 12-03 13-13 14-04, with NUM_MOMENTSORDER=5.
 * 0-00 1-10 2-20 3-30 4-01 5-11 6-21 7-02 8-12 9-03, with NUM_MOMENTSORDER=4
 * 0-00 1-10 2-20 3-01 4-11 5-02, with NUM_MOMENTSORDER=3
 * 0-00 1-10 2-01, with NUM_MOMENTSORDER=2
 */

fl::MomentsHolder::~MomentsHolder(){}
fl::MomentsHolder::MomentsHolder() : meanSet(false), binMeanSet(false) {}

/// Return the value of the raw binary moment of the order ( \p `p` + \p `q` ).
///
/// \param p Moment order p
/// \param q Moment order q
/// \return The raw binary moment of the order ( \p `p` + \p `q` ).
long long fl::MomentsHolder::getBinaryMoment(int p, int q) const{
    return this->binaryMoments[this->momIndex(p,q)];
}

/// Return the value of the raw grayscale moment of the order ( \p `p` + \p `q` ).
///
/// \param p Moment order p
/// \param q Moment order q
/// \return The raw binary moment of the order ( \p `p` + \p `q` ).
long long fl::MomentsHolder::getRawMoment(int p, int q) const{
    return this->rawMoments[this->momIndex(p,q)];
}

/// Return the value of the centralized moments of the order ( \p `p` + \p `q` ).
///
/// \param p Moment order p
/// \param q Moment order q
/// \param binCalc `false` (default) to calculate the grayscale moments. Set to
/// `true` to calculate the binary moments.
///
/// \return Binary or grayscale centralized moments of the order ( \p `p` + \p `q` ).
///
/// \note This function will calculate the centralized moments if they have not
/// previously been calculated. Does not check if binary or grayscale raw moments
/// were calculated.
double fl::MomentsHolder::getCentralMoment(int p, int q, bool binCalc){

    std::vector <long long> *rm;
    std::vector <double> *cm;
    std::vector <bool> *isSet;
    bool *ms;
    std::pair <double, double> * mean;

    if (binCalc){
        rm = &(this->binaryMoments);
        cm = &(this->binCentralMoments);
        isSet = &(this->binCentralSet);
        ms = &(this->binMeanSet);
        mean = &(this->negBinMean);

    }
    else{
        rm = &(this->rawMoments);
        cm = &(this->centralMoments);
        isSet = &(this->centralSet);
        ms = &(this->meanSet);
        mean = &(this->negMean);
    }

    if (!p && !q)
        return (double)(*rm)[0];
    if ((!p || !q) && (p+q)==1)
        return 0.0;
    if ((*rm)[0] == 0)
        return 0.0;

    int curIndex = this->momIndex(p,q);

    if ((*isSet)[curIndex])
        return (*cm)[curIndex];

    if (!(*ms)){
        mean->XMean = - (double)((*rm)[1]) / (*rm)[0];
        mean->YMean = - (double)((*rm)[this->order]) / (*rm)[0];
        (*ms) = true;
    }

    double mupq = 0.0;
    double binpk, binql = 1.0;
    long long y = 0;

    for (int l=0; l <=q; ++l){
        binpk = 1.0;
        for (int k=0; k <= p; ++k){
            mupq += binpk*binql*std::pow(mean->XMean, p-k)*std::pow(mean->YMean, q-l)*(*rm)[y+k];
            binpk *= (p-k)/(k+1.0);
        }
        binql *= (q-l)/(l+1.0);
        y += order-l;
    }

    (*cm)[curIndex] = mupq;
    (*isSet)[curIndex] = true;

    return mupq;
}

/// Return the value of the centralized moments of the order ( \p `p` + \p `q` ).
///
/// \param p Moment order p
/// \param q Moment order q
/// \param binCalc `false` (default) to calculate the grayscale moments. Set to
/// `true` to calculate the binary moments.
///
/// \return Binary or grayscale centralized moments of the order ( \p `p` + \p `q` ).
///
/// \note This function will not calculate the centralized moments if they have not
/// previously been calculated.
double fl::MomentsHolder::getCentralMomentNoCheck(int p, int q, bool binCalc) const{
    int curIndex = this->momIndex(p,q);
    const std::vector <double> *cm;
    const std::vector <long long> *rm;

    if (binCalc){
        cm = &(this->binCentralMoments);
        rm = &(this->binaryMoments);
    }
    else{
        cm = &(this->centralMoments);
        rm = &(this->rawMoments);
    }

    if (!p && !q)
        return (double)(*rm)[0];
    if ((!p || !q) && (p+q)==1)
        return 0.0;
    if ((*rm)[0] == 0)
        return 0.0;

    return (*cm)[curIndex];
}

/// Return the value of the normalized centralized moments of the order ( \p `p` + \p `q` ).
///
/// \param p Moment order p
/// \param q Moment order q
/// \param binCalc `false` (default) to calculate the grayscale moments. Set to
/// `true` to calculate the binary moments.
///
/// \return Binary or grayscale normalized centralized moments of the order ( \p `p` + \p `q` ).
///
/// \note This function will calculate the normalized centralized moments and
/// central moments if they have not previously been calculated. Does not check
/// if binary or grayscale raw moments were calculated.
double fl::MomentsHolder::getNormCentralMoment(int p, int q, bool binCalc){
    int curIndex = this->momIndex(p,q);

    std::vector <bool> *isSet;
    std::vector <double> *nm;
    std::vector <long long> *rm;

    if (binCalc){
        isSet = &(this->binNormSet);
        nm = &(this->binNormalizedMoments);
        rm = &(this->binaryMoments);
    }
    else{
        isSet = &(this->normSet);
        nm = &(this->normalizedMoments);
        rm = &(this->rawMoments);
    }

    if (!((*isSet)[curIndex])){
        double gamma = (p+q)/2 + 1.0;
        (*nm)[curIndex] = this->getCentralMoment(p,q, binCalc) / std::pow((double)((*rm)[0]),gamma);
        (*isSet)[curIndex] = true;
    }
    return (*nm)[curIndex];
}

/// Return the value of the normalized centralized moments of the order ( \p `p` + \p `q` ).
///
/// \param p Moment order p
/// \param q Moment order q
/// \param binCalc `false` (default) to calculate the grayscale moments. Set to
/// `true` to calculate the binary moments.
///
/// \return Binary or grayscale normalized centralized moments of the order ( \p `p` + \p `q` ).
///
/// \note This function will not calculate the normalized centralized moments if they have not
/// previously been calculated.
double fl::MomentsHolder::getNormCentralMomentNoCheck(int p, int q, bool binCalc) const{
    int curIndex = this->momIndex(p,q);
    const std::vector <double> *nm;

    if (binCalc)
        nm = &(this->binNormalizedMoments);
    else
        nm = &(this->normalizedMoments);

    return (*nm)[curIndex];
}

double fl::MomentsHolder::kurtosisCalc(double bnm40, double bnm20){
    if (bnm20 != 0.0)
        return bnm40 / (bnm20 * bnm20);
    return 0;
}

/// Return the value of kurtosis for the region.
///
/// \return The kurtosis of the region calculated based on  normalized central moments.
///
/// \note Will trigger the calculation of normalized central moments through
/// `MomentsHolder::getNormCentralMoment` if not previously calculated.
double fl::MomentsHolder::getKurtosis(){
    double bnm40 = this->getNormCentralMoment(4,0,true);
    double bnm20 = this->getNormCentralMoment(2,0,true);
    return MomentsHolder::kurtosisCalc(bnm40, bnm20);
}

/// Return the value of kurtosis for the region.
///
/// \return The kurtosis of the region calculated based on  normalized central moments.
///
/// \note Will not trigger the calculation of normalized central moments if not
/// previously calculated.
double fl::MomentsHolder::getKurtosisNoCheck() const{
    double bnm40 = this->getNormCentralMomentNoCheck(4,0,true);
    double bnm20 = this->getNormCentralMomentNoCheck(2,0,true);
    return MomentsHolder::kurtosisCalc(bnm40, bnm20);
}

double fl::MomentsHolder::roundnessCalc(long long bm00, double bcm20, double bcm02){
    return (double)bm00*bm00 / (2 * M_PI * (bcm20 + bcm02));
}

/// Return the roundness of the region.
///
/// \return The roundness of the region calculated based on the binary and centralized moments.
///
/// \note Will trigger the calculation of central moments through
/// `MomentsHolder::getCentralMoments` if not previously calculated.
double fl::MomentsHolder::getRoundness(){
    return MomentsHolder::roundnessCalc(this->getBinaryMoment(0,0), this->getCentralMoment(2,0,true), this->getCentralMoment(0,2,true));
}

/// Return the roundness of the region.
///
/// \return The roundness of the region calculated based on the binary and centralized moments.
///
/// \note Will not trigger the calculation of central moments if not
/// previously calculated.
double fl::MomentsHolder::getRoundnessNoCheck() const{
    return MomentsHolder::roundnessCalc(this->getBinaryMoment(0,0), this->getCentralMomentNoCheck(2,0,true), this->getCentralMomentNoCheck(0,2,true));
}

double fl::MomentsHolder::eccentricityCalc(double mu20, double mu02, double mu11){
    double term1 = ((mu20 + mu02)/2.0);
    double term2 = ((std::sqrt(4.0*std::pow(mu11, 2.0)+std::pow((mu20-mu02), 2.0))) / 2.0);

    double longAxis  = term1 + term2;
    double shortAxis = term1 - term2;

    return (double) std::sqrt(1.0 - (shortAxis/longAxis));
}

/// Return the eccentricity of the region.
///
/// \return The eccentricity of the region calculated based on the centralized moments.
///
/// \note Will trigger the calculation of the central moments through
/// `MomentsHolder::getCentralMoments` if not previously calculated.
double fl::MomentsHolder::getEccentricity(){
    double mu20 = this->getCentralMoment(2,0,true);
    double mu02 = this->getCentralMoment(0,2,true);
    double mu11 = this->getCentralMoment(1,1,true);

    return MomentsHolder::eccentricityCalc(mu20, mu02, mu11);
}

/// Return the eccentricity of the region.
///
/// \return The eccentricity of the region calculated based on the centralized moments.
///
/// \note Will not trigger the calculation of the central moments if not
/// previously calculated.
double fl::MomentsHolder::getEccentricityNoCheck() const{
    double mu20 = this->getCentralMomentNoCheck(2,0,true);
    double mu02 = this->getCentralMomentNoCheck(0,2,true);
    double mu11 = this->getCentralMomentNoCheck(1,1,true);

    return MomentsHolder::eccentricityCalc(mu20, mu02, mu11);
}

double fl::MomentsHolder::triangularityCalc(long long b00, double mu20, double mu02, double mu11){
    double i1 = ((mu20 * mu02) - std::pow(mu11, 2.0)) / std::pow((double)b00, 4.);
    double tau;
    if (i1 <= 1.0/108)
        tau = 108*i1;
    else
        tau = 1.0 / (108*i1);
    return tau;
}

/// Return the triangularity of the region.
///
/// \return A value describing the triangularity of the region based on binary
/// and centralized moments.
///
/// \note Will trigger the calculation of the central moments through
/// `MomentsHolder::getCentralMoments` if not previously calculated.
double fl::MomentsHolder::getTriangularity(){
    double mu20 = this->getCentralMoment(2,0,true);
    double mu02 = this->getCentralMoment(0,2,true);
    double mu11 = this->getCentralMoment(1,1,true);

    long long b00 = this->getBinaryMoment(0,0);

    return MomentsHolder::triangularityCalc(b00, mu20, mu02, mu11);
}

/// Return the triangularity of the region.
///
/// \return A value describing the triangularity of the region based on binary
/// and centralized moments.
///
/// \note Will not trigger the calculation of the central moments if not
/// previously calculated.
double fl::MomentsHolder::getTriangularityNoCheck() const{
    double mu20 = this->getCentralMomentNoCheck(2,0,true);
    double mu02 = this->getCentralMomentNoCheck(0,2,true);
    double mu11 = this->getCentralMomentNoCheck(1,1,true);

    int b00 = this->getBinaryMoment(0,0);

    return MomentsHolder::triangularityCalc(b00, mu20, mu02, mu11);
}

/// Return the Hu's invariant moments of the order \p p.
///
/// \param p The order of the moment (default value 1).
/// \remark Only implemented for the first order \p p = 1
///
/// \return Hu's invariant moment.
///
/// \note Will trigger the calculation of the normalized central moments through
/// `MomentsHolder::getNormCentralMoment` if not previously calculated.
double fl::MomentsHolder::getHuInvariant(int p){
    if (p == 1){
        return this->getNormCentralMoment(2,0,true) + this->getNormCentralMoment(0,2,true);
    }
    return 0.0;
}

/// Return the Hu's invariant moments of the order \p p.
///
/// \param p The order of the moment (default value 1).
/// \remark Only implemented for the first order \p p = 1
///
/// \return Hu's invariant moment.
///
/// \note Will not trigger the calculation of the normalized central moments if not
/// previously calculated.
double fl::MomentsHolder::getHuInvariantNoCheck(int p) const{
    if (p == 1)
        return this->getNormCentralMomentNoCheck(2,0,true) + this->getNormCentralMomentNoCheck(0,2,true);
    return 0.0;
}


/// Set the default cast value in case of a cast to `double` or output with `std::ostream& operator<<`
///
/// \p attribName `MomentType` defining the type of the moment.
/// \p dp The p order of the moment
/// \remark Ignored in case of `MomentType::kurtosis`, `MomentType::roundness`, `MomentType::eccentricity`, `MomentType::triangularity`.
/// \p dq The q order of the moment
/// \remark Ignored in all cases as \p p as well as `MomentType::hu`.
void fl::MomentsHolder::setDefaultCastValue(MomentType attribName, int dp, int dq){
    if (attribName <= MomentType::normal){
        this->dp = dp;
        this->dq = dq;
    }
    else if (attribName == MomentType::hu){
        this->dp = dp;
    }
    this->defaultValue = attribName;
}

/// Based on the maximal order p+q that will need to be
/// calculated.
///
/// \param order Maximal order p+q for the `MomentsAttribute`.
void fl::MomentsHolder::init(int order){
    this->order = order;
    if (this->order < 2)
        this->order = 2;
    else if (this->order > 5)
        this->order = 5;

    int vecSize = 1;
    for (int i=2; i <= this->order; ++i)
        vecSize += i;

    this->binaryMoments.resize(vecSize, 0);
    this->rawMoments.resize(vecSize, 0);
    this->centralSet.resize(vecSize, false);
    this->centralMoments.resize(vecSize, 0.0);
    this->binCentralSet.resize(vecSize, false);
    this->binCentralMoments.resize(vecSize, 0.0);
    this->binNormSet.resize(vecSize, false);
    this->binNormalizedMoments.resize(vecSize, 0.0);
    this->normSet.resize(vecSize, false);
    this->normalizedMoments.resize(vecSize, 0.0);

    this->defaultValue = MomentType::raw;
    this->dp = this->dq = 0;
}



int fl::MomentsHolder::momIndex(int p, int q) const{
    int prefix = 0;
    for (int i=0, sum = this->order; i < q; ++i, --sum)
        prefix += sum;

    return p+prefix;
}

void fl::MomentsHolder::calculateDefaultCastValue(void){
    switch (this->defaultValue){
        case MomentType::binary:
            this->getBinaryMoment(this->dp, this->dq);
            break;
        case MomentType::raw:
            this->getRawMoment(this->dp, this->dq);
            break;
        case MomentType::bcentral:
        case MomentType::central:
            this->getCentralMoment(this->dp, this->dq, (this->defaultValue == MomentType::bcentral));
            break;
        case MomentType::bnormal:
        case MomentType::normal:
            this->getNormCentralMoment(this->dp, this->dq, (this->defaultValue == MomentType::bnormal));
            break;
        case MomentType::kurtosis:
            this->getKurtosis();
            break;
        case MomentType::roundness:
            this->getRoundness();
            break;
        case MomentType::eccentricity:
            this->getEccentricity();
            break;
        case MomentType::triangularity:
            this->getTriangularity();
            break;
        case MomentType::hu:
        default:
            this->getHuInvariant(this->dp);
            break;
    }
}

std::ostream& fl::operator<<(std::ostream &os, const fl::MomentsHolder& obj){
    switch (obj.defaultValue){
        case MomentType::binary:
            os << "BinaryMoment (p=" << obj.dp << ", q=" << obj.dq << "): " << (obj.getBinaryMoment(obj.dp, obj.dq));
            break;
        case MomentType::raw:
            os << "RawMoment (p=" << obj.dp << ", q=" << obj.dq << "): " << obj.getRawMoment(obj.dp, obj.dq);
            break;
        case MomentType::bcentral:
            os << "BinaryCentralMoment (p=" << obj.dp << ", q=" << obj.dq << "): " << obj.getCentralMomentNoCheck(obj.dp, obj.dq, true);
            break;
        case MomentType::central:
            os << "CentralMoment (p=" << obj.dp << ", q=" << obj.dq << "): " << obj.getCentralMomentNoCheck(obj.dp, obj.dq);
            break;
        case MomentType::bnormal:
            os << "BinaryNormalizedMoment (p=" << obj.dp << ", q=" << obj.dq << "): " << obj.getNormCentralMomentNoCheck(obj.dp, obj.dq, true);
            break;
        case MomentType::normal:
            os << "NormalizedMoment (p=" << obj.dp << ", q=" << obj.dq << "): " << obj.getNormCentralMomentNoCheck(obj.dp, obj.dq);
            break;
        case MomentType::kurtosis:
            os << "Kurtosis: " << obj.getKurtosisNoCheck();
            break;
        case MomentType::roundness:
            os << "Roundness: " << obj.getRoundnessNoCheck();
            break;
        case MomentType::eccentricity:
            os << "Eccentricity: " << obj.getEccentricityNoCheck();
            break;
        case MomentType::triangularity:
            os << "Triangularity: " << obj.getTriangularityNoCheck();
            break;
        case MomentType::hu:
        default:
            os << "Hu invariant moment (" << obj.dp << "): " << obj.getHuInvariantNoCheck(obj.dp);
            break;
    }
    return os;
}
