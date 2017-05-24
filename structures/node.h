/// \file structures/node.h
/// \author Petra Bosilj

#ifndef NODE_H
#define NODE_H

#include "../misc/pixels.h"

#include <opencv2/core/core.hpp>

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <map>
#include <set>

#include <cmath>

/// \namespace fl
/// The main namespace of the project.

namespace fl{

#if 1
    class Attribute;
    template <typename AttType> class TypedAttribute;
    class AnyPatternSpectra2D;
#endif

/// \class Node
///
/// \brief Abstract `Node`, single element of a component tree
///
/// This class provides general functionality of a `Node`, provides
/// access and manipulation functions. Each `Node` contains it's
/// own elements (pixels) and the elements of it's children. No
/// assumption on own elements or number of children is made here.
///
/// \note To use, a concrete class PartitioningNode or
/// InclusionNode needs to be used.
///

    class Node{
        public:
            /// \brief Constructor assigning pixels to the `Node`.
            Node (const std::vector <std::pair <int, int> > &S);

            /// \brief Constructor assigning both pixels and all children to the `Node`.
            Node (const std::vector <std::pair <int, int> > &S,
                const std::vector <Node* > &children);

            /// \brief Class destructor.
            virtual ~Node() {}

            /// \brief Copy constructor.
            Node(const Node& other);

            //basic:

            /// \brief Get a parent or ancestral `Node`.
            virtual Node *parent(int depth=1) const;

            /// \brief Get a parent or ancestral `Node` determined by it's
            /// size.
            Node *parentBySize(double perc, int oriSize=0, int jumps=0);

            /// \brief Get a list of ancestral `Node`s of a `Node`, up to the
            /// ancestral `Node` larger than a size limit.
            void parentBySize(double perc, std::vector<Node *> &pathBetween, int oriSize=0, int jumps=0);

            /// \brief Check if the `Node` is a root `Node`.
            bool isRoot() const;

            /// \brief Assign a child `Node`.
            void addChild(Node *ch);

            /// \brief Add a single element to this `Node`.
            void addElement(const std::pair<int, int> &px);

            /// \brief Get all the pixels of the `Node`.
            void getElements(std::vector <std::pair <int, int> > &px) const;

            /// \brief Get all self-pixels of the `Node`.
            const std::vector <std::pair <int, int> > &getOwnElements() const;

            /// \brief Does the pixel exist in the `Node`.
            bool hasElement(const std::pair <int, int> &px) const;

            /// \brief Does the pixel exist directly in the `Node`.
            bool hasOwnElement(const std::pair <int, int> &px) const;

            /// \brief Assign a \p level to the `Node`.
            virtual void assignLevel(const double & level);

            /// \brief Get the level of a `Node`.
            virtual const double & level() const;

            /// \brief Assign a scalar gray level to the `Node` and all its children.
            template <class Function>
            Node *assignGrayLevelRec(Function f);

            /// \brief Assign a vectorial gray level to the `Node` and all its children.
            template <class Function>
            Node *assignHyperLevelRec(Function f);

            /// \brief Get the scalar gray level of a `Node`.
            virtual const int &grayLevel() const;

            /// \brief Get the vectorial gray level of a `Node`.
            virtual const std::vector<int> &hyperGraylevel() const;

            // utility;

            /// \brief Color the corresponding elements in an image with a flat color.
            void colorSolid(cv::Mat &img, const cv::Vec3b &value) const;

            /// \brief Color the corresponding elements in an image (using the graylevel assigned to the node).
            void colorMe(cv::Mat &img) const;

            /// \brief Color the corresponding elements in a multichannel
            /// image.
            void colorThem(std::vector <cv::Mat> &imgs) const;

            /// \brief The number of elements associated to the `Node`.
            int elementCount(void);

            /// \brief The number of `Node`s in the subtree of this `Node`.
            int nodeCount(void);

#if 1
            // attributes access:
            /// \brief Retrieve an `Attribute` of a specific type.
            Attribute *getAttribute(const std::string &name) const;

            /// \brief Retrieve children `Attribute`s of a specific type.
            void getChildrenAttributes(const std::string &name, std::vector <Attribute *> &cat) const;

            // pattern spectra access:
            /// \brief Retrieve an `PatternSpectra2D` of a specific type.
            AnyPatternSpectra2D *getPatternSpectra2D(const std::string &name) const;

            /// \brief Retrieve children `PatternSpectra2D`s of a specific type.
            void getChildrenPatternSpectra2D(const std::string &name, std::vector <AnyPatternSpectra2D *> &cps) const;
#endif
            // friends and algorithms:

