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

//void ImageTree::deallocateRoot(const Node *n){
//    const std::vector <Node *> &ch = n->_children;
//    for (int i=0, szi = ch.size(); i < szi; ++i){
//        if (ch[i] != NULL)
//            this->deallocateRoot(ch[i]);
//    }
//    delete n;
//}

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

void ImageTree::getLeafExtinctions(std::vector <std::pair <int, fl::Node *> > &leafExt){
    leafExt.clear();
    std::vector <fl::Node *> leaves;
    std::cout << "I will be getting leaves." << std::endl;
    getLeaves(leaves);
    std::cout << "I have gotten leaves. Have: " << leaves.size() << std::endl;
    this->addAttributeToTree<fl::RegionDynamicsAttribute>(new fl::RegionDynamicsSettings());
    int treeHeight = ((fl::RegionDynamicsAttribute*)this->_root->getAttribute(fl::RegionDynamicsAttribute::name))->value();
    for (int i=0, szi = leaves.size(); i < szi; ++i){
        //std::cout << "i: " << i << " " << leafExt.size() << std::endl;
        int extinction = treeHeight;
        fl::Node *par = leaves[i]->parent();
        //int parHeight = ((fl::RegionDynamicsAttribute*)par->getAttribute(fl::RegionDynamicsAttribute::name))->value();
        //int levelDiff = (leaves[i]->level() - par->level());
        for(int parHeight = ((fl::RegionDynamicsAttribute*)par->getAttribute(fl::RegionDynamicsAttribute::name))->value(),
            levelDiff = std::abs(leaves[i]->level() - par->level());
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

    //std::cout << "Removing attribute" << std::endl;
    this->deleteAttributeFromTree<fl::RegionDynamicsAttribute>();

    //std::cout << "Resetting visited" << std::endl;
    std::stack <fl::Node *> nodeStack;
    nodeStack.push(this->_root);
    for (fl::Node *cur; !nodeStack.empty();){
        cur = nodeStack.top();
        nodeStack.pop();
        cur->visited = false;
        for (int i=0, szi = cur->_children.size(); i < szi; ++i) nodeStack.push(cur->_children[i]);
    }
    //std::cout << "And done" << leafExt.size() << std::endl;
    return;
}

void ImageTree::printFromLeaves(const std::vector <std::pair <int, fl::Node *> > &leafExt,
                            int N){
    std::ofstream out[3];
    std::stringstream name;
    this->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
    //this->addAttributeToTree<fl::ValueDeviationAttribute>(new fl::ValueDeviationSettings());
    this->addAttributeToTree<fl::MomentsAttribute>(new fl::MomentsSettings(5, fl::MomentType::bnormal, 1, 1));

    for (int i=0, szi = std::min(N, (int)leafExt.size()); i < szi; ++i){
        name << "leaf" << std::setfill('0') << std::setw(3) << i << "Area.txt";
        out[0].open(name.str());
        name.str("");
        name.clear(); // Clear state flags.

        name << "leaf" << std::setfill('0') << std::setw(3) << i << "AL.txt";
        out[1].open(name.str());
        name.str("");
        name.clear(); // Clear state flags.

        name << "leaf" << std::setfill('0') << std::setw(3) << i << "VD.txt";
        out[2].open(name.str());
        name.str("");
        name.clear(); // Clear state flags.

        int area = ((fl::AreaAttribute*)leafExt[i].second->getAttribute(fl::AreaAttribute::name))->value();
        double mom = ((fl::MomentsAttribute*)leafExt[i].second->getAttribute(fl::MomentsAttribute::name))->value();
        int plevel = leafExt[i].second->level();

        out[0] << area << std::endl;

        out[1] << plevel << " " << area << std::endl;

        out[2] << plevel << " " << mom << std::endl;


        for (Node * cur = leafExt[i].second; !cur->isRoot(); cur = cur->parent()){
            int area = ((fl::AreaAttribute*)cur->parent()->getAttribute(fl::AreaAttribute::name))->value();
            double mom = ((fl::MomentsAttribute*)cur->parent()->getAttribute(fl::MomentsAttribute::name))->value();
            int plevel = cur->parent()->level();

            out[0] << area << std::endl;

            out[1] << plevel << " " << area << std::endl;

            out[2] << plevel << " " << mom << std::endl;

        }

        for (int j=0; j < 3; out[j++].close());
    }
    this->deleteAttributeFromTree<fl::AreaAttribute>();
    this->deleteAttributeFromTree<fl::MomentsAttribute>();
}


void ImageTree::selectFromLeaves(std::vector <fl::Node * > &selected,
                            const std::vector <std::pair <int, fl::Node *> > &leafExt,
                            int N){
    selected.clear();
    this->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
    for (int i=0, szi = std::min(N, (int)leafExt.size()); i < szi; ++i){
        //std::cout << "Node i=" << i << " stability " << leafExt[i].first << std::endl;
        Node *best = NULL, *cur;
//        double bestGrowth = 0;
        bool last = false;
        Node *par;
//        int bestArea = 0;
        std::vector<std::pair<double, std::pair<double, Node *> > > allGrowth;
        for (cur = leafExt[i].second; !cur->isRoot(); cur = cur->parent()){
            if (!last){
                par = cur->parent();
                last = true;
            }
            for (; !par->parent()->isRoot() && std::abs(cur->level() - par->parent()->level()) <= 25; par = par->parent());
            //std::cout << "parent value " << par->level() << " " << (int)par->isRoot() << std::endl;
            int parentArea = ((fl::AreaAttribute*)par->getAttribute(fl::AreaAttribute::name))->value();
            int selfArea = ((fl::AreaAttribute*)cur->getAttribute(fl::AreaAttribute::name))->value();
//            if (selfArea < 40)
//                continue;
//            else if (selfArea > 20000)
//                break;
//            if (selfArea > 100000)
//                break;
//            if (selfArea < 30)
//                break;
            //double growth = ( (double)(parentArea - selfArea) / (selfArea * std::abs(par->level() - cur->level())));
            double growth = (double)(parentArea - selfArea) / selfArea;
            allGrowth.emplace_back(std::make_pair(growth, std::make_pair(selfArea, cur)));

            //if (growth > 5)
            //    continue;
//            if (growth > 1.1){
//                std::cout << cur->parent()->level() << " " << cur->level() << std::endl;
//                best = cur;
//                bestGrowth = growth;
//                break;
//            }
//            if (std::abs(cur->parent()->level() - cur->level()) > 30){
//                std::cout << cur->level() << " " << cur->parent()->level() << " " << selfArea << " " << parentArea << std::endl;
//                best = cur;
//                break;
//            }
            //std::cout << "\t growth: " << growth << std::endl;
            //if (growth > 10)
            //    continue;
//            if (growth > bestGrowth){
//                best = cur;
//                bestGrowth = growth;
//                bestArea = selfArea;
//            }
        }
//        std::cout << "BestGrowth " << bestGrowth << " area:" << bestArea << std::endl;
        int bestIndex = 0;
        bool correction = false;
        std::sort(allGrowth.rbegin(), allGrowth.rend());
        for (int _i=1, sz_i = (int)allGrowth.size(); _i < sz_i && allGrowth[_i].first > 10; ++_i){
        //for (int _i=1, sz_i = (int)allGrowth.size(); _i < sz_i; ++_i){
            if (allGrowth[_i].second.first > 5e5)
                continue;
            if (allGrowth[_i].second.first > 20*allGrowth[bestIndex].second.first){
//            if (allGrowth[_i].second.first > 5*allGrowth[bestIndex].second.first && allGrowth[_i].first > 500){
                int cnt = 0;
                std::cout << "trying to replace " << allGrowth[bestIndex].second.first << " with " << allGrowth[_i].second.first << " (growth:" << allGrowth[_i].first << ")" << std::endl;
                for (; _i + cnt < (int)allGrowth.size() && allGrowth[_i+cnt].second.first > 0.85*allGrowth[_i].second.first
                    && allGrowth[_i+cnt].second.first < 1.15*allGrowth[_i].second.first;
                     std::cout << allGrowth[_i+cnt].second.first << " ", ++cnt);
                std::cout << std::endl;
                if (cnt >= 6){
                    std::cout << "Replacing " << allGrowth[bestIndex].second.first << " with " << allGrowth[_i].second.first <<  std::endl;
                    bestIndex = _i;
                    correction = true;
                    _i += cnt-1;
                }
            }
            //std::cout << "Satb: " << allGrowth[_i].first << " Area: " << allGrowth[_i].second << std::endl;
        }
        //if (bestGrowth > 0 && bestArea > 50){
//        if (correction){
//              selected.push_back(allGrowth[bestIndex].second.second);
//        }
        if (!allGrowth.empty() && allGrowth[bestIndex].second.first > 80){
            selected.push_back(allGrowth[bestIndex].second.second);
        }
//        if (bestGrowth > 0 && bestArea > 50){
//            selected.push_back(best);
//        }
    }
    this->deleteAttributeFromTree<fl::AreaAttribute>();
}


//produces markers for onion image
//void ImageTree::selectFromLeaves(std::vector <fl::Node * > &selected,
//                            const std::vector <std::pair <int, fl::Node *> > &leafExt,
//                            int N){
//    selected.clear();
//    this->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
//    for (int i=0, szi = std::min(N, (int)leafExt.size()); i < szi; ++i){
//        std::cout << "Node i=" << i << std::endl;
//        Node *best = NULL, *cur;
//        double bestGrowth = 0;
//        for (cur = leafExt[i].second; !cur->isRoot(); cur = cur->parent()){
////            int parentArea = ((fl::AreaAttribute*)cur->parent()->getAttribute(fl::AreaAttribute::name))->value();
////            int selfArea = ((fl::AreaAttribute*)cur->getAttribute(fl::AreaAttribute::name))->value();
////            double growth = ( (double)(parentArea - selfArea) / (selfArea * std::abs(cur->parent()->level() - cur->level())));
////            if (growth > 1.1){
////                std::cout << cur->parent()->level() << " " << cur->level() << std::endl;
////                best = cur;
////                bestGrowth = growth;
////                break;
////            }
//            if (std::abs(cur->parent()->level() - cur->level()) > 10){
//                best = cur;
//                break;
//            }
//            //std::cout << "\t growth: " << growth << std::endl;
//            //if (growth > 10)
//            //    continue;
////            if (growth > bestGrowth){
////                best = cur;
////                bestGrowth = growth;
////            }
//        }
//        //std::cout << "BestGrowth " << bestGrowth << std::endl;
//        selected.push_back(best);
//    }
//    this->deleteAttributeFromTree<fl::AreaAttribute>();
//}



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

Node *ImageTree::lowestPixelOf(pxCoord px) const{
    std::vector <Node *> toProcess(1, this->_root);
    Node *tmp;
    do{
        tmp = toProcess.back();
        toProcess.pop_back();
        if (tmp->hasOwnElement(px))
            return tmp;
        for (int i=0, szi = tmp->_children.size(); i < szi; ++i){
            toProcess.push_back(tmp->_children[i]);
        }
    }while(!toProcess.empty());
    return NULL;
}

/// \param image The image onto which to draw the regions. Must be an RGB
/// image.
/// \param toMark A list of `Node`s corresponding to the set of regions to
/// draw.
/// \param value The RGB value of the flat color with which to draw the
/// regions.
void ImageTree::markSelectedNodes(cv::Mat &image,
                                  const std::vector <Node *> &toMark,
                                  const cv::Vec3b &value) const{
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

    for (int i=0, szi = toMark.size(); i < szi; ++i)
        if (toMark[i] != NULL){
            //toMark[i]->colorSolid(image, value);

            toMark[i]->colorSolid(image, colors[i%14]);
//            cv::resizeWindow("Patch", image.cols/2, image.rows/2);
//            cv::imshow("Patch", image);
//            cv::waitKey(0);
        }
}

void ImageTree::markAroundNodes(cv::Mat &image,
                     const std::vector <Node *> &toMark,
                     const cv::Vec3b &value) const{
    std::stringstream outfilename;
    for (int i=0, szi = toMark.size(); i < szi; ++i){
        std::vector <pxCoord> elems;
        toMark[i]->getElements(elems);
        std::sort(elems.begin(), elems.end());
        pxCoord ll, ur, center;
        ll.X = std::max(elems[0].X-75, 0);
        ur.X = std::min(elems.back().X+75, image.cols);
        std::sort(elems.begin(), elems.end(), [](const pxCoord &left, const pxCoord &right) {
                return (left.Y < right.Y) || (left.Y == right.Y && left.X < right.X);
            });
        ll.Y = std::max(elems[0].Y-75, 0);
        ur.Y = std::min(elems.back().Y+75, image.rows);
        center.X = (ll.X + ur.X)/2;
        center.Y = (ll.Y + ur.Y)/2;
        int radius = (std::max(std::abs(center.X - ll.X), std::abs(center.Y - ll.Y))+60)*120/100;
//        cv::circle(image, cv::Point(center.X, center.Y), radius, value, 2);
        cv::rectangle(image, cv::Point(ll.X, ll.Y), cv::Point(ur.X, ur.Y), value, 2);
//        outfilename << "output" << i << ".png";
//        cv::Mat imcrop = image(cv::Rect(cv::Point(ll.X, ll.Y), cv::Point(ur.X, ur.Y)));
//        cv::imwrite(outfilename.str().c_str(), imcrop);
//        outfilename.clear();
//        outfilename.str(std::string());
    }
}

/// \param image The image onto which to draw the regions. Must be an RGB
/// image.
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
