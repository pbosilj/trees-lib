#define ATTRIBUTE_PS_COMPILE 1

#define GAREA 10
#define GSHAPE 6
#define GNC 56
#define TREES 4

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>


#include <iomanip>

#include "structures/node.h"
#include "structures/inclusionnode.h"
#include "structures/partitioningnode.h"

#include "structures/attribute.h"

#include "structures/areaattribute.h"
#include "structures/rangeattribute.h"
#include "structures/momentsattribute.h"
#include "structures/noncompactnessattribute.h"
#include "structures/regiondynamicsattribute.h"
#include "structures/diagonalminimumattribute.h"
#include "structures/entropyattribute.h"
#include "structures/valuedeviationattribute.h"
#include "structures/boundingspherediameterapprox.h"
#include "structures/yextentattribute.h"
#include "structures/sparsityattribute.h"
#include "structures/meanattribute.h"

#include "structures/imagetree.h"

#include "algorithms/predicate.h"

#include "structures/patternspectra2d.h"

#include "misc/pixels.h"
#include "misc/ellipse.h"
#include "misc/distance.h"

#include "algorithms/maxtreenister.h"
#include "algorithms/msernister.h"
#include "algorithms/tosgeraud.h"
#include "algorithms/alphatreedualmax.h"
#include "algorithms/omegatreealphafilter.h"
#include "algorithms/objectdetection.h"
#include "algorithms/regionclassification.h"

#include "algorithms/maxtreeberger.h"

#include <utility>
#include <vector>
#include <string>

#include <functional>
#include <algorithm>

#include <iostream>
#include <cstdio>

#include <map>
#include <queue>

#include <fstream>

#include <cstdlib>
#include <ctime>

#include <cstring>

static const cv::Vec3b bcolors[] =
{
    cv::Vec3b(0,0,255),
    cv::Vec3b(0,128,255),
    cv::Vec3b(0,255,255),
    cv::Vec3b(0,255,0),
    cv::Vec3b(255,128,0),
    cv::Vec3b(255,255,0),
    cv::Vec3b(255,0,0),
    cv::Vec3b(255,0,255),
    //cv::Vec3b(255,255,255),
    cv::Vec3b(128,0,255),
    cv::Vec3b(128,255,0),
    cv::Vec3b(0,255,128)
};


/************** TEST IMPLEMENTATIONS *************/

void testNister(int delta, cv::Mat &image1, cv::Mat &image2, cv::Mat &display, std::ofstream &outF, std::string featPath, std::string ellipsePath);
void testGeraud(int delta, cv::Mat &image1, cv::Mat &image2, cv::Mat &display, std::ofstream &outF, std::string featPath, std::string ellipsePath);
void testAlpha(int delta, cv::Mat &image1, cv::Mat &image2, cv::Mat &display, std::ofstream &outF, std::string featPath, std::string ellipsePath);

void getMSRFromTree(fl::ImageTree *tree, fl::ImageTree *dual, fl::mserParams &params, cv::Mat &image, cv::Mat &display, std::ofstream &outF, std::string featPath, std::string ellipsePath);
void getMSR2FromTree(fl::ImageTree *tree, fl::ImageTree *dual, fl::mserParams &params, cv::Mat &image, cv::Mat &display);


void testAllRange(const cv::Mat &image, fl::ImageTree *tos, fl::ImageTree *alpha, fl::ImageTree *max, fl::ImageTree *min, int delta);


/************** DEBUG FUNCTIONS ******************/

bool isMaxRegion(const std::vector<cv::Point>& region, const cv::Mat &img);
void testMSERocv(int delta);
void compare(int delta);
int getMaxComponent(const cv::Mat &img, fl::pxCoord start, int minValue);
bool comparator(const cv::Point &a, const cv::Point &b);

/************** TEST IMAGES *********************/

cv::Mat &testImage1(void);
cv::Mat &testImage2(void);
cv::Mat &testImage3(void);
cv::Mat &testImage4(void);
cv::Mat &testImage5(void);
cv::Mat &testImage6(void);
cv::Mat &testImage7(void);
cv::Mat &testImage8(void);
cv::Mat &testImage9(void);
cv::Mat &testImage10(void);
cv::Mat &testImage11(void);
cv::Mat &testImage12(void);

void makeTOS(void);

void testRange(void){

    //cv::Mat &img = testImage9();
    cv::Mat img = cv::imread("/home/pbosilj/Programming/Trees/lenaGray.png", CV_LOAD_IMAGE_GRAYSCALE);

    //fl::Node *root = fl::maxTreeNister(img, std::greater<int>());
    /*
    std::cout << "image" << std::endl;
    for (int i=0; i < img.rows; ++i){
        for (int j=0; j < img.cols; ++j){
            std::cout << (int)img.at<uchar>(i, j) << "  " ;
        }
        std::cout << std::endl;
    }

*/
/*
    std::cout << "min tree:" << std::endl;

    fl::Node *rootMin =fl::maxTreeNister(img, std::less<int>());
    fl::ImageTree *minTree = new fl::ImageTree(rootMin,std::make_pair(img.rows, img.cols));
    minTree->printTree();

    std::cout << std::endl;


    std::cout << "max tree:" << std::endl;

    fl::Node *rootMax =fl::maxTreeBerger(img, std::greater<int>());
    fl::ImageTree *maxTree = new fl::ImageTree(rootMax,std::make_pair(img.rows, img.cols));
    maxTree->printTree();

    std::cout << std::endl;

    std::cout << "tos:" << std::endl;

    fl::Node *rootTos =fl::tosGeraud(img);
    fl::ImageTree *tos = new fl::ImageTree(rootTos,std::make_pair(img.rows, img.cols));
    tos->printTree();

    std::cout << std::endl;
*/

    fl::Node *rootAlpha =fl::maxTreeBerger(img, std::greater<int>());
    fl::ImageTree *alphaTree = new fl::ImageTree(rootAlpha,std::make_pair(img.rows, img.cols));
    //alphaTree->printTree();

    //alphaTree->filterTreeByLevelPredicate(fl::AlphaPredicateX<int>(10));

    //alphaTree->displayTree();

    /*
    std::cout << "omega tree:" << std::endl;

    fl::Node *rootOmega =fl::omegaTreeAlphaFilter(img);
    fl::ImageTree *omegaTree = new fl::ImageTree(rootOmega,std::make_pair(img.rows, img.cols));
    omegaTree->printTree();

    std::cout << std::endl;
*/

    fl::Node *root = rootAlpha;

    //fl::Node *ch1 = root->children[0];

    //fl::ImageTree *alphaTree = new fl::ImageTree(root,std::make_pair(img.rows, img.cols));
    alphaTree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());
    alphaTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());


    //alphaTree->filterTreeByTIAttributePredicate<fl::AreaAttribute>(fl::GreaterThanX<int>(15));
    //alphaTree->filterTreeByTIAttributePredicate<fl::NonCompactnessAttribute>(fl::LessThanX<int>(2));

    //alphaTree->displayTree();

    //return;

    //alphaTree->addAttributeToTree<fl::RangeAttribute>(new fl::RangeSettings());

    //fl::ImageTree *omegaTree = new fl::ImageTree(fl::omegaTreeAlphaFilter(img), std::make_pair(img.rows, img.cols));

    //alphaTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
    //alphaTree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());


    //alphaTree->setImage(img);
    //std::cout << "area: " << std::endl;
    //alphaTree->printTreeWithAttribute<fl::AreaAttribute>();

    //std::cout << "non-compactness: " << std::endl;
    //alphaTree->printTreeWithAttribute<fl::NonCompactnessAttribute>();

    //return ;
    //alphaTree->unsetImage();

//        std::vector <fl::Node *> mser;
//        std::vector <std::pair<double, int> > div;
//
//        fl::markMserInTree(*alphaTree, 10, mser, div, 0.1, 30, 0.45, 0.2);
//        int minMserArea = -1;
//        for (int i=0, szi = mser.size(); i < szi; ++i){
//            int curVal = ((fl::AreaAttribute *)mser[i]->getAttribute(fl::AreaAttribute::name))->value();
//            if (minMserArea == -1 || curVal < minMserArea)
//                minMserArea = curVal;
//        }
//
//    std::cout << "min area " << minMserArea << std::endl;

    //return;

    alphaTree->setImage(img);
    alphaTree->addPatternSpectra2DToTree<fl::AreaAttribute, fl::NonCompactnessAttribute>
                (new fl::PatternSpectra2DSettings(fl::Binning(10, 1, NODE_VAL, fl::Binning::logarithmic),
                                                 fl::Binning(10, 1, 60, fl::Binning::logarithmic), false, false));

    //std::cout << "trying to print PS" << std::endl;


    const std::vector<std::vector<double> > &ps =
        root->getPatternSpectra2D(fl::AreaAttribute::name+fl::NonCompactnessAttribute::name)->getPatternSpectraMatrix();

    std::cout << ps.size() << " " << ps[0].size() << std::endl;

    double max = 0;
    for (int i=0; i < 10; ++i){
        for (int j=0; j < 10; ++j){
            if (max < ps[i+1][j+1])
                max = ps[i+1][j+1];
            std::cout << ps[i+1][j+1] << " ";
        }
        std::cout << std::endl;
    }

//    std::cout << max << std::endl;
//
//
    cv::Mat image = cv::Mat::zeros(100, 100, CV_8U);
    for (int i=0; i < 10; ++i){
        for (int j=0; j < 10; ++j){
            for (int ii=0; ii < 10; ++ii){
                for (int jj=0; jj < 10; ++jj){
                    if ((int)(std::pow(ps[i+1][j+1], 0.2) * 255 / std::pow(max, 0.2)) > 255)
                        image.at<uchar>(i*10+ii,j*10+jj) = 255;
                    else
                        image.at<uchar>(i*10+ii,j*10+jj) = (int)(std::pow(ps[i+1][j+1], 0.2) * 255 / std::pow(max, 0.2));

                }
            }
        }
    }

    //fl::displayMser(img, fl::ugvar);

    //std::cout << fl::ugvar.size() << std::endl;

    cv::imwrite("/home/pbosilj/Programming/Trees/ps2.png", image);

    //std::cout << "to print ps" << std::endl;

    //std::cout << "blah" << std::endl;


/*
    std::cout << "  ";
    for (int j=0, szj=ps.back().size(); j < szj; ++j)
        std::cout << j << " ";
    std::cout << std::endl;
    for (int i=0, szi = ps.size(); i < szi; ++i){
        std::cout << i << " ";
        for (int j=0, szj = ps.back().size(); j < szj; ++j){
            std::cout << ps[i][j] << " ";
        }
        std::cout << std::endl;
    }

    const std::vector<std::vector<double> > &ps1 =
        ch1->getPatternSpectra2D(fl::AreaAttribute::name+fl::NonCompactnessAttribute::name)->getPatternSpectraMatrix();

    std::cout << "to print ps1" << std::endl;

    std::cout << "  ";
    for (int j=0, szj=ps1.back().size(); j < szj; ++j)
        std::cout << j << " ";
    std::cout << std::endl;
    for (int i=0, szi = ps1.size(); i < szi; ++i){
        std::cout << i << " ";
        for (int j=0, szj = ps1.back().size(); j < szj; ++j){
            std::cout << ps1[i][j] << " ";
        }
        std::cout << std::endl;
    }

*/
    alphaTree->deleteAttributeFromTree<fl::AreaAttribute>();
    alphaTree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();
    alphaTree->deletePatternSpectra2DFromTree<fl::AreaAttribute, fl::NonCompactnessAttribute>();

    delete alphaTree;

    return;

}


void outputMSERPointsAndPS(int delta, cv::Mat &image, std::ofstream &outPS, std::ofstream &outP, int salientNum);
void outputGlobalPS(cv::Mat &image, std::ofstream &outPS);

std::string removeExtension(const std::string &filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
}

