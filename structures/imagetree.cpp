/// \file structures/imagetree.cpp
/// \author Petra Bosilj

#include "imagetree.h"

#include <vector>
#include <utility>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <cstdlib>

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

void ImageTree::deallocateRoot(const Node *n){
    const std::vector <Node *> &ch = n->_children;
    for (int i=0, szi = ch.size(); i < szi; ++i){
        if (ch[i] != NULL)
            this->deallocateRoot(ch[i]);
    }
    delete n;
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
