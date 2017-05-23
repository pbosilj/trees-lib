//  #include "ellipse.h"
//
// #include <cmath>
//
// #define PI 3.14159265
//
//fl::ellipse::ellipse(const std::vector<cv::Point> &region, int imgWidth, int imgHeight){
//
//    double sumx1 = 0, sumx2 = 0, sumsx1 = 0, sumsx2 = 0, sumx1x2 = 0;
//
//    int regSiz = region.size();
//
//
//    //std::cout << regSiz << " ";
//    for (int i=0; i < regSiz; ++i){
//        //std::cout << region[i].x << " " << region[i].y << " ";
//        sumx1+=region[i].x;
//        sumx2+=region[i].y;
//    }
//    //std::cout << std::endl;
//
//    this->u = (double)sumx1 / regSiz;
//    this->v = (double)sumx2 / regSiz;
//
//    bool debug = false;
////    if (this->u == 279 && this->v == 254.5)
////        debug = true;
//
//
//    if (debug)
//        std::cout << this->u << " " << this->v << std::endl;
//
//    for (int i=0; i < regSiz; ++i){
//        sumsx1  += (region[i].x-this->u) * (region[i].x-this->u);
//        sumsx2  += (region[i].y-this->v) * (region[i].y-this->v);
//        sumx1x2 += (region[i].x-this->u) * (region[i].y-this->v);
//    }
//
//    if (debug)
//        std::cout << "sums: " << sumsx1 << " " << sumsx2 << " " << sumx1x2 << std::endl;
//
//    double m20 = (double)sumsx1 / regSiz;
//    double m02 = (double)sumsx2 / regSiz;
//
//    double m11 = (regSiz==1)?((double)sumx1x2):((double)sumx1x2 / (regSiz-1));
//
//    if (debug)
//        std::cout << "m20: " << m20 << " m02: " << m02 << " m11: " << m11 << std::endl;
//
//    if (((m20-m02)*(m20-m02) + 4*m11*m11) < 0){
//        this->ok = false;
//        return;
//    }
//
//    double root = sqrt((m20-m02)*(m20-m02) + 4*m11*m11);
//    double eig1 = 0.5 * (m20+m02 + root);
//    double eig2 = 0.5 * (m20+m02 - root);
//
//    if (debug)
//        std::cout << "eig1: " << eig1 << " eig2: " << eig2 << std::endl;
//
//    double emax = std::max(eig1, eig2);
//    double emin = std::min(eig1, eig2);
//    if (emin <= 0 || emax <= 0){
//        this->ok = false;
//        return;
//    }
//    else{
//        this->ok = true;
//    }
//
//    double semiA = 4*sqrt(emax);
//    double semiB = 4*sqrt(emin);
//
//    double denom = m20 - emax;
//
//
//    double phi = std::atan2(m11, denom)+PI/2;
//
////    if (phi < (-PI / 2))
////        phi += PI;
////    else if (phi > (PI / 2))
////        phi -= PI;
//
//
//    ++this->u;
//    ++this->v;
//
//    this->a = std::cos(phi)*std::cos(phi)/(semiA*semiA) + std::sin(phi)*std::sin(phi)/(semiB*semiB);
//    this->b = std::cos(phi)*std::sin(phi)*((1/(semiA*semiA))-(1/(semiB*semiB)));
//    this->c = std::cos(phi)*std::cos(phi)/(semiB*semiB) + std::sin(phi)*std::sin(phi)/(semiA*semiA);
//
//    double bub = 2 * this->b * this->b - 4 * this->a * this->c;
//    double blab = sqrt((-4 * this->c) / bub);
//    double blob = sqrt((-4 * this->a) / bub);
//
//    if (this->u - blab < 0 ||
//        this->u + blab >= imgWidth ||
//        this->v - blob < 0 ||
//        this->v + blab >= imgHeight){
//            this->ok = false;
//    }
//
///*
//    double bub = 2 * this->b * this->b - 4 * this->a * this->c;
//    double blab = sqrt((-4 * this->c) / bub);
//    double blob = sqrt((-4 * this->a) / bub);
//    std::cout << "min x: " << this->u - blab << " max x: " << this->u + blab << " min y: " << this->v - blob << " max y: " << this->v + blob << std::endl;
//*/
//}
//
//fl::ellipse::~ellipse(){
//    //dtor
//}
//
//std::ostream& fl::operator<<(std::ostream &ost, const ellipse &eli){
//  ost << eli.u << " " << eli.v << " " << eli.a << " " << eli.b << " " << eli.c;
//  return ost;
//}


 #include "ellipse.h"

 #include <cmath>

 #include <algorithm>

 #define PI 3.14159265

