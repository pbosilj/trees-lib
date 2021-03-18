/// \file structures/soilpatternspectra.cpp
/// \author Petra Bosilj
/// \date 07/10/2019

#include "soilpatternspectra.h"

#include "../misc/misc.h"
#include "../algorithms/treeconstruction.h"

#include <fstream>

/// \brief Determine the output path for the granulometry based on
/// the original image path, the filtering rule and the tree type used.
///
/// \param imagePath The file path to the input image used in granulometry
/// calculation.
/// \param rule The filtering rule used in the granulometry calculation.
/// The rule values are ('count' = 0, 'sum' = 1, 'volume' = 2)
/// \param tt The tree type used in the granulometry calculation.
std::string outputFilePath(const std::string& imagePath, const int rule, const fl::treeType tt){
    std::string outputPath = removeExtension(imagePath) + "_GCF_";
    switch (rule){
        case 0: outputPath += "count_"; break;
        case 1: outputPath += "sum_"; break;
        case 2: outputPath += "volume_"; break;
        default: break;
    }
    switch (tt){
        case fl::treeType::minTree: outputPath += "min.txt"; break;
        case fl::treeType::maxTree: outputPath += "max.txt"; break;
        case fl::treeType::treeOfShapes: outputPath += "tos.txt"; break;
        case fl::treeType::alphaTree: outputPath += "alpha.txt"; break;
        case fl::treeType::omegaTree: outputPath += "omega.txt"; break;
        case fl::treeType::minMax: outputPath += "minmax.txt"; break;
        default: break;
    }
    return outputPath;
}

/// \brief Function to use for calculation of global (area) pattern spectra of a single image.
///
/// Usage: call as the only function from `main`, passing the input arguments from `main`.
///
/// input arguments are positional:
///     1 - path to image
///     2 - tree option: "min, max, tos, alpha, omega, minmax, all"
///     3 - filtering rule: "count, sum, volume, all", count = number of regions, sum = number of pixels (area), volume = number of pixels * contrast
void rTestSoil(int argc, char **argv){
    if (argc < 4){
        std::cerr << "Call with three arguments: granulometry [image_path] [tree_option:min, max, alpha, omega, minmax, all] [filtering_rule: count, summ, volume, all]." << std::endl;
        exit(1);
    }

    if (!fileExists(std::string(argv[1]))){
        std::cerr << "Please provide a correct [image_path]." << std::endl;
        exit(1);
    }
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE); // error catching for image input?


    std::vector <fl::treeType> treeTypes;
    char tt_c[50]; sscanf(argv[2], "%s", tt_c);
    std::string tt(tt_c);
    if (tt == "min")
        treeTypes.push_back(fl::treeType::minTree);
    else if (tt == "max")
        treeTypes.push_back(fl::treeType::maxTree);
    else if (tt == "tos")
        treeTypes.push_back(fl::treeType::treeOfShapes);
    else if (tt == "alpha")
        treeTypes.push_back(fl::treeType::alphaTree);
    else if (tt == "omega")
        treeTypes.push_back(fl::treeType::omegaTree);
    else if (tt == "minmax")
        treeTypes.push_back(fl::treeType::minMax);
    else if (tt == "all"){
        treeTypes.push_back(fl::treeType::minTree);
        treeTypes.push_back(fl::treeType::maxTree);
        treeTypes.push_back(fl::treeType::treeOfShapes);
        treeTypes.push_back(fl::treeType::alphaTree);
        treeTypes.push_back(fl::treeType::omegaTree);
        treeTypes.push_back(fl::treeType::minMax);
    }
    else{
        std::cerr << "The second argument [tree_option] has to be one of the following: [tree_option:min, max, alpha, omega, minmax, all]." << std::endl;
        exit(1);
    }

    std::vector <int> rules;
    char rule_c[50]; sscanf(argv[3], "%s", rule_c);
    std::string rule(rule_c);

    if (rule == "count")
        rules.push_back(0);
    else if (rule == "sum")
        rules.push_back(1);
    else if (rule == "volume")
        rules.push_back(2);
    else if (rule == "all"){
        rules.push_back(0);
        rules.push_back(1);
        rules.push_back(2);
    }
    else{
        std::cerr << "The third argument [filtering_rule] has to be one of the following: [filtering_rule: count, sum, volume, all]." << std::endl;
        exit(1);
    }

    for (int i=0, szi = treeTypes.size(); i < szi; ++i){
        std::vector<std::map<double, int> > histograms;
        outputGranulometryCurve(image, treeTypes[i], rules, histograms);
        for (int j=0, szj = rules.size(); j < szj; ++j){
            std::string outputPath = outputFilePath(std::string(argv[1]), rules[j], treeTypes[i]);

            std::ofstream ofs(outputPath, std::ofstream::out);

            std::cout << "Producing [" << outputPath << "]"<< std::endl;

            for(auto elem : histograms[j]){
                ofs << elem.first << " " << elem.second << std::endl;
            }
            ofs << std::endl;
        }
    }

}


/// Output a granulometric curve for a given image, using a
/// certain tree type and for a number of filtering rules.
/// \param image Input image
/// \param t Type of component tree to use
/// \param rule A list of filtering rules used to calculate the granulometries.
/// \param hist An output parameter in which the granulometries are returned.
void outputGranulometryCurve(const cv::Mat &image, fl::treeType t, const std::vector<int> &rule, std::vector <std::map<double, int> > &hist){

    fl::ImageTree *mainTree;
    fl::ImageTree *secondaryTree;
    if (t == fl::treeType::minMax){
        mainTree = fl::createTree(fl::treeType::minTree, image);
        secondaryTree = fl::createTree(fl::treeType::maxTree, image);
    }
    else{
        mainTree = createTree(t, image);
    }

    for (int i=0, szi = rule.size(); i < szi; ++i){
        if ((int)hist.size() <= i)
            hist.push_back(std::map<double, int>());
        //std::map<double, int> mainHist;

        calculateGranulometry(mainTree, hist[i], rule[i]);

        if (t == fl::treeType::minMax){
            std::map<double, int> secondaryHist;
            calculateGranulometry(secondaryTree, secondaryHist, rule[i]);

            hist[i] = std::accumulate( secondaryHist.begin(), secondaryHist.end(), hist[i],
                []( std::map<double, int> &m, const std::pair<const double, int> &p )
                {
                    return ( m[p.first] +=p.second, m );
                } );

        }
    }
}

/// Calculates an image granulometry from a component tree.
///
/// \param tree A component tree on which to calculate the granulometry.
/// \param hist An output parameter in which to store the calculated granulometry.
/// \param rule Filtering rule to be used for tree filtering. 0 = removed region count, 1 = area, 2 = volume
void calculateGranulometry(const fl::ImageTree *tree, std::map<double, int> &hist, const int rule){

    tree->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings()); // selection of attribute

    tree->calculateGranulometryHistogram<fl::AreaAttribute>(hist, rule);

    tree->deleteAttributeFromTree<fl::AreaAttribute>();
}
