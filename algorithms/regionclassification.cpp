/// \file structures/regionclassification.cpp
/// \author Petra Bosilj
/// \date 22/03/2018

#include "objectdetection.h"

namespace fl{

    /// Prompts the user to manually classify `Node`s into `weed`, `crop`,
    /// and `mixed` class. Regions proposed by the `Node`s can also be
    /// discarded. Allows correction ("return to previous step").
    ///
    /// \param toMark A vector of pointers to `Node`s to manually
    /// classify. The vector is reordered upon returning from the function,
    /// such that the first elements correspond to the `weed` patches,
    /// followed by `crop` patches, `mixed` patches and `discarded` patches.
    /// The number of each of the classified patches (except the discarded
    /// ones) is returned through the `classSizes` parameter.
    /// \param originalImage The image on which the object classification
    /// was done. Primary image for visualization purposes.
    /// \param rgbImage The colour image corresponding to the input. For
    /// visualization purposes.
    /// \note Should add a default value for the RGB image or a default
    /// behavior when no secondary input image is available.
    void manualRegionClassification(std::vector <Node *> &toMark, const cv::Mat &originalImage, const cv::Mat &rgbImage, std::vector <int> &classSizes){

        // initialize the user display (2 rows) x (3 columns)
        cv::Mat output(originalImage.rows*2, originalImage.cols*3, CV_8UC3);
        // (1,1)
        cv::Mat out1(output, cv::Rect(cv::Point(0,0), originalImage.size()));
        // (2,1)
        cv::Mat out2(output, cv::Rect(cv::Point(0, originalImage.rows), originalImage.size()));
        // (1,2)
        cv::Mat out3(output, cv::Rect(cv::Point(originalImage.cols, 0), originalImage.size()));
        // (2,2)
        cv::Mat out4(output, cv::Rect(cv::Point(originalImage.cols, originalImage.rows), originalImage.size()));
        // (1,3)
        cv::Mat out5(output, cv::Rect(cv::Point(originalImage.cols*2, 0), originalImage.size()));
        // (2,3)
        cv::Mat out6(output, cv::Rect(cv::Point(originalImage.cols*2, originalImage.rows), originalImage.size()));

        // class marks
        std::vector <int> classes;

        // the output image showing the original image with the region, greened, in a rectangle (1,1)
        cv::Mat rected(originalImage.size(), CV_8UC3, cv::Vec3b(0,0,0));
        // the output image showing enlarged, greened region on black background (2,1)
        cv::Mat rectcut(originalImage.size(), CV_8UC3, cv::Vec3b(0,0,0));
        // the output image showing the colour image with the region in a rectangle (1,2)
        cv::Mat colrect(originalImage.size(), CV_8UC3, cv::Vec3b(0,0,0));
        // the output image showing enlarged, colour region on black background (2,2)
        cv::Mat colrectcut(originalImage.size(), CV_8UC3, cv::Vec3b(0,0,0));
        // the output image showing all the segments (1,3)
        cv::Mat segment(originalImage.size(), CV_8U, cv::Scalar(0));
        // enlarged, original region on black background (2,3)
        cv::Mat maskcut(originalImage.size(), CV_8U, cv::Scalar(0));

        // both helper images to help obtain maskcut
        cv::Mat masked(originalImage.size(), CV_8U, cv::Scalar(0));
        cv::Mat segment_single(originalImage.size(), CV_8U, cv::Scalar(0));

        cv::namedWindow("Pick a class", cv::WINDOW_NORMAL);
        cv::resizeWindow("Pick a class", originalImage.cols/1.5 * 3, originalImage.rows/1.5*2);

        std::cout << "Marking " << toMark.size() << " regions" << std::endl;

        bool returned = false;
        for (int i=0, szi = toMark.size(); i < szi; ++i){
            if (toMark[i] == NULL)
                continue;

            // preparing display
            output.setTo(cv::Scalar(0));
            segment_single.setTo(cv::Scalar(0));
            masked.setTo(cv::Scalar(0));

            if (returned){ // if the user requested a correction - redraw everything. Backtracking too complicated
                segment.setTo(cv::Scalar(0));
                for (int j=0; j < i; ++j)
                    toMark[j]->colorSolid(segment, cv::Scalar(180));
                returned = false;
            }
            cv::threshold(segment, segment, 1, 90, cv::THRESH_BINARY);

            // paint the current region onto (1,3)
            toMark[i]->colorSolid(segment, cv::Scalar(180));
            // paint the current region into ()
            toMark[i]->colorSolid(segment_single, cv::Scalar(255));

            // masked - cutout of the current segment with colours from original image
            originalImage.copyTo(masked, segment_single);

            // copy the original image to rected -- draw the rectangle in later
            cv::cvtColor(originalImage, rected, cv::COLOR_GRAY2RGB);
            // copy the colour image to colrect -- draw the rectangle in later
            rgbImage.copyTo(colrect);

            // get pixel coordinates of the current region to find (maxx,maxy), (minx, miny) and to paint it in
            std::vector <pxCoord> elems;
            toMark[i]->getElements(elems);

            // paint it in green
            for (int j=0, szj = elems.size(); j < szj; ++j)
                rected.at<cv::Vec3b>(elems[j].Y, elems[j].X)[1] = std::min(rected.at<cv::Vec3b>(elems[j].Y, elems[j].X)[1]+50, 255);

            pxCoord ll, ur, center; // rectangle coordinates
            std::sort(elems.begin(), elems.end());
            ll.X = std::max(elems[0].X-75, 0);
            ur.X = std::min(elems.back().X+75, originalImage.cols);
            std::sort(elems.begin(), elems.end(), [](const pxCoord &left, const pxCoord &right) {
                    return (left.Y < right.Y) || (left.Y == right.Y && left.X < right.X);
                });
            ll.Y = std::max(elems[0].Y-75, 0);
            ur.Y = std::min(elems.back().Y+75, originalImage.rows);

            // draw the rectangles onto rected and colrect
            cv::rectangle(rected, cv::Point(ll.X, ll.Y), cv::Point(ur.X, ur.Y), cv::Vec3b(50, 75, 200), 8);
            cv::rectangle(colrect, cv::Point(ll.X, ll.Y), cv::Point(ur.X, ur.Y), cv::Vec3b(50, 75, 200), 8);

            // calculate how much I can resize the region to fit one of the display windows
            double resizeFactor = (double)originalImage.cols / (std::abs(ll.X - ur.X)-4);
            resizeFactor = std::min(resizeFactor, (double)originalImage.rows/ (std::abs(ll.Y - ur.Y)-16));

            // cut out from rected - enlarged, greened region on black background
            cv::resize(cv::Mat(rected, cv::Rect(cv::Point(ll.X+8, ll.Y+8), cv::Point(ur.X-8, ur.Y-8))), rectcut, cv::Size(0,0), resizeFactor, resizeFactor);
            // cut out from colrect - enlarged, original region on black background
            cv::resize(cv::Mat(masked, cv::Rect(cv::Point(ll.X+8, ll.Y+8), cv::Point(ur.X-8, ur.Y-8))), maskcut, cv::Size(0,0), resizeFactor, resizeFactor);
            // cut out from colrect - enlarged, colour region on black background
            cv::resize(cv::Mat(colrect, cv::Rect(cv::Point(ll.X+8, ll.Y+8), cv::Point(ur.X-8, ur.Y-8))), colrectcut, cv::Size(0,0), resizeFactor, resizeFactor);

            // draw all six images onto the output image
            rected.copyTo(out1);
            rectcut.copyTo(cv::Mat(out2, cv::Rect(cv::Point((out3.cols - rectcut.cols)/2 ,(out3.rows - rectcut.rows)/2), rectcut.size())));
            colrect.copyTo(out3);
            colrectcut.copyTo(cv::Mat(out4, cv::Rect(cv::Point((out6.cols - colrectcut.cols)/2 ,(out6.rows - colrectcut.rows)/2), colrectcut.size())));
            cv::cvtColor(segment, out5, cv::COLOR_GRAY2RGB);
            cv::cvtColor(maskcut, cv::Mat(out6, cv::Rect(cv::Point((out4.cols - maskcut.cols)/2 ,(out4.rows - maskcut.rows)/2), maskcut.size())), cv::COLOR_GRAY2RGB);

            cv::imshow("Pick a class", output);

            // user prompt
            std::cout << "[" << i+1 << " / " << toMark.size() << "] ";
            if (i > 0)
                std::cout << "Please chose a class for the highlighted vegetation patch:\n(c) Crop\n(w) Weed\n(m) Mixed\n(d) Discard\n(p) Previous - correct choice for previous patch\n";
            else
                std::cout << "Please chose a class for the highlighted vegetation patch:\n(c) Crop\n(w) Weed\n(m) Mixed\n(d) Discard\n";
            for (bool flag = true; flag;){
                char c = cv::waitKey(0) & 0xFF;
                switch (c){
                    case 'M':
                    case 'm':
                        std::cout << "MIXED patch" << std::endl;
                        classes.push_back(2);
                        flag = false;
                        break;
                    case 'C':
                    case 'c':
                        std::cout << "CROP patch" << std::endl;
                        classes.push_back(1);
                        flag = false;
                        break;
                    case 'W':
                    case 'w':
                        std::cout << "WEED patch" << std::endl;
                        classes.push_back(0);
                        flag = false;
                        break;
                    case 'D':
                    case 'd':
                        std::cout << "DISCARDED (ambiguous) patch" << std::endl;
                        classes.push_back(-1);
                        flag = false;
                        break;
                    case 'P':
                    case 'p':
                        if (i > 0){
                            std::cout << "Going back to PREVIOUS patch" << std::endl;
                            i -= 2;
                            classes.pop_back();
                            flag = false;
                            returned = true;
                        }
                    default:
                        break;

                }
            }
        }

        // re-order the input `Node`s by class and store the class sizes
        std::vector <Node *> toMarkReordered[4];
        for (int i=0, szi = classes.size(); i < szi; ++i)
            if (classes[i] >= 0)
                toMarkReordered[classes[i]].emplace_back(toMark[i]);
            else
                toMarkReordered[3].emplace_back(toMark[i]); // discarded regions
        int toMarkSize = toMark.size();
        classSizes.clear();
        classSizes.reserve(3);
        toMark.clear();
        toMark.reserve(toMarkSize);
        for (int i=0; i < 4; ++i){
            toMark.insert(toMark.end(), toMarkReordered[i].begin(), toMarkReordered[i].end());
            if (i < 3) // do not return the number of discarded regions. simply all the rest.
                classSizes.emplace_back(toMarkReordered[i].size());
        }
    }
}

