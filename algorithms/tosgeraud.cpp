#include "tosgeraud.h"

#include "maxtreeberger.h"

#include "../structures/node.h"
#include "../structures/inclusionnode.h"
#include "../misc/pixels.h"

#include <vector>
#include <algorithm>
#include <utility>
#include <map>

#include <iostream>

#define F first
#define S second
#define lower first
#define upper second

#include <cstdio>

namespace fl{
    namespace detail{
        class compareByX{
            public:
                compareByX(){}
                bool operator() (const pxCoord &lhs, const pxCoord &rhs);
        };

        class compareByY{
            public:
                compareByY(){}
                bool operator() (const pxCoord &lhs, const pxCoord &rhs);
        };

        bool isKhalimskyOriginalPixel(const pxCoord &kpx, const int oX, const int oY);
        pxCoord specialPop(std::vector <std::vector <pxCoord> > &q, int &qElems, const uchar &lo, uchar &level);
        void specialPush(std::vector <std::vector <pxCoord> > &q, int &qElems, const uchar &lo, const uchar &level,
                         const pxCoord &coord, const std::pair<uchar, uchar> &bounds);

        void interpolateImg(const cv::Mat &imgTrue, std::vector <std::vector <std::pair<uchar, uchar> > > &iimg);
        void sortImgElemsToS(const std::vector <std::vector <std::pair <uchar, uchar> > > &iimg,
                         std::vector <pxCoord> &order, cv::Mat &eimg);
        void unInterpolateParent(std::vector<std::vector <pxCoord> > parentOld, const cv::Mat &ub,
                            std::vector<std::vector <pxCoord> > &parentNew);
    }

    /// \param img The image used to construct the tree of shapes.
    ///
    /// \return A `PartitioningNode *` (as `Node *) to the root of the tree of shapes.
    Node *tosGeraud(const cv::Mat &img){

        std::vector <std::vector <std::pair<uchar, uchar> > > intImg;
        cv::Mat ub;
        std::vector <pxCoord> sorted;
        std::vector <std::vector <pxCoord> > parentInt(((img.cols+2)<<2)-1, std::vector<pxCoord> (((img.rows+2)<<2)-1));
        std::vector <std::vector <pxCoord> > parent;


        detail::interpolateImg(img, intImg);
        detail::sortImgElemsToS(intImg, sorted, ub);
        detail::maxTreeCore(sorted, fl::pxType::regular, parentInt);
        detail::canonizeTree(sorted, parentInt, ub);
        detail::unInterpolateParent(parentInt, ub, parent);

        return detail::makeNodeTree(parent,img)->assignGrayLevelRec(detail::maxTreeGrayLvlAssign(img));
    }

    namespace detail{
        bool compareByX::operator() (const pxCoord &lhs, const pxCoord &rhs){
            return lhs.X < rhs.X;
        }

        bool compareByY::operator() (const pxCoord &lhs, const pxCoord &rhs){
            return lhs.Y < rhs.Y;
        }

        bool isKhalimskyOriginalPixel(const pxCoord &kpx, const int oX, const int oY){
            if ((kpx.X - 5)%4 || (kpx.Y - 5)%4)
                return false;
            if ((kpx.X-5)>>2 < 0 || (kpx.X-5)>>2 >= oX)
                return false;
            if ((kpx.Y-5)>>2 < 0 || (kpx.Y-5)>>2 >= oY)
                return false;
            return true;
        }

