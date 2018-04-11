/// \file structures/imagetree.cpp
/// \author Petra Bosilj

#include "imagetree.h"

#include <vector>
#include <utility>
#include <iostream>
#include <stack>

#include <fstream>
#include <sstream>
#include <iomanip>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <cstdlib>

#include "regiondynamicsattribute.h"
#include "noncompactnessattribute.h"
#include "areaattribute.h"
#include "momentsattribute.h"


using namespace fl;

/// Organizes a previously linked collection of `Node`s into
/// an `ImageTree`, allowing the user to manipulate all the
/// `Node`s simultaneously as well as use the relations between
/// the `Node`s.
///
/// \param root A `Node *` to the `Node` in the collection of
/// previously linked `Node`s which is to be the root of the
/// new `ImageTree`.
///
/// \param imDim The dimensions of the image used for construction
/// of the hierarchy. The format is (height, width).
///
/// \note It is assumed that, when destructing the `ImageTree`, it
/// will call the `Node::~Node()` destructor for all the `Node`s
/// belonging to it. This behavior can be further controlled with:
///     - `toggleDelNodesOnDestruct()`
///     - `enableDelNodesOnDestruct()`
///     - `disableDelNodesOnDestruct()`
ImageTree::ImageTree(Node* root, std::pair< int, int > imDim)
    : _root(root), width(imDim.second), height(imDim.first) {
    this->img = NULL;
    this->delOnDest = true;
    this->randomInit = false;
  //this->checkConstraints();
}

/// Class destructor for `ImageTree`. It will also call the
/// destructor for every `Node` in the hierarchy if the appropriate
/// option is enabled.
ImageTree::~ImageTree(){
    if (this->_root != NULL && this->delOnDest){
        this->deallocateRoot(this->_root);
    }
}

/// Flip the settings regarding the destruction of
/// all the `Node`s in the hierarchy when the
/// `ImageTree::~ImageTree()` destructor is called
///
/// Other functions offering the control of the same
/// parameter:
/// - enableDelNodesOnDestruct()
/// - disableDelNodesOnDestruct()
void ImageTree::toggleDelNodesOnDestruct() {
    this->delOnDest = !this->delOnDest;
}

/// Enable the destruction of
/// all the `Node`s in the hierarchy when the
/// `ImageTree::~ImageTree()` destructor is called
///
/// Other functions offering the control of the same
/// parameter:
/// - toggleDelNodesOnDestruct()
/// - disableDelNodesOnDestruct()
void ImageTree::enableDelNodesOnDestruct() { this->delOnDest = true; }

/// Enable the destruction of
/// all the `Node`s in the hierarchy when the
/// `ImageTree::~ImageTree()` destructor is called
///
/// Other functions offering the control of the same
/// parameter:
/// - enableDelNodesOnDestruct()
/// - toggleDelNodesOnDestruct()
void ImageTree::disableDelNodesOnDestruct() { this->delOnDest = false; }

/// Associates the image used to build the hierarchy to the
/// constructed `ImageTree` class.
///
/// \note Required for certain operations like reconstruction,
/// or for operating with certain `Attribute`s.
///
/// \param img Image to be set.
///
/// \remark To be used when the `ImageTree` is based on
/// a monochannel image.
void ImageTree::setImage(const cv::Mat &img) {
    this->img = &img;
}

/// \return `true` if the image is associated to this
/// `ImageTree`, `false` otherwise.
///
/// \remark To be used when the `ImageTree` is based on
/// a monochannel image.
bool ImageTree::imageSet() const {
    return (this->img != NULL);
}

/// Unsets the image associated to this `ImageTree` and
/// releases it from the memory.
///
/// \remark To be used when the `ImageTree` is based on
/// a monochannel image.
void ImageTree::unsetImage() {
    this->img = NULL;
}

