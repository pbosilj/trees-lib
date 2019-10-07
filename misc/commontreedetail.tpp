#ifndef TPP_COMMONTREEDETAIL
#define TPP_COMMONTREEDETAIL

#include "commontreedetail.h"

#include <iostream>

namespace fl{

    template <class Compare>
    double detail::getCvMatMax(const cv::Mat &img, Compare cmp){
        if (img.type() == CV_8U)
            return *std::max_element(img.begin<uchar>(), img.end<uchar>(), cmp);
        else if (img.type() == CV_32S)
            return *std::max_element(img.begin<int32_t>(), img.end<int32_t>(), cmp);
        else if (img.type() == CV_32F)
            return *std::max_element(img.begin<float>(), img.end<float>(), cmp);
        else if (img.type() == CV_16S)
            return *std::max_element(img.begin<short>(), img.end<short>(), cmp);
        else if (img.type() == CV_16U)
            return *std::max_element(img.begin<unsigned short>(), img.end<unsigned short>(), cmp);
        return *std::max_element(img.begin<uchar>(), img.end<uchar>(), cmp);
    }

    template <class Compare>
    double detail::getCvMatMin(const cv::Mat &img, Compare cmp){
        if (img.type() == CV_8U)
            return *std::min_element(img.begin<uchar>(), img.end<uchar>(), cmp);
        else if (img.type() == CV_32S)
            return *std::min_element(img.begin<int32_t>(), img.end<int32_t>(), cmp);
        else if (img.type() == CV_32F)
            return *std::min_element(img.begin<float>(), img.end<float>(), cmp);
        else if (img.type() == CV_16S)
            return *std::min_element(img.begin<short>(), img.end<short>(), cmp);
        else if (img.type() == CV_16U)
            return *std::min_element(img.begin<unsigned short>(), img.end<unsigned short>(), cmp);
        return *std::min_element(img.begin<uchar>(), img.end<uchar>(), cmp);
    }

    template <typename T>
    detail::RMQ<T>::~RMQ(){ }

    /// \brief Preprocessing for the naive RMQ, O(n^2)
    ///
    /// \note Currently uses an additional array of size n^2. This can be
    /// reduced by a factor of 2 (the array is symetrical over the diagonal).
    template<typename T>
    detail::RMQNaive<T>::RMQNaive(const std::vector<T> &sequence) : RMQ<T>(sequence) {
        for (int i=0, szs = this->sequence.size(); i < szs; ++i){
            this->mem.emplace_back(std::vector<T>(szs));
            for (int j=0; j < i; ++j)
                this->mem[i][j] = this->mem[j][i];
            this->mem[i][i] = i;
            for (int j=i+1; j < szs; ++j){
                if (this->sequence[j] < this->sequence[this->mem[i][j-1]])
                    this->mem[i][j] = j;
                else
                    this->mem[i][j] = this->mem[i][j-1];
            }
        }
    }

    template<typename T>
    detail::RMQSparseTable<T>::RMQSparseTable(const std::vector<T> &sequence) : RMQ<T>(sequence) {
        for (int j=0, szi = this->sequence.size(), szj = std::ceil(std::log2(szi)); j < szj; ++j){
            for (int i=0; i < szi; ++i){
                if (j == 0)
                    this->mem.emplace_back(std::vector<T>(szj));

                unsigned int lowerRangeIndex, upperRangeIndex;
                bool upperExists = false;
                if (j==0){
                    lowerRangeIndex = i;
                    if (i < (szi-1)){
                        upperExists = true;
                        upperRangeIndex = i+1;
                    }
                }
                else{
                    lowerRangeIndex = this->mem[i][j-1];
                    if (i + std::pow(2, j) < szi){
                        upperExists = true;
                        upperRangeIndex = this->mem[i+std::pow(2,j)][j-1];
                    }
                }
                if (!upperExists || this->sequence[lowerRangeIndex] < this->sequence[upperRangeIndex])
                    this->mem[i][j] = lowerRangeIndex;
                else
                    this->mem[i][j] = upperRangeIndex;
            }
        }
    }

    template<typename T>
    unsigned int detail::RMQSparseTable<T>::operator ()(unsigned int iStart, unsigned int iEnd) const{
        if (iStart == iEnd)
            return iStart;
        if (iEnd < iStart){
            std::swap(iEnd, iStart);
        }
        if (iEnd - iStart == 1)
            return this->mem[iStart][0];
        int k = std::floor(std::log2(iEnd - iStart));
        unsigned int lowerIndex = this->mem[iStart][k-1];
        unsigned int upperIndex = this->mem[iEnd - std::pow(2, k) + 1][k-1];
        if (this->sequence[lowerIndex] < this->sequence[upperIndex])
            return lowerIndex;
        else
            return upperIndex;
    }

