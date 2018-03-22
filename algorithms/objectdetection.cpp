/// \file structures/objectdetection.cpp
/// \author Petra Bosilj
/// \date 21/03/2018

#include "objectdetection.h"

#include "../structures/areaattribute.h"

#include <algorithm>
#include <set>

#define SIMILAR_SUCCESSIVE 1
#define SIMILAR_GROWTH_LIMITED 0

namespace fl{

    int countSimilarSuccessive(std::vector<std::pair<double, std::pair<int, Node *> > >::iterator Start,
                               std::vector<std::pair<double, std::pair<int, Node *> > >::iterator End,
                               double similarityTolerance = 0.15){
        double initialArea = Start->second.first;
        int cnt = 0;
        for (std::vector<std::pair<double, std::pair<int, Node *> > >::iterator It = Start; It != End; ++It, ++cnt)
            if (!(It->second.first > (1-similarityTolerance)*initialArea &&
                  It->second.first < (1+similarityTolerance)*initialArea))
                    break;
        return cnt;
    }

    int countSimilarGrowthLimited(std::vector<std::pair<double, std::pair<int, Node *> > >::iterator Start,
                                  std::vector<std::pair<double, std::pair<int, Node *> > >::iterator End,
                                  double similarityTolerance = 0.15, double growthTolerance = 0.5){
        int initialArea = Start->second.first;
        double initialGrowth = Start->first;
        int cnt = 0;
        for (std::vector<std::pair<double, std::pair<int, Node *> > >::iterator It = Start; It != End && It->first > growthTolerance*initialGrowth; ++It){
            cnt+= (It->second.first > (1-similarityTolerance)*initialArea &&
                   It->second.first < (1+similarityTolerance)*initialArea);

        }
        return cnt;
    }

    void treeObjectDetection(const ImageTree &tree,
                            std::vector <Node *> &selectedObjects, std::vector <Node *> *associatedLeaves){

        int extinctionThreshold = 10;
        int delta = 30;
        int minArea = 150;
        int maxArea = 4e5;
        int minGrowth = 10;
        int K = 15;

#if SIMILAR_SUCCESSIVE
        int minSimilarRegions = 6;
#elif SIMILAR_GROWTH_LIMITED
        int minSimilarRegions = 4;
#endif

        std::vector <std::pair <int, Node *> > leafExt;
        tree.getLeafExtinctions(leafExt);
        std::sort(leafExt.rbegin(), leafExt.rend());

        std::cout << "Number of leaves before extinction filtering: " << leafExt.size() << std::endl;

        for (std::vector<std::pair <int, Node *> >::iterator it = leafExt.begin(); it != leafExt.end(); ++it){
            if (it->first < extinctionThreshold){
                leafExt.erase(it, leafExt.end());
                break;
            }
        }
        std::cout << "Number of leaves to process: " << leafExt.size() << std::endl;

        selectedObjects.clear();
        if (associatedLeaves) associatedLeaves->clear();

        tree.addAttributeToTree<AreaAttribute>(new AreaSettings());
        std::set <Node *> toggleVisit; // mark visited nodes along branches to reduce double-processing
        for (int i=0, szi = (int)leafExt.size(); i < szi; ++i){
            Node *cur, *par;

            // populate the allGrowth vector with all values in this branch
            std::vector<std::pair<double, std::pair<int, Node *> > > allGrowth; // (growth, (area, node))
            for (cur = par = leafExt[i].second; !cur->isRoot(); cur = cur->parent()){
                for (; !par->parent()->isRoot() && std::abs(cur->level() - par->parent()->level()) <= delta; par = par->parent()); // find the ancestor
                int parentArea = ((fl::AreaAttribute*)par->getAttribute(AreaAttribute::name))->value();
                int selfArea = ((fl::AreaAttribute*)cur->getAttribute(AreaAttribute::name))->value();
                if (selfArea > (int)(maxArea*1.15)) // examine a bit above maxArea if you need for later
                    break;
                double growth = (double)(parentArea - selfArea) / selfArea;
                allGrowth.emplace_back(std::make_pair(growth, std::make_pair(selfArea, cur)));
            }
            std::sort(allGrowth.rbegin(), allGrowth.rend());

            // look through all candidates; with a minimum allowed growth; stop if you find previously processed case.
            int bestIndex = 0;
            for (int _i=1, sz_i = (int)allGrowth.size(); _i < sz_i && allGrowth[_i].first > minGrowth &&
                                                         toggleVisit.find(allGrowth[bestIndex].second.second) == toggleVisit.end(); ++_i){
                if (allGrowth[_i].second.first > maxArea)
                    continue;

                if (allGrowth[_i].second.first > K*allGrowth[bestIndex].second.first) { // adjust selection. if the size if more than K*best; consider for adjustment

                    // count the number of regions with similar growth factors and similar area:
#if SIMILAR_SUCCESSIVE
                    int cnt = countSimilarSuccessive(allGrowth.begin()+_i, allGrowth.end());
#elif SIMILAR_GROWTH_LIMITED
                    int cnt = countSimilarGrowthLimited(allGrowth.begin()+_i, allGrowth.end());
#endif
                    // if there is enough similar regions, can replace:
                    if (cnt >= minSimilarRegions){
                        toggleVisit.emplace(allGrowth[bestIndex].second.second);
                        bestIndex = _i;
                    }
                }
            }

            // if regions exist, the one found has sufficient area and was not output before:
            if (!allGrowth.empty() && allGrowth[bestIndex].second.first > minArea &&
                toggleVisit.find(allGrowth[bestIndex].second.second) == toggleVisit.end()){
                selectedObjects.push_back(allGrowth[bestIndex].second.second); // output the selection
                if (associatedLeaves != NULL)
                    associatedLeaves->push_back(leafExt[i].second); // output the source leaf if requested
                toggleVisit.emplace(allGrowth[bestIndex].second.second);
            }
        }

        tree.deleteAttributeFromTree<fl::AreaAttribute>();
    }
}
