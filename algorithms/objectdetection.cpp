/// \file structures/objectdetection.cpp
/// \author Petra Bosilj
/// \date 21/03/2018

#include "objectdetection.h"

#include "../structures/areaattribute.h"

#include <algorithm>
#include <set>

namespace fl{
    void treeObjectDetection(const ImageTree &tree,
                            std::vector <Node *> &selectedObjects, std::vector <Node *> *associatedLeaves){

        int extinctionThreshold = 10;
        int delta = 30;
        int minArea = 150;
        int maxArea = 4e5;
        int minGrowth = 10;
        int K = 15;
        double similarityTolerance = 0.15;
        int minSimilarRegions = 6;

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
            std::vector<std::pair<double, std::pair<double, Node *> > > allGrowth; // (growth, (area, node))
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
                    int cnt = 0;
                    // count the number of regions with similar growth factors and similar area:
                    for (; _i + cnt < (int)allGrowth.size() &&
                        allGrowth[_i+cnt].second.first > (1-similarityTolerance)*allGrowth[_i].second.first &&
                        allGrowth[_i+cnt].second.first < (1+similarityTolerance)*allGrowth[_i].second.first;
                         ++cnt){}

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

    void treeObjectDetectionAlternative(const ImageTree &tree,
                            std::vector <Node *> &selectedObjects, std::vector <Node *> *associatedLeaves){
        int extinctionThreshold = 10;
        int delta = 30;
        int minArea = 150;
        int maxArea = 4e5;
        int minGrowth = 10;
        int K = 15;
        double similarityTolerance = 0.15;
        double growthTolerance = 0.5;
        int minSimilarRegions = 4;

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
            std::vector<std::pair<double, std::pair<double, Node *> > > allGrowth; // (growth, (area, node))
            for (cur = par = leafExt[i].second; !cur->isRoot(); cur = cur->parent()){
                for (; !par->parent()->isRoot() && std::abs(cur->level() - par->parent()->level()) <= delta; par = par->parent());
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

                if (allGrowth[_i].second.first > K*allGrowth[bestIndex].second.first) {
                    int cnt = 0;
                     for (int _j = 0;
                              _i + _j < (int)allGrowth.size() &&
                              allGrowth[_i+_j].first > growthTolerance*allGrowth[_i].first; // until the growth of the next node is at least half of the current best growth
                              cnt += (allGrowth[_i+_j].second.first > (1-similarityTolerance)*allGrowth[_i].second.first && // see and count if region similar enough (regarding to size)
                                      allGrowth[_i+_j].second.first < (1+similarityTolerance)*allGrowth[_i].second.first),
                         ++_j){
                         }
                    // if there is enough similar regions, can replace
                    if (cnt >= minSimilarRegions){
                        toggleVisit.emplace(allGrowth[bestIndex].second.second);
                        bestIndex = _i;
                        _i += cnt-1;
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
