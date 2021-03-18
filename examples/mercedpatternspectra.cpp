/// \file examples/mercedpatternspectra.cpp
/// \author Petra Bosilj
/// \date 11/10/2019

#include "mercedpatternspectra.h"

#include <iostream>
#include <utility>
#include <algorithm>
#include <iomanip>

#include "../misc/misc.h"

#include "../structures/imagetree.h"
#include "../algorithms/maxtreenister.h"

#include "../structures/attribute.h"
#include "../structures/areaattribute.h"
#include "../structures/noncompactnessattribute.h"
#include "../structures/entropyattribute.h"
#include "../structures/patternspectra2d.h"

/*************** HELPER FUNCTIONS -- NOT FOR PUBLIC USE *****************/

void getRanks(const std::vector<char> &response, int NG, int K, std::vector <double> &ranks){
    ranks.clear();
    for (int i=0; i < K && (int)ranks.size() < NG; ++i){
        if (!response[i])
            continue;
        ranks.push_back(i+1);
    }
    for (int i=ranks.size(); i < NG; ++i)
        ranks.push_back(1.25 * K);
}

double avgRank(const std::vector<char> &response, int NG, int K){
    std::vector <double> ranks;
    getRanks(response, NG, K, ranks);
    double result = 0.0;
    for (int i=0; i < NG; ++i){
        result += ranks[i];
    }
    result /= NG;
    return result;
}

double mrr(const std::vector<char> &response, int NG, int K){
    return avgRank(response, NG, K) - 0.5*(1+NG);
}

double nmrr(const std::vector<char> &response, int NG, int K){
    //std::cout << mrr(response, NG, K) / (1.25*K - 0.5*(1+NG)) << std::endl;
    return mrr(response, NG, K) / (1.25*K - 0.5*(1+NG));
}

double precisionM(const std::vector <char>&response, int m){
    int total = 0;
    for (int i=0, szi = response.size(); i < szi && i < (m-1); ++i)
        total += response[i];
    return (double)total / std::min(m,(int)response.size());
}

double averagePrecision(const std::vector<char> &response){
    int totalRelevant = 0;
    double sum = 0.0;
    for (int i=0, szi = response.size(); i < szi; ++i){
        if (response[i]){
            sum += precisionM(response, i+1);
            ++totalRelevant;
        }
    }
    return sum / (double)totalRelevant;
}

/*********************** MAIN FUNCTIONS OF THE EXAMPLE *************************/

