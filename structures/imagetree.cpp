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
void ImageTree::getLeaves(std::vector <fl::Node *> &leaves){
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

void ImageTree::getLeafExtinctions(std::vector <std::pair <int, fl::Node *> > &leafExt){
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
    int num;
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

#if 1

/// \param selected Output vector filled with selected `Node`s.
/// \param leafExt A list of leaf `Node`s and their associated extinction
/// values.
/// \param sourceLeaves An optional output, the leaf `Node`s associated
/// to the selected `Node`s, in order.
///
/// \note Pull parameters out, make more consistent
void ImageTree::selectFromLeaves(std::vector <fl::Node * > &selected,
                            const std::vector <std::pair <int, fl::Node *> > &leafExt,
                            std::vector <fl::Node *> *sourceLeaves){
    selected.clear();
    this->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
    for (int i=0, szi = (int)leafExt.size(); i < szi; ++i){
        //std::cout << "Node i=" << i << " stability " << leafExt[i].first << std::endl;
        Node *best = NULL, *cur;
//        double bestGrowth = 0;
        bool last = false;
        Node *par;
//        int bestArea = 0;
        std::vector<std::pair<double, std::pair<double, Node *> > > allGrowth;
        for (cur = leafExt[i].second; !cur->isRoot(); cur = cur->parent()){
            if (!last){
                par = cur;
                last = true;
            }
            /// TODO: 30 IS THE DELTA PARAMETER
            for (; !par->parent()->isRoot() && std::abs(cur->level() - par->parent()->level()) <= 30; par = par->parent());
            //std::cout << "parent value " << par->level() << " " << (int)par->isRoot() << std::endl;
            int parentArea = ((fl::AreaAttribute*)par->getAttribute(fl::AreaAttribute::name))->value();
            int selfArea = ((fl::AreaAttribute*)cur->getAttribute(fl::AreaAttribute::name))->value();
            if (selfArea > 5e5) /// TODO: MAXaREA PARAMETER
                break;
            double growth = (double)(parentArea - selfArea) / selfArea;
            allGrowth.emplace_back(std::make_pair(growth, std::make_pair(selfArea, cur)));

        }

        int bestIndex = 0;
        bool correction = false;
        std::sort(allGrowth.rbegin(), allGrowth.rend());

        for (int _i=1, sz_i = (int)allGrowth.size(); _i < sz_i && allGrowth[_i].first > 10; ++_i){
            if (allGrowth[_i].second.first > 4e5)
                continue;
            if (allGrowth[_i].second.first > 15*allGrowth[bestIndex].second.first) {
                int cnt = 0;
                for (; _i + cnt < (int)allGrowth.size() && allGrowth[_i+cnt].second.first > 0.85*allGrowth[_i].second.first
                    && allGrowth[_i+cnt].second.first < 1.15*allGrowth[_i].second.first;
                     ++cnt){
                     }
                if (cnt >= 6){
                    if (allGrowth[_i].second.second->visited){
                        bestIndex = -1;
                        break;
                    }
                    allGrowth[bestIndex].second.second -> visited = true;
                    bestIndex = _i;
                    correction = true;
                }
            }
        }

        /// alternative selection
//        for (int _i=1, sz_i = (int)allGrowth.size(); _i < sz_i; ++_i){
//            if (allGrowth[_i].second.first > 4e5)
//                continue;
//            //if (allGrowth[_i].second.first > 5*allGrowth[bestIndex].second.first){
//            if (allGrowth[_i].second.first > 15*allGrowth[bestIndex].second.first) {
//                int cnt = 0;
//                 for (int _j = 0; _i + _j < (int)allGrowth.size() && allGrowth[_i+_j].first > 0.5*allGrowth[_i].first;
//                     cnt += (allGrowth[_i+_j].second.first > 0.85*allGrowth[_i].second.first
//                        && allGrowth[_i+_j].second.first < 1.15*allGrowth[_i].second.first),
//                     ++_j){
//                     }
//                if (cnt >= 4){
//                    if (allGrowth[_i].second.second->visited){
//                        bestIndex = -1;
//                        break;
//                    }
//                    allGrowth[bestIndex].second.second -> visited = true;
//                    bestIndex = _i;
//                    correction = true;
//                    _i += cnt-1;
//                }
//            }
//        }

        /// TODO: 150 IS THE MINaREA PARAMETER
        if (bestIndex > -1 && !allGrowth.empty() && allGrowth[bestIndex].second.first > 150){
            if (! allGrowth[bestIndex].second.second->visited){

                selected.push_back(allGrowth[bestIndex].second.second);
                if (sourceLeaves != NULL)
                    sourceLeaves->push_back(leafExt[i].second);
                allGrowth[bestIndex].second.second->visited = true;
            }
        }
    }
    this->deleteAttributeFromTree<fl::AreaAttribute>();
}

#endif


/// Run manual classification on regions for a list of `Node`s
/// (e.g. segmentation output). Each region can be classified as
/// `crop`, `weed, `mixed` or discarded.
///
/// \param toMark A vector of pointers to `Node`s to manually
/// classify.
/// \param classes An output vector of classes assigned to `Node`s.
/// \note 0 = weed patch, 1 = crop patch, 2 = mixed patch, -1 = discarded.
/// \param rgbImg Should add default value. Used to visualization purposes
/// in manual classification.
void ImageTree::showPerNode(const std::vector <Node *> &toMark,
                                  std::vector <int> &classes,
                            const cv::Mat &rgbImg) const{
    const cv::Mat &image = *(this->img);
    cv::Mat segment(image.size(), CV_8U, cv::Scalar(0)), masked(image.size(), CV_8U, cv::Scalar(0));
    cv::Mat segment_single(image.size(), CV_8U, cv::Scalar(0)), prev(image.size(), CV_8U, cv::Scalar(0));;
    cv::Mat rected(image.size(), CV_8UC3, cv::Vec3b(0,0,0)), invmasked(image.size(), CV_8U, cv::Scalar(0));
    cv::Mat rectcut(image.size(), CV_8UC3, cv::Vec3b(0,0,0)), maskcut(image.size(), CV_8U, cv::Scalar(0));
    cv::Mat colrect(image.size(), CV_8UC3, cv::Vec3b(0,0,0)), colrectcut(image.size(), CV_8UC3, cv::Vec3b(0,0,0));

    cv::Mat output(image.rows*2, image.cols*3, CV_8UC3);
    cv::Mat out1(output, cv::Rect(cv::Point(0,0), image.size()));
    cv::Mat out3(output, cv::Rect(cv::Point(image.cols, 0), image.size()));
    cv::Mat out5(output, cv::Rect(cv::Point(image.cols*2, 0), image.size()));
    cv::Mat out2(output, cv::Rect(cv::Point(0, image.rows), image.size()));
    cv::Mat out4(output, cv::Rect(cv::Point(image.cols, image.rows), image.size()));
    cv::Mat out6(output, cv::Rect(cv::Point(image.cols*2, image.rows), image.size()));

    classes.clear();

    cv::namedWindow("Pick a class", cv::WINDOW_NORMAL);
    cv::resizeWindow("Pick a class", image.cols/1.5 * 3, image.rows/1.5*2);

    std::cout << toMark.size() << std::endl;
    cv::Vec3b colors[14] = { cv::Vec3b(0, 0, 200),
                            cv::Vec3b(0, 200, 0),
                            cv::Vec3b(200, 0, 0),
                            cv::Vec3b(150, 0, 150),
                            cv::Vec3b(0, 150, 150),
                            cv::Vec3b(150, 150, 0),
                            cv::Vec3b(50, 100, 200),
                            cv::Vec3b(200, 50, 100),
                            cv::Vec3b(100, 200, 50),
                            cv::Vec3b(200, 100, 50),
                            cv::Vec3b(100, 50, 200),
                            cv::Vec3b(50, 200, 100),
                            cv::Vec3b(60, 210, 40),
                            cv::Vec3b(180, 80, 50) };
//    cv::namedWindow("Patch", cv::WINDOW_NORMAL);

    bool returned = false;
    for (int i=0, szi = toMark.size(); i < szi; ++i)
        if (toMark[i] != NULL){
            output.setTo(cv::Scalar(0));
            segment_single.setTo(cv::Scalar(0));

            if (returned){
                segment.setTo(cv::Scalar(0));
                for (int j=0; j < i; ++j)
                    toMark[j]->colorSolid(segment, cv::Scalar(180));
                returned = false;
            }

            cv::threshold(segment, segment, 1, 90, cv::THRESH_BINARY);

            masked.setTo(cv::Scalar(0));
            invmasked.setTo(cv::Scalar(0));
            toMark[i]->colorSolid(segment, cv::Scalar(180));
            toMark[i]->colorSolid(segment_single, cv::Scalar(255));
            image.copyTo(masked, segment_single);
            image.copyTo(invmasked, cv::Scalar::all(255)-segment_single);
            //image.copyTo(rected);
            cv::cvtColor(image, rected, cv::COLOR_GRAY2RGB);
            rgbImg.copyTo(colrect);

            std::vector <pxCoord> elems;
            toMark[i]->getElements(elems);
            std::sort(elems.begin(), elems.end());
            pxCoord ll, ur, center;
            ll.X = std::max(elems[0].X-75, 0);
            ur.X = std::min(elems.back().X+75, image.cols);
            std::sort(elems.begin(), elems.end(), [](const pxCoord &left, const pxCoord &right) {
                    return (left.Y < right.Y) || (left.Y == right.Y && left.X < right.X);
                });
            for (int j=0, szj = elems.size(); j < szj; ++j)
                //std::cout << (int)rected.at<cv::Vec3b>(elems[i].X, elems[i].Y)[1] << std::endl;
                rected.at<cv::Vec3b>(elems[j].Y, elems[j].X)[1] = std::min(rected.at<cv::Vec3b>(elems[j].Y, elems[j].X)[1]+50, 255);
            //std::cout << "Ended redding the thing" << std::endl;

            ll.Y = std::max(elems[0].Y-75, 0);
            ur.Y = std::min(elems.back().Y+75, image.rows);
            //center.X = (ll.X + ur.X)/2;
            //center.Y = (ll.Y + ur.Y)/2;
            //int radius = (std::max(std::abs(center.X - ll.X), std::abs(center.Y - ll.Y))+60)*120/100;
    //        cv::circle(image, cv::Point(center.X, center.Y), radius, value, 2);
            cv::rectangle(rected, cv::Point(ll.X, ll.Y), cv::Point(ur.X, ur.Y), cv::Vec3b(50, 75, 200), 8);
            cv::rectangle(colrect, cv::Point(ll.X, ll.Y), cv::Point(ur.X, ur.Y), cv::Vec3b(50, 75, 200), 8);
            double resizeFactor = (double)image.cols / (std::abs(ll.X - ur.X)-4);
            resizeFactor = std::min(resizeFactor, (double)image.rows/ (std::abs(ll.Y - ur.Y)-16));

            cv::resize(cv::Mat(rected, cv::Rect(cv::Point(ll.X+8, ll.Y+8), cv::Point(ur.X-8, ur.Y-8))), rectcut, cv::Size(0,0), resizeFactor, resizeFactor);
            cv::resize(cv::Mat(masked, cv::Rect(cv::Point(ll.X+8, ll.Y+8), cv::Point(ur.X-8, ur.Y-8))), maskcut, cv::Size(0,0), resizeFactor, resizeFactor);
            cv::resize(cv::Mat(colrect, cv::Rect(cv::Point(ll.X+8, ll.Y+8), cv::Point(ur.X-8, ur.Y-8))), colrectcut, cv::Size(0,0), resizeFactor, resizeFactor);

            rected.copyTo(out1);
            colrect.copyTo(out3);
            //colrectcut.copyTo(out6);
            cv::cvtColor(segment, out5, cv::COLOR_GRAY2RGB);
            //segment.copyTo(out2);
            //masked.copyTo(out2);
            //invmasked.copyTo(out3);
            rectcut.copyTo(cv::Mat(out2, cv::Rect(cv::Point((out3.cols - rectcut.cols)/2 ,(out3.rows - rectcut.rows)/2), rectcut.size())));
            colrectcut.copyTo(cv::Mat(out4, cv::Rect(cv::Point((out6.cols - colrectcut.cols)/2 ,(out6.rows - colrectcut.rows)/2), colrectcut.size())));

            cv::cvtColor(maskcut, cv::Mat(out6, cv::Rect(cv::Point((out4.cols - maskcut.cols)/2 ,(out4.rows - maskcut.rows)/2), maskcut.size())), cv::COLOR_GRAY2RGB);
            //maskcut.copyTo(cv::Mat(out4, cv::Rect(cv::Point((out4.cols - maskcut.cols)/2 ,(out4.rows - maskcut.rows)/2), maskcut.size())));
            //maskcut.copyTo(cv::Mat(out6, cv::Rect(cv::Point(0,0), rectcut.size())));

            cv::imshow("Pick a class", output);
            std::cout << "[" << i+1 << " / " << toMark.size() << "] ";
            if (i > 0)
                std::cout << "Please chose a class for the highlighted vegetation patch:\n(c) Crop\n(w) Weed\n(m) Mixed\n(d) Discard\n(p) Previous - correct choice for previous patch\n";
            else
                std::cout << "Please chose a class for the highlighted vegetation patch:\n(c) Crop\n(w) Weed\n(m) Mixed\n(d) Discard\n";
            for (bool flag = true; flag;){
                char c = cv::waitKey(0) & 0xFF;
                switch (c){
                    case 'M':
                    case 'm':
                        std::cout << "MIXED patch" << std::endl;
                        classes.push_back(2);
                        flag = false;
                        break;
                    case 'C':
                    case 'c':
                        std::cout << "CROP patch" << std::endl;
                        classes.push_back(1);
                        flag = false;
                        break;
                    case 'W':
                    case 'w':
                        std::cout << "WEED patch" << std::endl;
                        classes.push_back(0);
                        flag = false;
                        break;
                    case 'D':
                    case 'd':
                        std::cout << "DISCARDED (ambiguous) patch" << std::endl;
                        classes.push_back(-1);
                        flag = false;
                        break;
                    case 'P':
                    case 'p':
                        if (i > 0){
                            std::cout << "Going back to PREVIOUS patch" << std::endl;
                            i -= 2;
                            classes.pop_back();
                            flag = false;
                            returned = true;
                        }
                    default:
                        break;

                }
            }

//            toMark[i]->colorSolid(image, colors[i%14]);
//            cv::resizeWindow("Patch", image.cols/2, image.rows/2);
//            cv::imshow("Patch", image);
//            cv::waitKey(0);
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