/// \return A reference to the image associated to this
/// `ImageTree`.
///
/// \remark To be used when the `ImageTree` is based on
/// a monochannel image.
const cv::Mat & ImageTree::image() const {
    return *this->img;
}

/// Associates the image used to build the hierarchy to the
/// constructed `ImageTree` class.
///
/// \note Required for certain operations like reconstruction,
/// or for operating with certain `Attribute`s.
///
/// \param imgs Multi-channel image to be set.
/// \remark One array element per channel.
///
/// \remark To be used when the `ImageTree` is based on
/// a multi-channel image.
void ImageTree::setImages(const std::vector<cv::Mat> &imgs) {
    this->imgs = &imgs;
}

/// \return `true` if the multi-channel image is associated to
/// this `ImageTree`, `false` otherwise.
///
/// \remark To be used when the `ImageTree` is based on
/// a multi-channel image.
bool ImageTree::imagesSet() const {return (this->imgs != NULL); }

/// Unsets the multi-channel image associated to this `ImageTree` and
/// releases it from the memory.
///
/// \remark To be used when the `ImageTree` is based on
/// a monochannel image.
void ImageTree::unsetImages() { this->imgs = NULL; }

/// \return A reference to the array of image associated
/// representing the multi-channel image associated to this
/// `ImageTree`.
///
/// \remark One array element per channel.
///
/// \remark To be used when the `ImageTree` is based on
/// a multi-channel image.
const std::vector<cv::Mat> &ImageTree::images() const { return *this->imgs; }

/// \return A const pointer to the `Node` which is the root of
/// this `ImageTree`.
/// \note The root accessed in this way can not be modified to
/// ensure the consistency of constructed hierarchies. Various
/// algorithms from the algorithm section allow handling the
/// `ImageTree` and its `Node`s
const Node *ImageTree::root() const { return this->_root; }

/// \return The width (in pixels) of the image associated to the `ImageTree`.
int ImageTree::treeWidth(void) const {return width;}

/// \return The height (in pixels) of the image associated to the `ImageTree`.
int ImageTree::treeHeight(void) const {return height;}

/// \return The number of `Node`s in this `ImageTree`.
int ImageTree::countNodes(void) const { return this->_root->nodeCount(); }

/// \return A pointer to a random `Node` from the `ImageTree`.

/// \note Even though
/// the pointer is not const, to allow the use of functions such as
/// `Node::elementCount()` (which use memoization), a `Node` obtained
/// this way should not be modified by the user.
Node *ImageTree::randomNode(){

    if (this->randomInit == false){
        this->allNodes.clear();
        this->_root->getAllDescendants(allNodes);
        this->randomInit = true;
    }

    Node *returnValue = this->allNodes[std::rand() % this->allNodes.size()];

    return returnValue;
}

/// \return A pair made of the minimum and maximum level value among
/// all the `Node`s in this `ImageTree`.
/// \remark cf. `Node::level()` to access the level of a single `Node`.
std::pair<double, double> ImageTree::minMaxLevel() const{
    return this->_root->minMaxLevel();
}

/// \note The input parameter is optional.
///
/// \param outStream The output stream to which the data should be
/// printed. By default, it equals to `std::cout`.
///
/// \remark Prints the number and a list of elements (pixels) associated
/// directly to each `Node`, as well as the `Node`'s level and gray level.
void ImageTree::printTree(std::ostream &outStream) const{
    this->_root->printElements(outStream);
}

/// \param outPath The path to where to save the image. If left empty, the
/// image is not saved on disk.
///
/// \note To implement for non-grayscale images.
void ImageTree::displayTree(const std::string &outPath) const{
    cv::Mat img = cv::Mat::zeros(this->height, this->width, CV_8U);

    this->_root->colorMe(img);

    cv::namedWindow("TreeReconstruction", 1);
    cv::imshow("TreeReconstruction", img);
    if (outPath != "")
        cv::imwrite(outPath, img);
    cv::waitKey(0);
    cv::destroyWindow("TreeReconstruction");
}

