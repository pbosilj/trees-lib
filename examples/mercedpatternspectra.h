/// \file examples/mercedpatternspectra.h
/// \author Petra Bosilj
/// \date 11/10/2019

#ifndef MERCEDPATTERNSPECTRA_H
#define MERCEDPATTERNSPECTRA_H


#define GAREA 10
#define GSHAPE 6
#define GNC 56
#define TREES 2

#include <fstream>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

namespace fl{

    /**
    \brief Generates the Pattern Spectra for the Merced data (or any other list of images).
    **/
    void generatePSMerced(int argc, char** argv);

    /**
    \brief Evaluates the retrieval with Pattern Spectra for the Merced data.
    \note The list file assumes the files are listed in order of classes, 100 files per
          class. Therefore this will only work for Merced (or some other dataset with 100
          files per class.
    **/
    void evaluateMerced(int argc, char** argv);

    template <typename SizeAttribute, typename ShapeAttribute>
    void outputGlobalPS(cv::Mat &image, std::ostream &outPS, int sizeBins = GAREA, int sizeMax = 150000, int shapeBins = GSHAPE, int shapeMax = GNC, int sizeScale = -1);

    void visualizePS(const std::vector<std::vector<double> > ps);
}

#include "mercedpatternspectra.tpp"

#endif