            friend std::pair<int, bool> mserRecursive(Node *root, const int deltaLvl, std::vector <Node * > &mserOrder, int minArea, double maxVariation);
            friend std::pair<int, bool> mserRecursive(Node *root, const int deltaLvl, std::vector <Node * > &mserOrder, std::vector <std::pair<double, int> > &div,
                           int minArea, double maxVariation);

            friend Node *constructRecursively(Node *root, std::vector <std::vector <char> > &seen);

            template <typename Compare>
            friend Node *maxTreeNister(const cv::Mat &img, Compare pxOrder = std::less<int>(), pxType curType = regular);
            friend std::pair <int, bool> areaDiff(Node *root, const int deltaLvl);

            friend class ImageTree;


    protected:

            /// \brief The list of own elements.

            /// The list of the `Node` not belonging to any of its children.
            std::vector<std::pair<int, int> > _S;

            /// The list of children of the `Node`.
            std::vector<Node *> _children;

            /// \brief The pointer to the parent `Node`.
            /// \remark Equals to `NULL` or `this` if the `Node`
            /// is the root of the tree (has no parent).
            Node *_parent;

            /// \brief The level of the `Node` in the hierarchy.
            /// \remark Can be any scalar value, but for some applications
            /// only the integral part may be used.
            double _level;

            /// \brief The gray level of the `Node`.
            ///
            /// In case of monochannel images.
            int _grayLevel;

            /// \brief Vectorial gray levels of the `Node`
            ///
            /// In case of multichannel images. One gray level per channel.
            std::vector < int > _hgrayLevels;

            /// \brief Indicator value of size.

            /// Can be:
            /// - `true` if the number of elements is precalculated
            /// - `false` if calculation is needed when calling `elementCount()`
            bool sizeKnown;

            /// \brief Indicator value for node count.
            ///
            /// Can be:
            /// - `true` if the number of nodes in a subtree is precalculated
            /// - `false` if calculation is needed when calling `nodeCount()`
            bool ncountKnown;

            /// \brief Delete a child `Node`.
            void deleteChild(int childIndex);

            /// \brief Check if the constraints according to `Node` type are valid.
            ///
            /// Virtual function to be implemented for each specific `Node` type.
            virtual void checkConstraints(void) const = 0;

            /// \brief Set the parent of a `Node`
            void setParent(Node *p);

            /// \brief Get all the `Node`s from the whole subtree of the `Node`.
            void getAllDescendants(std::vector <Node *> &allNodes);

#if 1
            // attribute adding:
            /// \brief Assign a specific `Attribute` to this `Node`.
            Attribute *addAttribute(Attribute *nat, const std::string &name);

            /// \brief Delete a specific `Attribute` from this `Node`.
            Attribute *deleteAttribute(const std::string &name);

            /// \brief Check if a specific `Attribute` is assigned to this `Node`.
            bool attributeExists(const std::string &name) const;

            // pattern spectra adding
            /// Assign a specific `PatterSpectra2D` to this `Node`.
            bool addPatternSpectra2D(AnyPatternSpectra2D *aps, const std::string &name);

            /// Delete a specific `PatternSpectra2D` from this `Node`.
            AnyPatternSpectra2D * deletePatternSpectra2D(const std::string &name);
#endif

            // utility protected:

            /// \brief Get the minimal and maximal levels present in the sub-tree.
            std::pair<double, double> minMaxLevel(void) const;

            /// \brief Print the subtree belonging to the `Node`.
            void printElements(std::ostream &outStream = std::cout, int depth = 0) const;

#if 1
            /// \brief Get the minimal and maximal `Attribute` value present in the sub-tree.
            template <typename TAT>
            std::pair<typename TAT::attribute_type, typename TAT::attribute_type> minMaxAttribute(void) const;

            /// \brief Print the subtree belonging to the `Node` and their value of an `Attribute`.
            template <typename AT>
            void printElementsWithAttribute(std::ostream &outStream = std::cout, int depth = 0) const;

//            // Still not sure if I need it or not.
//            template<class AT1, class AT2>
//            void printPartialWithAttribute(std::set <Node *> &toPrint,int cols, int rows);

#endif

    private:

#if 1
        std::map <std::string, AnyPatternSpectra2D *> patternspectra;
        std::map <std::string, Attribute *> attributes;
        std::map <std::string, int> attributeCount;
#endif

        const cv::Mat *referenceImg;

        /// The number of elements in this `Node`.
        int size;

        /// The number of `Node`s in the subtree.
        int ncount;

  };

}

#include "node.tpp"

#endif // NODE_H
