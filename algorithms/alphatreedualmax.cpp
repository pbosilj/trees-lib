#include "alphatreedualmax.h"
#include "../structures/node.h"

#include "../structures/inclusionnode.h"
#include "../structures/partitioningnode.h"

#include "../algorithms/maxtreenister.h"
#include "../algorithms/maxtreeberger.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <utility>
#include <typeinfo>

#include <iostream>
#include <cstdio>

namespace fl{
    Node *constructRecursively(Node *root, std::vector <std::vector <char> > &seen){
        PartitioningNode *rv;
        //std::cout << "in here   " << std::endl;
        if (root->level() == 0){
//            std::cout << "bla?" << std::endl;
            const std::vector < pxCoord > &elems = root->getOwnElements();
//            if (elems.size() == 0)
//                std::cout << "in zero level no elements" << std::endl;
            std::vector < pxCoord > newElems;
            for (int i=0, szi = elems.size(); i < szi; ++i){
                if (elems[i].X % 2){ // vertical (odd, even)
                    if (!seen[(elems[i].X-1)/2][elems[i].Y/2]){
                        seen[(elems[i].X-1)/2][elems[i].Y/2] = true;
                        newElems.push_back(make_pxCoord((elems[i].X-1)/2,elems[i].Y/2));
                    }
                    if (!seen[(elems[i].X+1)/2][elems[i].Y/2]){
                        seen[(elems[i].X+1)/2][elems[i].Y/2] = true;
                        newElems.push_back(make_pxCoord((elems[i].X+1)/2,elems[i].Y/2));
                    }
                }
                else { // horizonal (even, odd)
                    if (!seen[elems[i].X/2][(elems[i].Y-1)/2]){
                        seen[elems[i].X/2][(elems[i].Y-1)/2] = true;
                        newElems.push_back(make_pxCoord(elems[i].X/2,(elems[i].Y-1)/2));
                    }
                    if (!seen[elems[i].X/2][(elems[i].Y+1)/2]){
                        seen[elems[i].X/2][(elems[i].Y+1)/2] = true;
                        newElems.push_back(make_pxCoord(elems[i].X/2,(elems[i].Y+1)/2));
                    }
                }
            }
//            std::cout << "new elems " << newElems.size() << std::endl;
            rv = new PartitioningNode(newElems);
        }
        else {
            std::vector <PartitioningNode *> children;
            for (int i=0, szi = root->_children.size(); i < szi; ++i){
                children.push_back(dynamic_cast<PartitioningNode *>(constructRecursively(root->_children[i], seen)));
            }
            const std::vector < pxCoord > &elems = root->getOwnElements();

//            if (children.empty() && elems.empty())
//                std::cout << "no children and no elements" << std::endl;
//
            for (int i=0, szi = elems.size(); i < szi; ++i){

                if (elems[i].X % 2){ // vertical
//                    std::cout << "vertical " << std::endl;
                    if (!seen[(elems[i].X-1)/2][elems[i].Y/2]){
//                        std::cout << "seen" << std::endl;
                        seen[(elems[i].X-1)/2][elems[i].Y/2] = true;
                        PartitioningNode *zeroKid = new PartitioningNode(std::vector < pxCoord> (1, make_pxCoord((elems[i].X-1)/2,elems[i].Y/2)));
                        zeroKid->assignLevel(0);
                        children.push_back(zeroKid);
                    }
                    if (!seen[(elems[i].X+1)/2][elems[i].Y/2]){
//                        std::cout << "seen" << std::endl;
                        seen[(elems[i].X+1)/2][elems[i].Y/2] = true;
                        PartitioningNode *zeroKid = new PartitioningNode(std::vector < pxCoord> (1, make_pxCoord((elems[i].X+1)/2,elems[i].Y/2)));
                        zeroKid->assignLevel(0);
                        children.push_back(zeroKid);
                    }
                }
                else { // horizonal
////                    std::cout << "horizontal " << std::endl;
//                    std::cout << elems[i].X/2 << " " << (elems[i].Y-1)/2 << " / " << (elems[i].Y+1)/2 << std::endl;
                    if (!seen[elems[i].X/2][(elems[i].Y-1)/2]){
//                        std::cout << "seen" << std::endl;
                        seen[elems[i].X/2][(elems[i].Y-1)/2] = true;
                        PartitioningNode *zeroKid = new PartitioningNode(std::vector < pxCoord> (1, make_pxCoord(elems[i].X/2,(elems[i].Y-1)/2)));
                        zeroKid->assignLevel(0);
                        children.push_back(zeroKid);
                    }
                    if (!seen[elems[i].X/2][(elems[i].Y+1)/2]){
//                        std::cout << "seen" << std::endl;
                        seen[elems[i].X/2][(elems[i].Y+1)/2] = true;
                        PartitioningNode *zeroKid = new PartitioningNode(std::vector < pxCoord> (1, make_pxCoord(elems[i].X/2,(elems[i].Y+1)/2)));
                        zeroKid->assignLevel(0);
                        children.push_back(zeroKid);
                    }
                }
            }
//            if (children.empty()){
//                std::cout << "somehow no children " << std::endl;
//                std::cout << root->children.size() << " " << elems.size() << std::endl;
//            }
            if (children.size() == 1){
                return children[0];
            }
            else{
                rv = new PartitioningNode(children);
            }
        }
        rv->assignLevel(root->level());
        return rv;
    }

    /// Uses the `maxTreeNister()` algorithm for the max-tree internally.
    /// \param img The image used to construct the alpha-tree.
    ///
    /// \return A `PartitioningNode *` (as `Node *) to the root of the alpha-tree.
    Node *alphaTreeDualMax(const cv::Mat &img){
        cv::Mat imgDual(img.rows*2-1, img.cols*2-1, CV_8U, detail::getCvMatMax(img));
        //cv::Mat imgDual(img.rows*2-1, img.cols*2-1, CV_8U, *std::max_element(img.begin<uchar>(), img.end<uchar>()));
        for(int row = 0, szrow = img.rows; row < szrow; ++row) {;
            const uchar* pU = img.ptr(row);
            uchar *horiNew = imgDual.ptr(row*2);
            ++horiNew;

            const uchar* pD = NULL;
            uchar *vertiNew = NULL;
            if (row != (szrow-1)){
                pD = img.ptr(row+1);
                vertiNew = imgDual.ptr(row*2+1);
            }
            for(int col = 0, szcol = img.cols; col < szcol; ++col) {
                uchar curVal = *pU;
                if (col != (szcol-1)){
                    uchar horizontal = std::abs(curVal - *++pU);
                    *horiNew = horizontal;
                }
                if (row != (szrow-1)){
                    uchar vertical = std::abs(curVal - *pD++);
                    *vertiNew = vertical;
                }
                horiNew += 2;
                vertiNew += 2;
            }
        }

        Node *dualRoot = maxTreeNister(imgDual, std::less<int>(), dual);
        std::vector <std::vector <char> > seen(img.cols, std::vector <char>(img.rows, false));
        Node *alphaRoot = constructRecursively(dualRoot, seen);
        ImageTree *dualTree = new ImageTree(dualRoot, std::make_pair(img.rows, img.cols)); // <- why this line?

        delete dualTree;

        //return alphaRoot->assignGrayLevelRec(detail::alphaTreeGrayLvlAssign(img));
        return alphaRoot->assignGrayLevelRec(detail::maxTreeGrayLvlAssign(img));
    }
}