/// Returns a list of all the leaf `Node`s present in this
/// `ImageTree`.
///
/// \param leaves The output vector containing the list of
/// leaf `Node`s.
void ImageTree::getLeaves(std::vector <fl::Node *> &leaves) const{
    leaves.clear();
    std::stack<fl::Node *> nodeStack;
    nodeStack.push(this->_root);
    for (fl::Node *cur = nodeStack.top(); ; cur = nodeStack.top()){
        nodeStack.pop();
        if (cur->_children.empty()){
            leaves.push_back(cur);
        }
        else{
            for (int i=0, szi = cur->_children.size(); i < szi; nodeStack.push(cur->_children[i++]));
        }
        if (nodeStack.empty())
            break;
    }
    return;
}

/// Based on Silva, A.G, Alencar Lotufo, R.: "New extinction values from
/// efficient construction and analysis of extended attribute component
/// tree" (2008)
///
/// Calculates the extinction values (according to the standard definition)
/// or more precisely, `RegionDynamicsAttribute` assigned to each node.
///
/// \param leafExt An output vector to be filled with extinction values
/// associated to each leaf `Node` of the tree.
void ImageTree::getLeafExtinctions(std::vector <std::pair <int, fl::Node *> > &leafExt) const{
    leafExt.clear();
    std::vector <fl::Node *> leaves;
    this->getLeaves(leaves);
    this->addAttributeToTree<fl::RegionDynamicsAttribute>(new fl::RegionDynamicsSettings());
    int treeHeight = ((fl::RegionDynamicsAttribute*)this->_root->getAttribute(fl::RegionDynamicsAttribute::name))->value();
    for (int i=0, szi = leaves.size(); i < szi; ++i){
        int extinction = treeHeight;
        fl::Node *par = leaves[i]->parent();
        for(int parHeight = ((fl::RegionDynamicsAttribute*)par->getAttribute(fl::RegionDynamicsAttribute::name))->value(),
            levelDiff = std::abs(leaves[i]->level() - par->level()); // note: no abs in the original; only for one direction trees
            parHeight <= levelDiff;
            par = par->parent(),
            parHeight = ((fl::RegionDynamicsAttribute*)par->getAttribute(fl::RegionDynamicsAttribute::name))->value(),
            levelDiff = std::abs(leaves[i]->level() - par->level())){
            if (par->visited && par->_children.size() > 1 && parHeight == levelDiff)
                break;
            par->visited = true;
            if (par->isRoot())
                break;
        }
        extinction = std::abs(leaves[i]->level() - par->level());
        leafExt.emplace_back(extinction, leaves[i]);
    }

    this->deleteAttributeFromTree<fl::RegionDynamicsAttribute>();

    std::stack <fl::Node *> nodeStack;
    nodeStack.push(this->_root);
    for (fl::Node *cur; !nodeStack.empty();){
        cur = nodeStack.top();
        nodeStack.pop();
        cur->visited = false;
        for (int i=0, szi = cur->_children.size(); i < szi; ++i) nodeStack.push(cur->_children[i]);
    }
    return;
}


/// A unique identifier for each of the given `Node`s is output (one per
/// line). The identifier encodes the `Node`s position in the `ImageTree`,
/// by encoding the ordinal numbers of the child `Node`s to be taken at
/// each branching (1-indexed and 0-terminated). E.g. if a `Node` can be
/// reached in an `ImageTree` by traversing it from the root, to its third
/// child and then the second child of that child node, it would be encoded
/// as "3,2,0".
///
/// \param nodes A vector of `Nodes` for which the IDs are to be output
/// \param out The output stream to which the output the `Node` IDs.
///
/// \note `Node`s output with this function and in this format can be
/// input by the function `ImageTree::loadNodesFromIDFile()`.
void ImageTree::writeNodeIDToFile(const std::vector <Node *> &nodes, std::ostream &out) const{
    for (int i=0, szi = nodes.size(); i < szi; ++i){
        out << nodes[i]->getIDString() << " " << nodes[i]->level() << std::endl;
    }
}

