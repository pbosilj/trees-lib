/// \file structures/imagetree.tpp
/// \author Petra Bosilj

#ifndef TPP_IMAGETREE
#define TPP_IMAGETREE

#include "imagetree.h"

#include "areaattribute.h"

#include <set>

#define markedSelf first
#define markedBranch second

namespace fl{

/// Filters the tree by evaluating a `Predicate` on the level assigned
/// to the `Node`, applying the selected filtering rule.
/// Depending on the filtering rule, the gray level of certain `Node`s
/// could change, however the level of all the unfiltered `Node`s remains
/// unchanged.
///
/// \param predicate The functor object which operates on the `Node`s.
/// \note cf. the class `Predicate` to see the correct form of this
/// functor.
///
/// \param rule Filtering rule to be used. Options are:
///     - 0 = DIRECT FILTERING. No sub-tree adjustment.
///     - 1 = SUBTRACTIVE FILTERING. Contrast adjustment on the subtrees.
///     - 2 = MAX FILTERING. Sub-trees removed (collapsed).
///     - 3 = SOFT DIRECT. Like direct, but soft.
///     - 4 = SOFT SUBTRACTIVE. Like subtractive, but soft.
///     - 5 = SOFT MAX. Like max, but soft.
/// \note Soft filtering rules do not return `false` upon unsuccessfull
/// delete, but rather sets the gray level of the child `Node` to that
/// of its parent, thus `soft` deleting it.
///
/// \param root (discouraged) Should be omitted if performing the filtering
/// on the whole `ImageTree` (intended use). `Node *` to the subtree which
/// should be filtered.
/// If set to anything other than the root of the tree, it will only filter
/// the subtree.
///
/// \note Big implementation overlap with `filterTreeByAttributePredicate`.
/// Should figure out a better way to reuse code rather than copy it.
template<class Function>
void ImageTree::filterTreeByLevelPredicate(Function predicate, int rule, Node *root){
    if (root == NULL){
        filterTreeByLevelPredicate(predicate, rule, this->_root);
        return;
    }

    root->_propagatingHyperContrast.resize(root->hyperGraylevel().size(), 0);

    bool singleDeletionOK = true;
    bool survivingChild = false;

    std::vector<Node *> &chi = root->_children;
    for (int i=0, szi = chi.size(); i < szi; ++i){
        if (predicate(chi[i]->level(), root->level()) == false){
            if (singleDeletionOK &= root->deleteChildWithRule(i,rule)){
                --i;
                szi = chi.size();
            }
        }
        else
            survivingChild=true;
    }

    if (!singleDeletionOK ||    // If I tried deleting one child, but it's brothers wouldn't let me: delete them all (partitioning trees; rules 0,1,2)
        !survivingChild){        // Non-satisfying children surivive-collapse individually with rule = 3. If they are all such, all can be deleted instead.
            root->collapseSubtree();
    }
    else{
        for (int i=0, szi = chi.size(), szj = root->_propagatingHyperContrast.size(); i < szi; ++i){
            // contrast propagation
            chi[i]->_propagatingContrast += root->_propagatingContrast;

            // contrast propagation for hyperspectral images. won't do anything if not set.
            chi[i]->_propagatingHyperContrast.resize(chi[i]->hyperGraylevel().size(), 0);
            for (int j=0; j < szj; ++j){
                chi[i]->_propagatingHyperContrast[j] += root->_propagatingHyperContrast[j];
            }
            filterTreeByLevelPredicate(predicate, rule, chi[i]);
        }
    }
    root->_grayLevel += root->_propagatingContrast;
    for (int i=0, szi = root->_hgrayLevels.size(); i < szi; ++i)
        root->_hgrayLevels[i] += root->_propagatingHyperContrast[i];
    root->_propagatingContrast = 0;
    root->_propagatingHyperContrast.clear();
}

#if 1

/// Assigns a specific `TypedAttribute` to all the `Node`s in this `ImageTree`.
/// This ensures reserving the memory for any values that need to be stored for
/// the specific `TypedAttribute` calculation.
///
/// \note A copy of the attribute AT will be created at every `Node`, and
/// `Node::addAttribute()` will be called at each `Node`.
///
/// \note A corresponding call to `deleteAttributeFromTree<AT>()` should be made
/// for every call to this function.
///
/// \tparam AT Specifies the `TypedAttribute` to be assigned (e.g. `AreaAttribute`)
///
/// \param settings A pointer to the settings to be used throughout the hierarchy. The
/// same settings will be assigned to the `Attribute` at every `Node`.
/// The settings corresponding to the right attribute should be used (e.g. when using
/// `AreaAttribute`, `AreaSettings *` should be used).
///
/// \param deleteSettings (optional, default = `true`) Determines whether the \p settings
/// will be deleted after this call to `addAttributeToTree()` or not (allows the creation
/// with the `new` function and takes care of the `delete` if set to `true`). If the
/// `AttributeSettings *` \p settings wants to be preserved after this call for further
/// outside use, parameter should be set to `false`.
// where AT is Attribute
template<class AT>
void ImageTree::addAttributeToTree(AttributeSettings *settings, bool deleteSettings) const{
    this->addAttributeToNode<AT>(this->_root, settings);
    if (deleteSettings)
        delete settings;
}

/// The function will take care of multiple assigned
/// copies of `Attribute` to the `ImageTree`.
///
/// \note One call to this function must be made
/// for each call to `addAttributeToTree<AT>()`.
///
/// \note This should eventually take care of the `AttributeSettings`
/// associated with each assigned copy.
///
/// \remark This function is calls `Node::deleteAttribute()` for
/// every `Node` in the `ImageTree` and ensures the correctness
/// of its input parameters.
///
/// \tparam AT Specifies the `TypedAttribute` to be deleted (e.g. `AreaAttribute`)
//where AT is Attribute
template<class AT>
void ImageTree::deleteAttributeFromTree() const{
    this->deleteAttributeFromNode<AT>(this->_root);
}

/// Checks if the specific `TypedAttribute` requested is assigned to the
/// `Node`s of this `ImageTree`.
///
/// \tparam AT Specifies the `TypedAttribute` to be queried (e.g. `AreaAttribute`).
///
/// \return `true` if the `TypedAttribute` `AT` is assigned to the `ImageTree`
/// and it's `Node`s, `false` otherwise.
//where AT is attribute
template<class AT>
bool ImageTree::isAttributeInTree() const{
    return this->_root->attributeExists(AT::name);
}

/// \tparam TAT A specific `Attribute` class to be used in the search. Any specific
/// `TypedAttribute` where the `TypedAttribute::value()` returns a scalar types can be used
/// (e.g. `AreaAttribute`).
///
/// \return A pair made of the minimum and maximum level value of the specific
/// `TypedAttribute` among all the `Node`s in this `ImageTree`.
// Where TAT is optional
template <typename TAT>
std::pair<typename TAT::attribute_type, typename TAT::attribute_type> ImageTree::minMaxAttribute(void) const{
    return this->_root->minMaxAttribute<TAT>();
}

/// \tparam AT The specific `TypedAttribute` which is to be printed along the
/// subtree. The `TypedAttribute::value()` must be printable to a stream.
///
/// \note All the parameters are optional.
///
/// \param outStream The output stream to which the data should be
/// printed. By default, it equals to `std::cout`.
///
/// \remark Prints the number and a list of elements (pixels) associated
/// directly to each `Node`, as well as the `Node`'s level and
/// `TypedAttribute::value()` associated to each node.
//where AT is Attribute
template<class AT>
void ImageTree::printTreeWithAttribute(std::ostream &outStream) const {
    this->_root->printElementsWithAttribute<AT>();
}

/// \tparam AT The specific `TypedAttribute` which is to be analysed along the
/// subtree. The `TypedAttribute::value()` must be convertable to a double.
///
/// \param node The `Node` marking the start of the analysis. All the `Node`s
/// between this one and the root are output.
/// \param attributeValues The output parameter, to be filled with the values
/// of level and `Attribute` assigned to each node along the branch.
template <class AT>
void ImageTree::analyseBranch(const fl::Node *node, std::vector <std::pair<int, double> > &attributeValues) const{
    if (! this->isAttributeInTree<AT>())
        return;
    for (const Node *cur = node; !cur->isRoot(); cur = cur->parent()){
        attributeValues.emplace_back(cur->level(), (double)((AT*)cur->getAttribute(AT::name))->value());
        //out << cur->level() << " " << ((AT*)cur->getAttribute(AT::name))->value() << std::endl;
    }
}


/// TODO: check correctness, implement three different summation rules
/// rule = 0 -> number of regions
/// rule = 1 -> difference with parent * number of pixels

/// rule = 1 -> number of pixels  : OLD RULE, DEPRECATED, REFACTORING
template<class ATT>
void ImageTree::calculateGranulometryHistogram(std::map<double, int> &GCF, int rule, const fl::Node *_root) const{
    if (_root == NULL){
        this->calculateGranulometryHistogram<ATT>(GCF, rule, this->root());
        return;
    }

    GCF.clear();

    if (rule > 0)
        this->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());