void testMser(int argc, char **argv){
    cv::Mat inputImage;
    cv::Mat displayImage;

    inputImage = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    displayImage = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

    std::ofstream outF(argv[2]);

    int option;
    sscanf(argv[3], "%d", &option);

    fl::ImageTree *tree;
    fl::ImageTree *dual = NULL;
    fl::mserParams *params;

    int delta, minArea;
    double maxAreaPerc;

    //sscanf(argv[4], "%d", &delta);
    //sscanf(argv[5], "%lf", &maxAreaPerc);
    //sscanf(argv[6], "%d", &minArea);

    switch (option){
        case 0:
            dual = new fl::ImageTree(fl::maxTreeNister(inputImage, std::greater<int>()),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            tree = new fl::ImageTree(fl::maxTreeNister(inputImage, std::less<int>()),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            params = new fl::mserParams(7, 0.01, 30, 0.45, 0.25);
            //params = new fl::mserParams(delta, maxAreaPerc, minArea, 0.45, 0.25);
            break;
        case 1:
            tree = new fl::ImageTree(fl::tosGeraud(inputImage),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            params = new fl::mserParams(5, 0.01, 30, 0.4, 0.25);
            //params = new fl::mserParams(delta, maxAreaPerc, minArea, 0.4, 0.25);
            break;
        case 2:
            tree = new fl::ImageTree(fl::alphaTreeDualMax(inputImage),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            params = new fl::mserParams(8, 0.01, 30, 0.5, 0.2);
            //params = new fl::mserParams(delta, maxAreaPerc, minArea, 0.6, 0.2);
            break;
        case 3:
        default:
            tree = new fl::ImageTree(fl::omegaTreeAlphaFilter(inputImage),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            params = new fl::mserParams(85, 0.01, 70, 0.3, 0.4);
            //params = new fl::mserParams(delta, maxAreaPerc, minArea, 0.4, 0.3);
            break;
    }

    getMSRFromTree(tree, dual, *params, inputImage, displayImage, outF, std::string(argv[1])+"FT.ppm", std::string(argv[1])+"EL.ppm");
    //getMSRFromTree(tree, dual, *params, inputImage, displayImage, outF, "", "");

    delete params;
    delete tree;
    if (dual)
        delete dual;
    outF.close();
}

void testMSER2(cv::Mat &inputImage, cv::Mat &displayImage){
//void testMser(int argc, char **argv){
//    cv::Mat inputImage;
//    cv::Mat displayImage;
//
//    inputImage = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
//    displayImage = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
//
    std::ofstream outF("");
//
//    int option;
//    sscanf(argv[3], "%d", &option);

    int option = 0;

    fl::ImageTree *tree;
    fl::ImageTree *dual = NULL;
    fl::mserParams *params;

    int delta, minArea;
    double maxAreaPerc;

    //sscanf(argv[4], "%d", &delta);
    //sscanf(argv[5], "%lf", &maxAreaPerc);
    //sscanf(argv[6], "%d", &minArea);

    switch (option){
        case 0:
            dual = new fl::ImageTree(fl::maxTreeNister(inputImage, std::greater<int>()),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            tree = new fl::ImageTree(fl::maxTreeNister(inputImage, std::less<int>()),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            params = new fl::mserParams(15, 0.01, 30, 0.5, 0.3);
            //params = new fl::mserParams(delta, maxAreaPerc, minArea, 0.45, 0.25);
            break;
        case 1:
            tree = new fl::ImageTree(fl::tosGeraud(inputImage),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            params = new fl::mserParams(5, 0.01, 30, 0.4, 0.25);
            //params = new fl::mserParams(delta, maxAreaPerc, minArea, 0.4, 0.25);
            break;
        case 2:
            tree = new fl::ImageTree(fl::alphaTreeDualMax(inputImage),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            params = new fl::mserParams(8, 0.01, 30, 0.5, 0.2);
            //params = new fl::mserParams(delta, maxAreaPerc, minArea, 0.6, 0.2);
            break;
        case 3:
        default:
            tree = new fl::ImageTree(fl::omegaTreeAlphaFilter(inputImage),
                                        std::make_pair(inputImage.rows, inputImage.cols));
            params = new fl::mserParams(85, 0.01, 70, 0.3, 0.4);
            //params = new fl::mserParams(delta, maxAreaPerc, minArea, 0.4, 0.3);
            break;
    }

    getMSR2FromTree(tree, dual, *params, inputImage, displayImage);
    //getMSRFromTree(tree, dual, *params, inputImage, displayImage, outF, "", "");

    delete params;
    delete tree;
    if (dual)
        delete dual;
    outF.close();
}


//void simplifyImageWithTreeByLevel(int argc, char **argv){
void simplifyImageWithTreeByLevel(std::string input, std::string output, double strength){

    std::vector <cv::Mat> outputImages(4);
    cv::Mat &outputImage = outputImages[0];
    std::vector <cv::Mat> inputImages(4);
    //std::cout << argv[1] << std::endl;

    if (0){
    for(int i=1; i <= 3; ++i){
        std::stringstream imgName;
        imgName << removeExtension(input) << "PCA" << i << ".png";
        inputImages[i-1] = cv::imread(imgName.str().c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        //std::cerr << imgName.str() << " " << inputImages[i-1].rows << " " << inputImages[i-1].cols << std::endl;
    }
    cv::Mat &inputImage = inputImages[0];
    }

    cv::Mat inputImage = cv::imread(input.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    //inputImage = cv::imread(input.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

    //std::cout << "is something not input?" << std::endl;
    //std::cout << inputImage.cols << " " << inputImage.rows << std::endl;
    //outputImage = cv::Mat::zeros(inputImage.rows, inputImage.cols, CV_16U);
    for (int i=0; i < 3; ++i)
        outputImages[i] = cv::Mat::zeros(inputImage.rows, inputImage.cols, CV_8U);

    //std::cout << outputImage.cols << " " << outputImage.rows << std::endl;

    int option;
    //sscanf(argv[3], "%d", &option);
    option = 3;

    //int strength;
    //sscanf(argv[4], "%d", &strength);


    fl::Node *root;
    fl::ImageTree *tree;

    switch (option){
        case -1:
            root = fl::maxTreeNister(inputImage, std::greater<int>()); // max-tree
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            std::cout << tree->countNodes() << std::endl;
            std::cout << tree->minMaxLevel().first << " " << tree->minMaxLevel().second << std::endl;
            tree->displayTree();

            //tree->printTree();
            tree->filterTreeByLevelPredicate(fl::LessThanX<double>(tree->minMaxLevel().second-strength));
            std::cout << tree->countNodes() << std::endl;
            tree->displayTree();
            //tree->printTree();
            break;
        case 0:
            root = fl::maxTreeNister(inputImage, std::less<int>()); // min-tree
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            std::cout << tree->countNodes() << std::endl;
            std::cout << tree->minMaxLevel().first << " " << tree->minMaxLevel().second << std::endl;
            tree->displayTree();

            tree->filterTreeByLevelPredicate(fl::GreaterThanX<double>(tree->minMaxLevel().first+strength));
            std::cout << tree->countNodes() << std::endl;
            tree->displayTree();
            break;
        case 1:
            root = fl::tosGeraud(inputImage);
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            tree->addAttributeToTree<fl::RegionDynamicsAttribute>(new fl::RegionDynamicsSettings());

            std::cout << tree->countNodes() << std::endl;
            std::cout << tree->minMaxAttribute<fl::RegionDynamicsAttribute>().first << " " << tree->minMaxAttribute<fl::RegionDynamicsAttribute>().second << std::endl;
            tree->displayTree();

            tree->filterTreeByAttributePredicate<fl::RegionDynamicsAttribute>(fl::GreaterThanX<double>(strength));
            tree->deleteAttributeFromTree<fl::RegionDynamicsAttribute>();
            std::cout << tree->countNodes() << std::endl;
            tree->displayTree();
            break;
        case 2:
            std::cerr << "constructing tree" << std::endl;

            root = fl::alphaTreeDualMax(inputImage);
            //root = fl::alphaTreeDualMax(inputImages, fl::distanceL1Integral<const uchar>());
//            std::cerr << root->level() << " " << root->grayLevel() << std::endl;
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
//            //std::cout << tree->countNodes() << std::endl;
//            tree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
//            tree->attributeProfileIncreasing<fl::AreaAttribute>(fl::GreaterEqualThanX<int>(strength));
//            tree->deleteAttributeFromTree<fl::AreaAttribute>();

            //tree->addAttributeToTree<fl::BoundingSphereDiameterApprox>(new fl::BoundingSphereDiameterApproxSettings(4,  fl::distanceL1NonIntegral<double>()));
            //tree->deleteAttributeFromTree<fl::BoundingSphereDiameterApprox>();

//            std::cerr << root->level() << " " << root->grayLevel() << std::endl;
            //std::cout << tree->countNodes() << std::endl;
            //tree->displayTree();

            tree->addAttributeToTree<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::hu, 1));
            tree->attributeProfile<fl::MomentsAttribute>(fl::GreaterEqualThanX<double>(strength));
            tree->deleteAttributeFromTree<fl::MomentsAttribute>();

//            tree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());
//            tree->attributeProfile<fl::NonCompactnessAttribute>(fl::GreaterEqualThanX<double>(strength));
//            tree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();


            break;
        case 3:
        default:
            std::cout << "constructing tree" << std::endl;
            root = fl::omegaTreeAlphaFilter(inputImage);
            //root = fl::omegaTreeAlphaFilter(inputImages, fl::distanceSAM<const uchar>(), fl::distanceSAM<double>());
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            std::cout << "construction finished" << std::endl;
            //std::cout << tree->countNodes() << std::endl;
//            tree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
//            tree->attributeProfileIncreasing<fl::AreaAttribute>(fl::GreaterEqualThanX<int>(strength));
//            tree->deleteAttributeFromTree<fl::AreaAttribute>();
            //std::cout << tree->countNodes() << std::endl;
            //tree->displayTree();
//            tree->addAttributeToTree<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::hu, 1));
//            tree->attributeProfile<fl::MomentsAttribute>(fl::GreaterEqualThanX<double>(strength));
//            tree->deleteAttributeFromTree<fl::MomentsAttribute>();


            tree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());
            tree->attributeProfile<fl::NonCompactnessAttribute>(fl::GreaterEqualThanX<double>(strength));
            tree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();


            break;
    }

    root->colorMe(outputImage);
//    std::cout << "coloring" << std::endl;
    //root->colorThem(outputImages);
//    std::cout << "colored " << std::endl;
    delete tree;
    cv::imwrite(output.c_str(), outputImage);
//    for (int i=0; i < 4; ++i){
//        std::stringstream oName;
//        oName << removeExtension(output) << i+1 << ".png";
//        cv::imwrite(oName.str().c_str(), outputImages[i]);
//    }
}

void simplifyImageWithTreeByArea(int argc, char **argv){

    cv::Mat inputImage;
    cv::Mat outputImage;

    inputImage = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    std::cout << inputImage.cols << " " << inputImage.rows << std::endl;
    outputImage = cv::Mat::zeros(inputImage.cols, inputImage.rows, CV_8U);

    std::cout << outputImage.cols << " " << outputImage.rows << std::endl;

    //std::ofstream outF(argv[2]);

    int option;
    sscanf(argv[3], "%d", &option);

    int area;
    sscanf(argv[4], "%d", &area);

    fl::Node *root;
    fl::ImageTree *tree;

    switch (option){
        case -1:
            root = fl::maxTreeNister(inputImage, std::greater<int>()); // max-tree
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            break;
        case 0:
            root = fl::maxTreeNister(inputImage, std::less<int>()); // min-tree
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            break;
        case 1:
            root = fl::tosGeraud(inputImage);
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            break;
        case 2:
            root = fl::alphaTreeDualMax(inputImage);
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            break;
        case 3:
        default:
            root = fl::omegaTreeAlphaFilter(inputImage);
            tree = new fl::ImageTree(root,
                                        std::make_pair(inputImage.rows, inputImage.cols));
            break;
    }

    std::cout << root->level() << std::endl;
    tree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
    tree->filterTreeByAttributePredicate<fl::AreaAttribute>(fl::GreaterThanX<double>(area));
    tree->deleteAttributeFromTree<fl::AreaAttribute>();
    tree->displayTree();

    root->colorMe(outputImage);
    cv::imwrite(argv[2], outputImage);
}

void forYanwei(int delta, cv::Mat &image, bool minTree, int salientNum = -1);

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

void getRanks(const std::vector<char> &response, int NG, int K, std::vector <double> &ranks){
    ranks.clear();
    for (int i=0; i < K && (int)ranks.size() < NG; ++i){
        if (!response[i])
            continue;
        ranks.push_back(i+1);
    }
    for (int i=ranks.size(); i < NG; ++i)
        ranks.push_back(1.25 * K);
}

double avgRank(const std::vector<char> &response, int NG, int K){
    std::vector <double> ranks;
    getRanks(response, NG, K, ranks);
    double result = 0.0;
    for (int i=0; i < NG; ++i){
        result += ranks[i];
    }
    result /= NG;
    return result;
}

double mrr(const std::vector<char> &response, int NG, int K){
    return avgRank(response, NG, K) - 0.5*(1+NG);
}

double nmrr(const std::vector<char> &response, int NG, int K){
    std::cout << mrr(response, NG, K) / (1.25*K - 0.5*(1+NG)) << std::endl;
    return mrr(response, NG, K) / (1.25*K - 0.5*(1+NG));
}

double precisionM(const std::vector <char>&response, int m){
    int total = 0;
    for (int i=0, szi = response.size(); i < szi && i < (m-1); ++i)
        total += response[i];
    return (double)total / std::min(m,(int)response.size());
}

double averagePrecision(const std::vector<char> &response){
    int totalRelevant = 0;
    double sum = 0.0;
    for (int i=0, szi = response.size(); i < szi; ++i){
        if (response[i]){
            sum += precisionM(response, i+1);
            ++totalRelevant;
        }
    }
    return sum / (double)totalRelevant;
}

void globalPSForAll(int argc, char** argv){
    std::ifstream listFile(argv[1]);

    if (!listFile.is_open())
        return;

    std::vector <std::vector<double> > descriptors;

    std::string line;
    int i = 0;
    int zeroPerc=0;
    while (std::getline(listFile, line)){
        ++i;

        std::cout << "processing: " << line << std::endl;

        if (!fileExists((removeExtension(line)+".globps"))){

            std::ofstream descOut((removeExtension(line)+".globps").c_str());
            cv::Mat img1 = cv::imread(line, CV_LOAD_IMAGE_GRAYSCALE);
            cv::Mat img;
            cv::equalizeHist(img1, img);
//            std::ifstream imgIn(line.c_str());
//
//
//            int rows, cols;
//            imgIn >> rows >> cols;
//
//            cv::Mat img(rows,cols,CV_32S);
//
//            for (int _i=0; _i < rows; ++_i)
//                for (int _j=0; _j < cols; ++_j)
//                    imgIn >> img.at<int32_t>(_i, _j);




//            // global descriptors :D
//            outputGlobalPS(img, descOut);
//            continue;


//            cv::Mat img = cv::imread(line, CV_LOAD_IMAGE_COLOR);
            int rows = img.rows;
            int cols = img.cols;

//            outputGlobalPS(channels[0], descOut);
//            outputGlobalPS(channels[1], descOut);
//            outputGlobalPS(channels[2], descOut);

            int pyramidFactor = std::max(cols, rows);
            //int pyramidFactor = 2;

            descOut << GAREA*GSHAPE*TREES << std::endl;

            int descNum = 0;
            for (int dim = 80, offset = 16; dim < std::min(256,std::max(cols, rows))+2; dim *= pyramidFactor){

                int szi = (cols - dim) / offset + 1;
                if (((cols - dim) % offset) > 2) ++szi;
                int szj = (rows - dim) / offset +1;
                if (((rows - dim) % offset) > 2) ++szj;

                descNum += szi*szj;
            }

            descOut << descNum << std::endl;

            for (int dim = 80, offset = 16; dim < std::min(256,std::max(cols, rows))+2; dim *= pyramidFactor){
                int sum = 0;
                for (int _i=0; !_i || (_i - offset + dim < cols-2); _i += offset){
                    int dimi = std::min(cols-_i, dim);
                    if (dimi <= 2)
                        break;
                    for (int _j=0; !_j || (_j - offset + dim < rows-2); _j += offset){
                        int dimj = std::min(rows-_j, dim);

                        //std::cout << "out " <<  dimi << " " << dimj << " " << _i << " " << _j << std::endl;
                        if (dimj <= 2)
                            break;

                        cv::Mat patch = img(cv::Rect(_i, _j, dimi, dimj));
//
//                        cv::Mat channels[3];
//                        cv::split(patch, channels);
//
//                        outputGlobalPS(channels[0], descOut);
//                        outputGlobalPS(channels[1], descOut);
//                        outputGlobalPS(channels[2], descOut);
//std::cerr << "before global " << std::endl;
                        outputGlobalPS(patch, descOut);
//std::cerr << "after global " << std::endl;
                        ++sum;

                        if ((_j + dim) >= rows)
                            break;
                    }
                    if ((_i + dim) >= cols)
                        break;
                }
//                if (dim == 32){
//                    std::ofstream toErase((removeExtension(line)+".32er").c_str());
//                    toErase << sum << std::endl;
//                    toErase.close();
//                }
            }


            descOut.close();

        }



        std::ifstream readDesc((removeExtension(line)+".globps").c_str());
        descriptors.push_back(std::vector<double>(GAREA*GSHAPE*TREES,0));

        for (int j=0; j < GAREA*GSHAPE*TREES; ++j){
            readDesc >> descriptors.back()[j];
            zeroPerc += (int)(descriptors.back()[j]==0);
        }
    }

    //return;


    std::cout << "empty: " << ((double)(zeroPerc*100))/(GAREA*GSHAPE*TREES*i) << "%" << std::endl;

    double anmrr = 0.0;
    double mAP = 0.0;
    //double anmrr2 = 0.0;
    std::vector<std::pair<double, int> > dists(i,std::make_pair(0,-1.0));
    //std::vector<std::pair<double, int> > dists2(i,std::make_pair(0,-1.0));
    std::vector<char> responses;
    //std::vector<char> responses2;
    for (int j=0; j < i; ++j){
        dists.clear(); dists.resize(i,std::make_pair(0,-1.0));
        for (int k=0; k < i; ++k){
            //dists[k].second = k;
            dists[k].second = k;
            for (int index=0; index < GAREA*GSHAPE*TREES; ++index){
                // L1:
                dists[k].first += std::abs(descriptors[j][index] - descriptors[k][index]);
                // L2:
                // dists[k].first += (descriptors[j][index] - descriptors[k][index])*(descriptors[j][index] - descriptors[k][index]);
                // chi:
//                if (descriptors[j][index] && descriptors[k][index])
//                    dists[k].first += (descriptors[j][index] - descriptors[k][index])*(descriptors[j][index] - descriptors[k][index]) /
//                                            (descriptors[j][index] + descriptors[k][index]);
            }
        }
        std::sort(dists.begin(), dists.end());
        //std::sort(dists2.begin(), dists2.end());

        responses.clear(); responses.resize(i);
        //responses2.clear(); responses2.resize(i);

        for (int k=0; k < i; ++k){
            //std::cout << dists[k].second << " ";
            //std::cout << ((dists[k].second / 100)==(j/100)) << " " ;
            responses[k] = (char)((dists[k].second / 100)==(j/100));
            //std::cout << "(" << dists[k].first << " " << dists[k].second << " " << ((dists[k].second / 100)==(j/100)) << ") ";
        }
        //std::cout << std::endl;

//        for (int k=0; k < i; ++k){
//            //std::cout << dists[k].second << " ";
//            std::cout << ((dists2[k].second / 100)==(j/100)) << " " ;
//            responses2[k] = (char)((dists2[k].second / 100)==(j/100));
//            //std::cout << "(" << dists[k].first << " " << dists[k].second << " " << ((dists[k].second / 100)==(j/100)) << ") ";
//        }
//        std::cout << std::endl;
        //return;

        double mynmrr = nmrr(responses, 100, 200);
        double ap = averagePrecision(responses);
        std::cout << ap << std::endl;
        //double mynmrr2 = nmrr(responses2, 100, 200);
        anmrr += (mynmrr / i);
        mAP += (ap / i);
        //anmrr2 += (mynmrr2 / i);
    }

    std::cout << "ANMRR with K = 200 "  << anmrr << std::endl; // " " << anmrr2 << std::endl;
    std::cout << "mAP " << mAP << std::endl;
    return;
}


void forRomain(int argc, char** argv );

void testObjectDetection(int argc, char **argv);
void testUltimateOpening(int argc, char **argv);
//void savedObjectAnalysis(int argc, char **argv);

void runPipeline(int argc, char **argv);
void runObjectAnalysis(int argc, char **argv);
void runManualClassification(int argc, char **argv);
void runObjectDetectionAndSegmentation(int argc, char **argv);
void manualClassification(const fl::ImageTree *tree, std::vector <fl::Node *> selection, std::vector <fl::Node *> *npc, const cv::Mat &rgb);
void outputClassification(const fl::ImageTree *tree, const std::vector <fl::Node *> *npc, const std::string &classPath = "");
void outputSegmentation(const fl::ImageTree *tree, const std::vector <fl::Node *> &selection, const std::string &segPath = "");
void objectDetection(fl::ImageTree *tree, std::vector <fl::Node *> &selection, std::vector <fl::Node *> *source = NULL);
void loadNodes(const fl::ImageTree *tree, std::vector <fl::Node *> &selection, const std::string &detFile);
void saveNodes(const fl::ImageTree *tree, const std::vector <fl::Node *> &selection, const std::string &detFile);
void saveClassifiedNodes(const fl::ImageTree *tree, const std::vector <fl::Node *> *npc, std::string classFile);
void loadClassifiedNodes(const fl::ImageTree *tree, std::vector <fl::Node *> *npc, std::string classFile);
void objectAnalysis(fl::ImageTree *tree, const std::vector <fl::Node *> *npc, std::string prefix = "");
void branchAnalysis(fl::ImageTree *tree, const std::vector <fl::Node *>nodes);
bool doesFileExist(const std::string &fileName);

int main(int argc, char** argv ){

    runPipeline(argc, argv);
    //runObjectDetectionAndSegmentation(argc, argv);

    //testUltimateOpening(argc, argv);
    //return 0;
    //savedObjectAnalysis(argc, argv);
    return 0;
    testObjectDetection(argc, argv);
    return 0;

    //testMser(argc, argv);

    //return 0;

    cv::VideoCapture cap("IMG_6345.m4v"); // open the default camera
    //Video Capture cap(path_to_video); // open the video file
    if(!cap.isOpened()){  // check if we succeeded
        std::cout << "we did not succeed" << std::endl;
        return -1;
    }

    cv::namedWindow("Video",1);
    for(;;)
    {
        cv::Mat frame, grayframe;
        cap >> frame; // get a new frame from camera
        //cv::imshow("OneFrame", frame);
        //cv::waitKey(0);
        //cv::Rect roi(0,0, 400,400);
        //cv::Mat smallFrame = frame(roi);
        cv::cvtColor(frame, grayframe, cv::COLOR_BGR2GRAY);
        //cv::Mat outFrame;
        //smallFrame.copyTo(outFrame);
        cv::Mat r1, r2, r;

        cv::morphologyEx(grayframe, r1, cv::MORPH_BLACKHAT, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1,61), cv::Point(0,30)));
        cv::morphologyEx(grayframe, r2, cv::MORPH_BLACKHAT, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(61,1), cv::Point(30,0)));

        //cv::addWeighted(r1, 0.5, r2, 0.5, 0, r);
        cv::bitwise_and(r1, r2, r);

        cv::morphologyEx(r, r, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1,3), cv::Point(0,1)));
        cv::morphologyEx(r, r, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5), cv::Point(2,2)));
        cv::morphologyEx(r, r, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,3), cv::Point(2,1)));
        cv::morphologyEx(r, r, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5), cv::Point(2,2)));
        //cv::morphologyEx(r, r, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,1), cv::Point(3,0)));


        //cv::morphologyEx(grayframe, grayframe, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9,1), cv::Point(4,0)));