/// The `Node`s are input based on the unique identifiers given as input,
/// one per line. The identifier encodes the `Node`s position in the
/// `ImageTree`, by encoding the ordinal numbers of the child `Node`s to be
/// taken at each branching (1-indexed and 0-terminated). E.g. if a `Node`
/// can be reached in an `ImageTree` by traversing it from the root, to its
/// third child and then the second child of that child node, it would be
/// encoded as "3,2,0".
///
/// \param nodes A vector to be populated with `Node`s to be input from
/// their IDs.
/// \param in The input stream providing the `Node` IDs.
///
/// \note The `Node`s can be written out in the appropriate format suitable
/// for input through this function using the function
/// `ImageTree::writeNodeIDToFile()`.
void ImageTree::loadNodesFromIDFile(std::vector <Node *> &nodes, std::istream &in) const{
    std::string line;
    while (std::getline(in, line)){
        std::stringstream linestream(line);
        std::string svalue;
        int value;
        fl::Node *cur = this->_root;
        while (std::getline(linestream, svalue, ',')){
            value = std::stoi(svalue);
            if (value > 0)
                cur = cur->_children[value-1];
            else
                break;
        }
        nodes.push_back(cur);
    }
}


/// In-paints all the `Node`s of the `ImageTree` except the root `Node`.
/// Meant to be used to display remaining content in a filtered `ImageTree`.
///
/// \param image The image onto which to draw the regions.
void ImageTree::markAllPatches(cv::Mat &image) const{
    if (image.type() == CV_8UC3)
        this->markAllPatches(image, cv::Vec3b(0, 0, 200));
    else
        this->markAllPatches(image, cv::Scalar(200));
}

/// \copydetails markAllPatches(cv::Mat &image)
///
/// \param value The RGB value of the flat color with which to draw the
/// regions.
void ImageTree::markAllPatches(cv::Mat &image, const cv::Vec3b &value) const{
    const std::vector <Node *> &ch = this->_root->_children;
    for (int i=0, szi = ch.size(); i < szi; ++i){
        if (ch[i] != NULL){
            ch[i]->colorSolid(image, value);
        }
    }
}

/// \copydetails markAllPatches(cv::Mat &image)
///
/// \param value The intensity value of the flat grayvalue with which to draw the
/// regions.
void ImageTree::markAllPatches(cv::Mat &image, const cv::Scalar &value) const{
    const std::vector <Node *> &ch = this->_root->_children;
    for (int i=0, szi = ch.size(); i < szi; ++i){
        if (ch[i] != NULL){
            ch[i]->colorSolid(image, value);
        }
    }
}

/// In-paints all the given `Node`s. Meant to be used to display selected
/// `Node`s following a filtering or detection process filtered `ImageTree`.
///
/// \param image The image onto which to draw the regions.
/// \param toMark The `Node`s to be drawn onto the image.
void ImageTree::markSelectedNodes(cv::Mat &image,
                                  const std::vector <Node *> &toMark) const{
    if (image.type() == CV_8UC3)
        this->markSelectedNodes(image, toMark, cv::Vec3b(0, 0, 200));
    else
        this->markSelectedNodes(image, toMark, cv::Scalar(200));
}

/// \copydetails markSelectedNodes(cv::Mat &image, const std::vector <Node *> &toMark)
///
/// \param value The RGB value of the flat color with which to draw the
/// regions.
void ImageTree::markSelectedNodes(cv::Mat &image,
                                  const std::vector <Node *> &toMark,
                                  const cv::Vec3b &value) const{
    for (int i=0, szi = toMark.size(); i < szi; ++i)
        if (toMark[i] != NULL){
            toMark[i]->colorSolid(image, value);
        }
}

