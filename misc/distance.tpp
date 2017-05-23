#ifndef TPP_DISTANCE
#define TPP_DISTANCE

#include "distance.h"

#include <iostream>

#include <cmath>

/// \param x The first multispectral pixel (image element) to be compared (`integer`).
/// \param y The second multispectral pixel (image element) to be compared (`integer`).
/// \return The L1 distance between \p x and \p y (`integer`).
template <typename TI>
int fl::distanceL1Integral<TI>::operator ()(std::vector <TI *> x, std::vector <TI *> y) const{
    int rvalue = 0;
    if (x.size() != y.size())
        return (uchar)0;
    for (int i=0, szi = x.size(); i < szi; ++i){
        rvalue += std::abs(*x[i] - *y[i]);
    }
    return rvalue;
}

/// \param x The first multispectral pixel (image element) to be compared (`double`).
/// \param y The second multispectral pixel (image element) to be compared (`double`).
/// \return The L1 distance between \p x and \p y (`double`).
template <typename TNI>
double fl::distanceL1NonIntegral<TNI>::operator ()(std::vector <TNI *> x, std::vector <TNI *> y) const{
    double rvalue = 0;
    if (x.size() != y.size())
        return 0.0;
    for (int i=0, szi = x.size(); i < szi; ++i){
        rvalue += std::abs(*x[i] - *y[i]);
    }
    return rvalue;
}

/// \param x The first multispectral pixel (image element) to be compared (`integer`).
/// \param y The second multispectral pixel (image element) to be compared (`integer`).
/// \return The L2 distance between \p x and \p y squared (`integer`).
template <typename TI>
int fl::distanceL2square<TI>::operator()(std::vector <TI *> x, std::vector <TI *> y) const{
    int rvalue = 0;
    if (x.size() != y.size())
        return (uchar)0;
    for (int i=0, szi = x.size(); i < szi; ++i){
        rvalue += (*x[i] - *y[i])*(*x[i] - *y[i]);
    }
    return rvalue;
}

/// \param x The first multispectral pixel (image element) to be compared (any scalar values).
/// \param y The second multispectral pixel (image element) to be compared (any scalar values).
/// \return The L2 distance between \p x and \p y (`double`).
template <typename T>
double fl::distanceL2<T>::operator()(std::vector <T *> x, std::vector <T *> y) const{
    double rvalue = 0;
    if (x.size() != y.size())
        return 0.0;
    for (int i=0, szi = x.size(); i < szi; ++i){
        rvalue += (*x[i] - *y[i])*(*x[i] - *y[i]);
    }
    return std::sqrt(rvalue);
}

/// \param x The first multispectral pixel (image element) to be compared (any input type).
/// \param y The second multispectral pixel (image element) to be compared (any input type).
/// \return The SAM distance between \p x and \p y.
template <typename T>
double fl::distanceSAM<T>::operator ()(std::vector <T *>x, std::vector <T *> y) const{
    double xy = 0.0, xx = 0.0, yy = 0.0;
    if (x.size() != y.size()){
        return -1.0;
    }
    double value = (xy / (std::sqrt(xx)*std::sqrt(yy)));
    for (int i=0, szi = x.size(); i < szi; ++i){
        xy += (*x[i]) * (*y[i]);
        xx += (*x[i]) * (*x[i]);
        yy += (*y[i]) * (*y[i]);
    }
    return std::acos(value);
}
#endif