//        cv::dilate(grayframe, grayframe, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1,7), cv::Point(0,3)));
//        cv::erode(grayframe, grayframe, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1,7), cv::Point(0,3)));
//        cv::dilate(grayframe, grayframe, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,1), cv::Point(3,0)));
//        cv::erode(grayframe, grayframe, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,1), cv::Point(3,0)));


        testMSER2(r,frame);
        cv::imshow("Video", frame);
        //cv::waitKey(0);

        if(cv::waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;


//        globalPSForAll(argc, argv);
//        return 0;

        //globalPSForAll(argc, argv);
        //return 0;

         {
//        cv::Mat test = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH);
//        if (test.type() == CV_8U){
//            std::cerr << "char?" << std::endl;
//            std::cerr << (int)(*std::max_element(test.begin<uchar>(), test.end<uchar>())) << std::endl;
//        }
//        else{
//            std::cerr << (test.type() == CV_32S) << std::endl;
//            std::cerr << (test.type() == CV_16U) << std::endl;
//            std::cerr << "int!" << std::endl;
//            std::cerr << (unsigned short)(*std::max_element(test.begin<unsigned short>(), test.end<unsigned short>())) << std::endl;
//        }
//        return 0;

        //int thresh[14]={770, 1538, 2307, 3076, 3846, 4615, 5384, 6153, 6923, 7692, 8461, 9230, 10000, 10769};
        double M[16] = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};
        double thresh[4] = {0.2, 0.3, 0.4, 0.5};

        for (int k=12; k < 16; ++k){
            for (int j=0; j < 4; ++j){
                double i=thresh[j]*M[k]*2*M_PI;
                std::cout << "(k, j)*2*PI = i: (" << k << ", " << j << ") = " << i << std::endl;

//                try{
//                char outPath[50];
//                sprintf(outPath, "alphaNCAvg%3.1lfx%3.1lfPCA.png", M[k], thresh[j]);
//                simplifyImageWithTreeByLevel(std::string(argv[1]), std::string(outPath), i);
//                }
//                catch(std::string msg){
//                    std::cout << msg << std::endl;
//                }

                for (int pca=1; pca <= 4; ++pca){
                    char outPath[50];
                    //sprintf(outPath, "pca%1dMin%3.1lf.png", pca, thresh[j]);
                    sprintf(outPath, "omegaNCLevel%3.1lfx%3.1lfPCA%1d.png", M[k], thresh[j], pca);
                    std::stringstream imgName;
                    imgName << removeExtension(std::string(argv[1])) << pca << ".png";
                    simplifyImageWithTreeByLevel(imgName.str(), std::string(outPath), i);
                }
            }
        }
        return 0;
        }
//        forRomain(argc, argv);
//        return 0;

//        if (0) {
//
//

//        }

        testMser(argc, argv);

        return 0;

        {
        std::string dbFile(argv[1]);
        cv::Mat img = cv::imread(dbFile, CV_LOAD_IMAGE_GRAYSCALE);

        int minMax;
        std::sscanf(argv[argc-1], "%d", &minMax);

        if (minMax == 1){
            forYanwei(10, img, true);
        }
        else{
            forYanwei(10, img, false);
        }

        return 0;
        }


        int feats;
        std::sscanf(argv[argc-1], "%d", &feats);

        //std::ifstream dataFile(argv[1]);
        //while (true) {
            //std::string dbFile;
            //dataFile >> dbFile;
                //if( dataFile.eof() ) break;

        for (int i=1; i < argc-1; ++i){
        //for (int i=1; i < argc; ++i){
            std::string dbFile(argv[i]);

            //std::cout << "Getting feature points for the image: " << dbFile << std::endl;

            std::ofstream outPS((removeExtension(dbFile)+".testsif").c_str());
            //std::ofstream outFeat((removeExtension(dbFile)+".featsize").c_str());
            //std::ofstream outFeat((removeExtension(dbFile)+".feat").c_str());
            cv::Mat img = cv::imread(dbFile, CV_LOAD_IMAGE_GRAYSCALE);
            //cv::Mat img = cv::imread(dbFile, CV_LOAD_IMAGE_COLOR);

            //std::vector<cv::Mat> channels(3);
            // split img:
//            cv::split(img, channels);

//            outPS << 600 << std::endl;

//            outputMSERPointsAndPS(10, channels[0], outPS, outPS, feats);
//            outputMSERPointsAndPS(10, channels[1], outPS, outPS, feats);
            outputMSERPointsAndPS(10, img, outPS, outPS, feats);
            //outputGlobalPS(img, outPS);
            img.release();

        }

        return 0;


       //cv::Mat img = cv::imread("/home/pbosilj/Programming/Trees/lena.tif", CV_LOAD_IMAGE_GRAYSCALE);
        //cv::Mat &img = testImage6();

       std::ofstream outPS("patternspectra.txt");
       std::ofstream outFeat("feat.txt");
       //outputMSERPointsAndPS(10, img, outPS, outFeat);

       return 0;

        testRange();


        //cv::Mat &img = testImage2();
      cv::Mat img = cv::imread("/home/pbosilj/Programming/Trees/lenabig.bmp", CV_LOAD_IMAGE_GRAYSCALE);
      //cv::Mat img = imga(cv::Rect(15, 130, 15, 15));

        fl::ImageTree *alpha = new fl::ImageTree(fl::alphaTreeDualMax(img), std::make_pair(img.rows, img.cols));
        fl::ImageTree *max = new fl::ImageTree(fl::tosGeraud(img), std::make_pair(img.rows, img.cols));


        max->setImage(img);
        //max->printTree();
        max->displayTree();
        max->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
        max->filterTreeByAttributePredicate<fl::AreaAttribute>(fl::GreaterThanX<double>(250));
        //max->printTreeWithAttribute<fl::RangeAttribute>();
        max->displayTree();
        max->unsetImage();
        //testRange();

        std::cout << "all deletes after this point is stuff I forgot to kill manually. shame on me" << std::endl;

        return 0;

      //clock_t begin = std::clock();

      //scv::Mat &img = testImage10();
      //cv::Mat img = cv::imread("/home/pbosilj/Programming/Trees/lenabig.bmp", CV_LOAD_IMAGE_GRAYSCALE);
      //cv::Mat img = imga(cv::Rect(15, 130, 15, 15));

      fl::Node *root = fl::alphaTreeDualMax(img);
      //root->printElements();



      return 0;

      fl::ImageTree *tree = new fl::ImageTree(root, std::make_pair(img.rows, img.cols));
      tree->setImage(img);
      std::cout << "blargh" << std::endl;
      tree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings);
      //tree->addAttributeToTree<fl::RangeAttribute>(new fl::RangeSettings);
      std::cout << (double)((fl::AreaAttribute *)root->getAttribute(fl::AreaAttribute::name))->value() << std::endl;
      //std::cout << ((fl::RangeAttribute *)root->getAttribute(fl::RangeAttribute::name))->value() << std::endl;
      delete tree;

      //std::cout << root->nodeCount() << std::endl;

      //clock_t end = std::clock();
      //double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

      //std::cout << elapsed_secs/50 << std::endl;


/**
      std::cout << "and old: " << std::endl;

      fl::Node *root2 = fl::tosGeraudOld(img);
      root2->printElements();
      std::cout << root2->nodeCount() << std::endl;
**/
/*
      fl::Node* root = fl::maxTreeBerger(img);
      fl::Node* root2 = fl::maxTreeNister(img);
      std::cout << root->nodeCount() << std::endl;
      std::cout << root2->nodeCount() << std::endl;
*/
      return 0;

      testRange();

      return 0;

//    unsigned int seed = std::time(NULL);
//    //unsigned int seed = 1393593097;
//    //std::cout << "curret seed: " << seed << std::endl;
//
//    std::srand(seed);
//
//    cv::Mat image = cv::imread("./img2.ppm", CV_LOAD_IMAGE_GRAYSCALE);
//
//    fl::ImageTree *tos = new fl::ImageTree(fl::tosGeraud(image), std::make_pair(image.rows, image.cols));
//
//    fl::ImageTree *alpha = new fl::ImageTree(fl::alphaTreeDualMax(image), std::make_pair(image.rows, image.cols));
//
//    fl::ImageTree *max = new fl::ImageTree(fl::maxTreeNister(image, std::greater<int>()), std::make_pair(image.rows, image.cols));
//    fl::ImageTree *min = new fl::ImageTree(fl::maxTreeNister(image, std::less<int>()), std::make_pair(image.rows, image.cols));
//
//    testAllRange(image, tos, alpha, max, min, 1);
//    testAllRange(image, tos, alpha, max, min, 2);
//    testAllRange(image, tos, alpha, max, min, 3);
//    testAllRange(image, tos, alpha, max, min, 5);
//    testAllRange(image, tos, alpha, max, min, 7);
//    testAllRange(image, tos, alpha, max, min, 10);
//    testAllRange(image, tos, alpha, max, min, 12);
//    testAllRange(image, tos, alpha, max, min, 15);
//    testAllRange(image, tos, alpha, max, min, 20);
//    testAllRange(image, tos, alpha, max, min, 25);
//
//    return 0;

    //getMSRFromTree(tree, dual, int delta, cv::Mat &image, std::ofstream &outF, std::string featPath, std::string ellipsePath);

    cv::Mat image1, image2, display;

        //std::cout << "area: " << getComponent(image, fl::pxCoord(583, 782), 65) << std::endl;


    std::ofstream outF(argv[2]);

    int option;
    sscanf(argv[3], "%d", &option);

    if (option == 0){
        if (argc == 4){
            image1 = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
            display = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
            //testNister(10, image1, image1, display, outF, std::string(argv[1])+"FeatMymser.ppm", std::string(argv[1])+"EllipMymser.ppm");
            testNister(10, image1, image1, display, outF, "", "");
        }
        else if (argc >= 5 || true){
            image1 = cv::imread(std::string(argv[1])+"a.png", CV_LOAD_IMAGE_GRAYSCALE);
            image2 = cv::imread(std::string(argv[1])+"b.png", CV_LOAD_IMAGE_GRAYSCALE);
            display = cv::imread(std::string(argv[1])+".ppm", CV_LOAD_IMAGE_COLOR);
            testNister(10, image1, image2, display, outF, "", "");
        }
    }
    else if (option == 1){
        if (argc == 4){
            image1 = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
            display = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
            //testGeraud(5, image1, image1, display, outF, std::string(argv[1])+"FeatTreeos.ppm", std::string(argv[1])+"EllipTreeos.ppm");
            testGeraud(5, image1, image1, display, outF, "", "");
        }
        else if (argc >= 5 || true){
            image1 = cv::imread(std::string(argv[1])+"a.png", CV_LOAD_IMAGE_GRAYSCALE);
            image2 = cv::imread(std::string(argv[1])+"b.png", CV_LOAD_IMAGE_GRAYSCALE);
            display = cv::imread(std::string(argv[1])+".ppm", CV_LOAD_IMAGE_COLOR);
            testGeraud(5, image1, image2, display, outF, "", "");
        }

    }

    else{ // if (option == 2){
        if (argc == 4){
            image1 = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
            display = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
            //testAlpha(2, image1, image1, display, outF, std::string(argv[1])+"FeatAlphat.ppm", std::string(argv[1])+"EllipAlphat.ppm");
            testAlpha(2, image1, image1, display, outF, "", "");
        }
        else if (argc >= 5 || true){
            image1 = cv::imread(std::string(argv[1])+"a.png", CV_LOAD_IMAGE_GRAYSCALE);
            image2 = cv::imread(std::string(argv[1])+"b.png", CV_LOAD_IMAGE_GRAYSCALE);
            display = cv::imread(std::string(argv[1])+".ppm", CV_LOAD_IMAGE_COLOR);
            testAlpha(2, image1, image2, display, outF, "", "");
        }
    }

    outF.close();
    return 0;
}

void testUltimateOpening(int argc, char **argv){
    //cv::Mat &image = testImage12();
    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH);
    fl::Node *root;
    fl::ImageTree *tree = new fl::ImageTree(root = fl::maxTreeBerger(image, std::greater<int>()),
                                    std::make_pair(image.rows, image.cols)); // min-tree creation


    double min, max;
    cv::minMaxLoc(image, &min, &max);
    std::cout << "Image: min - " << min << " max - " << max << std::endl;
    //tree->printTree();



    tree->addAttributeToTree<fl::YExtentAttribute>(new fl::YExtentSettings());
    //std::cout << "Uo is not giving it" << std::endl;
    cv::Mat residual, scale;
    tree->ultimateOpening<fl::YExtentAttribute>(residual, scale);

    //std::cout << "Uo is giving it" << std::endl;

    //double min, max;
    cv::minMaxLoc(residual, &min, &max);
    std::cout << "Residual: min - " << min << " max - " << max << std::endl;
    residual.convertTo(residual, CV_16UC1, 65535.0/max);
    cv::minMaxLoc(scale, &min, &max);
    std::cout << "Scale: min - " << min << " max - " << max << std::endl;
    scale.convertTo(scale, CV_16UC1, 65535.0/max);

    if (argc > 2)
        cv::imwrite(argv[2], residual);
    if (argc > 3)
        cv::imwrite(argv[3], scale);
