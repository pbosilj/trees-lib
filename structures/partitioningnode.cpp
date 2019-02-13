#include "partitioningnode.h"

#include <string>
#include <iostream>

using namespace fl;

/// Constructor initializing internal `PartitioningNode` elements (pixels).
///
/// \param S An array of pixel coordinates
///
/// \remark Calls `checkConstraints()`.
PartitioningNode::PartitioningNode(const std::vector< std::pair< int, int > >& S) : Node(S) {
    this->checkConstraints();
}

/// Constructor initializing internal `PartitioningNode` elements (pixels),
/// as well as the list of children.
///
/// \param S An array of pixel coordinates.
/// \param children An array of pointers to children of type `PartitioningNode`.
///   \remark All children in \p children should be of type `PartitioningNode`
///   \remark Calls `checkConstraints()`.
PartitioningNode::PartitioningNode(const std::vector< std::pair< int, int > >& S,
				   const std::vector< PartitioningNode* >& children)
    : Node(S, std::vector <fl::Node *>(children.begin(), children.end())){
    this->checkConstraints();
}

/// Constructor initializing the list of children of the current `PartitioningNode`.
///
/// \param children An array of pointers to children of type `PartitioningNode`.
///   \remark All children in \p children should be of type `PartitioningNode`
PartitioningNode::PartitioningNode(const std::vector< PartitioningNode* >& children)
    : Node(std::vector<std::pair<int, int> >(),
	   std::vector <fl::Node *>(children.begin(), children.end())){
    this->checkConstraints();
}

/// Deletes a child from this node.
/// This function will assign all the elements (pixels) directly belonging
/// to the child `Node` being deleted to the parent `Node`. Also, all children
/// of the child `Node` become the children of the parent `Node`.
///
/// \param childIndex The index of the child to be deleted.
///
/// \return succcess of the `delete` operation.
/// \remark This function invalidates `Node::ncountKnown` and triggers a
///  recalculation at next call to `nodeCount()`
bool PartitioningNode::deleteChild(int childIndex){
    if ((int)this->_children.size() > childIndex &&
     ! ((PartitioningNode*)this->_children[childIndex])->_children.empty())
        return Node::deleteChild(childIndex);
    return Node::deleteChild(childIndex);
}

/// Checks some of the constraints of the `PartitioningNode`, testing it
/// for validity.
///
/// The constraints tested:
/// - completely empty `PartitioningNode` (not allowed)
/// - `PartitioningNode` containing both children `Node`s as well as
///     associated pixels (not allowed)
/// - if there are no elements (pixels), the number of children must
///     be greater or equal two
///
/// \remark Does not test for:
/// - coverage of the whole Image by the leaf elements (operation on
///     a single `PartitioningNode` can not examine other `Node`s)
/// - connectedness of the whole region (complexity reasons)
void PartitioningNode::checkConstraints(void) const
{
    if (this->_S.empty() && this->_children.size() < 2){
        throw std::string("constraints wrong in partitioning: empty node or inner node with less than 2 child nodes");
    }
    else if (!this->_S.empty() && !this->_children.empty()){
        std::cout << "S.size() " << this->_S.size() << " children.size() " << this->_children.size() << std::endl;
        throw std::string("constraints wrong in partitioning: pixels and children together");
    }
    //else if (this->_S.empty() && !this->_children.size() < 2){
    //    throw std::string("constraints wrong in partitioning: must have at least 2 child nodes");
    //}
}
