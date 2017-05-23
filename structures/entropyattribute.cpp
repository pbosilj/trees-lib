/// \file structures/entropyattribute.cpp
/// \author Petra Bosilj

#include "entropyattribute.h"

#include "node.h"
#include "imagetree.h"

#include "../misc/commontreedetail.h"

#include <iostream>
#include <cmath>

fl::EntropySettings::EntropySettings() {}
fl::EntropySettings::~EntropySettings() {}

const std::string fl::EntropyAttribute::name = "entropy";


/// \details \copydetails Attribute::Attribute()
fl::EntropyAttribute::EntropyAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, int deleteSettings)
    : TypedAttribute<double>(baseNode, baseTree, settings, deleteSettings), hist(std::vector<int>(0,0)) {
        this->initSettings();
}

fl::EntropyAttribute::~EntropyAttribute(){    }

/// Calculation of the `EntropyAttribute`. The calculation is done efficiently, by re-using
/// the `EntropyAttribute` of any child `Node`s in the `ImageTree` which contains the histogram
/// of values.
/// Any image element (pixel) position in the `ImageTree` is retrieved at most once.
///
/// \note The image needs to be associated to the `ImageTree` before
/// this function is called. This can be done by `ImageTree::setImage`.
/// After the calculation the image can be discarded by `ImageTree::unsetImage`.
void fl::EntropyAttribute::calculateAttribute(){
    if (! this->myTree->imageSet()){
        std::cerr << "Image not set for the ImageTree, giving up." << std::endl;
        std::exit(-2);
    }
    const cv::Mat &img = this->myTree->image();

    int minElem = detail::getCvMatMin(img);
    int maxElem = detail::getCvMatMax(img);

    this->hist.clear();
    this->hist.resize(maxElem-minElem+1, 0);

    int elems = 0;
    const std::vector <std::pair<int, int> > &ownElems = this->myNode->getOwnElements();
    for (int i=0, szi = ownElems.size(); i < szi; ++i){
//            int curElem = img.at<uchar>(ownElems[i].Y, ownElems[i].X);
        int curElem = detail::getCvMatElem(img, ownElems[i].X, ownElems[i].Y);
        ++this->hist[curElem-minElem];
        ++elems;
    }
    std::vector <Attribute *> childAttributes;
    this->myNode->getChildrenAttributes(EntropyAttribute::name, childAttributes);
    for (int i=0, szi = childAttributes.size(); i < szi; ++i){
        EntropyAttribute *chat = ((EntropyAttribute *)childAttributes[i]);
        chat->value();
        for (int j=0; j < maxElem-minElem+1; ++j){
            this->hist[j] += chat->hist[j];
            elems += chat->hist[j];
        }
    }

    this->attValue = 0;

    for (int i=0; i < maxElem-minElem+1; ++i){
        if (this->hist[i] > 0){
            double p = (double)this->hist[i]/elems;
            this->attValue -= p * std::log(p)/std::log(2.0);
        }
    }
    this->attValue+=1;
    TypedAttribute<double>::calculateAttribute();

}