//
    //std::cout << "Image = " << std::endl << image << std::endl;
    //std::cout << "Residual = " << std::endl << residual << std::endl;
    //std::cout << "Scale = " << std::endl << scale << std::endl;

    tree->deleteAttributeFromTree<fl::YExtentAttribute>();

}

bool doesFileExist(const std::string &fileName){
    std::ifstream infile(fileName);
    return infile.good();
}

void branchAnalysis(fl::ImageTree *tree, const std::vector <fl::Node *>nodes){
    tree->addAttributeToTree<fl::AreaAttribute>(new::AreaSettings());
    std::vector <std::pair<int, double> > areaValues;
    for (int i=0, szi = nodes.size(); i < szi; ++i){
        std::stringstream ss;
        ss << "branch_" << std::setfill('0') << std::setw(6) << i << ".txt";
        std::ofstream outF;
        outF.open(ss.str(), std::ios::out);
        std::cout << "BRANCH ANALYSIS OUTPUT" << std::endl;
        tree->analyseBranch<fl::AreaAttribute>(nodes[i], areaValues);
        for (int j=0, szj = areaValues.size(); j < szj; ++j)
            outF << areaValues[j].first << " " << (int)areaValues[j].second << std::endl;
        areaValues.clear();
        outF.close();
    }
    tree->deleteAttributeFromTree<fl::AreaAttribute>();
}

void objectAnalysis(fl::ImageTree *tree, const std::vector <fl::Node *> *npc, std::string prefix){

    std::cout << "OUTPUT ATTRIBUTE VALUES " << std::endl;

    tree->addAttributeToTree<fl::AreaAttribute>(new AreaSettings());

    std::cout << "Area" << std::endl;

    std::ofstream outF;
    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_ar.txt", std::ios::out);
    tree->writeAttributesToFile<fl::AreaAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_ar.txt", std::ios::out);
    tree->writeAttributesToFile<fl::AreaAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_ar.txt", std::ios::out);
    tree->writeAttributesToFile<fl::AreaAttribute>(npc[2], outF);
    outF.close();

    tree->deleteAttributeFromTree<fl::AreaAttribute>();

    tree->addAttributeToTree<fl::NonCompactnessAttribute>(new NonCompactnessSettings());

    std::cout << "Non-compactness" << std::endl;

    //std::ofstream outF;
    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_nc.txt", std::ios::out);
    tree->writeAttributesToFile<fl::NonCompactnessAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_nc.txt", std::ios::out);
    tree->writeAttributesToFile<fl::NonCompactnessAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_nc.txt", std::ios::out);
    tree->writeAttributesToFile<fl::NonCompactnessAttribute>(npc[2], outF);
    outF.close();

    tree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();

    tree->addAttributeToTree<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::roundness));

    std::cout << "Roundness" << std::endl;

    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_rnd.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_rnd.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_rnd.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[2], outF);
    outF.close();

    tree->changeAttributeSettings<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::kurtosis));

    std::cout << "Kurtosis" << std::endl;

    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_kur.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_kur.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_kur.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[2], outF);
    outF.close();

    tree->changeAttributeSettings<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::eccentricity));

    std::cout << "Eccentricity" << std::endl;

    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_ecc.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_ecc.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_ecc.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[2], outF);
    outF.close();

    tree->changeAttributeSettings<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::triangularity));

    std::cout << "Triangularity" << std::endl;

    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_tri.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_tri.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_tri.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MomentsAttribute>(npc[2], outF);
    outF.close();


    tree->deleteAttributeFromTree<fl::MomentsAttribute>();

    tree->addAttributeToTree<fl::SparsityAttribute>(new fl::SparsitySettings());
    //tree->addAttributeToTree<fl::AreaAttribute>(new AreaSettings());

    std::cout << "Sparsity" << std::endl;

    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_spa.txt", std::ios::out);
    tree->writeAttributesToFile<fl::SparsityAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_spa.txt", std::ios::out);
    tree->writeAttributesToFile<fl::SparsityAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_spa.txt", std::ios::out);
    tree->writeAttributesToFile<fl::SparsityAttribute>(npc[2], outF);
    outF.close();

    tree->deleteAttributeFromTree<fl::SparsityAttribute>();

    tree->addAttributeToTree<fl::RegionDynamicsAttribute>(new fl::RegionDynamicsSettings());
    //tree->addAttributeToTree<fl::AreaAttribute>(new AreaSettings());

    std::cout << "Region Dynamics " << std::endl;


    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_rdy.txt", std::ios::out);
    tree->writeAttributesToFile<fl::RegionDynamicsAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_rdy.txt", std::ios::out);
    tree->writeAttributesToFile<fl::RegionDynamicsAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_rdy.txt", std::ios::out);
    tree->writeAttributesToFile<fl::RegionDynamicsAttribute>(npc[2], outF);
    outF.close();

    tree->deleteAttributeFromTree<fl::RegionDynamicsAttribute>();

    tree->addAttributeToTree<fl::MeanAttribute>(new fl::MeanSettings());
    //tree->addAttributeToTree<fl::AreaAttribute>(new AreaSettings());

    std::cout << "Mean" << std::endl;

    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_mea.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MeanAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_mea.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MeanAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_mea.txt", std::ios::out);
    tree->writeAttributesToFile<fl::MeanAttribute>(npc[2], outF);
    outF.close();

    tree->deleteAttributeFromTree<fl::MeanAttribute>();

    std::cout << "Value Deviation " << std::endl;

    tree->addAttributeToTree<fl::ValueDeviationAttribute>(new fl::ValueDeviationSettings());

    std::cout << "Writing attributes for weed patches" << std::endl;
    outF.open(prefix+"weed_vde.txt", std::ios::out);
    tree->writeAttributesToFile<fl::ValueDeviationAttribute>(npc[0], outF);
    outF.close();
    std::cout << "Writing attributes for crop patches" << std::endl;
    outF.open(prefix+"crop_vde.txt", std::ios::out);
    tree->writeAttributesToFile<fl::ValueDeviationAttribute>(npc[1], outF);
    outF.close();
    std::cout << "Writing attributes for mixed patches" << std::endl;
    outF.open(prefix+"mixed_vde.txt", std::ios::out);
    tree->writeAttributesToFile<fl::ValueDeviationAttribute>(npc[2], outF);
    outF.close();

    tree->deleteAttributeFromTree<fl::ValueDeviationAttribute>();


}



void loadClassifiedNodes(const fl::ImageTree *tree, std::vector <fl::Node *> *npc, std::string classFile){

    std::cout << "LOADING NODES PER CLASS" << std::endl;

    std::ifstream inF;
    std::string::size_type idx = classFile.rfind('.');
    std::string baseName = classFile.substr(0, idx);
    std::string extension = classFile.substr(idx);

    std::cout << "Reading weed patches from " << baseName + "_weed" + extension << std::endl;

    inF.open(baseName + "_weed" + extension, std::ios::in);
    tree->loadNodesFromIDFile(npc[0], inF);
    inF.close();

    std::cout << "Reading crop patches from " << baseName + "_crop" + extension << std::endl;

    inF.open(baseName + "_crop" + extension, std::ios::in);
    tree->loadNodesFromIDFile(npc[1], inF);
    inF.close();

    std::cout << "Reading mixed patches from " << baseName + "_mixed" + extension << std::endl;

    inF.open(baseName + "_mixed" + extension, std::ios::in);
    tree->loadNodesFromIDFile(npc[2], inF);
    inF.close();


}

void saveClassifiedNodes(const fl::ImageTree *tree, const std::vector <fl::Node *> *npc, std::string classFile){

    std::cout << "SAVING NODES PER CLASS" << std::endl;

    std::ofstream outF;
    std::string::size_type idx = classFile.rfind('.');
    std::string baseName = classFile.substr(0, idx);
    std::string extension = classFile.substr(idx);

    std::cout << "Writing weed patches into " << baseName + "_weed" + extension << std::endl;

    outF.open(baseName + "_weed" + extension, std::ios::out);
    tree->writeNodeIDToFile(npc[0], outF);
    outF.close();

    std::cout << "Writing crop patches into " << baseName + "_crop" + extension << std::endl;

    outF.open(baseName + "_crop" + extension, std::ios::out);
    tree->writeNodeIDToFile(npc[1], outF);
    outF.close();

    std::cout << "Writing mixed patches into " << baseName + "_mixed" + extension << std::endl;

    outF.open(baseName + "_mixed" + extension, std::ios::out);
    tree->writeNodeIDToFile(npc[2], outF);
    outF.close();
}

void saveNodes(const fl::ImageTree *tree, const std::vector <fl::Node *> &selection, const std::string &detFile){
    std::cout << "SAVING DETECTED NODES" << std::endl;

    std::ofstream outF;

    std::cout << "Writing object patches into " << detFile << std::endl;

    outF.open(detFile, std::ios::out);
    tree->writeNodeIDToFile(selection, outF);
    outF.close();
}

void loadNodes(const fl::ImageTree *tree, std::vector <fl::Node *> &selection, const std::string &detFile){
    std::cout << "LOADING DETECTED NODES" << std::endl;

    std::cout << "Reading object patches from " << detFile << std::endl;

    std::ifstream inF;
    inF.open(detFile, std::ios::in);
    tree->loadNodesFromIDFile(selection, inF);
    inF.close();

}

void objectDetection(fl::ImageTree *tree, std::vector <fl::Node *> &selection, std::vector <fl::Node *> *source){

    std::cout << "OBJECT DETECTION" << std::endl;

    fl::treeObjectDetection(*tree, selection, source);
}


void outputSegmentation(const fl::ImageTree *tree, const std::vector <fl::Node *> &selection, const std::string &segPath){
    std::cout << "OUTPUT SEGMENTATION" << std::endl;

    std::cout << "outputing " << selection.size() << " nodes" << std::endl;

    cv::Mat display(tree->treeHeight(), tree->treeWidth(),
                    CV_8UC3, cv::Vec3b(0,0,0));

    //cv::Mat display(tree->image().size(), tree->image().type());
    //cv::Mat display;
    cv::cvtColor(tree->image(), display, cv::COLOR_GRAY2BGR);

    std::cout << "channels: " << display.channels() << std::endl;

    //std::cout << "channels: " << tree->image().channels() << std::endl;

    //tree->image().copyTo(display);
    tree->markSelectedNodes(display, selection, cv::Vec3b(255, 255, 255));
    if (!segPath.empty())
        cv::imwrite(segPath, display);
    else{
        cv::namedWindow("Segmentation", cv::WINDOW_NORMAL);
        cv::resizeWindow("Segmentation", display.cols/2, display.rows/2);
        cv::imshow("Segmentation", display);
        cv::waitKey(0);
        cv::destroyWindow("Segmentation");
    }
}

void outputClassification(const fl::ImageTree *tree, const std::vector <fl::Node *> *npc, const std::string &classPath){

    std::cout << "OUTPUT CLASSIFICATION" << std::endl;

    cv::Mat display(tree->treeHeight(), tree->treeWidth(),
                    CV_8UC3, cv::Vec3b(0,0,0));
    tree->markSelectedNodes(display, npc[2], cv::Vec3b(200, 80, 80));
    tree->markSelectedNodes(display, npc[0], cv::Vec3b(80, 80, 200));
    tree->markSelectedNodes(display, npc[1], cv::Vec3b(80, 200, 80));

    if (!classPath.empty())
        cv::imwrite(classPath, display);
    else{
        cv::namedWindow("Classification", cv::WINDOW_NORMAL);
        cv::resizeWindow("Classification", display.cols/2, display.rows/2);
        cv::imshow("Classification", display);
        cv::waitKey(0);
        cv::destroyWindow("Classification");
    }
}

void manualClassification(const fl::ImageTree *tree, std::vector <fl::Node *> selection, std::vector <fl::Node *> *npc, const cv::Mat &rgb){
    std::cout << "MANUAL CLASSIFICATION" << std::endl;

    std::vector <int> classSizes;
    fl::manualRegionClassification(selection, tree->image(), rgb, classSizes);

    int sizeSoFar = 0;
    for (int i=0, szi = classSizes.size(); i < szi; ++i)
        npc[i].insert(npc[i].end(), selection.begin()+sizeSoFar, selection.begin()+sizeSoFar+classSizes[i]);
}

void runObjectDetectionAndSegmentation(int argc, char **argv){
    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH);
    fl::Node *root;
    fl::ImageTree *tree = new fl::ImageTree(root = fl::maxTreeBerger(image, std::less<int>()),
                                    std::make_pair(image.rows, image.cols)); // min-tree creation

    std::vector <fl::Node *> selection;
    objectDetection(tree, selection);

    if (argc > 2)
        saveNodes(tree, selection, argv[2]);

    if (argc > 3){
        tree->setImage(image);
        outputSegmentation(tree, selection, argv[3]);
        tree->unsetImage();
    }
    //else
    //    outputSegmentation(tree, selection);
    delete tree;
}

void runManualClassification(int argc, char **argv){
    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH);
    fl::Node *root;
    fl::ImageTree *tree = new fl::ImageTree(root = fl::maxTreeBerger(image, std::less<int>()),
                                    std::make_pair(image.rows, image.cols)); // min-tree creation

    std::vector <fl::Node *> selection;

    if (argc > 2)
        loadNodes(tree, selection, argv[2]);
    else
        objectDetection(tree, selection);

    std::vector <fl::Node *> npc[3];

    tree->setImage(image);
    manualClassification(tree, selection, npc, image);
    tree->unsetImage();

    if (argc > 3)
        saveClassifiedNodes(tree, npc, argv[3]);

    delete tree;
}

void runObjectAnalysis(int argc, char **argv){
    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH);
    fl::Node *root;
    fl::ImageTree *tree = new fl::ImageTree(root = fl::maxTreeBerger(image, std::less<int>()),
                                    std::make_pair(image.rows, image.cols)); // min-tree creation

    std::vector <fl::Node *> selection;

    if (argc > 2)
        loadNodes(tree, selection, argv[2]);
    else
        objectDetection(tree, selection);

    std::vector <fl::Node *> npc[3];

    if (argc > 3){
        loadClassifiedNodes(tree, npc, argv[3]);
        objectAnalysis(tree, npc);
    }


    delete tree;
}

void runPipeline(int argc, char **argv){

    std::cout << "Load image " << argv[1] << std::endl;
    std::cout << "And the RGB for visualization: " << argv[2] << std::endl;

    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH);
    cv::Mat rgb = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);
    fl::Node *root;

    std::cout << "Constructing image tree " << std::endl;

    fl::ImageTree *tree = new fl::ImageTree(root = fl::maxTreeBerger(image, std::less<int>()),
                                    std::make_pair(image.rows, image.cols)); // min-tree creation

    std::vector <fl::Node *> selection;
    std::vector <fl::Node *> source;
    std::vector <fl::Node *> npc[3];

    bool curDet = false;
    if (argc > 3 && doesFileExist(argv[3]))
        loadNodes(tree, selection, argv[3]);
    else{

        objectDetection(tree, selection, &source);

        curDet = true;
        if (argc == 3)
            return;
        else{
            saveNodes(tree, selection, argv[3]);
            //saveNodes(tree, source, std::string(argv[3])+"_source");
            //branchAnalysis(tree, source);
        }
    }

    if (argc < 5){
        tree->setImage(image);
        outputSegmentation(tree, selection);
        tree->unsetImage();
        return;
    }

    if (!doesFileExist(argv[4]) || curDet){

//        for (int i=0, szi = selection.size(); i < szi; ++i){
//            std::stringstream ss;
//            ss << "segment_" << std::setfill('0') << std::setw(6) << i << ".png";
//            outputSegmentation(tree, std::vector<Node*>(1,selection[i]), ss.str());
//        }
        tree->setImage(image);
        outputSegmentation(tree, selection, argv[4]);
        tree->unsetImage();
    }
    if (argc < 6)
        return;

    std::string classBase(argv[5]);
    std::string::size_type idx = classBase.rfind('.');
    std::string baseName = classBase.substr(0, idx);
    std::string extension = classBase.substr(idx);

    bool curClass = false;
    if (!doesFileExist(baseName + "_weed" + extension) ||
        !doesFileExist(baseName + "_crop" + extension) ||
        !doesFileExist(baseName + "_mixed" + extension)){

        tree->setImage(image);
        curClass = true;
        manualClassification(tree, selection, npc, rgb);
        tree->unsetImage();
        saveClassifiedNodes(tree, npc, argv[5]);
    }
    else{
        loadClassifiedNodes(tree, npc, argv[5]);
    }

    if (argc >= 7 && (!doesFileExist(argv[6]) || curClass))
        outputClassification(tree, npc, argv[6]);

    tree->setImage(image);

    if (argc >= 8)
        objectAnalysis(tree, npc, argv[7]);
    else
        objectAnalysis(tree, npc);

    tree->unsetImage();

    delete tree;
}

