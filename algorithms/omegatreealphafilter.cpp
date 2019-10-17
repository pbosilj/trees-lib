/// \file algorithms/omegatreealphafilter.cpp
/// \author Petra Bosilj

#include "omegatreealphafilter.h"

#include "alphatreedualmax.h"

#include "predicate.h"

#include "../structures/node.h"
#include "../structures/partitioningnode.h"

#include "../structures/attribute.h"
#include "../structures/rangeattribute.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <cstdio>


namespace fl{


/*
    void filterSubtreeByRange(Node *subRoot){
        int myRange = ((fl::RangeAttribute *)subRoot->getAttribute(fl::RangeAttribute::name))->value();
        bool deletion;
        std::vector<Node *> &chi = subRoot->children;
        do{
            deletion = false;
            for (int i=0; i < (int)chi.size(); ++i){
                int childRange = ((fl::RangeAttribute *)chi[i]->getAttribute(fl::RangeAttribute::name))->value();
                if (childRange == myRange){
                    subRoot->deleteChild(i);
                    deletion = true;
                }
            }

        }while(deletion == true);

        for (int i=0, szi = subRoot->children.size(); i < szi; ++i)
            filterSubtreeByRange(chi[i]);
    }

*/
/*
    void assignRangeAsAttribute(Node *root){
        root->assignAttribute(((fl::RangeAttribute *)root->getAttribute(fl::RangeAttribute::name))->value());

        std::vector <Node *> &children = root->children;

        for (int i=0, szi = children.size(); i < szi; ++i)
            assignRangeAsAttribute(children[i]);

        return;
    }
*/

    Node *omegaTreeAlphaFilter(const cv::Mat &img){
        Node *alphaRoot = alphaTreeDualMax(img);

        ImageTree *alphaTreeTmp = new ImageTree(alphaRoot,std::make_pair(img.rows, img.cols));

        alphaTreeTmp->setImage(img);
        alphaTreeTmp->addAttributeToTree<RangeAttribute>(new RangeSettings);
        //alphaTreeTmp->filterTreeByAttributePredicate<RangeAttribute>(DifferentThanParent<double>());
        alphaTreeTmp->filterTreeByAttribute<RangeAttribute>();
        alphaTreeTmp->assignAttributeAsLevel<RangeAttribute>();
        alphaTreeTmp->deleteAttributeFromTree<RangeAttribute>();
        alphaTreeTmp->unsetImage();
        alphaTreeTmp->disableDelNodesOnDestruct();

        delete alphaTreeTmp;

        // alphaRoot->assignGrayLevelRec(detail::alphaTreeGrayLvlAssign(img)); // this one I don't need
        alphaRoot->assignGrayLevelRec(detail::maxTreeGrayLvlAssign(img));  // I do -- especially if it's level. levels get re-assigned

        return alphaRoot;
    }

/*
    namespace old{

        void filterSubtreeByRange(Node *subRoot, const cv::Mat &img){
            int myRange = subRoot->elementRange(img);
            bool deletion;
            do{
                deletion = false;

                for (int i=0; i < (int)subRoot->children.size(); ++i){
                    int childRange = subRoot->children[i]->elementRange(img);
                    if (childRange == myRange){
                        subRoot->deleteChild(i);
                        deletion = true;
                    }
                }

            }while(deletion == true);

            for (int i=0, szi = subRoot->children.size(); i < szi; ++i)
                filterSubtreeByRange(subRoot->children[i], img);
        }



        void assignRangeAsAttribute(Node *root, const cv::Mat &img){
            root->assignAttribute(root->elementRange(img));

            std::vector <Node *> &children = root->getChildren();

            for (int i=0, szi = children.size(); i < szi; ++i)
                assignRangeAsAttribute(children[i], img);

            return;
        }

        Node *omegaTreeAlphaFilter(const cv::Mat &img){
            Node *alphaRoot = alphaTreeDualMax(img);
            filterSubtreeByRange(alphaRoot, img);
            assignRangeAsAttribute(alphaRoot, img);
            return alphaRoot;
        }

    }
*/
}