    std::vector <const fl::Node *> toProcess(1, _root);

    const Node *cur;
    do{
        cur = toProcess.back();

        double attributeValue = (double)((ATT*)cur->getAttribute(ATT::name))->value();

        // insert attributeValue in GCF
        switch (rule){
            case 0: // number of regions -> sum 1 for every region
                ++GCF[attributeValue]; // ok, since if value does not exist, it will be initialized to 0
                break;
            // "area" not a real content metric; it's a difference metric
            //case 1: // number of pixels -> sum the area of the region
                // ok, since if value does not exist, it will be initialized to 0
                // this might not work as a measure at all
                // double parentValue = (double)((ATT*)(cur->parent())->getAttribute(ATT::name))->value();
                //GCF[attributeValue] += ((fl::AreaAttribute *)cur->getAttribute(fl::AreaAttribute::name))->value();
                //break;
            //case 2:
            case 1:
            default:
                if (! cur->isRoot()){
                    int nodeLvl = cur->grayLevel();
                    int parentLvl = cur->parent()->grayLevel();
                    GCF[attributeValue] += ((fl::AreaAttribute *)cur->getAttribute(fl::AreaAttribute::name))->value()*std::abs(nodeLvl-parentLvl);
                }
        }

        toProcess.pop_back();

        for (int i=0, szi = cur->_children.size(); i < szi; ++i)
            toProcess.push_back(cur->_children[i]);
    }while(!toProcess.empty());