void testObjectDetection(int argc, char **argv){

    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH);

//    double min, max;
//    cv::minMaxLoc(image, &min, &max);
//    std::cout << "Image: min - " << min << " max - " << max << std::endl;
//    //cv::imshow("Original", image);
//    //cv::waitKey(0);

    //cv::Mat display = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cv::Mat display(image.rows,
     image.cols,
      CV_8UC3,
       cv::Vec3b(0,0,0));
//    cv::Mat display(image.rows,
//     image.cols,
//      CV_8UC1,
//       cv::Scalar(0));

    fl::Node *root;
    fl::ImageTree *tree = new fl::ImageTree(root = fl::maxTreeBerger(image, std::less<int>()),
                                    std::make_pair(image.rows, image.cols)); // min-tree creation

////    tree->addAttributeToTree<fl::YExtentAttribute>(new fl::YExtentSettings());
////
////    cv::Mat residual, scale;
////    tree->ultimateOpening<fl::YExtentAttribute>(residual, scale);
////
////    cv::minMaxLoc(residual, &min, &max);
////    std::cout << "Residual: min - " << min << " max - " << max << std::endl;
////    residual.convertTo(residual, CV_16UC1, 65535.0/max);
////    cv::minMaxLoc(residual, &min, &max);
////
////    std::cout << "Residual: min - " << min << " max - " << max << std::endl;
////    cv::minMaxLoc(scale, &min, &max);
////    std::cout << "Scale: min - " << min << " max - " << max << std::endl;
////    scale.convertTo(scale, CV_16UC1, 65535.0/max);
////
////    tree->deleteAttributeFromTree<fl::YExtentAttribute>();
//
//    if (argc > 2)
//        cv::imwrite(argv[2], residual);
//    if (argc > 3)
//        cv::imwrite(argv[3], scale);
//
//
//    delete tree;
//    return;

    std::cout << "constructed tree" << std::endl;

    std::cout << "Selecting nodes" << std::endl;
    std::vector <fl::Node *> selection;
    fl::treeObjectDetection(*tree, selection);


    std::cout << "Marking" << std::endl;

    //tree->markSelectedNodes(display, selection);
    std::vector <int> classSizes;
    std::vector <Node*> npc[3];
    fl::manualRegionClassification(selection, image, image, classSizes);

    int sizeSoFar = 0;
    for (int i=0, szi = classSizes.size(); i < szi; ++i)
        npc[i].insert(npc[i].end(), selection.begin()+sizeSoFar, selection.begin()+sizeSoFar+classSizes[i]);

    //tree->markSelectedNodes(display, selection);
    std::ofstream outF;
    outF.open("weed.txt", std::ios::out);
    tree->markSelectedNodes(display, npc[0], cv::Vec3b(80, 80, 200));
    tree->writeNodeIDToFile(npc[0], outF);
    outF.close();
    outF.open("crop.txt", std::ios::out);
    tree->markSelectedNodes(display, npc[1], cv::Vec3b(80, 200, 80));
    tree->writeNodeIDToFile(npc[1], outF);
    outF.close();
    outF.open("mixed.txt", std::ios::out);
    tree->markSelectedNodes(display, npc[2], cv::Vec3b(80, 200, 200));
    tree->writeNodeIDToFile(npc[2], outF);
    outF.close();

    std::cout << "done marking" << std::endl;
    //cv::imshow("Detected patches 2", display);
    //cv::waitKey(0);


    if (argc > 2)
        cv::imwrite(argv[2], display);

    delete tree;
    return;

}

void testAllRange(const cv::Mat &image, fl::ImageTree *tos, fl::ImageTree *alpha, fl::ImageTree *max, fl::ImageTree *min, int delta){
    int iterations = 20000;

//    int tosC = 0 , alphaC = 0, mmC = 0;
    double tosP = 0, alphaP = 0, mmP = 0;
//    int tosZ = 0, alphaZ = 0, mmZ = 0;
    fl::Node *bigger;
    for (int i=0; i < iterations; ++i){

        fl::Node * tosNode = tos->randomNode();
        for (; tosNode->elementCount() < 30 || tosNode->elementCount() > image.rows * image. cols * 0.01; tosNode = tos->randomNode());

        if (tosNode->parent() != NULL){
            bigger = tosNode;
//            for (int j=0; (j + bigger->levelDistance(bigger->parent())) <= delta;){
//                j += bigger->levelDistance(bigger->parent());
//                bigger = bigger->parent();
//                if (bigger->parent() == NULL)
//                    break;
//            }

            double lDistance = std::abs(bigger->level() - bigger->parent()->level());
            for (int j=0; (j + lDistance) <= delta; lDistance = std::abs(bigger->level() - bigger->parent()->level())){
                j += lDistance;
                bigger = bigger->parent();
                if (bigger->parent() == NULL)
                    break;
            }

            //tosC += bigger->rangeDiff(image, tosNode);
            //tosP += (double)(bigger->pixelsBetween(*tosNode))/iterations;
            tosP += (double)(bigger->elementCount() - tosNode->elementCount())/iterations;
            //tosZ += (bigger->rangeDiff(image, tosNode)==0);
            //tosZ += (bigger == tosNode);
        }

        fl::Node *alphaNode = alpha->randomNode();
        for (; alphaNode->elementCount() < 80 || alphaNode->elementCount() > image.rows * image.cols * 0.01; alphaNode = alpha->randomNode());

        if (alphaNode->parent() != NULL){
            bigger = alphaNode;
            double lDistance = std::abs(bigger->level() - bigger->parent()->level());
            for (int j=0; (j + lDistance) <= delta; lDistance = std::abs(bigger->level() - bigger->parent()->level())){
                j += lDistance;
                bigger = bigger->parent();
                if (bigger->parent() == NULL)
                    break;
            }

            //alphaC += bigger->rangeDiff(image, alphaNode);
            //alphaP += (double)(bigger->pixelsBetween(*alphaNode))/iterations;
            alphaP += (double)(bigger->elementCount() - alphaNode->elementCount())/iterations;
            //alphaZ += (bigger->rangeDiff(image, alphaNode)==0);
        }

        fl::Node * maxNode = max->randomNode();
        for (; maxNode->elementCount() < 30 || maxNode->elementCount() > image.rows*image.cols*0.01; maxNode = max->randomNode());

        if (maxNode->parent() != NULL){
            bigger = maxNode;
            double lDistance = std::abs(bigger->level() - bigger->parent()->level());
            for (int j=0; (j + lDistance) <= delta; lDistance = std::abs(bigger->level() - bigger->parent()->level())){
                j += lDistance;
                bigger = bigger->parent();
                if (bigger->parent() == NULL)
                    break;
            }

            //mmC += bigger->rangeDiff(image, maxNode);
            //mmP += (double)(bigger->pixelsBetween(*maxNode))/(2*iterations);
            mmP += (double)(bigger->elementCount() - maxNode->elementCount())/(2*iterations);
            //std::cerr << bigger->pixelsBetween(*maxNode) << std::endl;
            //mmZ += (bigger->rangeDiff(image, maxNode)==0);

            //std::cout << diff << std::endl;
        }

        fl::Node *minNode = min->randomNode();
        for (; minNode->elementCount() < 30 || minNode->elementCount() > image.rows*image.cols*0.01; minNode = min->randomNode());

        if (minNode->parent() != NULL){
            bigger = minNode;
            double lDistance = std::abs(bigger->level() - bigger->parent()->level());
            for (int j=0; (j + lDistance) <= delta; lDistance = std::abs(bigger->level() - bigger->parent()->level())){
                j += lDistance;
                bigger = bigger->parent();
                if (bigger->parent() == NULL)
                    break;
            }

            //mmC += bigger->rangeDiff(image, minNode);
            //mmP += (double)(bigger->pixelsBetween(*minNode))/(2*iterations);
            mmP += (double)(bigger->elementCount() - minNode->elementCount())/(2*iterations);
            //mmZ += (bigger->rangeDiff(image, minNode)==0);
        }

    }

    //std::cout << (double)mmC / (2*iterations) << " " << (double)tosC / iterations << " " << (double)alphaC / iterations << std::endl;
    std::cout << mmP << " " << tosP << " " << alphaP << std::endl;
    //std::cerr << max->countNodes() << " " << min->countNodes() << " " << tos->countNodes() << " " << alpha->countNodes() << std::endl;
    //std::cout << (double)mmZ * (max->countNodes() + min->countNodes())/ (4*iterations) << " " << (double)tosZ * tos->countNodes() / iterations << " " << (double)alphaZ * alpha->countNodes()/ iterations << std::endl;
    //std::cout << (double) mmZ / (2*iterations) << " " << (double)tosZ / iterations << " " << (double)alphaZ / iterations << std::endl;
}


void makeTOS(){
    cv::Mat &image=testImage7();

    fl::Node *root = fl::tosGeraud(image);
    //root->printElements();
}


void testGeraud(int delta, cv::Mat &image1, cv::Mat &image2, cv::Mat &display, std::ofstream &outF, std::string featPath, std::string ellipsePath){
    //cv::Mat &image = testImage3();

    try{

        std::vector <std::pair<double, int> > div;
        std::vector <fl::Node *> mser;
        std::vector<std::vector<cv::Point> > points;

        for (int i=0; i < 2; ++i){

            if (i && ((&image1) == (&image2)))
                continue;

            cv::Mat &image = (i)?image1:image2;
            fl::ImageTree *tree = new fl::ImageTree(fl::tosGeraud(image), std::make_pair(image.rows, image.cols));
            fl::markMserInTree(*tree, delta, mser, div, 0.4, 15, 0.5, 0.2);
            fl::fillMserPoints(points, mser);

            delete tree;
        }


        outF << "1.0" << std::endl;
        int toPrintSize = 0;
        for (int i=0, szi = points.size(); i < szi; ++i){
            fl::ellipse current(points[i], image1.cols, image1.rows);
            if (current.isOK())
                ++toPrintSize;
        }
        outF << toPrintSize << std::endl;

        for (int i=0, szi = points.size(); i < szi; ++i){
            fl::ellipse current(points[i], image1.cols, image1.rows);
            if (current.isOK()){
                //outF << points[i].size() << " ";
                outF << current << std::endl;
                //std::sort(points[i].begin(), points[i].end(), comparator);
                //for (int j=0; j < (int)points[i].size(); ++j){
                //    outF << points[i][j].x << " " << points[i][j].y << " ";
                //}
                //outF << std::endl;
            }
        }
        //outF << std::endl;



        if (featPath != ""){

            cv::Mat ellipses;

            display.copyTo(ellipses);

            for( int i = (int)points.size()-1; i >= 0; i-- ){

                std::sort(points[i].begin(), points[i].end(), comparator);
                const std::vector<cv::Point>& r = points[i];

                for ( int j = 0; j < (int)r.size(); j++ )
                {
                    cv::Point pt = r[j];
                    display.at<cv::Vec3b>(pt) = bcolors[(i+r.size())%11];
                }

                cv::RotatedRect box = cv::fitEllipse(r);
                cv::ellipse(ellipses, box, cv::Scalar(196,255,255), 2);

//                if (!(i%40)){
//                    std::cout << "bla!" << std::endl;
//                    imshow( "regions", display);
//                    cv::waitKey(0);
//                }


            }
//            std::cout << "bla!" << std::endl;
//                    imshow( "regions", display);
//                    cv::waitKey(0);

            //cv::imwrite("./regions.png", display);


            cv::imwrite(featPath, display);
            cv::imwrite(ellipsePath, ellipses);
        }
        //imshow( "regions", display);

        //cv::waitKey(0);
    }
    catch (std::string i){
        std::cout << i << " error " << std::endl;
    }
}

void testAlpha(int delta, cv::Mat &image1, cv::Mat &image2, cv::Mat &display, std::ofstream &outF, std::string featPath, std::string ellipsePath){
    //cv::Mat &image = testImage6();
    //cv::Mat image = cv::imread("/home/pbosilj/Programming/Trees/brick2.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    try{

        std::vector <fl::Node *> mser;
        std::vector<std::vector<cv::Point> > points;
        std::vector <std::pair<double, int> > div;

        for (int i=0; i < 2; ++i){

            if (i && ((&image1) == (&image2)))
                continue;

            cv::Mat &image = (i)?image1:image2;

            fl::ImageTree *tree = new fl::ImageTree(fl::alphaTreeDualMax(image), std::make_pair(image.rows, image.cols));

            fl::markMserInTree(*tree, delta, mser, div, 0.4, 20, 0.6, 0.1);

            fl::fillMserPoints(points, mser);

            delete tree;
        }

        outF << "1.0" << std::endl;
        int toPrintSize = 0;
        for (int i=0, szi = points.size(); i < szi; ++i){
            fl::ellipse current(points[i], image1.cols, image1.rows);
            if (current.isOK())
                ++toPrintSize;
        }
        outF << toPrintSize << std::endl;

        for (int i=0, szi = points.size(); i < szi; ++i){
            fl::ellipse current(points[i], image1.cols, image1.rows);
            if (current.isOK()){
                //outF << points[i].size() << " ";
                outF << current << std::endl;
                //std::sort(points[i].begin(), points[i].end(), comparator);
                //for (int j=0; j < (int)points[i].size(); ++j){
                //    outF << points[i][j].x << " " << points[i][j].y << " ";
                //}
                //outF << std::endl;
            }
        }
        //outF << std::endl;




        if (featPath != ""){

            cv::Mat ellipses;

            display.copyTo(ellipses);

            for( int i = (int)points.size()-1; i >= 0; i-- ){

                std::sort(points[i].begin(), points[i].end(), comparator);
                const std::vector<cv::Point>& r = points[i];

                for ( int j = 0; j < (int)r.size(); j++ )
                {
                    cv::Point pt = r[j];
                    display.at<cv::Vec3b>(pt) = bcolors[(i+r.size())%11];
                }

                cv::RotatedRect box = cv::fitEllipse(r);
                cv::ellipse(ellipses, box, cv::Scalar(196,255,255), 2);

            }

            //cv::imwrite("./regions.png", display);


            cv::imwrite(featPath, display);
            cv::imwrite(ellipsePath, ellipses);
        }
//        imshow("regions", display);
//
//        cv::waitKey(0);
    }
    catch (std::string i){
        std::cout << i << " error " << std::endl;
    }
}



void outputGlobalPS(cv::Mat &image, std::ofstream &outPS){

        //outPS << "200" << std::endl;

        fl::ImageTree *maxTree = new fl::ImageTree(fl::maxTreeNister(image, std::greater<int>()),
                                                    std::make_pair(image.rows, image.cols));
//        maxTree->setImage(image);

//        fl::ImageTree *maxTree = new fl::ImageTree(fl::tosGeraud(image), std::make_pair(image.rows, image.cols));

        maxTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
        maxTree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());
        maxTree->addPatternSpectra2DToTree<fl::AreaAttribute, fl::NonCompactnessAttribute>
                (new fl::PatternSpectra2DSettings(fl::Binning(GAREA, 1, image.rows*image.cols, fl::Binning::logarithmic),
                                                  fl::Binning(GSHAPE, 1, GNC, fl::Binning::logarithmic), false, true, true));

//        maxTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
//        maxTree->addAttributeToTree<fl::EntropyAttribute>(new fl::EntropySettings());
//        maxTree->addPatternSpectra2DToTree<fl::AreaAttribute, fl::EntropyAttribute>
//                (new fl::PatternSpectra2DSettings(fl::Binning(GAREA, 1, image.rows*image.cols, fl::Binning::logarithmic),
//                                                  fl::Binning(GSHAPE, 1, GNC, fl::Binning::logarithmic), true, true));

        {
        const std::vector<std::vector<double> > &ps = maxTree->root()->getPatternSpectra2D(fl::AreaAttribute::name+fl::NonCompactnessAttribute::name)->getPatternSpectraMatrix();
//        const std::vector<std::vector<double> > &ps = maxTree->root()->getPatternSpectra2D(fl::AreaAttribute::name+fl::EntropyAttribute::name)->getPatternSpectraMatrix();


        for (int j=1; j < (int)ps.size()-1; ++j){
            for (int k=1; k < (int)ps[j].size()-1; ++k){
                outPS << std::pow(ps[j][k], 0.2) << " ";
            }
        }
        }

        maxTree->deletePatternSpectra2DFromTree<fl::AreaAttribute, fl::NonCompactnessAttribute>();
        maxTree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();
        maxTree->deleteAttributeFromTree<fl::AreaAttribute>();