        void interpolateImg(const cv::Mat &imgTrue, std::vector <std::vector <std::pair<uchar, uchar> > > &iimg){

            int iimgMRows = ((imgTrue.cols+2)<<2)-1;
            int iimgMCols = ((imgTrue.rows+2)<<2)-1;

            iimg.clear();
            iimg.resize(iimgMRows, std::vector <std::pair<uchar, uchar> > (iimgMCols, std::pair<uchar,uchar>(0,0)));


            std::vector <uchar> values; // median
            for (int i=0; i < imgTrue.cols; ++i){
                values.push_back(imgTrue.at<uchar>(0,i));
                values.push_back(imgTrue.at<uchar>(imgTrue.rows-1,i));
            }
            for (int i=0; i < imgTrue.rows; ++i){
                if (i)
                    values.push_back(imgTrue.at<uchar>(i,0));
                if (i != imgTrue.rows-1)
                    values.push_back(imgTrue.at<uchar>(i,imgTrue.cols-1));
            }
            std::sort(values.begin(), values.end()); // determine the median of the surrounding elems

            uchar median = values[values.size()/2];

            for (int col = 1; col < iimgMCols; col+=2){
                iimg[1][col].lower = iimg[1][col].upper =
                    iimg[iimgMRows-2][col].lower = iimg[iimgMRows-2][col].upper = median;
            }
            for (int row = 1; row < iimgMRows; row+=2){
                iimg[row][1].lower = iimg[row][1].upper =
                    iimg[row][iimgMCols-2].lower = iimg[row][iimgMCols-2].upper = median;
            }

            const uchar *p = NULL;
            for(int row = 1, szrow = imgTrue.rows+2; row < szrow; ++row) {
                if (row < imgTrue.rows+1)
                    p = imgTrue.ptr(row-1);

                for(int col = 1, szcol = imgTrue.cols+2; col < szcol; ++col) {
                    if (row < imgTrue.rows+1 && col < imgTrue.cols+1)
                        iimg[(col<<2)+1][(row<<2)+1].lower = iimg[(col<<2)+1][(row<<2)+1].upper = *p++;

                    iimg[(col<<2)+1-2][(row<<2)+1].lower = iimg[(col<<2)+1-2][(row<<2)+1].upper =
                        std::max(iimg[(col<<2)+1][(row<<2)+1].lower, iimg[(col<<2)+1-4][(row<<2)+1].lower);

                    iimg[(col<<2)+1][(row<<2)+1-2].lower = iimg[(col<<2)+1][(row<<2)+1-2].upper =
                        std::max(iimg[(col<<2)+1][(row<<2)+1].lower, iimg[(col<<2)+1][(row<<2)+1-4].lower);
                }
            }

            for (int irow = 3, szir = iimgMCols; irow < szir; irow+=4){
                for (int icol = 3, szic = iimgMRows; icol < szic; icol+=4){
                    iimg[icol][irow].lower = iimg[icol][irow].upper =
                        std::max(iimg[icol-2][irow].lower, iimg[icol+2][irow].lower);
                    //equivalent:
                    //iimg[icol][irow].lower = iimg[icol][irow].upper =
                    //  std::max(iimg[icol][irow-2].lower, iimg[icol][irow+2].lower);
                }
            }
            for (int irow=0, szir = iimgMCols; irow < szir; irow+=2){
                for (int icol=1, szic = iimgMRows; icol < szic; icol+=2){
                    if (irow && irow != szir-1){
                        iimg[icol][irow].lower = std::min(iimg[icol][irow-1].lower, iimg[icol][irow+1].lower);
                        iimg[icol][irow].upper = std::max(iimg[icol][irow-1].upper, iimg[icol][irow+1].upper);
                    }
                    else if (!irow){
                        iimg[icol][irow].lower = iimg[icol][irow].upper = iimg[icol][irow+1].lower;
                    }
                    else{
                        iimg[icol][irow].lower = iimg[icol][irow].upper = iimg[icol][irow-1].lower;
                    }
                }
            }
            for (int irow=1, szir = iimgMCols; irow < szir; irow+=2){
                for (int icol=0, szic = iimgMRows; icol < szic; icol+=2){
                    if (icol && icol != szic-1){
                        iimg[icol][irow].lower = std::min(iimg[icol-1][irow].lower, iimg[icol+1][irow].lower);
                        iimg[icol][irow].upper = std::max(iimg[icol-1][irow].upper, iimg[icol+1][irow].upper);
                    }
                    else if (!icol){
                        iimg[icol][irow].lower = iimg[icol][irow].upper = iimg[icol+1][irow].lower;
                    }
                    else{
                        iimg[icol][irow].lower = iimg[icol][irow].upper = iimg[icol-1][irow].lower;
                    }
                }
            }
            for (int irow=0, szir = iimgMCols; irow < szir; irow+=2){
                for (int icol=0, szic = iimgMRows; icol < szic; icol+=2){
                    if (irow && irow != szir-1){
                        iimg[icol][irow].lower = std::min(iimg[icol][irow-1].lower, iimg[icol][irow+1].lower);
                        iimg[icol][irow].upper = std::max(iimg[icol][irow-1].upper, iimg[icol][irow+1].upper);
                    }
                    else if (!irow){
                        iimg[icol][irow].lower = iimg[icol][irow+1].lower;
                        iimg[icol][irow].upper = iimg[icol][irow+1].upper;
                    }
                    else{
                        iimg[icol][irow].lower = iimg[icol][irow-1].lower;
                        iimg[icol][irow].upper = iimg[icol][irow-1].upper;
                    }
                }
            }
        }

        pxCoord specialPop(std::vector <std::vector <pxCoord> > &q, int &qElems, const uchar &lo, uchar &level){
            pxCoord rValue;

            if (!qElems)
                return rValue;
            for (int i=0; ; ++i){
                if ((level-lo+i) < (int)q.size() && !q[level-lo+i].empty()){
                    rValue = q[level-lo+i].back();
                    q[level-lo+i].pop_back(); --qElems;
                    level+=i;
                    break;
                }
                else if (i && (level-lo-i) >= 0 && !q[level-lo-i].empty()){
                    rValue = q[level-lo-i].back();
                    q[level-lo-i].pop_back(); --qElems;
                    level-=i;
                    break;
                }
            }
            return rValue;
        }
        void specialPush(std::vector <std::vector <pxCoord> > &q, int &qElems, const uchar &lo, const uchar &level,
                         const pxCoord &coord, const std::pair<uchar, uchar> &bounds){
            int pLevel;
            if (bounds.first > level)
                pLevel = bounds.first;
            else if (bounds.second < level)
                pLevel = bounds.second;
            else
                pLevel = level;
            q[pLevel-lo].push_back(coord); ++qElems;
        }

