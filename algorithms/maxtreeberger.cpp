/// \file algorithms/maxtreeberger.cpp
/// \author Petra Bosilj

#include "maxtreeberger.h"

#include "../misc/pixels.h"

#include "../structures/node.h"

#include <vector>
#include <utility>

namespace fl{
    /// \details \copydetails fl::maxTreeNister()
    Node *maxTreeBerger(const cv::Mat &img){
        return maxTreeBerger(img, std::less<int>(), fl::pxType::regular);
    }

    namespace detail{
        pxCoord imgIndexGenerator::operator()() {
            if (this->hits >= (img.cols * img.rows))
                return this->countToCoord(-1);
            return this->countToCoord(this->hits++);
        }

        pxCoord imgIndexGenerator::countToCoord(int count){
            return make_pxCoord((count % img.cols), (count / img.cols));
        }

        pxCoord findRoot(const pxCoord &px, std::vector<std::vector<pxCoord> > &zpar){
            if (zpar[px.X][px.Y] != px)
                zpar[px.X][px.Y] = findRoot(zpar[px.X][px.Y], zpar);
            return zpar[px.X][px.Y];
        }

        /// TODO check if the non-recursive re-implementation works
        //pxCoord findRoot(const pxCoord &px, std::vector<std::vector<pxCoord> > &zpar){
        //    std::stack<std::pair<pxCoord, bool > > coordStack;
        //    coordStack.emplace(px, false);
        //    //for (const std::pair<pxCoord, pxCoord> &cur = coordStack.back(); !coordStack.empty(); coordStack.pop() ){
        //    do{
        //        const std::pair<pxCoord, bool> &cur = coordStack.top();
        //        if (cur.second == false){
        //            coordStack.top().second = true;
        //            if (zpar[cur.first.X][cur.first.Y] != cur.first){
        //                coordStack.emplace(zpar[cur.first.X][cur.first.Y], false);
        //            }
        //        }
        //        else{
        //            pxCoord par = zpar[cur.first.X][cur.first.Y];
        //            coordStack.pop();
        //            if (!coordStack.empty())
        //                zpar[coordStack.top().first.X][coordStack.top().first.Y] = par;
        //        }
        //    }while(!coordStack.empty());
        //
        //    return zpar[px.X][px.Y];
        //}

        void maxTreeCore(const std::vector<pxCoord> &sorted, pxType curType, std::vector<std::vector<pxCoord> > &parent){
            std::vector <std::vector<pxCoord> > zpar(parent.size(), std::vector<pxCoord>(parent.back().size(), make_pxCoord(-1,-1)));
            //for (int i=0, szi = sorted.size(); i < szi; ++i){
            for (int i=sorted.size()-1; i >=0; --i){
                const pxCoord &cur = sorted[i];
                parent[cur.X][cur.Y] = zpar[cur.X][cur.Y] = cur;
                pxDirected curDir = make_pxDirected(cur, 1, curType);
                for (; nextDir(curDir); ){
                    pxCoord n = nextCoord(curDir);
                    if (!coordOk(n, parent.size(), parent.back().size()) || zpar[n.X][n.Y] == make_pxCoord(-1,-1))
                        continue;
                    pxCoord root = findRoot(n, zpar);
                    if (root != cur){
                        parent[root.X][root.Y] = zpar[root.X][root.Y] = cur;
                    }
                }
            }
        }

        void canonizeTree(const std::vector<pxCoord> &sorted, std::vector<std::vector<pxCoord> > &parent, const cv::Mat &img){
            //for (int i=sorted.size()-1; i >=0; --i){
            for (int i=0, szi = sorted.size(); i < szi; ++i){
                const pxCoord &p = sorted[i];
                const pxCoord &q = parent[p.X][p.Y];
                if (detail::getCvMatElem(img, parent[q.X][q.Y]) == detail::getCvMatElem(img, q))
                    parent[p.X][p.Y] = parent[q.X][q.Y];
            }
        }

        void assignNewNode(std::vector <Node *> &nodes, std::vector<std::vector<int> > &nodeIndices,
                           const pxCoord &coord, const int &value){
            nodes.emplace_back(new InclusionNode(std::vector<pxCoord> (1, coord)));
            nodes.back()->assignLevel(value);
            nodeIndices[coord.X][coord.Y] = nodes.size()-1;
        }

        Node* makeNodeTree(const std::vector<std::vector<pxCoord> > &parent, const cv::Mat &img, const cv::Mat &mask){
            int counter = 0;
            std::vector <Node *> nodes;
            int rootIndex = -1;

            std::vector <std::vector <int> > nodeIndices(img.cols, std::vector<int>(img.rows, -1));
            for(cv::MatConstIterator it = detail::getCvMatBegin(img), end = detail::getCvMatEnd(img); it != end; ++it, ++counter){
                const pxCoord &curCoord = make_pxCoord(counter % img.cols, counter / img.cols);
                const int &curValue = detail::getDerefCvMatConstIterator(it, img.type());
                if (curValue < -9000 || (!mask.empty() && detail::getCvMatElem(mask, curCoord.X, curCoord.Y) == 0))
                    continue;
                const pxCoord &parCoord = parent[curCoord.X][curCoord.Y];
                const int &parValue = detail::getCvMatElem(img, parCoord);
                if (nodeIndices[parCoord.X][parCoord.Y] == -1)
                    assignNewNode(nodes, nodeIndices, parCoord, parValue);
                if (curValue == parValue){
                    if (curCoord != parCoord) // normal node->parent
                        nodes[nodeIndices[parCoord.X][parCoord.Y]]->addElement(curCoord);
                    else // if this is root of the tree!!
                        rootIndex = nodeIndices[parCoord.X][parCoord.Y]; // = nodeIndices[curCoord.X][curCoord.Y]
                }
                else{ // if curValue != parValue --> I am pivot!
                    if (nodeIndices[curCoord.X][curCoord.Y] == -1)
                        assignNewNode(nodes, nodeIndices, curCoord, curValue);
                    nodes[nodeIndices[parCoord.X][parCoord.Y]]->addChild(nodes[nodeIndices[curCoord.X][curCoord.Y]]);
                }
            }
            return nodes[rootIndex];
        }
    }
}
