/// \file algorithms/msernister.h
/// \author Petra Bosilj

#ifndef MSERNISTER_H_INCLUDED
#define MSERNISTER_H_INCLUDED

#include "../structures/imagetree.h"
#include "../structures/node.h"

#include <vector>

namespace fl{

    /// \struct mserParams
    ///
    /// \brief Holds all the parameters needed to define a MSER call.
    struct mserParams{

        /// The constructor taking a percentage of the image area as the upper size limit.
        /// \param delta The delta parameter for MSER. Bigger delta requires higher MSER stability
        /// \param maxAreaPerc The maximal area of the detected MSER, as the percentage of the total image ares.
        /// \param minArea The minimal area of the detected MSER, as the number of pixels. Default value is zero (no cutoff for small MSER detections).
        /// \param maxVariation The maximal variation of the MSER to be considered stable. Smaller values constrict stronger the detected points.
        /// \param minDiversity The minimal required difference between two nested MSER detections. Higher values constrict stronger the detected points.
        mserParams(int delta, double maxAreaPerc, int minArea = 0, double maxVariation = 0.25, double minDiversity = 0.2) :
            delta(delta), maxAreaPix(-1), maxAreaPerc(maxAreaPerc), minArea(minArea), maxVariation(maxVariation), minDiversity(minDiversity), perc(true) {}
        mserParams(int delta, int maxAreaPix = -1, int minArea = 0, double maxVariation = 0.25, double minDiversity = 0.2) :
            delta(delta), maxAreaPix(maxAreaPix), maxAreaPerc(0.0), minArea(minArea), maxVariation(maxVariation), minDiversity(minDiversity), perc(false) {}
        int delta;
        int maxAreaPix;
        double maxAreaPerc;
        int minArea;
        double maxVariation;
        double minDiversity;
        bool perc;
    };

    void markMserInTree(const ImageTree &tree, std::vector <Node *> &mser, std::vector <std::pair<double, int> > &div, const mserParams &params);
    void markMserInTree(const ImageTree &tree, int deltaLvl, std::vector <Node *> &mser, std::vector <std::pair<double, int> > &div,
                        int maxArea = -1, int minArea = 0, double maxVariation = 0.25, double minDiversity = 0.2);
    void markMserInTree(const ImageTree &tree, int deltaLvl, std::vector <Node *> &mser, std::vector <std::pair<double, int> > &div,
                        double maxArea = 0.00, int minArea = 0, double maxVariation = 0.25, double minDiversity = 0.2);
    void displayMser(const cv::Mat &img, const std::vector <Node *> &mser);
    void fillMserPoints(std::vector<std::vector<cv::Point> > &points, const std::vector <Node *> &mser);

}

#endif // MSERNISTER_H_INCLUDED
