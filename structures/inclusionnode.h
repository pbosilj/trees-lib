#ifndef INCLUSIONNODE_H
#define INCLUSIONNODE_H

#include "node.h"

#include <map>
#include <iostream>

namespace fl {


/// \class InclusionNode
///
/// \brief `InclusionNode` is a single element of any inclusion
/// component tree.
///
/// The concrete class `InclusionNode`. The constraint on the
/// contents of this type of `Node` depend on whether it is a leaf
/// or an inner `Node`. `Node`s in any partitioning tree follow these
/// rules:
/// - leaf `Node`: contains one or more elements (pixels)
/// - inner `Node`: has at least a single element (pixel) not contained
///     in any of it's children.
class InclusionNode : public fl::Node
{

public:
    /// \brief Constructor assigning pixels to the `InclusionNode`.
    InclusionNode(const std::vector< std::pair< int, int > >& S);

    /// \brief Constructor assigning both pixels and all children to the `InclusionNode`.
    InclusionNode(const std::vector< std::pair< int, int > >& S,
        const std::vector< fl::InclusionNode* >& children);

    /// \brief Copy constructor
    InclusionNode(const InclusionNode &other);

    /// \brief Way to access a static copy of an `InclusionNode` at a
    /// certain level which contains no elements.
    static InclusionNode & dummy(const double &level);

    /// \brief Class destructor
    virtual ~InclusionNode() {}

    /// \brief Assign a \p level to the `Node`.
    virtual void assignLevel(const double &level);

    /// \brief \copybrief Node::deleteChild().
    bool deleteChild(int childIndex);

protected:
    /// \brief Check if the constraints are valid for a `InclusionNode`.
    virtual void checkConstraints(void) const;

private:
    static int counter;
    int myNumber;

    bool isDummy;
    static std::map <double, InclusionNode *> dummies;
    InclusionNode(const double &a);
};

}


#endif // INCLUSIONNODE_H
