/// \file structures/patternspectra2d.tpp
/// \author Petra Bosilj

#ifndef TPP_PATTERNSPECTRA2D
#define TPP_PATTERNSPECTRA2D

#include "patternspectra2d.h"

#include "node.h"
#include "areaattribute.h"

#include "../algorithms/maxtreenister.h"

#include "momentsattribute.h"

#include "imagetree.h"

#include <iostream>

/// \details \copydetails AnyPatternSpectra2D::AnyPatternSpectra2D()
template <class AT1, class AT2>
fl::PatternSpectra2D<AT1,AT2>::PatternSpectra2D(Node *baseNode, const ImageTree *baseTree, PatternSpectra2DSettings *settings, bool deleteSettings)
                                    : AnyPatternSpectra2D(baseNode, baseTree, settings, deleteSettings){
    if (this->mySettings->cutOff && AT1::name == "area"){ // sets up the upper bin limits if Area is used for the increasing attribute and cutOff is set
        bool pass = false;
        if (this->myNode->parent() != NULL){ // if myNode is not root
            std::vector <double> &ulp = ((PatternSpectra2D<AT1,AT2> *)this->myNode->parent()->getPatternSpectra2D(AT1::name+AT2::name))->mySettings->upperLimits;
            if (ulp[0] != 0){ // copy upper limits from parent
                this->mySettings->upperLimits = ulp;
                pass = true;
            }
        }
        if (!pass){ // failing that, construct its own upper limit
            for (int i=0, szi = this->mySettings->firstAttBin.nBins; i < szi; ++i){
                double percentageLimit = this->mySettings->firstAttBin.getUnscaledUpperLimit(i+1);
                int upper = this->mySettings->firstAttBin.maxValue;
                int lower = this->mySettings->firstAttBin.minValue;
                int upperArea = percentageLimit * (upper-lower) + lower;

                cv::Mat image = cv::Mat::zeros(upperArea, 1, CV_8U);
                fl::Node *root =fl::maxTreeNister(image, std::greater<int>());
                fl::ImageTree *tree = new fl::ImageTree(root,std::make_pair(image.rows, image.cols));
                tree->addAttributeToTree<AT2>(this->myNode->getAttribute(AT2::name)->mySettings, false);
                this->mySettings->upperLimits[i] = this->mySettings->secondAttBin.getBin(((AT2 *)root->getAttribute(AT2::name))->value());

                tree->deleteAttributeFromTree<AT2>();
                delete tree;
            }
        }
    }
}

template <class AT1, class AT2>
void fl::PatternSpectra2D<AT1,AT2>::calculatePatternSpectra2D(PatternSpectra2DSettings *baseSettings, std::vector<Node *> *pathDown){
    PatternSpectra2DSettings *currentSettings = (baseSettings==NULL)?(this->mySettings):(baseSettings);

    int dimAt1 = currentSettings->firstAttBin.nBins + 2;
    int dimAt2 = currentSettings->secondAttBin.nBins + 2;

    std::vector<std::vector <double> > *curPSHolder = NULL;
    if (this->useTmp)
        curPSHolder = &(this->tmp);
    else
        curPSHolder = &(this->ps);

    curPSHolder->clear();
    curPSHolder->resize(dimAt1, std::vector<double>(dimAt2, 0)); // postponing reservation until here.

    std::vector <AnyPatternSpectra2D *> allChildrenPatternSpectra;
    this->myNode->getChildrenPatternSpectra2D(AT1::name+AT2::name,allChildrenPatternSpectra);

    for (int i=0, szi = allChildrenPatternSpectra.size(); i < szi; ++i){

        PatternSpectra2D<AT1,AT2> *cps = (PatternSpectra2D<AT1,AT2> *)allChildrenPatternSpectra[i];

        if (pathDown != NULL && pathDown->back() != cps->myNode)
            continue;

        if (this->mySettings->areaNormalize && this->mySettings->areaNorm != 1 &&
            !(this->mySettings->firstAttBin.relativeScale || this->mySettings->secondAttBin.relativeScale)){
            // this depends on "my settings" even if different base is passed around -- the norm in the base node for all the children
            cps->mySettings->areaNorm = this->mySettings->areaNorm;
        }

        const std::vector<std::vector<double> > &cpsvec = cps->getPatternSpectraMatrix(false, baseSettings, pathDown);

        for (int _i=0, _szi = cpsvec.size(); _i < _szi; ++_i){
            for (int _j=0, _szj = cpsvec.back().size(); _j < _szj; ++_j){
                if (cpsvec[_i][_j] > 0){
                }
                (*curPSHolder)[_i][_j] += cpsvec[_i][_j];
            }
        }

        if (! currentSettings->storeValue )
            cps->resetPatternSpectraMatrix();
    }

    int area = ((AreaAttribute *)(this->myNode->getAttribute(AreaAttribute::name)))->value();

    int bin1 = currentSettings->firstAttBin. getBin(double(((AT1 *)this->myNode->getAttribute(AT1::name))->value()));
    int bin2 = currentSettings->secondAttBin.getBin(double(((AT2 *)this->myNode->getAttribute(AT2::name))->value()));

    if (this->myNode->parent() != NULL){

        if (bin1 == 0 || bin2 == 0 || bin1 == (currentSettings->firstAttBin.nBins +1) || bin2 == (currentSettings->secondAttBin.nBins +1) ||
            currentSettings->cutOff == false ||
            bin2 <= currentSettings->upperLimits[bin1-1]){
            (*curPSHolder)[bin1][bin2] += std::abs(this->myNode->level() - this->myNode->parent()->level()) * area / currentSettings->areaNorm;
        }
    }

    AnyPatternSpectra2D::calculatePatternSpectra2D(baseSettings);
}

template <class AT1, class AT2>
const std::vector<std::vector<double> > &fl::PatternSpectra2D<AT1,AT2>::getPatternSpectraMatrix(std::vector<Node *> *pathDown){
    this->myTree->template ensureDefaultSettings<AT1>();
    this->myTree->template ensureDefaultSettings<AT2>();

    const std::vector<std::vector<double> > &rv = fl::AnyPatternSpectra2D::getPatternSpectraMatrix(pathDown);

    this->myTree->template revertSettingsChanges<AT1>();
    this->myTree->template revertSettingsChanges<AT2>();

    return rv;
}

template<class AT1, class AT2>
const std::vector<std::vector<double> > &fl::PatternSpectra2D<AT1, AT2>::getPatternSpectraMatrix(bool userCall, PatternSpectra2DSettings *settings, std::vector<Node *> *pathDown){
    return fl::AnyPatternSpectra2D::getPatternSpectraMatrix(userCall, settings, pathDown);
}

#endif // TPP_PATTERNSPECTRA2D
