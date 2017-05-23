 #include "pixels.h"

namespace fl{
    pxCoord nextCoord (pxDirected &dpx){
        pxCoord rValue = dpx.coord;
        if (dpx.pxtype == regular){
            if (dpx.pxdir <= 1){
                --rValue.X;
            }
            else if (dpx.pxdir <= 2){
                ++rValue.Y;
            }
            else if (dpx.pxdir <= 4){
                ++rValue.X;
            }
            else if (dpx.pxdir <= 8){
                --rValue.Y;
            }
        }
        else if (dpx.pxtype == dual){

            if (! ((dpx.coord.X %2)-(dpx.coord.Y%2)) ) // (odd,odd) or (even,even) -- error
                return dpx.coord;
            else if (dpx.coord.X % 2){ // (odd,even) -> vertical
                if (dpx.pxdir <= 1<<0){
                    rValue.X -= 2;
                }
                else if (dpx.pxdir <= 1<<1){
                    --rValue.X; ++rValue.Y;
                }
                else if (dpx.pxdir <= 1<<2){
                    ++rValue.X; ++rValue.Y;
                }
                else if (dpx.pxdir <= 1<<3){
                    rValue.X += 2;
                }
                else if (dpx.pxdir <= 1<<4){
                    ++rValue.X; --rValue.Y;
                }
                else if (dpx.pxdir <= 1<<5){
                    --rValue.X; --rValue.Y;
                }
            }
            else { // (even,odd) -> horizontal
                if (dpx.pxdir <= 1<<0){
                    --rValue.X; ++rValue.Y;
                }
                else if (dpx.pxdir <= 1<<1){
                    rValue.Y += 2;
                }
                else if (dpx.pxdir <= 1<<2){
                    ++rValue.X; ++rValue.Y;
                }
                else if (dpx.pxdir <= 1<<3){
                    ++rValue.X; --rValue.Y;
                }
                else if (dpx.pxdir <= 1<<4){
                    rValue.Y -= 2;
                }
                else if (dpx.pxdir <= 1<<5){
                    --rValue.X; --rValue.Y;
                }
            }
        }
        if (dpx.pxdir < 1) dpx.pxdir = 2;
        else dpx.pxdir <<= 1;
        return rValue;

    }
    bool coordOk (const pxCoord &coord, int imgCols, int imgRows){
        if (coord.X < 0 || coord.Y < 0)
            return false;
        if (coord.X >= imgCols || coord.Y >= imgRows)
            return false;
        return true;
    }
    bool nextDir(const pxDirected &dpx){
        if (dpx.pxtype == regular)
            return (dpx.pxdir <= 1<<3);
        else if (dpx.pxtype == dual)
            return (dpx.pxdir <= 1<<5);
        else
            return false;
    }
}
