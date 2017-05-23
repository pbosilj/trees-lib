#ifndef PARTITIONINGNODE_H
#define PARTITIONINGNODE_H

#include "node.h"
#include <iostream>

namespace fl {

/// \class PartitioningNode
///
/// \brief `PartitioningNode` is a single element of any partitioning
/// component tree.
///
/// The concrete class `PartitioningNode`. The constraint on the
/// contents of this type of `Node` depend on whether it is a leaf
/// or an inner `Node`. `Nodes` in any partitioning tree follow these
/// rules:
/// - leaf `Node`: contains one or more elements (pixels)
///     -# all the leaf `Node`s in a single partitioning tree must
///         cover all the elements (pixels) of the image used in
///         construction
/// - inner `Node`: comprises only elements of it's children `Node`s
///     -# constructed, a `PartitioningNode` can not contain any of
///         it's own elements if it has any children
///     -# the number of children of a `PartitioningNode` is always
///         /(2/) or more.
///
/// \note The constraints of a `PartitioningNode` are only partially
/// ensured by the structure for complexity and performance reasons.
class PartitioningNode : public fl::Node{

    protected:
        /// \brief Check if the constraints are valid for a `PartitioningNode`.
        virtual void checkConstraints(void) const;

    public:
        /// \brief Constructor assigning pixels to the `PartitioningNode`.
        PartitioningNode(const std::vector< std::pair< int, int > >& S);

        /// \brief Constructor assigning both pixels and all children to the `PartitioningNode`.
        PartitioningNode(const std::vector< std::pair< int, int > >& S,
                 const std::vector< fl::PartitioningNode* >& children);

        /// \brief Constructor assigning all the children to this `PartitioningNode`
        PartitioningNode(const std::vector< fl::PartitioningNode* >& children);

        /// \brief Class destructor
        virtual ~PartitioningNode() { }
    private:
};

}

#endif // PARTITIONINGNODE_H