void fl::generatePSMerced(int argc, char** argv){
    std::string goodFormat = "Expecting the call formatted as: ./Trees [listOfFiles] atr1=[area] bins1=[number] scale1=[-1 or size] atr2=[cnc,entropy] bins2=[number] max2=[number] tiling=[global,tiled,pyramid] [startSize (only for tiling=tiled,pyramid)]=[number]";
    if (argc < 9){
        std::cerr << "FAIL: " << goodFormat << std::endl;
        return;
    }

    std::ifstream listFile(argv[1]);

    std::cout << "Reading the list of input files from: " << argv[1] << std::endl;

    if (!listFile.is_open() ||
        std::string(argv[2]) != "area" ||
        (std::string(argv[5]) != "cnc" &&
         std::string(argv[5]) != "entropy") ||
        (std::string(argv[8]) != "global" &&
         std::string(argv[8]) != "tiled" &&
         std::string(argv[8]) != "pyramid")){
         std::cout << argv[8] << std::endl;
        std::cerr << "FAIL: " << goodFormat << std::endl;
        return;
    }

    int bins1, bins2, scale1, max2;
    sscanf(argv[3], "%d", &bins1);
    sscanf(argv[6], "%d", &bins2);
    sscanf(argv[4], "%d", &scale1);
    sscanf(argv[7], "%d", &max2);

    int startSize = -1;
    if (std::string(argv[8]) != "global"){
        if (argc < 10){
            std::cerr << "FAIL: " << goodFormat << std::endl;
            return;
        }
        else{
            sscanf(argv[9], "%d", &startSize);
        }
    }

    int descriptorLength = bins1*bins2*2;

    std::string line;
    int i = 0;
    while (std::getline(listFile, line)){
        ++i;

        std::cout << "processing: " << line << "\t\toutput:" << (removeExtension(line)+"."+ std::string(argv[8])+"ps").c_str() << std::endl;

        //if (!fileExists((removeExtension(line)+".ps"))){
        if (1){
            std::ofstream descOut((removeExtension(line)+"."+ std::string(argv[8])+"ps").c_str());

            descOut << descriptorLength << std::endl;

            cv::Mat img1 = cv::imread(line, cv::IMREAD_GRAYSCALE);
            cv::Mat img;
            cv::equalizeHist(img1, img);

            if (std::string(argv[8]) == "global"){
                // global descriptor only
                descOut << 1 << std::endl; // only 1 global descriptor per image
                if (std::string(argv[5]) == "cnc")
                    outputGlobalPS<fl::AreaAttribute, fl::NonCompactnessAttribute>(img, descOut, bins1, img.rows*img.cols, bins2, max2, scale1);
                else
                    outputGlobalPS<fl::AreaAttribute, fl::EntropyAttribute>(img, descOut, bins1, img.rows*img.cols, bins2, max2, scale1);

                continue; // to the next image
            }


            int rows = img.rows;
            int cols = img.cols;

            int pyramidFactor;
            if (std::string(argv[8]) == "tiled"){
                pyramidFactor = std::max(cols, rows); // this is to ensure extraction on a single scale
            }
            else{
                pyramidFactor = 2;
            }

            int descNum = 0;
            for (int dim = startSize, offset = 16; dim < std::min(256,std::max(cols, rows))+2; dim *= pyramidFactor){

                int szi = (cols - dim) / offset + 1;
                if (((cols - dim) % offset) > 2) ++szi;
                int szj = (rows - dim) / offset +1;
                if (((rows - dim) % offset) > 2) ++szj;

                descNum += szi*szj;
            }

            descOut << descNum << std::endl;

            for (int dim = startSize, offset = 16; dim < std::min(256,std::max(cols, rows))+2; dim *= pyramidFactor){
                int sum = 0;
                for (int _i=0; !_i || (_i - offset + dim < cols-2); _i += offset){
                    int dimi = std::min(cols-_i, dim);
                    if (dimi <= 2)
                        break;
                    for (int _j=0; !_j || (_j - offset + dim < rows-2); _j += offset){
                        int dimj = std::min(rows-_j, dim);

                        if (dimj <= 2)
                            break;

                        cv::Mat patch = img(cv::Rect(_i, _j, dimi, dimj));

                        if (std::string(argv[5]) == "cnc")
                            outputGlobalPS<fl::AreaAttribute, fl::NonCompactnessAttribute>(patch, descOut, bins1, patch.rows*patch.cols, bins2, max2, scale1);
                        else
                            outputGlobalPS<fl::AreaAttribute, fl::EntropyAttribute>(patch, descOut, bins1, patch.rows*patch.cols, bins2, max2, scale1);

                        ++sum;

                        if ((_j + dim) >= rows)
                            break;
                    }
                    if ((_i + dim) >= cols)
                        break;
                }
//                if (dim == 32){
//                    std::ofstream toErase((removeExtension(line)+".32er").c_str());
//                    toErase << sum << std::endl;
//                    toErase.close();
//                }
            }

            descOut.close();
        }

    }
}

