/// \file structures/node.tpp
/// \author Petra Bosilj

using namespace fl;

#ifndef TPP_NODE
#define TPP_NODE

#include "node.h"
#include "../misc/ellipse.h"

#include <cstdio>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>


/// The gray level is assigned based on the function \p f and the
/// elements associated to the `Node`.
///
/// \remark To be used when the gray level of a `Node` is a scalar
/// value (i.e. hierarchies from single channel images).

/// \note If the used function \p f requires the values of elements
/// associated with the `Node`, the user needs to ensure that an image
/// is associated with the hierarchy with ImageTree::setImage()
///
/// \param f A function used to calculate and assign the gray level.
template <class Function>
Node* Node::assignGrayLevelRec(Function f){
    std::vector <int> chSizes;
    std::vector <int> chGLevels;
    for (int i=0, szi = this->_children.size(); i < szi; ++i){
        this->_children[i]->assignGrayLevelRec(f);
        chSizes.push_back(this->_children[i]->elementCount());
        chGLevels.push_back(this->_children[i]->_grayLevel);
    }
    this->_grayLevel = f((int)this->_level, this->_S, chGLevels, chSizes);
    return this;
}


/// The gray level is assigned based on the function \p f and the
/// elements associated to the `Node`.
///
/// \remark To be used when the gray level of a `Node` is a vector value
/// (i.e. hierarchies from multichannel images).
///
/// \note If the used function \p f requires the values of elements
/// associated with the `Node`, the user needs to ensure that an image
/// is associated with the hierarchy with ImageTree::setImages()
///
/// \param f A function used to calculate and assign the gray level.
template <class Function>
Node* Node::assignHyperLevelRec(Function f){
    std::vector <int> chSizes;
    std::vector <std::vector < int > > chGLevels;
    for (int i=0, szi = this->_children.size(); i < szi; ++i){
        this->_children[i]->assignHyperLevelRec(f);
        chSizes.push_back(this->_children[i]->elementCount());
        chGLevels.push_back(this->_children[i]->_hgrayLevels);
    }

    this->_hgrayLevels = f(this->_level, this->_S, chGLevels, chSizes);
    return this;
}

/// \tparam TAT A specific `Attribute` class to be used in the search. Any specific
/// `TypedAttribute` where the `TypedAttribute::value()` returns a scalar types can be used
/// (e.g. `AreaAttribute`).
///
/// \return A pair made of the minimum and maximum level value of the specific `Attribute`
/// within
/// the sub-tree of this `Node`.
template <typename TAT>
std::pair<typename TAT::attribute_type, typename TAT::attribute_type> Node::minMaxAttribute(void) const{
    std::pair<typename TAT::attribute_type, typename TAT::attribute_type> minmaxval = std::make_pair(-1,-1);

    TAT *attr = ((TAT *)this->getAttribute(TAT::name));
    if (minmaxval.first == -1 || attr->value() < minmaxval.first)
        minmaxval.first = attr->value();
    if (minmaxval.second == -1 || attr->value() > minmaxval.second)
        minmaxval.second = attr->value();
    std::pair<typename TAT::attribute_type, typename TAT::attribute_type> childminmax;
    for (int i=0, szi = (int)this->_children.size(); i < szi; ++i){
        childminmax = this->_children[i]->minMaxAttribute<TAT>();
        if (minmaxval.first == -1 || childminmax.first < minmaxval.first)
            minmaxval.first = childminmax.first;
        if (minmaxval.second == -1 || childminmax.second > minmaxval.second)
        minmaxval.second = childminmax.second;
    }
    return minmaxval;
}

/// \tparam AT The specific `TypedAttribute` which is to be printed along the
/// subtree. The `TypedAttribute::value()` must be printable to a stream.
///
/// \note All the parameters are optional.
///
/// \param outStream The output stream to which the data should be
/// printed. By default, it equals to `std::cout`.
///
/// \param depth The current depth in the tree. Corresponds to the
/// amount of indentation printed before the information for that
/// `Node`. By default, it equals \(0\) (i.e. first `Node` information
/// not indented).
///
/// \remark Prints the number and a list of elements (pixels) associated
/// directly to each `Node`, as well as the `Node`'s level and
/// `TypedAttribute::value()` associated to each node.
template <typename AT>
void fl::Node::printElementsWithAttribute(std::ostream &outStream, int depth) const{
    if (this->getAttribute(AT::name) == NULL){
        return;
    }
    for (int i=0; i < depth; ++i)
        outStream << "\t";
    if (this->_S.empty())
        outStream << "empty ";
    else{
        outStream << this->_S.size() << " ";
        for (int i=0; i < (int)(this->_S).size(); ++i){
            outStream << "(" << this->_S[i].first << " " << this->_S[i].second << ") ";
        }
    }

    AT* attr = ((AT *)this->getAttribute(AT::name));

    outStream << " -> " << this->level() << " " << attr->value() << std::endl;
    for (int i=0, szi = (int)this->_children.size(); i < szi; ++i)
        this->_children[i]->printElementsWithAttribute<AT>(outStream, depth+1);
    return;
}

// not sure if we need this or not...
//template<class AT1, class AT2>
//void Node::printPartialWithAttribute(std::set <Node *> &toPrint, int cols, int rows){
//    if (this->getAttribute(AT1::name)== NULL || this->getAttribute(AT2::name)== NULL){
//        return;
//    }
//
//    bool printAtr = (this->isRoot() || (toPrint.find(this) != toPrint.end()));
//
//    if (printAtr){
//        const std::vector<std::vector<double> > &ps = this->
//                    getPatternSpectra2D(AT1::name+AT2::name)->getPatternSpectraMatrix();
//
//        std::cout << "<Node " << AT1::name+AT2::name << "=\"";
//        for (int j=1; j < (int)ps.size()-1; ++j){
//            for (int k=1; k < (int)ps[j].size()-1; ++k){
//                std::cout << std::pow(ps[j][k], 0.2) << ",";
//            }
//        }
//        std::cout << "\">" << std::endl;
//
//        std::vector <std::pair <int, int> > px;
//        this->getElements(px);
//
//
//        std::vector<cv::Point> forEllipse;
//        for (int j=0, szj = px.size(); j < szj; ++j)
//            forEllipse.push_back(cv::Point(px[j].first, px[j].second));
//
//        ellipse current(forEllipse, cols, rows);
//        std::cerr << current << std::endl;
//
//    }
//    for (int i=0, szi = (int)this->_children.size(); i < szi; ++i)
//        this->_children[i]->printPartialWithAttribute<AT1,AT2>(toPrint, cols, rows);
//
//    if (printAtr){
//        std::cout << "</Node>" << std::endl;
//    }
//}

#endif // TPP_NODE
