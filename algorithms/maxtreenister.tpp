/// \file algorithms/maxtreenister.tpp
/// \author Petra Bosilj

#ifndef TPP_MAXTREENISTER
#define TPP_MAXTREENISTER

#include "maxtreenister.h"
#include "../misc/pixels.h"
#include "../misc/commontreedetail.h"

#include <stack>
#include <queue>
#include <vector>
#include <algorithm>

#include <opencv2/highgui/highgui.hpp>

namespace fl{

    namespace detail{
        template <typename Compare>
        class pqcomparison{
            private:
            Compare myComp;
            public:
            pqcomparison(Compare &cmp){
                myComp = cmp;
            }
            bool operator() (const std::pair<double, pxDirected> &lhs, const std::pair<double,pxDirected> &rhs){
                return myComp(rhs.first, lhs.first);
            }
        };
    }

    /// \details \copydetails maxTreeNister(const cv::Mat &img)
    ///
    /// \param pxOrder the comparison operator used to determine the order of the pixels.
    template <typename Compare>
    Node *maxTreeNister(const cv::Mat &img, Compare pxOrder){
        return maxTreeNister(img, pxOrder, pxType::regular);
    }

    /// \details \copydetails fl::maxTreeNister(const cv::Mat &img, Compare pxOrder)
    ///
    /// \param curType the `pxType` type of the current pixel (fl::pxType::regular, fl::pxType::dual).
    template <typename Compare>
    Node *maxTreeNister(const cv::Mat &img, Compare pxOrder, pxType curType){

        std::vector <std::vector <char> > accessible(img.cols, std::vector<char> (img.rows, false));

        std::priority_queue<std::pair<double, pxDirected>,
                            std::vector<std::pair<double, pxDirected> >,
                            detail::pqcomparison<Compare> > boundary((detail::pqcomparison<Compare>(pxOrder)));

        std::stack<InclusionNode *> components;

        double dummyElem = detail::getCvMatMax(img, pxOrder);

//        FIXME -- assuming no overflow
        if (pxOrder(dummyElem, dummyElem+1))
            dummyElem=dummyElem+1;
        else
            dummyElem=dummyElem-1;

        components.push(&InclusionNode::dummy(dummyElem));

        double currentLevel;
        pxDirected current = make_pxDirected(make_pxCoord(0,1), 0, curType);
        accessible[current.coord.X][current.coord.Y] = true;
        for (bool setPass = true;;){
            if (setPass){ // set up in another step
                setPass = false;
            }
            else{ // pop the heap of boundary pixels
                current = boundary.top().second;
                boundary.pop();
            }
            currentLevel = detail::getCvMatElem(img, current.coord.X, current.coord.Y);
            if (current.pxdir < 1){
                InclusionNode *ctop = components.top();
                if (ctop->level() == currentLevel)
                    ctop->addElement(current.coord);
                else{
                    InclusionNode *tmp = new InclusionNode(std::vector<pxCoord>(1, current.coord));
                    tmp->assignLevel(currentLevel);
                    components.push(tmp);
                }
            }

            for (; nextDir(current); ){
                pxCoord nextPx;
                for (nextPx = nextCoord(current); !coordOk(nextPx, img.cols, img.rows); nextPx = nextCoord(current));
                if (!accessible[nextPx.X][nextPx.Y]){
                    accessible[nextPx.X][nextPx.Y] = true;
                    double nextLevel = detail::getCvMatElem(img, nextPx.X, nextPx.Y);
                    if (!pxOrder(nextLevel, currentLevel) ){
                        boundary.push(std::make_pair(nextLevel, make_pxDirected(nextPx,0,curType)));
                    }
                    else{
                        // next pixel in order
                        boundary.push(std::make_pair(currentLevel, current));
                        current = make_pxDirected(nextPx,0,curType);
                        setPass = true;
                        break;
                    }
                }
            }

            if (setPass)
                continue;
            if (boundary.empty())
                break;

            //if next pixel gray level is at hihger gray level than current
            double nLvl = detail::getCvMatElem(img, boundary.top().second.coord.X, boundary.top().second.coord.Y);

            if (pxOrder(currentLevel, nLvl)){
                for (;;){
                    InclusionNode *stackTop = components.top();
                    components.pop();
                    InclusionNode *ctop = components.top();
                    if (pxOrder(nLvl, ctop->level())){
                        InclusionNode *newNode = new InclusionNode(std::vector<pxCoord>(1, boundary.top().second.coord), std::vector <InclusionNode *> (1, stackTop));
                        newNode->assignLevel(nLvl);
                        components.push(newNode);

                        std::pair<double, pxDirected> tmp = boundary.top();
                        boundary.pop();
                        tmp.second.pxdir = 1;
                        boundary.push(tmp);
                        break;
                    }
                    else{
                        ctop->addChild(stackTop);
                        if (ctop->level() == nLvl)
                            break;
                    }
                }
            }
        }

        components.top()->setParent(NULL);

        Node  *returnValue =  components.top()->assignGrayLevelRec(detail::maxTreeGrayLvlAssign(img));

        return returnValue;
    }
}

#endif