/// \copydetails markSelectedNodes(cv::Mat &image, const std::vector <Node *> &toMark)
///
/// \param value The intensity value of the flat grayvalue with which to draw the
/// regions.
void ImageTree::markSelectedNodes(cv::Mat &image,
                                  const std::vector <Node *> &toMark,
                                  const cv::Scalar &value) const{
    for (int i=0, szi = toMark.size(); i < szi; ++i)
        if (toMark[i] != NULL){
            toMark[i]->colorSolid(image, value);
        }
}

// private methods start here

void ImageTree::reconstructUO(const std::vector< int > &res,
                              const std::vector<int> &scl,
                              cv::Mat &residual, cv::Mat &scale,
                              std::pair<fl::Node *, int> &current) const{

    const fl::Node* cur = current.first;
    if (cur == this->root()){
        residual = cv::Mat(this->treeHeight(), this->treeWidth(), CV_16U, cv::Scalar(0));
        scale = cv::Mat(this->treeHeight(), this->treeWidth(), CV_16U, cv::Scalar(0));

        //double min, max;
        //cv::minMaxLoc(residual, &min, &max);
        //std::cout << "Residual: min - " << min << " max - " << max << std::endl;
    }

    const std::vector <fl::pxCoord > &elems = cur->getOwnElements();
    for (int i=0, szi = elems.size(); i < szi; ++i){

        scale.at<unsigned short>(elems[i].Y, elems[i].X) = (unsigned short)scl[current.second];
        residual.at<unsigned short>(elems[i].Y, elems[i].X) = (unsigned short)res[current.second];
    }
    //std::cout << res[current.second] << std::endl;

    for (int i=0, szi = cur->_children.size(); i < szi; ++i){
        ++current.second;
        current.first = cur->_children[i];
        this->reconstructUO(res, scl, residual, scale, current);
    }
}

// non-recursive
void ImageTree::deallocateRoot(const Node *n){
    std::vector <std::pair<const Node *, bool> > toProcess(1, std::make_pair(n, false));
    std::pair<const Node *, bool> tmp;
    do{
        tmp = toProcess.back();
        if (tmp.second || tmp.first->_children.empty()) { // second time on queue
            delete tmp.first;
            toProcess.pop_back();
        }
        else{ // first time on queue, children not processed
            toProcess.back().second = true;
            for (int i=0, szi = tmp.first->_children.size(); i < szi; ++i){
                toProcess.push_back(std::make_pair(tmp.first->_children[i], false));
            }
        }
    }while(!toProcess.empty());
}

void ImageTree::checkConstraints() const{
  std::vector < std::vector <bool> > covered (this->height, std::vector<bool> (this->width, false));

  std::vector < std::pair <int, int> > px;
  this->_root->getElements(px);

  for (int i=0, szi = px.size(); i < szi; ++i){
    if (covered[px[i].first][px[i].second]){
      std::cout << px[i].first << " " << px[i].second << std::endl;
      throw std::string("pixel present multiple times");
    }
    covered[px[i].first][px[i].second] = true;
  }
  for (int i=0; i < this->height; ++i)
    for (int j=0; j < this->width; ++j)
      if (!covered[i][j]){
          std::cout << i << " " << j << std::endl;
        throw std::string("pixel missing from complete tree");
      }


  return;
}


//Node *ImageTree::lowestPixelOf(pxCoord px) const{
//    std::vector <Node *> toProcess(1, this->_root);
//    Node *tmp;
//    do{
//        tmp = toProcess.back();
//        toProcess.pop_back();
//        if (tmp->hasOwnElement(px))
//            return tmp;
//        for (int i=0, szi = tmp->_children.size(); i < szi; ++i){
//            toProcess.push_back(tmp->_children[i]);
//        }
//    }while(!toProcess.empty());
//    return NULL;
//}
