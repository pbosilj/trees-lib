/// \file structures/cropweedspipeline.cpp
/// \author Petra Bosilj
/// \date 07/10/2019

#include "cropweedspipeline.h"

#include "../algorithms/maxtreeberger.h"
#include "../algorithms/regionclassification.h"
#include "../algorithms/objectdetection.h"

#include "../structures/noncompactnessattribute.h"
#include "../structures/areaattribute.h"
#include "../structures/momentsattribute.h"
#include "../structures/sparsityattribute.h"
#include "../structures/regiondynamicsattribute.h"
#include "../structures/meanattribute.h"
#include "../structures/valuedeviationattribute.h"

#include "../misc/misc.h"

#include <fstream>
#include <iomanip>

/// \brief Runs the pipeline that segments and classifies the
/// vegetation regions in an NDVI image
///
/// Arguments are positional:
/// 1 - path to the image to process (NDVI image)
/// 2 - path to the registered RGB for visualisation
/// 3 - path to the output of segmentation (list of nodes).
///     If the file exists, the nodes are loaded for the image
///     and the segmentation process is not run.
///     If the file does not exist, the segmentation is run and
///     the output saved to the provided text file.
/// [4] - path to the output of segmentation (image).
///     Can be omitted (call with 3 arguments only).
///     If the file (3) exists, and this file exists,
///     no changes are made to the file (4).
///     If the file (3) does not exist, or this file
///     does not exist, the segementation output is saved
///     as an image in the provided file.
/// [5] - template path to the output of classification (list
///     of nodes).
///     Can be omitted (call with 3 or 4 arguments only).
///     If the path provided is 'XYZ.txt', the files 'XYZ_mixed.txt',
///     'XYZ_crop.txt' and 'XYZ_weed.txt' are required.
///     If the above files exist, the nodes are loaded for the
///     image and the classification is not run.
///     If any of the above files does not exist, the (manual)
///     classification process is ran on the image and the
///     output saved in three files with names as above.
/// [6] - path to the output of classification (image).
///     Can be omitted (call with 3-5 arguments only).
///     If the files (5) exist, and this file exists,
///     no changes are made to the file (6).
///     If the files (5) do not exist, or this file
///     does not exist, the classifcation output is saved
///     as an image in the provided file.
/// [7] - template path for the output of attributes per
///     classified `Node`.
///     Can be omitted (call with 3-6 arguments only).
///     The attributes are as follows: area, non-compactness,
///     roundness, kurtosis, eccentricity, triangularity,
///     sparsity, region dynamics, mean, and value deviation.
///     A separate files is created for each of the three
///     vegetation classes (weeds, crops, mixed) for each attribute,
///     with the order of `Node`s per file corresponding to the
///     order in files of (5).
void rCropWeedsPipeline(int argc, char **argv){

    std::cout << "Loading the image " << argv[1] << std::endl;
    std::cout << "And the RGB for visualization: " << argv[2] << std::endl;

    cv::Mat image = cv::imread(argv[1], cv::IMREAD_ANYDEPTH);
    cv::Mat rgb = cv::imread(argv[2], cv::IMREAD_COLOR);
    fl::Node *root;

    std::cout << "Constructing image tree " << std::endl;

    fl::ImageTree *tree = new fl::ImageTree(root = fl::maxTreeBerger(image, std::greater<int>()),
                                    std::make_pair(image.rows, image.cols)); // min-tree creation

    std::vector <fl::Node *> selection;
    std::vector <fl::Node *> source;
    std::vector <fl::Node *> npc[3];

    bool curDet = false;
    if (argc > 3 && fileExists(argv[3]))
        loadNodes(tree, selection, argv[3]);
    else{

        objectDetection(tree, selection, &source); // new implementation

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

    if (!fileExists(argv[4]) || curDet){

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
    if (!fileExists(baseName + "_weed" + extension) ||
        !fileExists(baseName + "_crop" + extension) ||
        !fileExists(baseName + "_mixed" + extension)){

        tree->setImage(image);
        curClass = true;
        manualClassification(tree, selection, npc, rgb); // new implementation
        tree->unsetImage();
        saveClassifiedNodes(tree, npc, argv[5]);
    }
    else{
        loadClassifiedNodes(tree, npc, argv[5]);
    }

    if (argc >= 7 && (!fileExists(argv[6]) || curClass))
        outputClassification(tree, npc, argv[6]);

    tree->setImage(image);

    if (argc >= 8)
        objectAnalysis(tree, npc, argv[7]);
    else
        objectAnalysis(tree, npc);

    tree->unsetImage();

    delete tree;
}

void runObjectAnalysis(int argc, char **argv){
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_ANYDEPTH);
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

void runManualClassification(int argc, char **argv){
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_ANYDEPTH);
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
void runObjectDetectionAndSegmentation(int argc, char **argv){
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_ANYDEPTH);
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

void manualClassification(const fl::ImageTree *tree, std::vector <fl::Node *> selection, std::vector <fl::Node *> *npc, const cv::Mat &rgb){
    std::cout << "MANUAL CLASSIFICATION" << std::endl;

    std::vector <int> classSizes;
    fl::manualRegionClassification(selection, tree->image(), rgb, classSizes);

    int sizeSoFar = 0;
    for (int i=0, szi = classSizes.size(); i < szi; ++i)
        npc[i].insert(npc[i].end(), selection.begin()+sizeSoFar, selection.begin()+sizeSoFar+classSizes[i]);
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

void outputSegmentation(const fl::ImageTree *tree, const std::vector <fl::Node *> &selection, const std::string &segPath){
    std::cout << "OUTPUT SEGMENTATION" << std::endl;

    std::cout << "outputing " << selection.size() << " nodes" << std::endl;

    cv::Mat display(tree->treeHeight(), tree->treeWidth(),
                    CV_8UC3, cv::Vec3b(0,0,0));

    //cv::Mat display(tree->image().size(), tree->image().type());
    //cv::Mat display;
    //cv::cvtColor(tree->image(), display, cv::COLOR_GRAY2BGR);

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

void objectDetection(fl::ImageTree *tree, std::vector <fl::Node *> &selection, std::vector <fl::Node *> *source){

    std::cout << "OBJECT DETECTION" << std::endl;

    fl::treeObjectDetection(*tree, selection, source);
    //fl::treeObjectDetection(*tree, selection, source, 30, 150, 5e4, 40, 30, 50);
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


