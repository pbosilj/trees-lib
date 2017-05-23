/// \file algorithms/predicate.tpp

#ifndef TPP_PREDICATE
#define TPP_PREDICATE

#include "predicate.h"

template <class valType>
fl::Predicate<valType>::Predicate(){ }

template <class valType>
fl::Predicate<valType>::~Predicate(){ }

template <class valType>
bool fl::DifferentThanParent<valType>::operator ()(valType myValue, valType parentValue) const{
    return !(myValue == parentValue);
}

template <class valType>
bool fl::GreaterThanX<valType>::operator ()(valType myValue, valType /*parentValue*/) const{
    return myValue > this->x;
}

template <class valType>
bool fl::ParentGreaterThanX<valType>::operator ()(valType /*myValue*/, valType parentValue) const{
    return this->gtx(parentValue, parentValue);
}

template <class valType>
bool fl::LessThanX<valType>::operator ()(valType myValue, valType /*parentValue*/) const{
    return myValue < this->x;
}

template <class valType>
bool fl::ParentLessThanX<valType>::operator ()(valType /*myValue*/, valType parentValue) const{
    return this->ltx(parentValue, parentValue);
}

template <class valType>
bool fl::LessEqualThanX<valType>::operator ()(valType myValue, valType /*parentValue*/) const{
    return !(this->gtx(myValue, myValue));
}

template <class valType>
bool fl::ParentLessEqualThanX<valType>::operator ()(valType /*myValue*/, valType parentValue) const{
    return this->letx(parentValue, parentValue);
}

template <class valType>
bool fl::GreaterEqualThanX<valType>::operator ()(valType myValue, valType /*parentValue*/) const{
    return !(this->ltx(myValue, myValue));
}

template <class valType>
bool fl::ParentGreaterEqualThanX<valType>::operator ()(valType /*myValue*/, valType parentValue) const{
    return this->getx(parentValue, parentValue);
}

template <class valType>
bool fl::AlphaPredicateX<valType>::operator ()(valType myValue, valType parentValue) const{
    return this->pgtx(myValue,parentValue);
    //return ((myValue >= this->x ) || (parentValue > this->x));
}


#endif // TPP_PREDICATE