//        maxTree->deletePatternSpectra2DFromTree<fl::AreaAttribute, fl::EntropyAttribute>();
//        maxTree->deleteAttributeFromTree<fl::EntropyAttribute>();
//        maxTree->deleteAttributeFromTree<fl::AreaAttribute>();

//        maxTree->unsetImage();
        delete maxTree;


        fl::ImageTree *minTree = new fl::ImageTree(fl::maxTreeNister(image, std::less<int>()),
                                                    std::make_pair(image.rows, image.cols));

        minTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
        minTree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());
        minTree->addPatternSpectra2DToTree<fl::AreaAttribute, fl::NonCompactnessAttribute>
                (new fl::PatternSpectra2DSettings(fl::Binning(GAREA, 1, image.rows*image.cols, fl::Binning::logarithmic),
                                                  fl::Binning(GSHAPE, 1, GNC, fl::Binning::logarithmic), false, true, true));



//        minTree->setImage(image);
//        minTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
//        minTree->addAttributeToTree<fl::EntropyAttribute>(new fl::EntropySettings());
//        minTree->addPatternSpectra2DToTree<fl::AreaAttribute, fl::EntropyAttribute>
//                (new fl::PatternSpectra2DSettings(fl::Binning(GAREA, 1, image.cols*image.rows, fl::Binning::logarithmic),
//                                                  fl::Binning(GSHAPE, 1, GNC, fl::Binning::logarithmic), true, true));

        {
        const std::vector<std::vector<double> > &ps = minTree->root()->getPatternSpectra2D(fl::AreaAttribute::name+fl::NonCompactnessAttribute::name)->getPatternSpectraMatrix();
//        const std::vector<std::vector<double> > &ps = minTree->root()->getPatternSpectra2D(fl::AreaAttribute::name+fl::EntropyAttribute::name)->getPatternSpectraMatrix();
        for (int j=1; j < (int)ps.size()-1; ++j){
            for (int k=1; k < (int)ps[j].size()-1; ++k){
                outPS << std::pow(ps[j][k], 0.2) << " ";
            }
        }
        }

        minTree->deletePatternSpectra2DFromTree<fl::AreaAttribute, fl::NonCompactnessAttribute>();
        minTree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();
        minTree->deleteAttributeFromTree<fl::AreaAttribute>();

//        minTree->deletePatternSpectra2DFromTree<fl::AreaAttribute, fl::EntropyAttribute>();
//        minTree->deleteAttributeFromTree<fl::EntropyAttribute>();
//        minTree->deleteAttributeFromTree<fl::AreaAttribute>();
//        minTree->unsetImage();
        delete minTree;

        outPS << "\n";

}

/* yanwei code */

void forYanwei(int delta, cv::Mat &image, bool minTree, int salientNum){
    try{
        int matchesWanted;
        std::vector <fl::Node *> mser;
        std::vector <std::pair<double, int> > div;

        std::vector <std::vector<cv::Point> > points;
        std::set <fl::Node *> salientMser;
        std::vector <std::pair<std::pair<double, int >, int> > salientDiv;

        fl::ImageTree *tree;
        fl::Node *myRoot;

        if (minTree){
            tree = new fl::ImageTree(myRoot=fl::maxTreeBerger(image, std::less<int>()),
                                    std::make_pair(image.rows, image.cols));
        }
        else{
            tree = new fl::ImageTree(myRoot=fl::maxTreeBerger(image, std::greater<int>()),
                                    std::make_pair(image.rows, image.cols));
        }

        fl::markMserInTree(*tree, delta, mser, div, 0.2, 50, 0.45, 0.2);

        fl::fillMserPoints(points, mser);

        std::sort(div.begin(), div.end(), std::less<std::pair <double, int> >());

        int added=0;
        for (int i=0, szi = div.size(); i < szi; ++i){
            fl::ellipse current(points[div[i].second], image.cols, image.rows);
            if (current.isOK()){
                salientMser.insert(mser[div[i].second]);
                ++added;
            }
        }

        if (salientMser.find(myRoot) == salientMser.end())
            ++added;

        std::cerr << "1.0" << std::endl;
        std::cerr << added << std::endl;

        tree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
        tree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());
        tree->addAttributeToTree<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::bnormal, 1, 1));
        tree->addPatternSpectra2DToTree<fl::AreaAttribute, fl::NonCompactnessAttribute>
                (new fl::PatternSpectra2DSettings(fl::Binning(10, 1, NODE_VAL, fl::Binning::logarithmic, 1000),
                                                  fl::Binning(6, 1, 53, fl::Binning::logarithmic), false, true, true));


        //tree->printPartialTreeWithSpectrum<fl::AreaAttribute, fl::NonCompactnessAttribute>(salientMser, image.cols, image.rows);

        points.clear();
        mser.clear();
        div.clear();
        salientDiv.clear();
        salientMser.clear();

        tree->deletePatternSpectra2DFromTree<fl::AreaAttribute, fl::NonCompactnessAttribute>();
        tree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();
        tree->deleteAttributeFromTree<fl::MomentsAttribute>();
        tree->deleteAttributeFromTree<fl::AreaAttribute>();

        delete tree;
    }
    catch (std::string i){
        std::cout << i << " error " << std::endl;
    }

}


void outputMSERPointsAndPS(int delta, cv::Mat &image, std::ofstream &outPS, std::ofstream &outP, int salientNum){
    try{
        int matchesWanted;
        std::vector <fl::Node *> mser;
        std::vector <std::pair<double, int> > div;

        std::vector <std::vector<cv::Point> > points;
        std::vector <fl::Node *> salientMser;
        std::vector <std::pair<std::pair<double, int >, int> > salientDiv;

        fl::ImageTree *maxTree = new fl::ImageTree(fl::maxTreeBerger(image, std::greater<int>()),
                                                std::make_pair(image.rows, image.cols));

        fl::markMserInTree(*maxTree, delta, mser, div, 0.2, 50, 0.45, 0.2);
        fl::fillMserPoints(points, mser);

        std::cerr << points.size() << std::endl;



/*
        int minMserArea = -1;
        for (int i=0, szi = mser.size(); i < szi; ++i){
            int curVal = ((fl::AreaAttribute *)mser[i]->getAttribute(fl::AreaAttribute::name))->value();
            if (minMserArea == -1 || curVal < minMserArea)
                minMserArea = curVal;
        }
*/

        std::sort(div.begin(), div.end(), std::less<std::pair <double, int> >());
        matchesWanted = (salientNum==-1)?((int)div.size()):(salientNum*2);

        for (int i=0, szi = div.size(), added = 0; i < szi && added < matchesWanted; ++i){
            fl::ellipse current(points[div[i].second], image.cols, image.rows);
            if (current.isOK()){
                salientMser.push_back(mser[div[i].second]);
                salientDiv.push_back(std::make_pair(std::make_pair(div[i].first, 1), added));
                ++added;
            }
        }

        fl::ImageTree *minTree = new fl::ImageTree(fl::maxTreeBerger(image, std::less<int>()),
                                                std::make_pair(image.rows, image.cols));

        fl::markMserInTree(*minTree, delta, mser, div, 0.2, 50, 0.45, 0.2);
        points.clear();
        fl::fillMserPoints(points, mser);

        std::cerr << points.size() << std::endl;


/*
        for (int i=0, szi = mser.size(); i < szi; ++i){
            int curVal = ((fl::AreaAttribute *)mser[i]->getAttribute(fl::AreaAttribute::name))->value();
            if (minMserArea == -1 || curVal < minMserArea)
                minMserArea = curVal;
        }
*/
        std::sort(div.begin(), div.end(), std::less<std::pair <double, int> >());
        matchesWanted = (salientNum==-1)?((int)div.size()):(salientNum*2);

        for (int i=0, szi = div.size(), added = salientMser.size(), asz = salientMser.size() + matchesWanted; i < szi && added < asz; ++i){
            fl::ellipse current(points[div[i].second], image.cols, image.rows);
            if (current.isOK()){
                salientMser.push_back(mser[div[i].second]);
                salientDiv.push_back(std::make_pair(std::make_pair(div[i].first, 0), added));
                ++added;
            }
        }

        mser.clear();
        div.clear();

        points.clear();
        fl::fillMserPoints(points, salientMser);
        std::sort(salientDiv.begin(), salientDiv.end(), std::less<std::pair<std::pair<double, int> , int> > ());

        maxTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
        maxTree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());
        maxTree->addAttributeToTree<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::bnormal, 1, 1));
        maxTree->addPatternSpectra2DToTree<fl::AreaAttribute, fl::NonCompactnessAttribute>
                (new fl::PatternSpectra2DSettings(fl::Binning(10, 1, NODE_VAL, fl::Binning::logarithmic, 1000),
                                                  fl::Binning(6, 1, 53, fl::Binning::logarithmic), false, true, true));

        minTree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
        minTree->addAttributeToTree<fl::NonCompactnessAttribute>(new fl::NonCompactnessSettings());
        minTree->addAttributeToTree<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::bnormal, 1, 1));
        minTree->addPatternSpectra2DToTree<fl::AreaAttribute, fl::NonCompactnessAttribute>
                (new fl::PatternSpectra2DSettings(fl::Binning(10, 1, NODE_VAL, fl::Binning::logarithmic, 1000),
                                                  fl::Binning(6, 1, 53, fl::Binning::logarithmic), false, true, true));

        outPS << 66 << std::endl;
        //outP << "1.0" << std::endl << (((int)salientDiv.size() >= salientNum*2 && salientNum != -1)?(salientNum*2):((int)salientDiv.size())) << std::endl;

        { // max-tree global PS
            const std::vector<std::vector<double> > &ps = maxTree->root()->
                    getPatternSpectra2D(fl::AreaAttribute::name+fl::NonCompactnessAttribute::name)->getPatternSpectraMatrix();

            for (int j=1; j < (int)ps.size()-1; ++j){
                for (int k=1; k < (int)ps[j].size()-1; ++k){
                    outPS << std::pow(ps[j][k], 0.2) << " ";
                }
            }
            outPS << " 0 0 0 0 0";
            outPS << " 2";
            outPS << std::endl;
        }

        { // min-tree global PS
            const std::vector<std::vector<double> > &ps = minTree->root()->
                    getPatternSpectra2D(fl::AreaAttribute::name+fl::NonCompactnessAttribute::name)->getPatternSpectraMatrix();

            for (int j=1; j < (int)ps.size()-1; ++j){
                for (int k=1; k < (int)ps[j].size()-1; ++k){
                    outPS << std::pow(ps[j][k], 0.2) << " ";
                }
            }
            outPS << " 0 0 0 0 0";
            outPS << " 0";
            outPS << std::endl;
        }

        int szi = (salientNum == -1)?((int)salientDiv.size()):(std::min((int)salientDiv.size(), salientNum*2));
        for (int i=0; i < szi; ++i){

//            std::vector <fl::Node *> pathDown;
//            salientMser[salientDiv[i].second]->parentBySize(4.5, pathDown);
//            const std::vector<std::vector<double> > &ps = pathDown.back()->getPatternSpectra2D(fl::AreaAttribute::name+fl::NonCompactnessAttribute::name)
//                                                            ->getPatternSpectraMatrix(&pathDown);

            //maxTree->changeAttributeSettings<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::hu, 1));
            //minTree->changeAttributeSettings<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::hu, 1));

            const std::vector<std::vector<double> > &ps = salientMser[salientDiv[i].second]->parentBySize(7.5)->
                    getPatternSpectra2D(fl::AreaAttribute::name+fl::NonCompactnessAttribute::name)->getPatternSpectraMatrix();

            //outP << (((fl::AreaAttribute *)salientMser[salientDiv[i].second]->parentBySize(7.5)->getAttribute(fl::AreaAttribute::name))->value()) << std::endl;

            double sum = 0.0;
            for (int j=1; j < (int)ps.size()-1; ++j){
                for (int k=1; k < (int)ps[j].size()-1; ++k){
                    sum += std::pow(ps[j][k], 0.2);
                }
            }

            //std::cout << "minMserArea: " << 50 << " area: " << ((fl::AreaAttribute *)salientMser[salientDiv[i].second]->getAttribute(fl::AreaAttribute::name))->value() << std::endl;
            for (int j=1; j < (int)ps.size()-1; ++j){
                for (int k=1; k < (int)ps[j].size()-1; ++k){
                    outPS << std::pow(ps[j][k], 0.2) << " ";
                }
                //outPS << std::endl;
            }

            fl::MomentsHolder allMoments = (((fl::MomentsAttribute *)salientMser[salientDiv[i].second]->parentBySize(7.5)->getAttribute(fl::MomentsAttribute::name))->value());
            allMoments.setDefaultCastValue(fl::MomentType::bnormal,1,1);
            double bnm = double(allMoments);
            allMoments.setDefaultCastValue(fl::MomentType::bnormal,2,0);
            double bnm2 = double(allMoments);
            allMoments.setDefaultCastValue(fl::MomentType::bnormal,0,2);
            double bnm3 = double(allMoments);
            allMoments.setDefaultCastValue(fl::MomentType::bnormal,4,0);
            double bnm4 = double(allMoments);
            allMoments.setDefaultCastValue(fl::MomentType::bnormal,0,4);
            double bnm5 = double(allMoments);

            outPS << " " << bnm*20 << " " << bnm2*10 << " " << bnm3*10 << " " << bnm4*10  << " " << bnm5*10;
            outPS << " " << salientDiv[i].first.second*2;
            outPS << std::endl;
//            std::cout << "zeros: " << zeros << " zeros %: " << double(zeros) / double((ps.size()-2) * (ps.front().size()-2)) << " offBinsFull: " << offBinsFull << " % " << double(offBins-offBinsFull)/offBins << std::endl;
//            std::cout << "node count: " << salientMser[salientDiv[i].second]->nodeCount() << std::endl << std::endl;
//
//            fl::ellipse current(points[salientDiv[i].second], image.cols, image.rows);
//
//            outP << current << std::endl;

        }

//        for (int j=0; j < 12; ++j){
//            for (int k=0; k < 12; ++k){
//                std::cout << psStats[j][k] / szi << " ";
//            }
//            std::cout << std::endl;
//        }
//
//        std::cout << std::endl;
//
//        for (int j=0; j < 12; ++j){
//            for (int k=0; k < 12; ++k){
//                std::cout << (int)((psStats[j][k] / szi) > 0.2) << " ";
//            }
//            std::cout << std::endl;
//        }

        points.clear();
        salientDiv.clear();
        salientMser.clear();

        maxTree->deletePatternSpectra2DFromTree<fl::AreaAttribute, fl::NonCompactnessAttribute>();
        maxTree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();
        maxTree->deleteAttributeFromTree<fl::MomentsAttribute>();
        maxTree->deleteAttributeFromTree<fl::AreaAttribute>();

        //maxTree->deleteAttributeFromTree<fl::MomentsHackAttribute>();
        //maxTree->deleteAttributeFromTree<fl::MomentsHack2Attribute>();
        //maxTree->deleteAttributeFromTree<fl::MomentsHack3Attribute>();
        //maxTree->deleteAttributeFromTree<fl::MomentsHack4Attribute>();
        //maxTree->deleteAttributeFromTree<fl::MomentsHack5Attribute>();
        delete maxTree;

        minTree->deletePatternSpectra2DFromTree<fl::AreaAttribute, fl::NonCompactnessAttribute>();
        minTree->deleteAttributeFromTree<fl::NonCompactnessAttribute>();
        minTree->deleteAttributeFromTree<fl::MomentsAttribute>();
        minTree->deleteAttributeFromTree<fl::AreaAttribute>();

        //minTree->deleteAttributeFromTree<fl::MomentsHackAttribute>();
        //minTree->deleteAttributeFromTree<fl::MomentsHack2Attribute>();
        //minTree->deleteAttributeFromTree<fl::MomentsHack3Attribute>();
        //minTree->deleteAttributeFromTree<fl::MomentsHack4Attribute>();
        //minTree->deleteAttributeFromTree<fl::MomentsHack5Attribute>();
        delete minTree;
    }
    catch (std::string i){
        std::cout << i << " error " << std::endl;
    }

}

