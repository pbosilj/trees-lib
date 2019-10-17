/// \file algorithms/msernister.cpp
/// \author Petra Bosilj

#include "msernister.h"

#include "../structures/imagetree.h"
#include "../structures/node.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <set>
#include <utility>

#include <cstdlib>
#include <cstdio>

#include <iostream>

#include "../misc/ellipse.h"

#define ORIGINAL 0
#define ONE_HIGH 1

typedef std::pair<int, bool> mserData;

#define diff first
#define ext second

#define emptyData(sz) (std::make_pair( (sz), true))

namespace fl{
    mserData areaDiff(Node *root, const int deltaLvl){
        Node *high = root;

        double cumulative = 0;
        for (; high->_parent != NULL; high = high->_parent){
            double thisStep = std::abs(high->level() - high->_parent->level());
            if ((cumulative + thisStep) > deltaLvl)
                break;
            cumulative += thisStep;
        }

        mserData rValue = emptyData(root->elementCount() + 1);

        rValue.diff = high->elementCount() - root->elementCount();
        return rValue;
    }

    mserData mserRecursive(Node *root, const int deltaLvl, std::vector <Node * > &mserOrder, std::vector <std::pair<double, int> > &div,
                           int minArea, double maxVariation){

        int myElems = root->elementCount();
        if (myElems < minArea){
            return areaDiff(root, deltaLvl);
        }

        mserData forRoot = areaDiff(root, deltaLvl);

        for (int i=0, szi = root->_children.size(); i < szi; ++i){

            int childElems = root->_children[i]->elementCount();
            mserData child = mserRecursive(root->_children[i], deltaLvl, mserOrder, div, minArea, maxVariation);

            if (std::abs(root->level()-root->_children[i]->level()) > 1){
                if (child.ext && childElems >= minArea && child.diff<maxVariation*childElems){
                    mserOrder.push_back(root->_children[i]);
                    div.push_back(std::make_pair(double(child.diff) / childElems, 0));
                }
            }

            else{
                if ((double)child.diff / childElems < (double)forRoot.diff / myElems){
                    forRoot.ext = false;
                    if (child.ext && childElems >= minArea && child.diff<maxVariation*childElems){
                        mserOrder.push_back(root->_children[i]);
                        div.push_back(std::make_pair(double(child.diff) / childElems, 0));
                    }
                }
            }
        }

        return forRoot;
    }

    void mserTrimDiversity(std::vector <Node *> &mserOrder, std::vector <std::pair<double, int> > &div, double minDiversity, int maxArea){

        std::set <Node *> mserSearch(mserOrder.begin(), mserOrder.end());

        for (int i=0, szi = mserOrder.size(); i < szi; ++i){

            if (mserOrder[i]->elementCount() > maxArea){
                mserSearch.erase(mserOrder[i]);
                continue;
            }
            if (mserSearch.find(mserOrder[i]) == mserSearch.end()){
                continue;
            }
            Node *parent;
            for (parent = mserOrder[i]->parent(); parent != NULL; parent = parent->parent()){
                if (mserSearch.find(parent) != mserSearch.end()){
                    break;
                }
            }
            if (parent == NULL){
                continue;
            }
            int mySize = mserOrder[i]->elementCount();
            int parentSize = parent->elementCount();
            if ((parentSize-mySize) < (minDiversity*parentSize)){
                mserSearch.erase(mserOrder[i]);
            }
        }

        std::vector <Node *> toCopy = mserOrder;
        std::vector <std::pair<double, int> > divs = div;

        div.clear();
        mserOrder.clear();
        //mserOrder.insert(mserOrder.begin(), mserSearch.begin(), mserSearch.end());
        for (int i=0, szi = toCopy.size(); i < szi; ++i){
            if (mserSearch.find(toCopy[i]) != mserSearch.end()){
                mserOrder.push_back(toCopy[i]);
                div.push_back(std::make_pair(divs[i].first, mserOrder.size()-1));
            }
        }
    }

