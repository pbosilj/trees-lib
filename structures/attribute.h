/// \file structures/attribute.h
/// \author Petra Bosilj

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <vector>

#include <algorithm>
#include <iostream>


namespace fl{

    class ImageTree;
    class Node;

/// \class AttributeSettings
///
/// \brief Virtual interface to implement all the settings required for a
/// specific type of `Attribute`.
///
    class AttributeSettings{
        public:
            /// \brief Class constructor.
            AttributeSettings();
            /// \brief Class destructor.
            virtual ~AttributeSettings();
            /// \brief Copy constructor.
            AttributeSettings (const AttributeSettings &other);
            /// \brief Needs to be re-implemented by each specific `AttributeSetting`
            /// to allow copying from base class pointer `AttributeSetting *`.
            virtual AttributeSettings *clone() const = 0;
    };


/// \class Attribute
///
/// \brief Abstract `Attribute`, virtual interface to manipulate with all kinds of attributes.
///
    class Attribute{
        public:
            /// \brief A unique name distinguishing every type of derived `Attribute`.
            static const std::string name;

            /// \brief The constructor of `Attribute`, the base class for any concrete `Attribute`.
            Attribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, bool deleteSettings = false);

            /// \brief The destructor of `Attribute`.
            virtual ~Attribute();

            /// \brief Triggers the calculation of the `Attribute` value for a `Node`.
            virtual void calculateAttribute() = 0;
        protected:

            /// \brief Initializes any of the internal variables dependent of the `AttributeSettings` for
            /// the `Attribute`. Should be called in the derived class constructor.
            virtual void initSettings() = 0;

            /// \brief Changes the `AttributeSettings` for this `Attribute`.
            virtual bool changeSettings(AttributeSettings *nset, bool deleteSettings = false) = 0;

            /// \brief Retrieves the `AttributeSettings *` associated to this `Attribute`.
            virtual AttributeSettings *getSettings() const = 0;

            /// \brief Ensures that the all the dependent `AttributeSettings` (from other dependent
            /// attrbutes) are set the the needed default for the calculation of this `Attribute`.
            virtual void ensureDefaultSettings() = 0;

            /// \brief Reverts back to the settings used before the call to `ensureDefaultSettings()`.
            virtual void revertSettingsChanges() = 0;

            /// \brief Indicator value indicating if the `Attribute` value is calculated.
            bool valueSet;

            /// \brief The pointer to the `Node` to which this `Attribute` is associated.
            const Node *myNode;

            /// \brief The pointer to the `ImageTree` to which the `Node` \p baseNode belongs to.
            const ImageTree *myTree;

            /// \brief The pointer to the `AttributeSettings` holding the necessary settings for
            /// this `Attribute`.
            AttributeSettings *mySettings;


            template <typename AT1, typename AT2> friend class PatternSpectra2D;
            friend class ImageTree;
        private:
    };

///
/// \class TypedAttribute
///
/// \brief Virtual interface to manipulate with all kinds of attributes, defining the type
/// of `Attribute` value.
///
/// Provides a function interface allowing to access the attribute value.
///
    template <typename AttType> class TypedAttribute : public Attribute
    {
        public:
            /// \brief \copybrief Attribute::name
            static const std::string name;

            /// \brief Returns the value of this `Attribute`
            AttType &value();

            /// \brief The constructor of `TypedAttribute`.
            TypedAttribute(const Node *baseNode, const ImageTree *baseTree, AttributeSettings *settings, bool deleteSettings = false);

            virtual void calculateAttribute() = 0;

            /// \brief Allows access to the return type for the value of `TypedAttribute`.
            using attribute_type = AttType;

        protected:
            virtual bool changeSettings(AttributeSettings *nset, bool deleteSettings = false) {
                if (deleteSettings)
                    delete nset;
                return false;
            }

            virtual AttributeSettings *getSettings() const {return this->mySettings;}
            virtual void ensureDefaultSettings() { this->storedSettings++; }
            virtual void revertSettingsChanges() { this->storedSettings = std::max(0, this->storedSettings-1); }
            virtual void initSettings() { }

            /// \brief Check if the `TypedAttribute` value is calculated.
            /// \return `true` if value is set, `false` otherwise.
            bool &vset() { return this->valueSet; }

            /// \brief Check if the `TypedAttribute` has stored settings.
            ///
            /// A `TypedAttribute` would store settings after a call to
            /// ensureDefaultSettings().
            ///
            /// \return `true` if there are stored `AttributeSettings`, `false` otherwise.
            bool hasStoredSettnigs() {return storedSettings > 0; }

            /// \brief The value of this `TypedAttribute`.
            AttType attValue;
        private:
            int storedSettings;
    };


}

#include "attribute.tpp"

#endif // ATTRIBUTE_H