void fl::evaluateMerced(int argc, char** argv){
    std::string goodFormat = "Expecting the call formatted as: ./Trees [listOfFiles] bins1=[number] bins2=[number] tiling=[global,tiled,pyramid]";
    if (argc < 5){
        std::cerr << "FAIL: " << goodFormat << std::endl;
        return;
    }

    std::ifstream listFile(argv[1]);

    std::cout << "Reading the list of input files from: " << argv[1] << std::endl;

    if (!listFile.is_open()){
        std::cerr << "FAIL: " << goodFormat << std::endl;
        return;
    }

    int bins1, bins2;
    sscanf(argv[2], "%d", &bins1);
    sscanf(argv[3], "%d", &bins2);

    int descriptorLength = bins1*bins2*2;

    std::vector <std::vector<double> > descriptors;

    std::string line;
    int i = 0;
    int zeroPerc=0;
    while (std::getline(listFile, line)){
        ++i;

        std::cout << "processing: " << (removeExtension(line)+"."+ std::string(argv[4])+"ps").c_str() << std::endl;

        std::ifstream readDesc((removeExtension(line)+"."+ std::string(argv[4])+"ps").c_str());
        descriptors.push_back(std::vector<double>(descriptorLength,0));

        for (int j=0; j < descriptorLength+2; ++j){
            if (j < 2){
                double temp;
                readDesc >> temp;
                continue;
            }
            readDesc >> descriptors.back()[j-2];
            zeroPerc += (int)(descriptors.back()[j-2]==0);
        }
    }

    std::cout << "Empty bins (0.0): " << ((double)(zeroPerc*100))/(descriptorLength*i) << "%" << std::endl;

    double anmrr = 0.0;
    double mAP = 0.0;
    std::vector<std::pair<double, int> > dists(i,std::make_pair(0,-1.0));
    std::vector<char> responses;
    for (int j=0; j < i; ++j){ // go over every image
        dists.clear(); dists.resize(i,std::make_pair(0,-1.0));
        for (int k=0; k < i; ++k){
            dists[k].second = k;
            for (int index=0; index < descriptorLength; ++index){
                // L1:
                dists[k].first += std::abs(descriptors[j][index] - descriptors[k][index]);
                // L2:
                // dists[k].first += (descriptors[j][index] - descriptors[k][index])*(descriptors[j][index] - descriptors[k][index]);
                // chi:
//                if (descriptors[j][index] && descriptors[k][index])
//                    dists[k].first += (descriptors[j][index] - descriptors[k][index])*(descriptors[j][index] - descriptors[k][index]) /
//                                            (descriptors[j][index] + descriptors[k][index]);
            }
        }
        std::sort(dists.begin(), dists.end());

        responses.clear(); responses.resize(i);

        for (int k=0; k < i; ++k){
            responses[k] = (char)((dists[k].second / 100)==(j/100));
        }

        double mynmrr = nmrr(responses, 100, 200);
        double ap = averagePrecision(responses);
        std::cout << "Average precision for image " << j << ": " << ap << std::endl;
        anmrr += (mynmrr / i);
        mAP += (ap / i);
    }

    std::cout << "ANMRR with K = 200 "  << anmrr << std::endl; // " " << anmrr2 << std::endl;
    std::cout << "mAP " << mAP << std::endl;
    return;

}


void fl::visualizePS(const std::vector<std::vector<double> > ps){
    int cell = 50;
    cv::Mat output = cv::Mat::zeros(ps.size()*cell, ps.back().size()*cell, CV_8U);

    std::ios oldState(nullptr);
    oldState.copyfmt(std::cout);


    // guarantee of all-positive matrix
    double maxElem = 0;
    for (int i=0, szi = ps.size(); i < szi; ++i){
        double currentMax = *(std::max_element(ps[i].begin(), ps[i].end()));
        if (currentMax > maxElem)
            maxElem = currentMax;
    }

    std::cout << maxElem << std::endl;

    for (int pi=0, szpi = ps.size(); pi < szpi; ++pi){
        double sumRow = 0;
        for (int pj=0, szpj = ps[pi].size(); pj < szpj; ++pj){
            sumRow += ps[pi][pj];
            int value = int(std::round(ps[pi][pj]*255/maxElem));


            for (int i=0; i < cell; ++i)
                for (int j=0; j < cell; ++j)
                    output.at<uchar>(pi * cell + i, pj*cell + j) = (uchar)value;

            std::cout << "(";
            std::cout << std::fixed << std::setw( 8 ) << std::setprecision( 4 )
                << std::setfill( '0' ) << ps[pi][pj];
            std::cout <<  " " << std::fixed << std::setw( 3 ) << std::setfill(' ') << value << ") ";
        }
        std::cout << std::fixed << std::setw( 8 ) << std::setprecision( 4 )
                << std::setfill( '0' ) << sumRow;
        std::cout << std::endl;



//        cv::namedWindow("PS", cv::WINDOW_NORMAL);
//        cv::imshow("PS", output);
//        cv::waitKey(0);
//        cv::destroyWindow("PS");
    }
    cv::namedWindow("PS", cv::WINDOW_NORMAL);
    cv::imshow("PS", output);
    cv::waitKey(0);
    cv::destroyWindow("PS");

    std::cout.copyfmt(oldState);

}