    if (rule > 0)
        this->deleteAttributeFromTree<fl::AreaAttribute>();

    //std::partial_sum(GCF.begin(), GCF.end(), GCF.begin(),
    //                 [](const std::pair<double, int>& x, const std::pair<double, int>& y){return std::make_pair(y.first, x.second + y.second);});
}

/// \param nodes A vector of nodes for which the output is written
/// \param out An output stream to which to write the output.
/// \tparam AT the `Attribute` which to output to a file.
/// \note the `Attribute` AT needs to be assigned to the tree externally.
/// regions.
template <class AT>
void ImageTree::writeAttributesToFile(const std::vector <Node *> &nodes, std::ostream &out) const{
    //this->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
    for (int i=0, szi = nodes.size(); i < szi; ++i){
        double value = ((AT*)nodes[i]->getAttribute(AT::name))->value();
        out << value << std::endl;
    }
    //this->deleteAttributeFromTree<fl::AreaAttribute>();
}

/// If the specific `TypedAttribute` `AT` is assigned to this `ImageTree`,
/// return the pointer to the `AttributeSettings` used (common settings
/// are always used for all the `Node`s in the `ImageTree`).
///
/// \tparam AT Specifies the `TypedAttribute` to be queried (e.g. `AreaAttribute`).
///
/// \return `NULL` if the `TypedAttribute` `AT` is not assigned to this
/// `ImageTree`. Otherwise, returns `AttributeSettings *` used for `AT`.
/// (The return value should be appropriately cast, e.g. to `AreaSettings *`
/// in case `AreaAttribute` is used for the query.
template<class AT>
AttributeSettings * ImageTree::getAttributeSettings() const{
    if (!(this->isAttributeInTree<AT>())){
        std::cerr << "not in tree " << std::endl;
        return NULL;
    }
    return this->_root->getAttribute(AT::name)->getSettings(); // blah
}