    template<typename T>
    detail::RMQPlusMinusOne<T>::RMQPlusMinusOne(const std::vector<T> &sequence) : RMQ<T>(sequence) {
        this->blockSize = std::ceil(std::log2(sequence.size())/2);
        //std::cout << "Input length " << sequence.size() << std::endl;
        //std::cout << "Block size : " << this->blockSize << " A' length: " << std::ceil((double)sequence.size()/this->blockSize) << std::endl;
        unsigned int code = 0;


        //                                      { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33}
        //std::vector <int> test_pmo_sequence = { 3, 2, 1, 2, 3, 4, 5, 6, 5, 4, 5, 6, 7, 6, 5, 4, 3, 2, 3, 4, 5, 6, 5, 4, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4};

        for (int i=0, szi = this->sequence.size(), pos = 0; i < szi; ++i){
            if ((i % this->blockSize) == 0){
                pos = i;
                code = 0;
            }
            else{
                if (this->sequence[i] < this->sequence[pos])
                    pos = i;

                if (!(std::abs(this->sequence[i] - this->sequence[i-1]) == 1))
                    throw "Invalid input sequence";

                code <<= 1;
                if ((this->sequence[i] - this->sequence[i-1]) == 1)
                    code += 1;

            }

            if (((i+1) % this->blockSize) == 0 || i == (szi-1)){
                this->reducedSequence.emplace_back(this->sequence[pos]);
                this->reducedIndices.emplace_back(pos);
                this->reducedCodes.emplace_back(code);
            }
        }

        this->helperRMQ = new RMQSparseTable<T>(reducedSequence);
        std::vector <T> smallSequence;
        for (unsigned int code = 0, totalCodes = 1<<(this->blockSize-1); code < totalCodes; ++code){
            RMQPlusMinusOne<T>::codeToSequence(code, this->blockSize, smallSequence);
            this->normalizedBlocks.emplace_back(smallSequence);
        }
    }

    template <typename T>
    detail::RMQPlusMinusOne<T>::~RMQPlusMinusOne(){ delete this->helperRMQ; }

    template<typename T>
    unsigned int detail::RMQPlusMinusOne<T>::operator ()(unsigned int iStart, unsigned int iEnd) const{

        if (iEnd < iStart){
            std::swap(iEnd, iStart);
        }

        int wholeBottomBlock = std::ceil((float)iStart/this->blockSize);
        int wholeTopBlock = std::floor((float)(iEnd+1)/this->blockSize) -1;

        unsigned int minIndex = 0, tempMinIndex;
        bool minSet = false;

        unsigned int curStart, curEnd;

        if (iStart < wholeBottomBlock * this->blockSize){ // both ranges together or just bottom range
            curStart = iStart;
            curEnd = wholeBottomBlock * this->blockSize - 1;
            if (iEnd <= curEnd)
                curEnd = iEnd;

            minIndex = this->normalizedBlocks[this->reducedCodes[wholeBottomBlock-1]](curStart % this->blockSize, curEnd % this->blockSize)
                        + (wholeBottomBlock-1) * this->blockSize;
            minSet = true;

            if (iEnd == curEnd)
                return minIndex;
        }
        if (iEnd >= (wholeTopBlock+1)*this->blockSize){
            curEnd = iEnd;
            curStart = (wholeTopBlock+1)*this->blockSize;
            tempMinIndex = this->normalizedBlocks[this->reducedCodes[wholeTopBlock+1]](curStart % this->blockSize, curEnd % this->blockSize) + curStart;
            if (!minSet || this->sequence[tempMinIndex] < this->sequence[minIndex])
                minIndex = tempMinIndex;
            minSet = true;
        }
        if (wholeBottomBlock <= wholeTopBlock){

            tempMinIndex = reducedIndices[(*(this->helperRMQ))(wholeBottomBlock, wholeTopBlock)];

            if (!minSet || this->sequence[tempMinIndex] < this->sequence[minIndex])
                minIndex = tempMinIndex;
            minSet = true;
        }

        if (!minSet)
            throw "Something went wrong";

        return minIndex;
    }

    template<typename T>
    void detail::RMQPlusMinusOne<T>::codeToSequence(const unsigned int code, const unsigned int len, std::vector<T> &sequence){
        sequence.clear();
        sequence.resize(1); // first element default
        unsigned int factor = 1<<(len-2);
        while (factor > 0){
            if (code & factor)
                sequence.emplace_back(sequence.back() + 1);
            else
                sequence.emplace_back(sequence.back() - 1);
            factor >>= 1;
        }
    }
}

#endif

