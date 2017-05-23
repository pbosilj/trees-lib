/// \file structures/momentsholder.h
/// \author Petra Bosilj

#ifndef MOMENTSHOLDER_H
#define MOMENTSHOLDER_H

#include <string>
#include <vector>

#include <iostream>

namespace fl{

    /// \brief Type of moments which is the default cast value of `MomentsHolder`
    ///
    /// Used to define which type of moment to return by `MomentsHolder`.
    enum class MomentType {
        binary,         ///< Raw binary moments
        raw,            ///< Raw grayscale moments
        bcentral,       ///< Centralized binary moments
        central,        ///< Centralized grayscale moments
        bnormal,        ///< Binary normalized centralized moments
        normal,         ///< Normalized centralized moments
        kurtosis,       ///< Kurtosis of the region
        roundness,      ///< Roundness of the region
        eccentricity,   ///< Eccentricity of the region
        triangularity,  ///< Triangularity of the region
        hu              ///< First order Hu moments
    };

    /// \class MomentsHolder
    ///
    /// \brief A class holding the value of various moments calculated for a region
    ///
    /// All the available moments are calculated efficiently and simultaneously, while only
    /// the output type (in case of a cast to `double` or output with `std::ostream& operator<<`)
    /// is determined by `MomentType` set by `MomentsHolder::setDefaultCastValue`
    class MomentsHolder{
        public:

            /// \brief Constructor for `MomentsHolder`.
            MomentsHolder();

            /// \brief Destructor for `MomentsHolder`.
            ~MomentsHolder();

            /// \brief Get the value of the raw binary moment.
            long long getBinaryMoment(int p, int q) const;

            /// \brief Get the value of the raw grayscale moment.
            long long getRawMoment(int p, int q) const;

            /// \brief Get the value of the centralized moment.
            double getCentralMoment(int p, int q, bool binCalc = false);

            /// \brief Get the value of the normalized centralized moment
            double getNormCentralMoment(int p, int q, bool binCalc = false);

            /// \brief Get the value of kurtosis.
            double getKurtosis();

            /// \brief Get the roundness value.
            double getRoundness();

            /// \brief Get the value of eccentricity.
            double getEccentricity();

            /// \brief Get the value of triangularity.
            double getTriangularity();

            /// \brief Get the invariant moment of Hu.
            double getHuInvariant(int p = 1);

            /// \brief Set the default cast value of the `MomentsHolder`.
            void setDefaultCastValue(MomentType attribName, int dp = -1, int dq = -1);

            /// \brief Retrieves the value of the specified moment through a cast in `double`.
            ///
            /// The default value needs to be set using `MomentType` through
            /// `MomentsHolder::setDefaultCastValue`.
            operator double() {
                switch(this->defaultValue){
                    case MomentType::binary:
                        return (double)(this->getBinaryMoment(this->dp, this->dq));
                        break;
                    case MomentType::raw:
                        return (double)this->getRawMoment(this->dp, this->dq);
                        break;
                    case MomentType::bcentral:
                    case MomentType::central:
                        return this->getCentralMoment(this->dp, this->dq, (this->defaultValue == MomentType::bcentral));
                        break;
                    case MomentType::bnormal:
                    case MomentType::normal:
                        return this->getNormCentralMoment(this->dp, this->dq, (this->defaultValue == MomentType::bnormal));
                        break;
                    case MomentType::kurtosis:
                        return this->getKurtosis();
                        break;
                    case MomentType::roundness:
                        return this->getRoundness();
                        break;
                    case MomentType::eccentricity:
                        return this->getEccentricity();
                        break;
                    case MomentType::triangularity:
                        return this->getTriangularity();
                        break;
                    case MomentType::hu:
                    default:
                        return this->getHuInvariant(this->dp);
                        break;
                }
            }

            friend std::ostream& operator<<(std::ostream &os, const MomentsHolder &obj);

            friend class MomentsAttribute;
        protected:

            /// \brief Retrieve the value of the centralized moment.
            double getCentralMomentNoCheck(int p, int q, bool binCalc = false) const;

            /// \brief Retrieve the value of the normaliezd centralized moment.
            double getNormCentralMomentNoCheck(int p, int q, bool binCalc = false) const;

            /// \brief Retrieve the value of kurtosis.
            double getKurtosisNoCheck() const;

            /// \brief Retrieve the roundness value.
            double getRoundnessNoCheck() const;

            /// \brief Retrieve the value of eccentricity.
            double getEccentricityNoCheck() const;

            /// \brief Retrieve the value of triangularity.
            double getTriangularityNoCheck() const;

            /// \brief Retrieve the invariant moment of Hu.
            double getHuInvariantNoCheck(int p) const;
        private:
            // maybe not, duplicating info? delete moments and pass as a param?
            int order;
            std::vector <long long> binaryMoments;
            std::vector <long long> rawMoments;
            std::vector <bool> centralSet;
            std::vector <double> centralMoments;
            std::vector <bool> binCentralSet;
            std::vector <double> binCentralMoments;
            std::vector <bool> binNormSet;
            std::vector <double> binNormalizedMoments;
            std::vector <bool> normSet;
            std::vector <double> normalizedMoments;

            static double kurtosisCalc(double bnm40, double bnm20);
            static double roundnessCalc(long long bm00, double bcm20, double bcm02);
            static double eccentricityCalc(double mu20, double mu02, double mu11);
            static double triangularityCalc(long long bm00, double mu20, double mu02, double mu11);

            void calculateDefaultCastValue(void);

            int momIndex(int p, int q) const;

            /// \brief Initializes the holders for different types of moments.
            void init(int order);

            bool meanSet;
            std::pair<double, double> negMean;
            bool binMeanSet;
            std::pair<double, double> negBinMean;

            MomentType defaultValue;
            int dp, dq;
    };

    /// \brief Output operator for `MomentsHolder`.
    std::ostream& operator<<(std::ostream &os, const MomentsHolder &obj);
}

#endif // MOMENTSHOLDER_H