/// If the specific `TypedAttribute` `AT` is assigned to this `ImageTree`,
/// change the `AttributeSettings` used for this `Attribute` in the `ImageTree`
/// (common settings are always used for all the `Node`s in the `ImageTree`).
///
/// \tparam AT Specifies the `TypedAttribute` whose settings are to be changed
/// (e.g. `AreaAttribute`).
///
/// \param nsettings A pointer to the new settings to be used throughout the hierarchy. The
/// same settings will be assigned to the `Attribute` at every `Node`.
/// The settings corresponding to the right attribute should be used (e.g. when using
/// `AreaAttribute`, `AreaSettings *` should be used).
///
/// \param deleteSettings (optional, default = `true`) Determines whether the \p nsettings
/// will be deleted after this call to `changeAttributeSettings()` or not (allows the creation
/// with the `new` function and takes care of the `delete` if set to `true`). If the
/// `AttributeSettings *` \p settings wants to be preserved after this call for further
/// outside use, parameter should be set to `false`.
///
/// \return `false` if the `TypedAttribute` `AT` is not assigned to this
/// `ImageTree`. Otherwise, return `true` upon successfully changing the
/// `AttributeSettings`.
///
/// \note This function does not allow access or storage of the old `AttributeSettings`. In
/// case those want to be obtained before deletion, `getAttributeSettings()` should be used.
template<class AT>
bool ImageTree::changeAttributeSettings(AttributeSettings *nsettings, bool deleteSettings) const{
    bool rValue = this->changeAttributeSettingsOfNode<AT>(this->_root, nsettings);

    if (deleteSettings)
        delete nsettings;

    return rValue;
}

/// If the specific `TypedAttribute` `AT` is assigned to this `ImageTree`,
/// ensure that the `AttributeSettings` used for this `Attribute` in the `ImageTree`
/// are the default ones for that `Attribute`.
/// (common settings are always used for all the `Node`s in the `ImageTree`).
///
/// \tparam AT Specifies the `TypedAttribute` whose settings are to be changed
/// (e.g. `AreaAttribute`) to default.
///
/// \note Some attributes do not have a possibility to change the `AttributeSettings`.
/// If an `Attribute` wishes to ensure the correct functionality of this function,
/// it should overwrite the function pair `TypedAttribute::ensureDefaultSettings()`
/// and `TypedAttribute::revertSettingsChanges()`.
/// If an `Attribute` implements this function, the previous settings should be
/// stored and then later restored.
///
/// \remark To revert the changes back to the stored `AttributeSettings`, use the
/// function `ImageTree::revertSettingsChanges()`.
template<class AT>
void ImageTree::ensureDefaultSettings() const{
    (this->_root->getAttribute(AT::name))->ensureDefaultSettings();
}

/// If the specific `TypedAttribute` `AT` is assigned to this `ImageTree` and was
/// fixed to use the default `AttributeSettings`,
/// revert that change of the `AttributeSettings` used for this `Attribute` in the `ImageTree`
/// to their previous value.
/// (common settings are always used for all the `Node`s in the `ImageTree`).
///
/// \tparam AT Specifies the `TypedAttribute` whose settings are to be changed
/// (e.g. `AreaAttribute`) to default.
///
/// \note Some attributes do not have a possibility to change the `AttributeSettings`.
/// If an `Attribute` wishes to ensure the correct functionality of this function,
/// it should overwrite the function pair `TypedAttribute::revertSettingsChanges()`
/// and `TypedAttribute::ensureDefaultSettings()`.
/// If an `Attribute` implements this function, the previous settings should be
/// stored and then later restored.
///
/// \remark Used to revert the changes back to the stored `AttributeSettings`, if they
/// have been changed by the function `ImageTree::ensureDefaultSettings()`.
template <class AT>
void ImageTree::revertSettingsChanges() const{
    (this->_root->getAttribute(AT::name))->revertSettingsChanges();
}

/// Filters the tree by keeping at most one consequtive `Node` with
/// the same `Attribute` value. Uses the selected filtering rule.
/// Depending on the filtering rule, the gray level of certain `Node`s
/// could change, however the level of all the `Node`s remains unchanged.
///
/// \param rule Filtering rule to be used. Options are:
///     - 0 = DIRECT FILTERING. No sub-tree adjustment.
///     - 1 = SUBTRACTIVE FILTERING. Contrast adjustment on the subtrees.
///     - 2 = MAX FILTERING. Sub-trees removed (collapsed).
///     - 3 = SOFT DIRECT. Like direct, but soft.
///     - 4 = SOFT SUBTRACTIVE. Like subtractive, but soft.
///     - 5 = SOFT MAX. Like max, but soft.
/// \note Soft filtering rules do not return `false` upon unsuccessfull
/// delete, but rather sets the gray level of the child `Node` to that
/// of its parent, thus `soft` deleting it.
///
/// \param root (optional) Should be omitted if performing the filtering
/// on the whole `ImageTree`. `Node *` to the subtree which should be
/// filtered. If set to anything other than the root of
/// the tree, it will only filter the subtree.
///
/// \tparam TAT Specifies the `TypedAttribute<X>` whose values are used for
/// filtering (e.g. `AreaAttribute`).
///
/// \remark `typename TAT::attribute_type` (that is, the type `X` when `TypedAttribute<X>`
/// is used) must implement the `==` operator
///
/// \note Implements the first step of complexity-driven simplification from
/// P. Bosilj, S. Lefevre, E. Kijak: "Hierarchical Image Representation Simplification
/// Driven by Region Complexity" (for the second step use `assignAttributeAsLevel()`)
///
/// \note Is used when constructing an (omega)-tree from an alpha-tree
template<class TAT>
void ImageTree::filterTreeByAttribute(int rule, Node *root){
    filterTreeByAttributePredicate<TAT>(DifferentThanParent<typename TAT::attribute_type>(), rule, root);
}

