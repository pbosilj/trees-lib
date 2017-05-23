#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

#include <iostream>

namespace fl{

    class ellipse
    {
        public:
            ellipse(const std::vector<cv::Point> &region, int imgWidth, int imgHeight);
            virtual ~ellipse();

            friend std::ostream& operator<<(std::ostream & ost, const ellipse &eli);
            bool isOK() {return ok;}
            double centerX() {return u;}
            double centerY() {return v;}
            bool small;
        protected:
        private:
            double a, b, c;
            double u,v;
            bool ok;
    };

    std::ostream& operator<<(std::ostream & ost, const ellipse &eli);
}

#endif // ELLIPSE_H
