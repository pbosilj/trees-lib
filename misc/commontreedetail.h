#ifndef COMMONTREEDETAIL_H
#define COMMONTREEDETAIL_H

#include <vector>
#include <map>

#include "pixels.h"

#include <opencv2/imgproc/imgproc.hpp>

namespace fl{
    namespace detail{

        class maxTreeGrayLvlAssign{
            public:
                maxTreeGrayLvlAssign(const cv::Mat &img) : img(img) {}
                int operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const;
            private:
                const cv::Mat &img;
        };

        class alphaTreeGrayLvlAssign{
            public:
                alphaTreeGrayLvlAssign(const cv::Mat &img) : img(img) {}
                int operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const;
            private:
                const cv::Mat &img;
        };

        class alphaTreeGrayLvlHyperAssign{
            public:
                alphaTreeGrayLvlHyperAssign(const std::vector<cv::Mat> &imgs) : imgs(imgs) {}
                std::vector<int> operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chGL, const std::vector<int> &chSz) const;
            private:
                const std::vector<cv::Mat> &imgs;
        };

        class alphaTreeMinLvlAssign{
            public:
                alphaTreeMinLvlAssign(const cv::Mat &img) : img(img) {}
                int operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const;
            private:
                const cv::Mat &img;
        };

        class alphaTreeMinLvlHyperAssign{
            public:
                alphaTreeMinLvlHyperAssign(const std::vector<cv::Mat> &imgs) : imgs(imgs) {}
                std::vector<int> operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chGL, const std::vector<int> &chSz) const;
            private:
                const std::vector<cv::Mat> &imgs;
        };

        class alphaTreeMaxLvlAssign{
            public:
                alphaTreeMaxLvlAssign(const cv::Mat &img) : img(img) {}
                int operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<int> &chGL, const std::vector<int> &chSz) const;
            private:
                const cv::Mat &img;
        };

        class alphaTreeMaxLvlHyperAssign{
            public:
                alphaTreeMaxLvlHyperAssign(const std::vector<cv::Mat> &imgs) : imgs(imgs) {}
                std::vector<int> operator() (double myLvl, const std::vector <pxCoord> &ownElems, const std::vector<std::vector<int> > &chGL, const std::vector<int> &chSz) const;
            private:
                const std::vector<cv::Mat> &imgs;
        };

        double getCvMatElem(const cv::Mat &img, const std::pair<int, int> &coord);
        double getCvMatElem(const cv::Mat &img, int X, int Y);
        cv::MatConstIterator getCvMatBegin(const cv::Mat &img);
        cv::MatConstIterator getCvMatEnd(const cv::Mat &img);
        double getDerefCvMatConstIterator (cv::MatConstIterator &it, int matType);

        double getCvMatMax(const cv::Mat &img);
        double getCvMatMin(const cv::Mat &img);

        template <class Compare>
        double getCvMatMax(const cv::Mat &img, Compare cmp);

        template <class Compare>
        double getCvMatMin(const cv::Mat &img, Compare cmp);

        void histogramToGCF(std::map <double, int> &data);


        /// \class RMQ
        ///
        /// \brief An abstract `RMQ` Functor which is the basis to implement
        /// Range Minimum Query (used in solving Least Common Ancestor) lookup
        /// on a sequence
        ///
        /// \tparam T The type of elements in the sequence.
        template <typename T>
        class RMQ{
            public:
                /// \brief The RMQ constructor.
                /// \param sequence The sequence on which we will do RMQ.
                RMQ(const std::vector<T> &sequence) : sequence(sequence) {}
                /// \brief The desctructor.
                virtual ~RMQ();
                /// \brief Operator which returns RMQ(sequence)[start, end], that
                /// is the index of the minimal element between the given indices.
                /// \param iStart Index of the start of the range.
                /// \param iEnd Index of the end of the range.
                virtual unsigned int operator() (unsigned int iStart, unsigned int iEnd) const = 0;
            protected:
                const std::vector<T> &sequence;
                std::vector <std::vector<T> > mem;
        };

        /// \class RMQNaive
        ///
        /// \brief The naive implementation of RMQ, requires O(n^2) preprocessing time
        /// and O(1) query time for a sequence of the length n.
        template <typename T>
        class RMQNaive : RMQ<T>{
            public:
                /// \copydoc RMQ::RMQ()
                RMQNaive(const std::vector<T> &sequence);
                /// \copydoc RMQ::~RMQ()
                virtual ~RMQNaive() {}
                /// \copydoc RMQ::operator()
                /// \note add array-out-of-bounds check.
                unsigned int operator() (unsigned int iStart, unsigned int iEnd) const {return this->mem[iStart][iEnd];}
        };

        /// \class RMQSparseTable
        ///
        /// \brief The sparse table implementation of RMQ, requires O(n logn) preprocessing time
        /// and O(1) query time for a sequence of the length n.
        template <typename T>
        class RMQSparseTable : RMQ<T>{
            public:
                /// \copydoc RMQ::RMQ()
                RMQSparseTable(const std::vector<T> &sequence);
                /// \copydoc RMQ::~RMQ()
                virtual ~RMQSparseTable() {}
                /// \copydoc RMQ::operator()
                /// \note add array-out-of-bounds check.
                unsigned int operator() (unsigned int iStart, unsigned int iEnd) const;
        };


        /// \class RMQSparseTable
        ///
        /// \brief The implementation of RMQ specialized for +-1 sequences, that is
        /// sequences where the difference between any two neighbouring elements is
        /// exactly plus or minus one.
        /// This implementation enables O(n) preprocessing time and O(1) query time
        /// for such sequences.
        template <typename T>
        class RMQPlusMinusOne : RMQ<T>{
            public:
                /// \copydoc RMQ::RMQ()
                /// \note add plus-minus-one-check
                RMQPlusMinusOne(const std::vector<T> &sequence);
                /// \copydoc RMQ::~RMQ()
                virtual ~RMQPlusMinusOne();
                /// \copydoc RMQ::operator()
                /// \note add array-out-of-bounds check.
                unsigned int operator() (unsigned int iStart, unsigned int iEnd) const;
            private:
                unsigned int blockSize;
                std::vector <T> reducedSequence;
                std::vector <unsigned int> reducedIndices;
                std::vector <unsigned int> reducedCodes;

                std::vector <RMQNaive<T>> normalizedBlocks;
                RMQSparseTable<T> *helperRMQ;

                static void codeToSequence(const unsigned int code, const unsigned int len, std::vector<T> &sequence);
        };



    }
}

#include "commontreedetail.tpp"

#endif // COMMONTREEDETAIL_H
