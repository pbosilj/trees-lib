#ifndef PIXELS_H_INCLUDED
#define PIXELS_H_INCLUDED

#include <utility>

#define X first
#define Y second
#define coord first
#define pxdir second.first
#define pxtype second.second

#define make_pxCoord(x,y) std::make_pair((x), (y))
#define make_pxDirected(px,dir,type) std::make_pair((px),std::make_pair((dir),(type)))

namespace fl{

    /// \brief Type of pixel. Regular -- 4 connected image graph, Dual -- Inverted edges and nodes of the image graph.
    enum class pxType {regular, dual};

    /// \brief Pixel coordinates. A pair of integer coordinates.
    typedef std::pair <int, int> pxCoord;

    /// \brief A directed pixel. Made of coordinates, the direction of the pixel, and `pxType`.
    typedef std::pair <pxCoord, std::pair <int, pxType> > pxDirected;

    /// \brief Get the coordinates of the next pixel from a directed pixel.
    pxCoord nextCoord (pxDirected &dpx);

    /// \brief Check if the pixel coordinates exist within the image
    bool coordOk (const pxCoord &coord, int imgCols, int imgRows);

    /// \brief Check if the next pixel exists in a directed pixel.
    bool nextDir(const pxDirected &dpx);
}

#endif // PIXELS_H_INCLUDED