/// Filters the tree by evaluating a `Predicate` on the values of
/// the chosen `TypedAttribute`, applying the selected filtering rule.
/// Depending on the filtering rule, the gray level of certain `Node`s
/// could change, however the level of all the `Node`s remains unchanged.
///
/// \tparam TAT Specifies the `TypedAttribute<X>` whose values are used for
/// filtering (e.g. `AreaAttribute`).
///
/// \remark The \param predicate has to take in operands of the type
/// `typename TAT::attribute_type` (that is, the type `X` when `TypedAttribute<X>`
/// is used).
///
/// \param predicate The functor object which operates on the `Node`s
/// `TypedAttribute` values.
/// \note cf. the class `Predicate` to see the correct form of this
/// functor.
///
/// \param rule Filtering rule to be used. Options are:
///     - 0 = DIRECT FILTERING. No sub-tree adjustment.
///     - 1 = SUBTRACTIVE FILTERING. Contrast adjustment on the subtrees.
///     - 2 = MAX FILTERING. Sub-trees removed (collapsed).
///     - 3 = SOFT DIRECT. Like direct, but soft.
///     - 4 = SOFT SUBTRACTIVE. Like subtractive, but soft.
///     - 5 = SOFT MAX. Like max, but soft.
/// \note Soft filtering rules do not return `false` upon unsuccessfull
/// delete, but rather sets the gray level of the child `Node` to that
/// of its parent, thus `soft` deleting it.
///
/// \param root (discouraged) Should be omitted if performing the filtering
/// on the whole `ImageTree` (intended usage). `Node *` to the subtree which
/// should be filtered.
/// If set to anything other than the root of the tree, it will only filter
/// the subtree.
/// \note Big implementation overlap with `filterTreeByLevelPredicate`.
/// Should figure out a better way to reuse code rather than copy it.
template<class TAT, class Function>
void ImageTree::filterTreeByAttributePredicate(Function predicate, int rule, Node *root){
    if (root == NULL){
        filterTreeByAttributePredicate<TAT>(predicate, rule, this->_root);
        return;
    }

    root->_propagatingHyperContrast.resize(root->hyperGraylevel().size(), 0);

    bool singleDeletionOK = true;
    bool survivingChild = false;

    std::vector<Node *> &chi = root->_children;
    for (int i=0, szi = chi.size(); i < szi; ++i){
        if (predicate(((TAT*)chi[i]->getAttribute(TAT::name))->value(), ((TAT*)root->getAttribute(TAT::name))->value()) == false){
            if (singleDeletionOK &= root->deleteChildWithRule(i,rule)){
                --i;
                szi = chi.size();
            }
        }
        else
            survivingChild = true;
    }

    if (!singleDeletionOK ||    // If I tried deleting one child, but it's brothers wouldn't let me: delete them all (partitioning trees; rules 0,1,2)
        !survivingChild){       // Non-satisfying children surivive-collapse individually with rule = 3. If they are all such, all can be deleted instead.
            root->collapseSubtree();
    }
    else{
        for (int i=0, szi = chi.size(), szj = root->_propagatingHyperContrast.size(); i < szi; ++i){
            // contrast propagation
            chi[i]->_propagatingContrast += root->_propagatingContrast;

            // contrast propagation for hyperspectral images. won't do anything if not set.
            chi[i]->_propagatingHyperContrast.resize(chi[i]->hyperGraylevel().size(), 0);
            for (int j=0; j < szj; ++j){
                chi[i]->_propagatingHyperContrast[j] += root->_propagatingHyperContrast[j];
            }

            filterTreeByAttributePredicate<TAT>(predicate, rule, chi[i]);
        }
    }

    root->_grayLevel += root->_propagatingContrast;
    for (int i=0, szi = root->_hgrayLevels.size(); i < szi; ++i)
        root->_hgrayLevels[i] += root->_propagatingHyperContrast[i];
    root->_propagatingContrast = 0;
    root->_propagatingHyperContrast.clear();
}

