/// \file structures/soilpatternspectra.cpp
/// \author Petra Bosilj
/// \date 07/10/2019

#include "soilpatternspectra.h"

#include "../misc/misc.h"
#include "../algorithms/treeconstruction.h"

#include <fstream>
#include <numeric>
#include <iostream>

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
        //case 1: outputPath += "sum_"; break; // case 1: deprecated; replaced
        case 1: outputPath += "volume_"; break;
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

void rClassicalMorphology(int argc, char **argv){
    std::string correctCall = "Call with following arguments: ./Trees [image_path] [operation: opening, closing, both] [filtering_rule: count, volume, both] [binning: log, arbitrary] [bins: [if log: number_of_bins upper_limit] [if arbitrary: bin limits]].";
    if (argc < 6){
        std::cerr << "Not enough arguments" << std::endl;
        std::cerr << correctCall << std::endl;
        exit(1);
    }

    if (!fileExists(std::string(argv[1]))){
        std::cerr << "Please provide a correct [image_path]." << std::endl;
        exit(1);
    }
    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE); // error catching for image input?
    std::vector <int> limits;
    std::string binning;
    if (std::string(argv[4]) == "log"){
        int numBins, upperLimit;
        if (argc < 7){
            std::cerr << "Not enough arguments for log" << std::endl;
            std::cerr << correctCall << std::endl;
            exit(1);
        }
        sscanf(argv[5], "%d", &numBins);
        sscanf(argv[6], "%d", &upperLimit);
        double base = std::pow(upperLimit, 1.0/numBins);
        double limit = 1.0;
        for (int i=0; i < numBins; ++i){
            limit *= base;
            limits.push_back(std::ceil(std::sqrt(limit)));
        }
        binning = "log_b" + std::to_string(numBins);
    }
    else if (std::string(argv[4]) == "arbitrary"){
        limits.resize(argc-5);
        for (int i=5; i < argc; ++i){
            sscanf(argv[i], "%d", &limits[i-5]);
            limits[i-5] = std::ceil(std::sqrt(limits[i-5]));
        }
        binning = "arbitrary";
    }


    std::cout << "Number of bins: " << limits.size() << " for binning: " << argv[4] << std::endl;
    for (int i=0, szi = limits.size(); i < szi; ++i)
        std::cout << limits[i] << " ";
    std::cout  << std::endl;

    //int volumeOriginal = detail::getImageVolume(image);
    //std::cout << "Original image volume: " << volumeOriginal << std::endl;

    cv::Mat filtered;
    cv::Mat previous = image.clone();
    int volumePrevious = detail::getImageVolume(previous);
    std::vector <int> volume;
    std::vector <int> ccount;


    if (std::string(argv[2]) == "opening" || std::string(argv[2]) == "both"){
        fl::ImageTree *originalTree = fl::createTree(fl::treeType::minTree, image);
        int originalNodes = originalTree->countNodes();
        delete originalTree;
        for (int i=0, szi = limits.size(); i < szi; ++i){
            cv::morphologyEx(image, filtered, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(limits[i], limits[i])));
            fl::ImageTree *filteredTree = fl::createTree(fl::treeType::minTree, filtered);
            int filteredNodes = filteredTree->countNodes();
            delete filteredTree;
            //cv::erode(image, filtered, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(limits[i], limits[i])));

            //std::cout << "\t" << "volume previous: " << volumePrevious << " volume filtered: " << volumeFiltered << " volume original: " << volumeOriginal << std::endl;

            if (std::string(argv[3]) == "volume" || std::string(argv[3]) == "both"){
                int volumeFiltered = detail::getImageVolume(filtered);
                volume.push_back(volumePrevious - volumeFiltered);

//                std::cout << "Volume diff erosion ( " << limits[i] << " x " << limits[i] << " = " << limits[i]*limits[i] << "): " << volumePrevious-volumeFiltered << std::endl;
                volumePrevious = volumeFiltered;
            }
            if (std::string(argv[3]) == "count" || std::string(argv[3]) == "both"){
                ccount.push_back(originalNodes-filteredNodes);

//                std::cout << "Sum    diff erosion ( " << limits[i] << " x " << limits[i] << " = " << limits[i]*limits[i] << "): " << differencesFiltered << std::endl;
            }
            previous = filtered.clone();
        }

        if (std::string(argv[3]) == "volume" || std::string(argv[3]) == "both"){
            int volumeSum = std::accumulate(volume.begin(), volume.end(), 0);
            std::string output = removeExtension(argv[1]) + "_volume_opening_" + binning + "_text.txt";
            std::ofstream openingVolume(output);

            openingVolume << removeExtension(argv[1]) + "_volume_opening_" + std::string(argv[4]) << " " << limits.size() << " ";
            for (int i=0, szi = limits.size(); i < szi; ++i){
//                std::cout << volume[i] << " ";
                openingVolume << (double)volume[i]/volumeSum << " ";
            }
            openingVolume << 1.0/volumeSum << std::endl;
//            std::cout << volumeSum << std::endl;

            openingVolume.close();

            volume.clear();
            volumePrevious = detail::getImageVolume(previous);
        }

        if (std::string(argv[3]) == "count" || std::string(argv[3]) == "both"){
            int countSum = std::accumulate(ccount.begin(), ccount.end(), 0);
            std::string output = removeExtension(argv[1]) + "_count_opening_" + binning + "_text.txt";

            std::ofstream openingCount(output);

            openingCount << removeExtension(argv[1]) + "_count_opening_" + std::string(argv[4]) << " " << limits.size() << " ";
            for (int i=0, szi = limits.size(); i < szi; ++i)
                openingCount << (double)ccount[i]/countSum << " ";
            openingCount << 1.0/countSum << std::endl;

            openingCount.close();

            ccount.clear();
        }
        // clean-up for next one if needed
        previous = image.clone();
    }

    if (std::string(argv[2]) == "closing" || std::string(argv[2]) == "both"){
        fl::ImageTree *originalTree = fl::createTree(fl::treeType::maxTree, image);
        int originalNodes = originalTree->countNodes();
        delete originalTree;
        for (int i=0, szi = limits.size(); i < szi; ++i){
            cv::dilate(image, filtered, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(limits[i], limits[i])));
            fl::ImageTree *filteredTree = fl::createTree(fl::treeType::maxTree, filtered);
            int filteredNodes = filteredTree->countNodes();
            delete filteredTree;
            //std::cout << "\t" << "volume previous: " << volumePrevious << " volume filtered: " << volumeFiltered << " volume original: " << volumeOriginal << std::endl;

            if (std::string(argv[3]) == "volume" || std::string(argv[3]) == "both"){
                int volumeFiltered = detail::getImageVolume(filtered);
                volume.push_back(volumeFiltered - volumePrevious);

//                std::cout << "Volume diff dilation ( " << limits[i] << " x " << limits[i] << " = " << limits[i]*limits[i] << "): " << volumeFiltered-volumePrevious << std::endl;
                volumePrevious = volumeFiltered;
            }
            if (std::string(argv[3]) == "count" || std::string(argv[3]) == "both"){
                ccount.push_back(originalNodes-filteredNodes);

//                std::cout << "Sum    diff dilation ( " << limits[i] << " x " << limits[i] << " = " << limits[i]*limits[i] << "): " << differencesFiltered << std::endl;
            }
            previous = filtered.clone();
        }

        if (std::string(argv[3]) == "volume" || std::string(argv[3]) == "both"){
            int volumeSum = std::accumulate(volume.begin(), volume.end(), 0);
            std::string output = removeExtension(argv[1]) + "_volume_closing_" + binning + "_text.txt";
            std::ofstream closingVolume(output);

            closingVolume << removeExtension(argv[1]) + "_volume_closing_" + std::string(argv[4]) << " " << limits.size() << " ";
            for (int i=0, szi = limits.size(); i < szi; ++i)
                closingVolume << (double)volume[i]/volumeSum << " ";
            closingVolume << 1.0/volumeSum << std::endl;

            closingVolume.close();

            volume.clear();
            volumePrevious = detail::getImageVolume(previous);
        }

        if (std::string(argv[3]) == "count" || std::string(argv[3]) == "both"){
            int countSum = std::accumulate(ccount.begin(), ccount.end(), 0);
            std::string output = removeExtension(argv[1]) + "_count_closing_" + binning + "_text.txt";

            std::ofstream closingCount(output);

            closingCount << removeExtension(argv[1]) + "_count_closing_" + std::string(argv[4]) << " " << limits.size() << " ";
            for (int i=0, szi = limits.size(); i < szi; ++i)
                closingCount << (double)ccount[i]/countSum << " ";
            closingCount << 1.0/countSum << std::endl;

            closingCount.close();

            ccount.clear();
        }
    }
}

/// \brief Function to use for calculation of global (area) pattern spectra of a single image.
///
/// Usage: call as the only function from `main`, passing the input arguments from `main`.
///
/// input arguments are positional:
///     1 - path to image
///     2 - tree option: "min, max, tos, alpha, omega, minmax, all"
///     3 - filtering rule / content measure: "count, volume, both", count = number of regions, volume = number of pixels * contrast
void rTestSoil(int argc, char **argv){
    if (argc < 4){
        std::cerr << "Call with three arguments: ./Trees [image_path] [tree_option:min, max, alpha, omega, minmax, all] [filtering_rule: count, volume, both]." << std::endl;
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
    else if (rule == "volume")
        rules.push_back(1);
    else if (rule == "both"){
        rules.push_back(0);
        rules.push_back(1);
    }
    else{
        std::cerr << "The third argument [filtering_rule] has to be one of the following: [filtering_rule: count, volume, both]." << std::endl;
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
        mainTree = fl::createTree(t, image);
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
