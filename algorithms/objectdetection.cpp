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


    /// Performs object detection on an `ImageTree` by trying to find the
    /// regions in tree branches just before the branch exhibits a sudden
    /// growth in area. Various tolerances can be adjusted.
    ///
    /// Bosilj, Cielniak, Duckett, 2018, "Connected attribute morphology for
    /// unified vegetation segmentation and classification in precision
    /// agriculture"
    ///
    /// \param `ImageTree` representing the image on which to perform object
    /// detection
    /// \param selectedObjects Output vector filled with selected `Node`s.
    /// \param associatedLeaves An optional output, the leaf `Node`s associated
    /// to the selected `Node`s, in order.
    ///
    /// \param minArea Optional parameter. Only regions with size larger than
    /// `minArea` pixels will be selected.
    /// \param maxArea Optional parameter. Only regions with size smaller than
    /// `maxArea` pixels will be selected.
    /// \param delta Optional parameter. Stability parameter for measuring
    /// the growth rate. Larger `delta` means the growth rate will be calculated
    /// for a larger range of gray levels.
    /// \param extinctionThreshold Optional parameter. Only branches starting in
    /// local extrema with exctinction values larger then `extinctionThreshold`
    /// will be considered for object detection.
    /// \param minGrowth Optional parameter. Only regions with growth in area
    /// larger than `minGrowth` will be considered.
    /// \param K Optional parameter. If several regions exhibit strong "sudden
    /// growth", larger regions with smaller growth will be considered over
    /// smaller regions if their size is `K` or more times larger than that of
    /// smaller regions originally selected.
    void treeObjectDetection(const ImageTree &tree,
                            std::vector <Node *> &selectedObjects, std::vector <Node *> *associatedLeaves,
                            int delta, int minArea, int maxArea, int extinctionThreshold, int minGrowth, int K){

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

        std::cout << "Selecting nodes" << std::endl;

        tree.addAttributeToTree<AreaAttribute>(new AreaSettings());
        std::set <Node *> visited; // mark visited nodes along branches to reduce double-processing
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
                                                         visited.find(allGrowth[bestIndex].second.second) == visited.end(); ++_i){
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
                        visited.emplace(allGrowth[bestIndex].second.second);
                        bestIndex = _i;
                    }
                }
            }

            // if regions exist, the one found has sufficient area and was not output before:
            if (!allGrowth.empty() && allGrowth[bestIndex].second.first > minArea &&
                visited.find(allGrowth[bestIndex].second.second) == visited.end()){
                selectedObjects.push_back(allGrowth[bestIndex].second.second); // output the selection
                if (associatedLeaves != NULL)
                    associatedLeaves->push_back(leafExt[i].second); // output the source leaf if requested
                visited.emplace(allGrowth[bestIndex].second.second);
            }
        }

        tree.deleteAttributeFromTree<fl::AreaAttribute>();
    }
}