/// \tparam TAT Specifies the `TypedAttribute<X>` whose values are used as
/// new levels in this `ImageTree` (e.g. `AreaAttribute`).
///
/// \remark `typename TAT::attribute_type` (that is, the type `X` when `TypedAttribute<X>`
/// is used) needs to be a scalar value, i.e. castable to `double`.
///
/// \param root (discouraged) Should be omitted if associating a `TypedAttribute` value
/// to all the `Node`s in the `ImageTree` (intended usage).
/// `Node *` to the subtree which should be
/// filtered. If set to anything other than the root of
/// the tree, it will only filter the subtree.
///
/// \note Implements the second step of complexity-driven simplification from
/// P. Bosilj, S. Lefevre, E. Kijak: "Hierarchical Image Representation Simplification
/// Driven by Region Complexity" (for the first step use `filterTreeByAttribute()`)
template<class TAT>
void ImageTree::assignAttributeAsLevel(Node *root){
    if (root == NULL){
        assignAttributeAsLevel<TAT>(this->_root);
        return;
    }

    root->assignLevel(((TAT*)root->getAttribute(TAT::name))->value());
    std::vector <Node *> &children = root->_children;
    for (int i=0, szi = children.size(); i < szi; ++i)
        assignAttributeAsLevel<TAT>(children[i]);

    return;
}

/// Calculates the Ultimate Opening (cf. Fabrizio, Marcotegui: Fast Implementation
/// of Ultimate Opening, ISMM 2009) for the given `Attribute`
///
/// \tparam AT Specifies the `TypedAttribute` to be used for the base attribute opening.
/// Must be increasing. Must be assigned to the image with `addAttributeToTree` beforehand.
///
/// \param residual Output parameter for the residual component of the ultimate opening.
/// \param scale Output parameter for the scale component of the ultimate opening.
///
template<class AT> // where AT is increasing Attribute
void ImageTree::ultimateOpening(cv::Mat &residual, cv::Mat &scale) const{
    // assuming AT is in tree
    std::vector <int> res;
    std::vector <int> scl;
    std::pair<fl::Node *, int> start(this->_root, 0);

    this->addAttributeToTree<fl::AreaAttribute>(new fl::AreaSettings());
    this->calculateUO<AT>(res, scl, start, NULL, -1);
    this->reconstructUO(res, scl, residual, scale, start);
    this->deleteAttributeFromTree<fl::AreaAttribute>();
}

#if 3

/// Assigns a `PatternSpectra2D` specified by two concrete `TypedAttribute`s
/// \p AT1 and \p AT2 to all the `Node`s in this `ImageTree`.
/// This ensures reserving the memory for any values that need to be stored for
/// the `PatternSpectra2D` calculation.
/// The function assumed `TypedAttribute`s \p AT1 and \p AT2 were previously
/// assigned to the tree.
///
/// \note A copy of the attribute `PatternSpectra2D<AT1,AT2>` will be created at
/// every `Node`, and
/// `Node::addPatternSpectra2D()` will be called at each `Node`.
///
/// \note A corresponding call to `deletePatternSpectra2DFromTree<AT1, AT2>()` should be
/// made for every call to this function.
///
/// \tparam AT1 Specifies the first `TypedAttribute` to be used with the
/// `PatternSpectra2D` (e.g. `AreaAttribute`)
///
/// \tparam AT1 Specifies the first `TypedAttribute` to be used with the
/// `PatternSpectra2D` (e.g. `NonCompactnessAttribute`)
///
/// \param settings A pointer to the settings to be used throughout the hierarchy. The
/// same settings will be assigned to the `PatternSpectra2D<AT1,AT2>` at every `Node`.
/// Different `PatternSpectra2DSettings` can be used for every `PatternSpectra2D` based
/// on different `TypedAttribute`s.
///
/// \param deleteSettings (optional, default = `true`) Determines whether the \p settings
/// will be deleted after this call to `addPatternSpectra2DToTree()` or not (allows the creation
/// with the `new` function and takes care of the `delete` if set to `true`). If the
/// `PatternSpectra2DSettings *` \p settings wants to be preserved after this call for further
/// outside use, parameter should be set to `false`.
template<class AT1, class AT2>
void ImageTree::addPatternSpectra2DToTree(PatternSpectra2DSettings *settings, bool deleteSettings) const{
    this->addPatternSpectra2DToNode<AT1, AT2>(this->_root, settings);
    if (deleteSettings)
        delete settings;
}

