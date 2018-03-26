#include "inclusionnode.h"

#include <string>

using namespace fl;

int InclusionNode::counter = 0;

std::map <double, InclusionNode *> InclusionNode::dummies = std::map<double, InclusionNode *>();

InclusionNode::InclusionNode(const InclusionNode &other) : Node(other), myNumber(other.myNumber), isDummy(other.isDummy){ }

/// Constructor initializing internal `InclusionNode` elements (pixels).
///
/// \param S An array of pixel coordinates
///
/// \remark Calls `checkConstraints()`.
InclusionNode::InclusionNode(const std::vector< std::pair< int, int > >& S) : Node (S) {
    this->isDummy = false;
    this->checkConstraints();
}

/// Constructor initializing internal `InclusionNode` elements (pixels),
/// as well as the list of children.
///
/// \param S An array of pixel coordinates.
/// \param children An array of pointers to children of type `InclusionNode`.
///   \remark All children in \p children should be of type `InclusionNode`
///   \remark Calls `checkConstraints()`.
InclusionNode::InclusionNode(const std::vector< std::pair< int, int > >& S,
    const std::vector< fl::InclusionNode* >& children)
        : Node (S, std::vector <fl::Node *>(children.begin(), children.end())){
    this->isDummy = false;
    this->checkConstraints();
}

// private constructor used for producing singleton dummies
InclusionNode::InclusionNode(const double &level)
  : Node(std::vector <std::pair <int, int> >()){

  this->isDummy = false;
  this->assignLevel(level);
  this->isDummy = true;
}

/// Function used to obtain a reference to a singleton copy of an `InclusionNode`
/// at a certain level.
///
/// \remark A separate singleton is created for every different \p level requested.
///
/// \param level The level of the singleton `InclusionNode` requested.
///
/// \return A reference to the singleton `InclusionNode` for the requested \p level.
InclusionNode& InclusionNode::dummy(const double &level){
    std::map<double, InclusionNode *>::iterator it;
    if ((it=InclusionNode::dummies.find(level)) == InclusionNode::dummies.end()){
        it = InclusionNode::dummies.insert(std::pair<double, InclusionNode *>(level, new InclusionNode(level))).first; // <- 3 hidden calls to copy constructor :/
    }
    return *(it->second);
}

/// \remark Even though most hierarchies operate with levels of integral
/// type (i.e. integers), the \p level can be any scalar (double value).
///
/// \note Depending on the action performed, only the integral part of
/// the level might be considered.
///
/// \param level The new level to be assigned.
///
/// \remark Takes care if the current `InclusionNode` is a dummy singleton.
void InclusionNode::assignLevel(const double &level){

    double old = this->level();

    fl::Node::assignLevel(level);

    if (this->isDummy && level != old){
        std::map<double, InclusionNode *>::iterator it = InclusionNode::dummies.find(old);
        InclusionNode::dummies.insert(std::make_pair(level, it->second));
        InclusionNode::dummies.erase(it);
    }
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
bool InclusionNode::deleteChild(int childIndex){
    return Node::deleteChild(childIndex);
}

/// Checks some of the constraints of the `InclusionNode`, testing it
/// for validity.
///
/// The constraints tested:
/// - completely empty `InclusionNode` (not allowed)
/// - the `InclusionNode` contains no own elements (pixels) (not allowed)
///
/// \remark Does not test for:
/// - coverage of the whole Image by the root elements (operation on
///     a single `InclusionNode` can not examine other `Node`s in the
///     hierarchy)
/// - connectedness of the whole region (complexity reasons)
void InclusionNode::checkConstraints(void) const {
    if (! this->isDummy && this->_S.empty())
        throw std::string("constraints wrong in inclusion");
}


