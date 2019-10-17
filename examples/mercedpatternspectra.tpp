/// \file examples/mercedpatternspectra.tpp
/// \author Petra Bosilj
/// \date 15/10/2019

#ifndef TPP_MERCEDPATTERNSPECTRA
#define TPP_MERCEDPATTERNSPECTRA

#include <iostream>
#include <utility>
#include <algorithm>
#include <iomanip>

#include "../misc/misc.h"

#include "../structures/imagetree.h"
#include "../algorithms/maxtreenister.h"
#include "../algorithms/attributeconstruction.h"

#include "../structures/attribute.h"
#include "../structures/areaattribute.h"
#include "../structures/noncompactnessattribute.h"
#include "../structures/entropyattribute.h"
#include "../structures/patternspectra2d.h"

template <typename SizeAttribute, typename ShapeAttribute>
void fl::outputGlobalPS(cv::Mat &image, std::ostream &outPS, int sizeBins, int sizeMax, int shapeBins, int shapeMax, int sizeScale){
        {
            fl::ImageTree *maxTree = new fl::ImageTree(fl::maxTreeNister(image, std::greater<int>()), // max-tree
                                                        std::make_pair(image.rows, image.cols));
            maxTree->setImage(image);
            maxTree->addAttributeToTree<SizeAttribute>(getSettingsForAttributeType<SizeAttribute>());
            maxTree->addAttributeToTree<ShapeAttribute>(getSettingsForAttributeType<ShapeAttribute>());

            if (sizeScale > 0){
                maxTree->addPatternSpectra2DToTree<SizeAttribute, ShapeAttribute>
                    (new fl::PatternSpectra2DSettings(fl::Binning(sizeBins,  1, sizeMax, fl::Binning::Scale::logarithmic, sizeScale),
                                                      fl::Binning(shapeBins, 1, shapeMax, fl::Binning::Scale::logarithmic), true, true, true));
            }
            else{
                maxTree->addPatternSpectra2DToTree<SizeAttribute, ShapeAttribute>
                    (new fl::PatternSpectra2DSettings(fl::Binning(sizeBins,  1, sizeMax, fl::Binning::Scale::logarithmic),
                                                      fl::Binning(shapeBins, 1, shapeMax, fl::Binning::Scale::logarithmic), true, true, true));
            }

            const std::vector<std::vector<double> > &ps = maxTree->root()->getPatternSpectra2D(SizeAttribute::name+ShapeAttribute::name)->getPatternSpectraMatrix();

            std::vector<std::vector<double> >pps;

            for (int j=1; j < (int)ps.size()-1; ++j){
                pps.push_back(std::vector<double>());
                for (int k=1; k < (int)ps[j].size()-1; ++k){
                    outPS << std::pow(ps[j][k], 0.2) << " ";
                    pps.back().push_back(std::pow(ps[j][k], 0.2)); // store the PS root
                    // pps.back().push_back(ps[j][k]);          // store PS normally
                }
            }
            //visualizePS(pps);

            maxTree->deletePatternSpectra2DFromTree<SizeAttribute, ShapeAttribute>();
            maxTree->deleteAttributeFromTree<ShapeAttribute>();
            maxTree->deleteAttributeFromTree<SizeAttribute>();

            maxTree->unsetImage();
            delete maxTree;
        }
        {
            fl::ImageTree *minTree = new fl::ImageTree(fl::maxTreeNister(image, std::less<int>()), // min-tree
                                                        std::make_pair(image.rows, image.cols));
            minTree->setImage(image);

            minTree->addAttributeToTree<SizeAttribute>(fl::getSettingsForAttributeType<SizeAttribute>());
            minTree->addAttributeToTree<ShapeAttribute>(fl::getSettingsForAttributeType<ShapeAttribute>());

            if (sizeScale > 0){
                minTree->addPatternSpectra2DToTree<SizeAttribute, ShapeAttribute>
                    (new fl::PatternSpectra2DSettings(fl::Binning(sizeBins,  1, sizeMax, fl::Binning::Scale::logarithmic, sizeScale),
                                                      fl::Binning(shapeBins, 1, shapeMax, fl::Binning::Scale::logarithmic), true, true, true));
            }
            else{
                minTree->addPatternSpectra2DToTree<SizeAttribute, ShapeAttribute>
                    (new fl::PatternSpectra2DSettings(fl::Binning(sizeBins,  1, sizeMax, fl::Binning::Scale::logarithmic),
                                                      fl::Binning(shapeBins, 1, shapeMax, fl::Binning::Scale::logarithmic), true, true, true));
            }

            const std::vector<std::vector<double> > &ps = minTree->root()->getPatternSpectra2D(SizeAttribute::name+ShapeAttribute::name)->getPatternSpectraMatrix();

            std::vector<std::vector<double> >pps;

            for (int j=1; j < (int)ps.size()-1; ++j){
                pps.push_back(std::vector<double>());
                for (int k=1; k < (int)ps[j].size()-1; ++k){
                    outPS << std::pow(ps[j][k], 0.2) << " ";
                    pps.back().push_back(std::pow(ps[j][k], 0.2));
                    //pps.back().push_back(ps[j][k]);
                }
            }

            //visualizePS(pps);
            minTree->deletePatternSpectra2DFromTree<SizeAttribute, ShapeAttribute>();
            minTree->deleteAttributeFromTree<ShapeAttribute>();
            minTree->deleteAttributeFromTree<SizeAttribute>();

            minTree->unsetImage();
            delete minTree;
        }

        outPS << std::endl;
}


#endif