        void sortImgElemsToS(const std::vector <std::vector <std::pair <uchar, uchar> > > &iimg,
                         std::vector <pxCoord> &order, cv::Mat &eimg){

            order.clear();
            std::vector <std::vector <bool> > seen(iimg.size(), std::vector <bool> (iimg.back().size(), false));
            eimg.create(iimg.back().size(), iimg.size(), CV_8U);


            compareByX cbf = compareByX();
            compareByY cbs = compareByY();
            uchar lo = std::min_element(iimg[0].begin(), iimg[0].end(), cbs)->first;
            uchar hi = std::max_element(iimg[0].begin(), iimg[0].end(), cbs)->second;
            for (int i=1, szi = iimg.size(); i < szi; ++i){
                lo = std::min(lo, std::min_element(iimg[i].begin(), iimg[i].end(), cbf)->first);
                hi = std::max(hi, std::max_element(iimg[i].begin(), iimg[i].end(), cbs)->second);
            }

            std::vector <std::vector <pxCoord> > q (hi-lo+1,(std::vector<pxCoord>()));
            int qElems = 0;

            q[iimg[0][0].lower - lo].push_back(std::make_pair(0,0)); ++qElems;
            seen[0][0] = true;
            uchar level = iimg[0][0].first;

            for (; qElems; ){
                pxCoord cur = specialPop(q, qElems, lo, level);
                eimg.at<uchar>(cur.Y, cur.X) = level;
                order.push_back(cur);

                pxDirected curDir = make_pxDirected(cur, 1, fl::pxType::regular);
                for (pxCoord ngb = nextCoord(curDir); ; ngb = nextCoord(curDir)){
                    if (!coordOk(ngb, iimg.size(), iimg.back().size()) || seen[ngb.X][ngb.Y]){
                        if (nextDir(curDir))
                            continue;
                        else
                            break;
                    }
                    seen[ngb.X][ngb.Y] = true;
                    specialPush(q, qElems, lo, level, ngb, iimg[ngb.X][ngb.Y]);
                    if (!nextDir(curDir))
                        break;
                }
            }
        }

        void unInterpolateParent(std::vector<std::vector <pxCoord> > parentOld, const cv::Mat &ub,
                            std::vector<std::vector <pxCoord> > &parentNew){

            const pxCoord dummy = make_pxCoord(-1,-1);
            std::vector <std::vector<pxCoord> > repCanon(parentOld.size(), std::vector<pxCoord>(parentOld[0].size(), make_pxCoord(-1,-1)));

            const int oX = ((parentOld.size()+1)>>2)-2;
            const int oY = ((parentOld[0].size()+1)>>2)-2;

            parentNew.clear();
            parentNew.resize(oX, std::vector<pxCoord>(oY));

            for (int i=5, szi = parentOld.size()-5; i < szi; i+=4){ // setting the replacement canons
                for (int j=5, szj = parentOld[i].size()-5; j < szj; j+=4){
                    if (ub.at<uchar>(j,i) != ub.at<uchar>(parentOld[i][j].Y, parentOld[i][j].X) || // canon element
                        parentOld[i][j] == make_pxCoord(i,j)){ // tree root
                            continue;
                    }
                    if (!isKhalimskyOriginalPixel(parentOld[i][j], oX, oY) &&
                        repCanon[parentOld[i][j].X][parentOld[i][j].Y] == dummy){
                            repCanon[parentOld[i][j].X][parentOld[i][j].Y] = make_pxCoord(i,j);
                    }
                }
            }
            for (int i=5; i < ((oX<<2) + 5); i+=4){
                for (int j=5; j < ((oY<<2) + 5); j+=4){
                    const pxCoord &pold = parentOld[i][j];
                    pxCoord &pnew = parentNew[(i-5)>>2][(j-5)>>2];
                    if (repCanon[pold.X][pold.Y] == dummy){ // no replacement
                        pnew = make_pxCoord((pold.X-5)>>2, (pold.Y-5)>>2);
                    }
                    else if (repCanon[pold.X][pold.Y] == make_pxCoord(i,j)){ // if current is becoming canon
                        const pxCoord ppold = parentOld[pold.X][pold.Y];
                        if (repCanon[ppold.X][ppold.Y] == dummy) // point to what the old canon was pointing to
                            pnew = make_pxCoord((ppold.X-5)>>2, (ppold.Y-5)>>2);
                        else // ba careful if that also got replacement
                            pnew = make_pxCoord((repCanon[ppold.X][ppold.Y].X-5)>>2,(repCanon[ppold.X][ppold.Y].Y-5)>>2);
                    }
                    else
                        pnew = make_pxCoord((repCanon[pold.X][pold.Y].X-5)>>2, (repCanon[pold.X][pold.Y].Y-5)>>2);
                }
            }
        }
    }
}
