#ifndef CROPWEEDSPIPELINE_H
#define CROPWEEDSPIPELINE_H

#include "../structures/imagetree.h"
#include "../structures/node.h"

#include <vector>
#include <string>

/// \brief Runs the pipeline that segments and classifies the
/// vegetation regions in an NDVI image
void runPipeline(int argc, char **argv);


void runObjectAnalysis(int argc, char **argv);
void runManualClassification(int argc, char **argv);
void runObjectDetectionAndSegmentation(int argc, char **argv);
void manualClassification(const fl::ImageTree *tree, std::vector <fl::Node *> selection, std::vector <fl::Node *> *npc, const cv::Mat &rgb);

/// \brief Saves the (region-)classified image (colour).
void outputClassification(const fl::ImageTree *tree, const std::vector <fl::Node *> *npc, const std::string &classPath = "");
/// \brief Saves the segmented image (black and white).
void outputSegmentation(const fl::ImageTree *tree, const std::vector <fl::Node *> &selection, const std::string &segPath = "");

/// \brief Runs the object detection to segment the image.
void objectDetection(fl::ImageTree *tree, std::vector <fl::Node *> &selection, std::vector <fl::Node *> *source = NULL);

/// \brief Save the IDs of certain `Node`s from an `ImageTree`
/// into a file.
void saveNodes(const fl::ImageTree *tree, const std::vector <fl::Node *> &selection, const std::string &detFile);
/// \brief Load a list of `Node`s for an `ImageTree` into a
/// file from their IDs.
void loadNodes(const fl::ImageTree *tree, std::vector <fl::Node *> &selection, const std::string &detFile);

/// \brief Save the IDs of certain `Node`s divided in three
/// classes from an `ImageTree` into files.
void saveClassifiedNodes(const fl::ImageTree *tree, const std::vector <fl::Node *> *npc, std::string classFile);
/// \brief Load a list of `Node`s divided into three classes
/// for an `ImageTree` into a file from their IDs.
void loadClassifiedNodes(const fl::ImageTree *tree, std::vector <fl::Node *> *npc, std::string classFile);

void objectAnalysis(fl::ImageTree *tree, const std::vector <fl::Node *> *npc, std::string prefix = "");
void branchAnalysis(fl::ImageTree *tree, const std::vector <fl::Node *>nodes);

#endif //CROPWEEDSPIPELINE_H