//namespace fl{
template <typename AT1, typename AT2> class PatternSpectra2D;
//}

/// Deletes a `PatternSpectra2D` specified by two concrete `TypedAttribute`s
/// \p AT1 and \p AT2 from all the `Node`s in this `ImageTree`.
/// This ensures freeing the memory for any values that need to be stored for
/// the `PatternSpectra2D` calculation.
///
/// \note A call to `Node::deletePatternSpectra2D()` will be made at every
/// `Node`.
///
/// \note A call to this function should be made for every corresponding
/// call to `addPatternSpectra2DToTree<AT1, AT2>()`
///
/// \tparam AT1 Specifies the first `TypedAttribute` defining the
/// `PatternSpectra2D` (e.g. `AreaAttribute`)
///
/// \tparam AT1 Specifies the first `TypedAttribute` defining the
/// `PatternSpectra2D` (e.g. `NonCompactnessAttribute`)
template<class AT1, class AT2>
void ImageTree::deletePatternSpectra2DFromTree() const{
    this->deletePatternSpectra2DFromNode<AT1, AT2>(this->_root);
}


#endif // 3

#endif // 1

// private methods start here

#if 1

// where AT is Attribute
template<class AT>
void ImageTree::addAttributeToNode(Node *cur, AttributeSettings *settings) const{
    // non-recursive reimplementation
    std::vector <Node *> toProcess(1, cur);
    Node *tmp;
    do{
        tmp = toProcess.back();
        {
            AT *nat = new AT(tmp, this, settings); // how is memory lost here???
            AT *oat = (AT *)(tmp->addAttribute(nat, AT::name));
            if (oat != NULL)
                delete oat;
        }
        toProcess.pop_back();
        for (int i=0, szi = tmp->_children.size(); i < szi; ++i){
            toProcess.push_back(tmp->_children[i]);
        }
    }while(!toProcess.empty());
}

//where AT is Attribute
template<class AT>
void ImageTree::deleteAttributeFromNode(Node *cur) const{
    // non-recursive implementation
    std::vector <Node *> toProcess(1, cur);
    Node *tmp;
    do{
        tmp = toProcess.back();
        {
            AT *oat = (AT *)(tmp->deleteAttribute(AT::name));
            if (oat != NULL)
                delete oat;
        }
        toProcess.pop_back();
        for (int i=0, szi = tmp->_children.size(); i < szi; ++i){
            toProcess.push_back(tmp->_children[i]);
        }
    }while(!toProcess.empty());
}


template<class AT>
bool ImageTree::changeAttributeSettingsOfNode(Node *cur, AttributeSettings *nsettings) const{
    if (cur->getAttribute(AT::name)->changeSettings(nsettings)){
        for (int i=0, szi = cur->_children.size(); i < szi; ++i){
            this->changeAttributeSettingsOfNode<AT>(cur->_children[i], nsettings);
        }
        return true;
    }
    return false;
}



