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
        return maxTreeBerger(img, std::less<int>(), regular);
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
                if (img.at<uchar>(parent[q.X][q.Y].Y, parent[q.X][q.Y].X) == img.at<uchar>(q.Y, q.X))
                    parent[p.X][p.Y] = parent[q.X][q.Y];
            }

        }

        void assignNewNode(std::vector <Node *> &nodes, std::vector<std::vector<int> > &nodeIndices,
                           const pxCoord &coord, const int &value){
            nodes.push_back(new InclusionNode(std::vector<pxCoord> (1, coord)));
            nodes.back()->assignLevel(value);
            nodeIndices[coord.X][coord.Y] = nodes.size()-1;
        }

        Node* makeNodeTree(const std::vector<std::vector<pxCoord> > &parent, const cv::Mat &img){
            //cv::MatIterator_<uchar> it, end;
            int counter = 0;
            std::vector <Node *> nodes;
            int rootIndex = -1;
            std::vector <std::vector <int> > nodeIndices(img.cols, std::vector<int>(img.rows, -1));
            for(cv::MatConstIterator_<uchar> it = img.begin<uchar>(), end = img.end<uchar>(); it != end; ++it, ++counter){
                const pxCoord &curCoord = make_pxCoord(counter % img.cols, counter / img.cols);
                const pxCoord &parCoord = parent[curCoord.X][curCoord.Y];
                const int &curValue = (int)*it;
                const int &parValue = (int)img.at<uchar>(parCoord.Y, parCoord.X);
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