    void markMserInTree(const ImageTree &tree, std::vector <Node *> &mser, std::vector <std::pair<double, int> > &div, const mserParams &params){
        if (params.perc)
            markMserInTree(tree, params.delta, mser, div, params.maxAreaPerc, params.minArea, params.maxVariation, params.minDiversity);
        else
            markMserInTree(tree, params.delta, mser, div, params.maxAreaPix, params.minArea, params.maxVariation, params.minDiversity);

    }

    void markMserInTree(const ImageTree &tree, int deltaLvl, std::vector <Node *> &mser, std::vector <std::pair<double, int> > &div,
                        double maxArea, int minArea, double maxVariation, double minDiversity){
        if (maxArea <= 0.00)
            markMserInTree(tree,deltaLvl,mser,div,-1,minArea,maxVariation,minDiversity);

        markMserInTree(tree,deltaLvl,mser,div,(int)((double)tree.treeHeight()*tree.treeWidth()*maxArea),minArea,maxVariation,minDiversity);

        return;
    }

    void markMserInTree(const ImageTree &tree, int deltaLvl, std::vector <Node *> &mser, std::vector <std::pair<double, int> > &div,
                        int maxArea, int minArea, double maxVariation, double minDiversity){
        mser.clear();
        div.clear();

        if (maxArea <= 0)
            maxArea = tree.treeHeight()*tree.treeWidth();

        mserRecursive(tree._root, deltaLvl, mser, div, minArea, maxVariation);
        mserTrimDiversity(mser, div, minDiversity, maxArea);

        return;
    }

    void displayMser(const cv::Mat &img, const std::vector <Node *> &mser){

        const cv::Vec3b bcolors[] =
            {
                cv::Vec3b(0,0,255),
                cv::Vec3b(0,128,255),
                cv::Vec3b(0,255,255),
                cv::Vec3b(0,255,0),
                cv::Vec3b(255,128,0),
                cv::Vec3b(255,255,0),
                cv::Vec3b(255,0,0),
                cv::Vec3b(255,0,255),
                cv::Vec3b(255,255,255),
                cv::Vec3b(128,0,255),
                cv::Vec3b(128,255,0),
                cv::Vec3b(0,255,128)
            };

        cv::Mat newImg;
        img.copyTo(newImg);
        cv::cvtColor(img, newImg, CV_GRAY2RGB);
        std::vector <cv::Point> component;

        std::vector <std::pair <int, int> > px;
        int displayed = 0;
        for (int i= mser.size() -1 ; i >= 0; --i){
            px.clear();

            mser[i]->getElements(px);
            if (px.size() >= 5){
                ++displayed;


                component.clear();
                for (int j=0, szj = px.size(); j < szj; ++j){
                    newImg.at<cv::Vec3b>(px[j].second, px[j].first) = bcolors[i % 12];
                    //component.push_back(cv::Point(px[j].second, px[j].first));
                }
                //cv::RotatedRect box = cv::fitEllipse( component );
                //cv::ellipse( newImg, box, cv::Scalar(196,255,255), 2 );
                // mser[i]->parent()->elementCount() << " " <<
                //std::cout <<  px.size() << std::endl;
            //}
            }
        }
        cv::namedWindow("bub", 2);
        cv::imshow("bub", newImg);
        cv::imwrite("/home/pbosilj/Programming/Trees/marked.png", newImg);
        cv::waitKey(0);
        std::cout << "msers displayed " << displayed << std::endl;
    }

    void fillMserPoints(std::vector<std::vector<cv::Point> > &points, const std::vector <Node *> &mser){
        std::vector <std::pair <int, int> > px;
        for (int i=0, szi = mser.size(); i < szi; ++i){
            px.clear();
            mser[i]->getElements(px);

            points.push_back(std::vector<cv::Point>());
            for (int j=0, szj = px.size(); j < szj; ++j)
                points.back().push_back(cv::Point(px[j].first, px[j].second));
        }
    }
}