/// \note Still didn't undergo rigorous testing.
template<class AT>
void ImageTree::calculateUO(std::vector <int> &residualMap,
                            std::vector <int> &scaleMap,
                            const std::pair <fl::Node *, int> &current,
                            const fl::Node * ancestor, // the one with maximum contrast
                            int parent) const{ // first parent

    int selfAttribute = ((AT*)current.first->getAttribute(AT::name))->value();
    int selfArea = ((fl::AreaAttribute*)current.first->getAttribute(fl::AreaAttribute::name))->value();
    int selfGlevel = current.first->grayLevel();

    //std::cout << selfAttribute << " " << selfArea << " " << selfGlevel << std::endl;

    int ancArea = selfArea;

    if (ancestor != NULL){
        int ancGlevel = ancestor->grayLevel();
        ancArea = ((fl::AreaAttribute*)ancestor->getAttribute(fl::AreaAttribute::name))->value();
        residualMap.emplace_back(selfGlevel - ancGlevel);
    }
    else{
        residualMap.emplace_back(0);
    }

    if (parent == -1){
        scaleMap.emplace_back(0);
    }
    else if (residualMap[parent] > residualMap[current.second]){
        residualMap[current.second] = residualMap[parent];
        scaleMap.emplace_back(scaleMap[parent]);
    }
    else{
        scaleMap.emplace_back(selfAttribute+1);
    }

    for (int i=0, szi = current.first->_children.size(); i < szi; ++i){
        int childAttribute = ((AT*)current.first->_children[i]->getAttribute(AT::name))->value();
        if (childAttribute != selfAttribute){
            this->calculateUO<AT>(residualMap, scaleMap, std::make_pair(current.first->_children[i], (int)residualMap.size()), current.first, current.second);
        }
        else{
            this->calculateUO<AT>(residualMap, scaleMap, std::make_pair(current.first->_children[i], (int)residualMap.size()), ancestor, current.second);
        }
    }

//    residualMap[current.second] *= (double)selfArea/ancArea;
    double k = 205./100;
    //double k = 10./7;
    if (residualMap[current.second] > 0) {
        //std::cout << "Recalculating residual with factor: " << selfArea << " " << ancArea << " values old/new: ";
        //std::cout << residualMap[current.second] << " ";
        //std::cout << "factor: " << k * (1 - (double)selfArea/ancArea) << " " << k << " " <<  (double)selfArea/ancArea;
        residualMap[current.second] -= k * (1 - (double)selfArea/ancArea);
        if (residualMap[current.second] < 0) residualMap[current.second] = 0;
        if (residualMap[current.second] == 0)
            scaleMap[current.second] = 0;
        //std::cout << residualMap[current.second] << std::endl;
    }
}

#if 3

template<class AT1, class AT2>
void ImageTree::addPatternSpectra2DToNode(Node *cur, PatternSpectra2DSettings *settings) const{
    // non-recursive reimplementation
    std::vector <Node *> toProcess(1, cur);
    Node *tmp;
    do{
        tmp = toProcess.back();
        {
            PatternSpectra2D<AT1, AT2> *nps = new PatternSpectra2D<AT1, AT2>(tmp, this, settings, false);
            if (!(tmp->addPatternSpectra2D(nps, AT1::name+AT2::name))){ // false means it was not assigned
                delete nps;
                return; // because if it failed at one `Node` it should at all of them.
            }
        }
        toProcess.pop_back();
        for (int i=0, szi = tmp->_children.size(); i < szi; ++i){
            toProcess.push_back(tmp->_children[i]);
        }
    }while(!toProcess.empty());
}

template<class AT1, class AT2>
void ImageTree::deletePatternSpectra2DFromNode(Node *cur) const{
    // non-recursive reimplementation
    std::vector <Node *> toProcess(1, cur);
    Node *tmp;
    do{
        tmp = toProcess.back();
        {
            PatternSpectra2D<AT1, AT2> *ops = (PatternSpectra2D<AT1, AT2> *)(tmp->deletePatternSpectra2D(AT1::name+AT2::name));
            if (ops != NULL)
                delete ops;
            else // assuming, if first, root, does not have it, neither do the children
                return;
        }
        toProcess.pop_back();
        for (int i=0, szi = tmp->_children.size(); i < szi; ++i){
            toProcess.push_back(tmp->_children[i]);
        }
    }while(!toProcess.empty());
}

#endif // 3


#endif // 1

}

#endif

//template<class TAT>
//// where TAT is TypedAttribute<int>
//void ImageTree::filterTreeByTIAttribute(Node *root){
//    if (root == NULL){
//        filterTreeByTIAttribute<TAT>(this->_root);
//        return;
//    }
//
//    int myRange = ((TAT*)root->getAttribute(TAT::name))->value();
//    bool deletion;
//    std::vector<Node *> &chi = root->_children;
//    do{
//        deletion = false;
//        for (int i=0; i < (int)chi.size(); ++i){
//            int childRange = ((TAT*)chi[i]->getAttribute(TAT::name))->value();
//            //if (((TAT*)chi[i]->getAttribute(TAT::name))->value() == ((TAT*)root->getAttribute(TAT::name))->value()){
//            if (childRange == myRange){
//                root->deleteChild(i);
//                deletion = true;
//            }
//        }
//
//    }while(deletion == true);
//
//    for (int i=0, szi = chi.size(); i < szi; ++i)
//        filterTreeByTIAttribute<TAT>(chi[i]);
//}
