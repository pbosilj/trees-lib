/// \file structures/sparsityattribute.cpp
/// \author Petra Bosilj

#include "sparsityattribute.h"

#include "node.h"

const std::string fl::SparsityAttribute::name = "sparsity";

/// \details \copydetails Attribute::Attribute()
fl::SparsityAttribute::SparsityAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings) { this->initSettings(); }

fl::SparsityAttribute::~SparsityAttribute() { }

/// Calculation of the `SparsityAttribute`. The calculation is done efficiently, by re-using
/// the `SparsityAttribute` of any child `Node`s in the `ImageTree`.
/// Any image element (pixel) position in the `ImageTree` is retrieved at most once.
void fl::SparsityAttribute::calculateAttribute(){
    //this->attValue = this->myNode->getOwnElements().size();

    //const std::vector <std::pair <int, int> > &getOwnElements() const;

    std::vector <cv::Point2f> for_calc;
    std::vector <Attribute *> childAttributes;

    std::vector <cv::Point2f> for_calc_control;
    std::vector <cv::Point2f> convex_hull_control;

    this->myNode->getChildrenAttributes(SparsityAttribute::name, childAttributes);
    int myArea =0;

    for (int i=0, szi = childAttributes.size(); i < szi; ++i){
        SparsityAttribute *chat = ((SparsityAttribute *)childAttributes[i]);
        chat->value();
        for_calc.insert(for_calc.end(), chat->convex_hull.begin(), chat->convex_hull.end());
        myArea += chat->area;
        //this->attValue += ((AreaAttribute *)childAttributes[i])->value();
    }

    const std::vector <std::pair<int, int> > &own_points = this->myNode->getOwnElements();
    myArea += own_points.size();

    this->area = myArea;

    for (int i=0, szi = own_points.size(); i < szi; ++i)
        for_calc.emplace_back(cv::Point2f(own_points[i].second, own_points[i].first));

    cv::convexHull(cv::Mat(for_calc), this->convex_hull, false);

//    // Now calculate the area of convex hull 'ch':
//    double area = 0;
//    for (int i = 0, szi = this->convex_hull.size(); i < szi; ++i){
//        int next_i = (i+1)%(szi);
//        double dX   = this->convex_hull[next_i].x - this->convex_hull[i].x;
//        double avgY = (this->convex_hull[next_i].y + this->convex_hull[i].y)/2;
//        area += dX*avgY;  // This is the integration step.
//    }

    if (myArea >= std::abs(cv::contourArea(this->convex_hull)))
        this->attValue = 1;
    else
        this->attValue = (double)myArea / std::abs(cv::contourArea(this->convex_hull));

    TypedAttribute<double>::calculateAttribute();
}
