#ifndef TPP_ALPHATREEDUALMAX
#define TPP_ALPHATREEDUALMAX

#include "alphatreedualmax.h"

namespace fl{
    template <class Function>
    Node *alphaTreeDualMax(const std::vector <cv::Mat> &img, Function distance){
        if (img.size() == 1){
            return alphaTreeDualMax(img[0]);
        }
        if (img.empty()){
            return NULL;
        }

//            cv::Mat img(rows,cols,CV_32S);
//
//            for (int _i=0; _i < rows; ++_i)
//                for (int _j=0; _j < cols; ++_j)
//                    imgIn >> img.at<int32_t>(_i, _j);



        //cv::Mat imgDual(img[0].rows*2-1, img[0].cols*2-1, CV_32S, *std::max_element(img[0].begin<uchar>(), img[0].end<uchar>()));
        // why max element??

        //cv::Mat imgDual(img[0].rows*2-1, img[0].cols*2-1, CV_32S, 255*255*img.size());

        cv::Mat imgDual(img[0].rows*2-1, img[0].cols*2-1, CV_32F, 255*255*img.size());


        for(int row = 0, szrow = img[0].rows; row < szrow; ++row) {
            std::vector <const uchar *> pU;
            for (int i=0; i < img.size(); ++i){
                pU.push_back(img[i].ptr(row));
            }
            //const uchar* pU = img.ptr(row);
            float *horiNew = imgDual.ptr<float>(row*2);
            ++horiNew;

            //const uchar* pD = NULL;
            std::vector <const uchar *> pD;
            float *vertiNew = NULL;
            if (row != (szrow-1)){
                //pD = img.ptr(row+1);
                for (int i=0, szi = img.size(); i < szi; ++i){
                    pD.push_back(img[i].ptr(row+1));
                }
                vertiNew = imgDual.ptr<float>(row*2+1);
            }
            for(int col = 0, szcol = img[0].cols; col < szcol; ++col) {
                //uchar curVal = *pU;
                std::vector <const uchar *> curVal(pU);
                if (col != (szcol-1)){
                    for (int i=0, szi = img.size(); i < szi; ++i)
                        pU[i]++;
                    float horizontal = distance(curVal, pU);
                    //uchar horizontal = std::abs(curVal - *++pU);
                    *horiNew = horizontal;
                }
                if (row != (szrow-1)){
                    //uchar vertical = std::abs(curVal - *pD++);
                    float vertical = distance(curVal, pD);
                    for (int i=0, szi = img.size(); i < szi; ++i)
                        pD[i]++;
                    *vertiNew = vertical;
                }
                horiNew += 2;
                vertiNew += 2;
            }
        }

        std::cout << "calling max tree following: " << std::endl;

        Node *dualRoot = maxTreeNister(imgDual, std::less<float>(), dual);

        std::cout << "max tree constructed" << std::endl;

        std::vector <std::vector <char> > seen(img[0].cols, std::vector <char>(img[0].rows, false));
        Node *alphaRoot = constructRecursively(dualRoot, seen);
        ImageTree *dualTree = new ImageTree(dualRoot, std::make_pair(img[0].rows, img[0].cols)); // <- to delete it

        delete dualTree;


        //std::cerr << *std::max_element(imgDual.begin<int32_t>(), imgDual.end<int32_t>()) << std::endl;
        alphaRoot->assignHyperLevelRec(detail::alphaTreeGrayLvlHyperAssign(img));
        return alphaRoot->assignGrayLevelRec(detail::alphaTreeGrayLvlAssign(img[0]));
    }
}
#endif