void getMSRFromTree(fl::ImageTree *tree, fl::ImageTree *dual, fl::mserParams &params, cv::Mat &image, cv::Mat &display, std::ofstream &outF, std::string featPath, std::string ellipsePath){
    std::vector <fl::Node *> mser;
    std::vector <std::vector <cv::Point> > points;
    std::vector <std::pair <double, int> > div;

    //std::vector <std::pair <double, int> > divTotal;
    int firstSize = 0;

    for (int i=0; i < 2; ++i){
        if (i && (dual == NULL || dual == tree))
            continue;

        fl::ImageTree *t = (!i)?(tree):(dual);

        fl::markMserInTree(*t, mser, div, params);

        //divTotal.insert(divTotal.end(), div.begin(), div.end());
        firstSize = mser.size();

        fl::fillMserPoints(points, mser);

    }

    outF << "1.0" << std::endl;
    int toPrintSize = 0;
    for (int i=0, szi = points.size(); i < szi; ++i){
        fl::ellipse current(points[i], image.cols, image.rows);
        if (current.isOK())
            ++toPrintSize;
    }
    outF << toPrintSize << std::endl;

//    for (int i=0, szi = points.size(); i < szi; ++i){
//        fl::ellipse current(points[i], image.cols, image.rows);
//        if (current.isOK()){
//            //if (i == 256 || i == 250) outF << "blub " << std::endl;
//            outF << current << std::endl;
//            //outF << current << "  " << divTotal[i].first << std::endl;
//
//        }
//    }

    if (featPath != ""){

        cv::Mat ellipses;
        cv::Mat features;

        display.copyTo(ellipses);
        display.copyTo(features);


        for( int i = (int)points.size()-1; i >= 0; i-- ){

            fl::ellipse current(points[i], image.cols, image.rows);
            if (!(current.isOK()))
                continue;

            std::sort(points[i].begin(), points[i].end(), comparator);
            const std::vector<cv::Point>& r = points[i];

            for ( int j = 0; j < (int)r.size(); j++ )
            {
                cv::Point pt = r[j];
                display.at<cv::Vec3b>(pt) = bcolors[(i+r.size())%11];
            }

            //cv::RotatedRect box = cv::fitEllipse(r);
            cv::RotatedRect box = cv::minAreaRect(r);
            cv::ellipse(ellipses, box, cv::Scalar(196,255,255), 2);

//            std::cout << i << std::endl;
//
//            imshow("regions", display);
//            cv::waitKey(0);
//
//            imshow("regions", ellipses);
//            cv::waitKey(0);
//
//
//            if (current.small){
//            std::cout << "i: " << i << std::endl;
//            //fl::ellipse current(points[i], image.cols, image.rows);
//            imshow("regions", display);
//            cv::waitKey(0);
//
//            imshow("regions", ellipses);
//            cv::waitKey(0);
//            }

        }

           imshow("regions", display);
            cv::waitKey(0);

            imshow("regions", ellipses);
            cv::waitKey(0);


        cv::imwrite(featPath, display);
        cv::imwrite(ellipsePath, ellipses);
    }
}

void getMSR2FromTree(fl::ImageTree *tree, fl::ImageTree *dual, fl::mserParams &params, cv::Mat &image, cv::Mat &display){
    std::vector <fl::Node *> mser;
    std::vector <std::vector <cv::Point> > points;
    std::vector <std::pair <double, int> > div;

    //std::vector <std::pair <double, int> > divTotal;
    int firstSize = 0;

    for (int i=0; i < 2; ++i){
        if (i && (dual == NULL || dual == tree))
            continue;

        fl::ImageTree *t = (!i)?(tree):(dual);

        fl::markMserInTree(*t, mser, div, params);

        //divTotal.insert(divTotal.end(), div.begin(), div.end());
        firstSize = mser.size();

        fl::fillMserPoints(points, mser);

    }

//    int toPrintSize = 0;
//    for (int i=0, szi = points.size(); i < szi; ++i){
//        fl::ellipse current(points[i], image.cols, image.rows);
//        if (current.isOK())
//            ++toPrintSize;
//    }

//    for (int i=0, szi = points.size(); i < szi; ++i){
//        fl::ellipse current(points[i], image.cols, image.rows);
//        if (current.isOK()){
//            //if (i == 256 || i == 250) outF << "blub " << std::endl;
//            outF << current << std::endl;
//            //outF << current << "  " << divTotal[i].first << std::endl;
//
//        }
//    }


//        cv::Mat ellipses;
//
//        display.copyTo(ellipses);

        std::cout << points.size() << std::endl;
        for( int i = (int)points.size()-1; i >= 0; i-- ){

            fl::ellipse current(points[i], image.cols, image.rows);
            if (!(current.isOK()))
                continue;

//            std::sort(points[i].begin(), points[i].end(), comparator);
            const std::vector<cv::Point>& r = points[i];

            for ( int j = 0; j < (int)r.size(); j++ )
            {
                cv::Point pt = r[j];
                display.at<cv::Vec3b>(pt) = bcolors[(i+r.size())%11];
            }

            //cv::RotatedRect box = cv::fitEllipse(r);
//            cv::RotatedRect box = cv::minAreaRect(r);
//            cv::ellipse(ellipses, box, cv::Scalar(196,255,255), 2);
        }
//           imshow("regions", display);
//            cv::waitKey(0);

//            imshow("regions", ellipses);
//            cv::waitKey(0);


//        cv::imwrite(featPath, display);
//        cv::imwrite(ellipsePath, ellipses);
}


void testNister(int delta, cv::Mat &image1, cv::Mat &image2, cv::Mat &display, std::ofstream &outF, std::string featPath, std::string ellipsePath){

    //cv::Mat image = cv::imread("/home/pbosilj/Programming/Trees/lenabig.bmp", CV_LOAD_IMAGE_GRAYSCALE);

    //cv::Mat &image = testImage5();

    try{
        std::vector <fl::Node *> mser;
        std::vector<std::vector<cv::Point> > points;
        std::vector <std::pair<double, int> > div;


        for (int i=0; i < 2; ++i){

            if (i && ((&image1) == (&image2)))
                continue;

            cv::Mat &image = (i)?image1:image2;


            fl::ImageTree *tree = new fl::ImageTree(fl::maxTreeNister(image, std::greater<int>()),
                                                    std::make_pair(image.rows, image.cols));

            fl::markMserInTree(*tree, delta, mser, div, 0.4, 15, 0.45, 0.2);
            fl::fillMserPoints(points, mser);

            delete tree;

            tree = new fl::ImageTree(fl::maxTreeNister(image, std::less<int>()),
                                                    std::make_pair(image.rows, image.cols));

            fl::markMserInTree(*tree, delta, mser, div, 0.4, 15, 0.45, 0.2);
            fl::fillMserPoints(points, mser);

            delete tree;

        }

        outF << "1.0" << std::endl;
        int toPrintSize = 0;
        for (int i=0, szi = points.size(); i < szi; ++i){
            fl::ellipse current(points[i], image1.cols, image1.rows);
            if (current.isOK())
                ++toPrintSize;
        }
        outF << toPrintSize << std::endl;

        for (int i=0, szi = points.size(); i < szi; ++i){
            fl::ellipse current(points[i], image1.cols, image1.rows);
            if (current.isOK()){
                //outF << points[i].size() << " ";
                outF << current << std::endl;
                //std::sort(points[i].begin(), points[i].end(), comparator);
                //for (int j=0; j < (int)points[i].size(); ++j){
                //    outF << points[i][j].x << " " << points[i][j].y << " ";
                //}
                //outF << std::endl;
            }
        }
        //outF << std::endl;



        if (featPath != ""){

            cv::Mat ellipses;

            display.copyTo(ellipses);

            for( int i = (int)points.size()-1; i >= 0; i-- ){

                std::sort(points[i].begin(), points[i].end(), comparator);
                const std::vector<cv::Point>& r = points[i];

                for ( int j = 0; j < (int)r.size(); j++ )
                {
                    cv::Point pt = r[j];
                    display.at<cv::Vec3b>(pt) = bcolors[(i+r.size())%11];
                }

                cv::RotatedRect box = cv::fitEllipse(r);
                cv::ellipse(ellipses, box, cv::Scalar(196,255,255), 2);

            }

            //cv::imwrite("./regions.png", display);


            cv::imwrite(featPath, display);
            cv::imwrite(ellipsePath, ellipses);
        }
        //imshow( "regions", display);

        //cv::waitKey(0);

    }
    catch (std::string i){
        std::cout << i << " error " << std::endl;
    }
}

/************** TEST IMAGES *********************/