fl::ellipse::ellipse(const std::vector<cv::Point> &region, int imgWidth, int imgHeight) : small (false){

    double sumx1 = 0, sumx2 = 0, sumsx1 = 0, sumsx2 = 0, sumx1x2 = 0;

    int regSiz = region.size();


    //std::cout << regSiz << " ";
    for (int i=0; i < regSiz; ++i){
        //std::cout << region[i].x << " " << region[i].y << " ";
        sumx1+=region[i].x;
        sumx2+=region[i].y;
    }
    //std::cout << std::endl;

    this->u = (double)sumx1 / regSiz;
    this->v = (double)sumx2 / regSiz;

    bool debug = false;
//    if (this->u == 279 && this->v == 254.5)
//        debug = true;


    if (debug)
        std::cout << this->u << " " << this->v << std::endl;

    for (int i=0; i < regSiz; ++i){
        sumsx1  += (region[i].x-this->u) * (region[i].x-this->u);
        sumsx2  += (region[i].y-this->v) * (region[i].y-this->v);
        sumx1x2 += (region[i].x-this->u) * (region[i].y-this->v);
    }

    if (debug)
        std::cout << "sums: " << sumsx1 << " " << sumsx2 << " " << sumx1x2 << std::endl;

    double m20 = (double)sumsx1 / regSiz;
    double m02 = (double)sumsx2 / regSiz;

    double m11 = (regSiz==1)?((double)sumx1x2):((double)sumx1x2 / (regSiz-1));

    if (debug)
        std::cout << "m20: " << m20 << " m02: " << m02 << " m11: " << m11 << std::endl;

    if (((m20-m02)*(m20-m02) + 4*m11*m11) < 0){
        this->ok = false;
        return;
    }

    double root = sqrt((m20-m02)*(m20-m02) + 4*m11*m11);
    double eig1 = 0.5 * (m20+m02 + root);
    double eig2 = 0.5 * (m20+m02 - root);

    if (debug)
        std::cout << "eig1: " << eig1 << " eig2: " << eig2 << std::endl;

    double emax = std::max(eig1, eig2);
    double emin = std::min(eig1, eig2);
    if (emin <= 0 || emax <= 0){
        this->ok = false;
        return;
    }
    else{
        this->ok = true;
    }

    double semiA = 4*sqrt(emax);
    double semiB = 4*sqrt(emin);

    double denom = m20 - emax;


    double phi = std::atan2(m11, denom)+PI/2;

    double phidegabs = std::abs(phi*180/PI);

    //std::cout << semiA << " " << semiB << " " << phi*180/PI << std::endl;

    if (semiA / semiB >= 4 && semiB <= 2.5 &&
        (phidegabs < 4 ||
        (phidegabs < 184 && phidegabs >= 176) ||
        (phidegabs < 94  && phidegabs >=  86) ||
        (phidegabs < 274 && phidegabs >= 266))){
        this->ok = false;


    }

//    if (phi < (-PI / 2))
//        phi += PI;
//    else if (phi > (PI / 2))
//        phi -= PI;


    ++this->u;
    ++this->v;

    this->a = std::cos(phi)*std::cos(phi)/(semiA*semiA) + std::sin(phi)*std::sin(phi)/(semiB*semiB);
    this->b = std::cos(phi)*std::sin(phi)*((1/(semiA*semiA))-(1/(semiB*semiB)));
    this->c = std::cos(phi)*std::cos(phi)/(semiB*semiB) + std::sin(phi)*std::sin(phi)/(semiA*semiA);

    /*
    double bub = 2 * this->b * this->b - 4 * this->a * this->c;
    double blab = sqrt((-4 * this->c) / bub);
    double blob = sqrt((-4 * this->a) / bub);

    if (this->u - blab < 0 ||
        this->u + blab >= imgWidth ||
        this->v - blob < 0 ||
        this->v + blab >= imgHeight){
            this->ok = false;
    }
    */

    double A = semiA * std::cos(phi);
    double B = semiB * std::sin(phi);
    double C = semiA * std::sin(phi);
    double D = semiB * std::cos(phi);

    if ((this->u - (sqrt(A*A + B*B))) < 0 ||
        (this->u + (sqrt(A*A + B*B))) >= imgWidth ||
        (this->v - (sqrt(C*C + D*D))) < 0 ||
        (this->v + (sqrt(C*C + D*D))) >= imgHeight){

        this->small = true;
        //this->ok = false;
    }

    //if (this->ok){
        //std::cout << (this->u - (sqrt(A*A + B*B))) << " " << (this->u + (sqrt(A*A + B*B))) << " " << (this->v - (sqrt(C*C + D*D))) << " " << (this->v + (sqrt(C*C + D*D))) << std::endl;
    //}

/*
    double bub = 2 * this->b * this->b - 4 * this->a * this->c;
    double blab = sqrt((-4 * this->c) / bub);
    double blob = sqrt((-4 * this->a) / bub);
    std::cout << "min x: " << this->u - blab << " max x: " << this->u + blab << " min y: " << this->v - blob << " max y: " << this->v + blob << std::endl;
*/
}

fl::ellipse::~ellipse(){
    //dtor
}

std::ostream& fl::operator<<(std::ostream &ost, const ellipse &eli){
  ost << eli.u << " " << eli.v << " " << eli.a << " " << eli.b << " " << eli.c;
  return ost;
}