cv::Mat &testImage1(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(4, 4, CV_8U);
    if (firstCall){
        for (int i=0; i < 4; ++i){
            for (int j=0; j < 2; ++j){
                image.at<uchar>(i,j) = (uchar)255;
            }
            for (int j=3; j < 4; ++j)
                image.at<uchar>(i,j) = (uchar)180;

        }
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage2(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(70, 110, CV_8U);
    if (firstCall){
        for (int i=0; i < 110; ++i){
            for (int j=0; j < 10;++j){
                image.at<uchar>(0+j, i) = (uchar)255;
                image.at<uchar>(60+j, i) = (uchar)255;

                if (i < 70){
                    image.at<uchar>(i,  0+j) = (uchar)255;
                    image.at<uchar>(i, 100+j) = (uchar)255;
                }
            }
        }
        for (int i=10; i < 100; ++i){
            for (int j=0; j < 10; ++j){
                image.at<uchar>(10+j, i) = (uchar)180;
                image.at<uchar>(50+j, i) = (uchar)180;
                if (i < 60){
                    image.at<uchar>(i, 10+j) = (uchar)180;
                    image.at<uchar>(i, 50+j) = (uchar)180;
                    image.at<uchar>(i, 90+j) = (uchar)180;
                }
            }
        }
        for (int i=20; i < 50; ++i)
            for (int j=20; j < 50; ++j)
                image.at<uchar>(i,j) = (uchar)255;
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage3(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(7, 11, CV_8U);
    std::cout << image.rows << " " << image.cols << std::endl;
    if (firstCall){
        for (int i=0; i < 11; ++i){
            image.at<uchar>(0, i) = (uchar)255;
            image.at<uchar>(6, i) = (uchar)255;
            if (i < 7){
                image.at<uchar>(i,  0) = (uchar)255;
                image.at<uchar>(i, 10) = (uchar)255;
            }
        }
        for (int i=1; i < 10; ++i){
            image.at<uchar>(1, i) = (uchar)180;
            image.at<uchar>(5, i) = (uchar)180;
            if (i < 6){
                image.at<uchar>(i, 1) = (uchar)180;
                image.at<uchar>(i, 5) = (uchar)180;
                image.at<uchar>(i, 9) = (uchar)180;
            }
        }
        for (int i=2; i < 5; ++i)
            for (int j=2; j < 5; ++j)
                image.at<uchar>(i,j) = (uchar)255;
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage4(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(9, 9, CV_8U);
    if (firstCall){
        for (int i=1; i < 8; ++i){
            image.at<uchar>(1, i) = (uchar)50;
            image.at<uchar>(7, i) = (uchar)50;
            image.at<uchar>(i, 1) = (uchar)50;
            image.at<uchar>(i, 7) = (uchar)50;
        }
        for (int i=2; i < 7; ++i)
            for (int j=2; j < 7; ++j){
                if (i == 5 && (j==3 || j == 4))
                    image.at<uchar>(i,j) = (uchar)255;
                else if (!(i == 3 && (j==4 || j == 5)))
                    image.at<uchar>(i,j) = (uchar)180;
            }
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage5(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(2, 2, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 255;
        image.at<uchar>(1,0) = 150;
        image.at<uchar>(0,1) = 150;
        firstCall = false;
    }
    return image;
}

cv::Mat &testImage6(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(3, 4, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 1;
        image.at<uchar>(1,0) = 4;
        image.at<uchar>(2,0) = 7;
        image.at<uchar>(0,1) = 3;
        image.at<uchar>(1,1) = 2;
        image.at<uchar>(2,1) = 6;
        image.at<uchar>(0,2) = 7;
        image.at<uchar>(1,2) = 3;
        image.at<uchar>(2,2) = 5;
        image.at<uchar>(0,3) = 2;
        image.at<uchar>(1,3) = 1;
        image.at<uchar>(2,3) = 4;
    }
    return image;
}

cv::Mat &testImage7(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(6, 6, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 6;
        image.at<uchar>(1,0) = 4;
        image.at<uchar>(2,0) = 4;
        image.at<uchar>(3,0) = 8;
        image.at<uchar>(4,0) = 4;
        image.at<uchar>(5,0) = 3;
        image.at<uchar>(0,1) = 7;
        image.at<uchar>(1,1) = 3;
        image.at<uchar>(2,1) = 9;
        image.at<uchar>(3,1) = 8;
        image.at<uchar>(4,1) = 3;
        image.at<uchar>(5,1) = 4;
        image.at<uchar>(0,2) = 8;
        image.at<uchar>(1,2) = 7;
        image.at<uchar>(2,2) = 1;
        image.at<uchar>(3,2) = 1;
        image.at<uchar>(4,2) = 6;
        image.at<uchar>(5,2) = 5;
        image.at<uchar>(0,3) = 6;
        image.at<uchar>(1,3) = 5;
        image.at<uchar>(2,3) = 0;
        image.at<uchar>(3,3) = 2;
        image.at<uchar>(4,3) = 8;
        image.at<uchar>(5,3) = 9;
        image.at<uchar>(0,4) = 8;
        image.at<uchar>(1,4) = 9;
        image.at<uchar>(2,4) = 7;
        image.at<uchar>(3,4) = 6;
        image.at<uchar>(4,4) = 2;
        image.at<uchar>(5,4) = 8;
        image.at<uchar>(0,5) = 9;
        image.at<uchar>(1,5) = 4;
        image.at<uchar>(2,5) = 7;
        image.at<uchar>(3,5) = 3;
        image.at<uchar>(4,5) = 2;
        image.at<uchar>(5,5) = 1;
    }
    return image;
}

cv::Mat &testImage8(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(2, 5, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 3;
        image.at<uchar>(1,0) = 4;

        image.at<uchar>(0,1) = 3;
        image.at<uchar>(1,1) = 1;

        image.at<uchar>(0,2) = 1;
        image.at<uchar>(1,2) = 2;

        image.at<uchar>(0,3) = 4;
        image.at<uchar>(1,3) = 3;

        image.at<uchar>(0,4) = 2;
        image.at<uchar>(1,4) = 1;
    }
    return image;
}

cv::Mat &testImage9(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(5, 6, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 1;
        image.at<uchar>(1,0) = 1;
        image.at<uchar>(2,0) = 1;
        image.at<uchar>(3,0) = 1;
        image.at<uchar>(4,0) = 1;

        image.at<uchar>(0,1) = 1;
        image.at<uchar>(1,1) = 0;
        image.at<uchar>(2,1) = 0;
        image.at<uchar>(3,1) = 0;
        image.at<uchar>(4,1) = 1;

        image.at<uchar>(0,2) = 1;
        image.at<uchar>(1,2) = 0;
        image.at<uchar>(2,2) = 0;
        image.at<uchar>(3,2) = 0;
        image.at<uchar>(4,2) = 1;

        image.at<uchar>(0,3) = 1;
        image.at<uchar>(1,3) = 0;
        image.at<uchar>(2,3) = 0;
        image.at<uchar>(3,3) = 0;
        image.at<uchar>(4,3) = 1;

        image.at<uchar>(0,4) = 1;
        image.at<uchar>(1,4) = 4;
        image.at<uchar>(2,4) = 4;
        image.at<uchar>(3,4) = 4;
        image.at<uchar>(4,4) = 1;

        image.at<uchar>(0,5) = 1;
        image.at<uchar>(1,5) = 1;
        image.at<uchar>(2,5) = 1;
        image.at<uchar>(3,5) = 1;
        image.at<uchar>(4,5) = 1;

    }
    return image;
}

cv::Mat &testImage10(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(5, 4, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = image.at<uchar>(0,1) = image.at<uchar>(0,2) = image.at<uchar>(0,3) = 70;
        image.at<uchar>(4,0) = image.at<uchar>(4,1) = image.at<uchar>(4,2) = image.at<uchar>(4,3) = 70;
        image.at<uchar>(1,0) = image.at<uchar>(1,3) = image.at<uchar>(2,0) = image.at<uchar>(2,3) = image.at<uchar>(3,0) = image.at<uchar>(3,3) = 70;
        image.at<uchar>(1,1) = 66;
        image.at<uchar>(2,1) = 69;
        image.at<uchar>(3,1) = 67;
        image.at<uchar>(1,2) = 59;
        image.at<uchar>(2,2) = 63;
        image.at<uchar>(3,2) = 60;

    }
    return image;
}

cv::Mat &testImage11(void){
    static cv::Mat image = cv::Mat::zeros(1024, 1, CV_8U);
    return image;
}

cv::Mat &testImage12(void){
    static bool firstCall = true;
    static cv::Mat image = cv::Mat::zeros(4, 5, CV_8U);
    if (firstCall){
        image.at<uchar>(0,0) = 1;
        image.at<uchar>(1,0) = 1;
        image.at<uchar>(2,0) = 1;
        image.at<uchar>(3,0) = 0;

        image.at<uchar>(0,1) = 4;
        image.at<uchar>(1,1) = 6;
        image.at<uchar>(2,1) = 6;
        image.at<uchar>(3,1) = 0;

        image.at<uchar>(0,2) = 4;
        image.at<uchar>(1,2) = 6;
        image.at<uchar>(2,2) = 6;
        image.at<uchar>(3,2) = 0;

        image.at<uchar>(0,3) = 0;
        image.at<uchar>(1,3) = 0;
        image.at<uchar>(2,3) = 0;
        image.at<uchar>(3,3) = 0;

        image.at<uchar>(0,4) = 5;
        image.at<uchar>(1,4) = 0;
        image.at<uchar>(2,4) = 9;
        image.at<uchar>(3,4) = 2;

    }
    return image;
}

/**************   DEBUG FUNCTIONS ****************/

bool isMaxRegion(const std::vector<cv::Point>& region, const cv::Mat &img){

    std::vector <std::vector <char> > markers (img.rows, std::vector<char> (img.cols, 0));


    int minInside = -1, maxInside = -1;
    int minOutside = -1, maxOutside = -1;

    for (int i=0; i < (int)region.size(); ++i){
        const cv::Point &pt = region[i];
        markers[pt.x][pt.y] = 1;
        if (pt.x > 0 && !markers[pt.x-1][pt.y])
            markers[pt.x-1][pt.y] = -1;
        if (pt.x < (img.rows-1) && !markers[pt.x+1][pt.y])
            markers[pt.x+1][pt.y] = -1;
        if (pt.y > 0 && !markers[pt.x][pt.y-1])
            markers[pt.x][pt.y-1] = -1;
        if (pt.y < (img.cols-1) && !markers[pt.x][pt.y+1])
            markers[pt.x][pt.y+1] = -1;
    }

    for (int i = 0; i < img.rows; ++i){
        for (int j=0; j < img.cols; ++j){
            if (!markers[i][j])
                continue;
            if (markers[i][j] == +1){ // inside
                if (minInside == -1 || img.at<unsigned char>(j,i) < minInside){
                    if (img.at<unsigned char>(j,i) == 76)
                        std::cout << "76 inside at: (" << i << ", " << j << ")" << std::endl;
                    minInside = img.at<unsigned char>(j,i);
                }
                if (maxInside == -1 || img.at<unsigned char>(j,i) > maxInside){
                    maxInside = img.at<unsigned char>(j,i);
                }
            }
            if (markers[i][j] == -1){ // outside
                if (maxOutside == -1 || img.at<unsigned char>(j,i) > maxOutside){
                    if (img.at<unsigned char>(j,i) == 76)
                        std::cout << "76 outside at: (" << i << ", " << j << ")" << std::endl;
                    maxOutside = img.at<unsigned char>(j,i);
                }
                if (minOutside == -1 || img.at<unsigned char>(j,i) < minOutside){
                    minOutside = img.at<unsigned char>(j,i);
                }
            }
        }
    }

    std::cout << "inside the region [" << minInside << ", " << maxInside << "]" << std::endl;

    std::cout << "outside the region [" << minOutside << ", " << maxOutside << "]" << std::endl;

    return minInside < maxOutside;

}

//void testMSERocv(int delta){
//    cv::Mat img0, img, ellipses;
//
//    img0 = cv::imread("/home/pbosilj/Programming/Trees/lenabig.bmp", CV_LOAD_IMAGE_GRAYSCALE);
//    cv::imwrite("/home/pbosilj/Programming/Trees/lenabiggray.bmp", img0);
//
//    //cv::cvtColor(img0, yuv, cv::COLOR_BGR2YCrCb);
//    //cv::cvtColor(img0, gray, cv::COLOR_BGR2GRAY);
//    cv::cvtColor(img0, img, cv::COLOR_GRAY2BGR);
//    img0.copyTo(ellipses);
//
//    std::vector<std::vector<cv::Point> > contours;
//    std::fflush(stdout);
//    double t = (double)cv::getTickCount();
//    cv::MSER(delta, 60, 15000, 0.25, 0.2, 0, 0, 0, 0)(img0, contours,cv::Mat());
//    t = (double)cv::getTickCount() - t;
//    std::printf( "MSER extracted %d contours in %g ms.\n", (int)contours.size(),
//           t*1000./cv::getTickFrequency() );
//    std::fflush(stdout);
//
//    // draw mser's with different colors
//    for( int i = (int)contours.size()-1; i >= 0; i-- )
//    {
//
//        std::sort(contours[i].begin(), contours[i].end(), comparator);
//        const std::vector<cv::Point>& r = contours[i];
//
//
//        for ( int j = 0; j < (int)r.size(); j++ )
//        {
//            cv::Point pt = r[j];
//            img.at<cv::Vec3b>(pt) = bcolors[i%11];
//        }
//
//        //std::cout << "comp.size: " << r.size() << std::endl;
//        //if (r.size() > 15000)
//            //std::cout << "the region is max region (y/n): " << (int)(isMaxRegion(r, img0)) << std::endl;
//
//        // find ellipse (it seems cvfitellipse2 have error or sth?)
//        cv::RotatedRect box = cv::fitEllipse( r );
//
//        //box.angle=(float)CV_PI/2-box.angle;
//        cv::ellipse( ellipses, box, cv::Scalar(196,255,255), 2 );
//    }
//
//    //imshow( "original", img0 );
//    imshow( "responseOCV", img );
//    imshow( "ellipsesOCV", ellipses );
//
//    cv::waitKey(0);
//}

//void compare(int delta){
//    cv::Mat imgMain, displayMiss, displayMissEl;
//
//    imgMain = cv::imread("/home/pbosilj/Programming/Trees/lenabig.bmp", CV_LOAD_IMAGE_GRAYSCALE);
//    cv::cvtColor(imgMain, displayMiss, cv::COLOR_GRAY2BGR);
//    imgMain.copyTo(displayMissEl);
//
//    std::vector <std::vector <cv::Point> > contoursOCV, contoursMY;
//
//    cv::MSER(delta, 60, 15000, 0.25, 0.2, 0, 0, 0, 0)(imgMain, contoursOCV,cv::Mat());
//
//    for (int i=0, szi = contoursOCV.size(); i < szi; ++i)
//        std::sort(contoursOCV[i].begin(), contoursOCV[i].end(), comparator);
//
//
//
//    fl::ImageTree *tree = new fl::ImageTree(fl::maxTreeNister(imgMain, std::greater<int>()),
//                                                std::make_pair(imgMain.rows, imgMain.cols));
//
//    std::vector <fl::Node *> mser;
//    std::vector <std::pair<double, int> > div;
//    fl::markMserInTree(*tree, delta, mser, div, 15000, 60, 0.25, 0.2);
//    fl::fillMserPoints(contoursMY, mser);
//
//    delete tree;
//    tree = new fl::ImageTree(fl::maxTreeNister(imgMain, std::less<int>()),
//                                 std::make_pair(imgMain.rows, imgMain.cols));
//
//    fl::markMserInTree(*tree, delta, mser, div, 15000, 60, 0.25, 0.2);
//    fl::fillMserPoints(contoursMY, mser);
//
//    for (int i=0, szi = contoursMY.size(); i < szi; ++i)
//        std::sort(contoursMY[i].begin(), contoursMY[i].end(), comparator);
//
//    std::vector <int> noMatchOCV(contoursOCV.size(), -1), noMatchMY(contoursMY.size(), -1);
//
//    int perfectMatches = 0;
//
//    for (int i=0, szi = contoursMY.size(); i < szi; ++i){
//        int bestMatch = 0;
//
//        const std::vector <cv::Point> &currMY = contoursMY[i];
//
//        for (int j=0, szj = contoursOCV.size(); j < szj; ++j){
//            int matchAmount = 0;
//            const std::vector <cv::Point> &currOCV = contoursOCV[j];
//            for (int k=0, l=0, szk = currMY.size(), szl = currOCV.size(); k < szk && l < szl; ){
//                if (currMY[k].x == currOCV[l].x && currMY[k].y == currOCV[l].y){
//                    ++matchAmount;
//                    ++k, ++l;
//                }
//                else if (comparator(currMY[k], currOCV[l])){
//                    ++k;
//                }
//                else{
//                    ++l;
//                }
//            }
//            if (matchAmount > bestMatch){
//                bestMatch = matchAmount;
//            }
//            if (matchAmount == (int)currMY.size() && matchAmount == (int)currOCV.size()){
//                noMatchMY[i] = j;
//                noMatchOCV[j] = i;
//                break;
//            }
//        }
//
//        if (((double)bestMatch / currMY.size()) > 0.7)
//            ++perfectMatches;
//        else{
//            noMatchMY[i] = -2;
//        }
//
//        std::cout << "bestMatch: " << bestMatch << " percentage: " << (double)bestMatch*100/currMY.size() << " size: " << currMY.size() << std::endl;
//    }
//
//    for( int i = (int)contoursMY.size()-1; i >= 0; i-- ){
//        if (noMatchMY[i] != -2)
//            continue;
//        const std::vector<cv::Point>& r = contoursMY[i];
//
//        for ( int j = 0; j < (int)r.size(); j++ )
//        {
//            cv::Point pt = r[j];
//            displayMiss.at<cv::Vec3b>(pt) = bcolors[i%11];
//        }
//
//        // find ellipse (it seems cvfitellipse2 have error or sth?)
//        cv::RotatedRect box = cv::fitEllipse( r );
//
//        //box.angle=(float)CV_PI/2-box.angle;
//        cv::ellipse( displayMissEl, box, cv::Scalar(196,255,255), 2 );
//    }
//
//
//    std::cout << "near perfect matches: " << perfectMatches << " out of " << contoursMY.size() << std::endl;
//
//    cv::waitKey(0);
//
//    imshow( "miss", displayMiss );
//
//    cv::waitKey(0);
//
//    imshow( "miss ellipses", displayMissEl );
//
//    cv::waitKey(0);
//
//    delete tree;
//}

int getMaxComponent(const cv::Mat &img, fl::pxCoord start, int minValue){
    std::queue <fl::pxCoord> toVisit;
    std::vector <fl::pxCoord> px;
    char visited[img.rows][img.cols];

    int minInside, maxInside;
    int minOutside = 0, maxOutside = 0;
    bool outMinSet = false, outMaxSet = false;

    if (img.at<uchar>(start.X, start.Y) < minValue)
        return 0;

    std::vector<cv::Point> contour;

    int num = 1;

    for (int i=0; i < (int)img.rows; ++i)
        for (int j=0; j < (int)img.cols; ++j)
            visited[i][j] = false;

    visited[start.X][start.Y] = true;

    toVisit.push(start);
    contour.push_back(cv::Point(start.Y, start.X));
    px.push_back(start);

    minInside = maxInside = (int)img.at<uchar>(start.X, start.Y);

    for (fl::pxCoord current; !toVisit.empty(); ){

        current = toVisit.front();
        int currentVal = img.at<uchar>(current.X, current.Y);
        if (currentVal > maxInside)
            maxInside = currentVal;
        else if (currentVal < minInside)
            minInside = currentVal;

        toVisit.pop();

        fl::pxCoord next = current;
        ++next.X;

        if (fl::coordOk(next, img.cols, img.rows)){
            if (!visited[next.X][next.Y]){
                int nextVal = img.at<uchar>(next.X, next.Y);
                if (nextVal >= minValue){
                    visited[next.X][next.Y] = true;
                    toVisit.push(next);
                    contour.push_back(cv::Point(next.Y, next.X));
                    px.push_back(next);
                    ++num;
                }
                else{
                    if (!outMaxSet || nextVal > maxOutside){
                        maxOutside = nextVal;
                        outMaxSet = true;
                    }
                    if (!outMinSet || nextVal < minOutside){
                        minOutside = nextVal;
                        outMinSet = true;
                    }
                }
            }
        }

        next = current;
        --next.X;

        if (fl::coordOk(next, img.cols, img.rows)){
            if (!visited[next.X][next.Y]){
                int nextVal = img.at<uchar>(next.X, next.Y);
                if (nextVal >= minValue){
                    visited[next.X][next.Y] = true;
                    toVisit.push(next);
                    contour.push_back(cv::Point(next.Y, next.X));
                    px.push_back(next);
                    ++num;
                }
                else{
                    if (!outMaxSet || nextVal > maxOutside){
                        maxOutside = nextVal;
                        outMaxSet = true;
                    }
                    if (!outMinSet || nextVal < minOutside){
                        minOutside = nextVal;
                        outMinSet = true;
                    }
                }
            }
        }


        next = current;
        ++next.Y;

        if (fl::coordOk(next, img.cols, img.rows)){
            if (!visited[next.X][next.Y]){
                int nextVal = img.at<uchar>(next.X, next.Y);
                if (nextVal >= minValue){
                    visited[next.X][next.Y] = true;
                    toVisit.push(next);
                    contour.push_back(cv::Point(next.Y, next.X));
                    px.push_back(next);
                    ++num;
                }
                else{
                    if (!outMaxSet || nextVal > maxOutside){
                        maxOutside = nextVal;
                        outMaxSet = true;
                    }
                    if (!outMinSet || nextVal < minOutside){
                        minOutside = nextVal;
                        outMinSet = true;
                    }
                }
            }
        }


        next = current;
        --next.Y;

        if (fl::coordOk(next, img.cols, img.rows)){
            if (!visited[next.X][next.Y]){
                int nextVal = img.at<uchar>(next.X, next.Y);
                if (nextVal >= minValue){
                    visited[next.X][next.Y] = true;
                    toVisit.push(next);
                    contour.push_back(cv::Point(next.Y, next.X));
                    px.push_back(next);
                    ++num;
                }
                else{
                    if (!outMaxSet || nextVal > maxOutside){
                        maxOutside = nextVal;
                        outMaxSet = true;
                    }
                    if (!outMinSet || nextVal < minOutside){
                        minOutside = nextVal;
                        outMinSet = true;
                    }
                }
            }
        }

    }

    std::cout << "in: " << minInside << " " << maxInside << std::endl;
    std::cout << "out: " << minOutside << " " << maxOutside << std::endl;
    std::cout << "size: " << contour.size() << std::endl;

//    std::sort(px.begin(), px.end());
//    for (int i=0; i < (int)px.size(); ++i)
//        std::cout << px[i].X << " " << px[i].Y << std::endl;

    cv::Mat img0;

    cv::cvtColor(img, img0, cv::COLOR_GRAY2BGR);




        for ( int j = 0; j < (int)contour.size(); j++ )
        {
            cv::Point pt = contour[j];
            img0.at<cv::Vec3b>(pt) = bcolors[0];
        }

        //std::cout << "comp.size: " << r.size() << std::endl;
        //if (r.size() > 15000)
            //std::cout << "the region is max region (y/n): " << (int)(isMaxRegion(r, img0)) << std::endl;

        // find ellipse (it seems cvfitellipse2 have error or sth?)


    //imshow( "original", img0 );
    imshow( "responseOCV", img0 );

    cv::waitKey(0);

    return num;

}

bool comparator(const cv::Point &a, const cv::Point &b) {
    if (a.x != b.x)
        return a.x < b.x;
    return a.y < b.y;
}
