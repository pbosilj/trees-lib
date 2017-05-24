/// \file algorithms/predicate.h
/// \author Petra Bosilj

#ifndef PREDICATE_H
#define PREDICATE_H

namespace fl{
    /// \class Predicate
    ///
    /// \brief An abstract `Predicate` Functor to be used in filtering the
    /// `ImageTree`.
    ///
    /// \tparam valType The type of operands (values) for the `Predicate` to work with.
    ///
    /// \note Every concrete `Predicate` needs to implement the operator
    /// `Predicate::operator()()`.
    template <class valType>
    class Predicate
    {
        public:
            /// \brief Constructor.
            Predicate();
            /// \brief Destructor.
            virtual ~Predicate();
            /// \brief Operator which performs the evaluation of the `Predicate`.
            /// \param myValue The value (of the `Node`) being evaluated.
            /// \param parentValue The value of the parent (of the `Node`) being evaluated.
            /// \return `true` if the `Predicate` accepts the value (of the `Node`), `false` otherwise.
            virtual bool operator()(valType myValue, valType parentValue) const = 0;
        protected:
        private:
    };

    /// \class RelativeIncreaseGreaterThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with the parent value more
    /// than X*100% greater than the `Node` value.
    template <class valType>
    class RelativeIncreaseGreaterThanX : Predicate<valType>{
        public:
            /// \brief The class constructor.
            /// \param xp The percentage to be used.
            RelativeIncreaseGreaterThanX(double xp) : xp(xp) { if (xp < 0) xp = 0.5;}
            virtual ~RelativeIncreaseGreaterThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            double xp;
    };

    /// \class RelativeIncreaseGreaterThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with the parent value more
    /// than X greater than the `Node` value.
    template <class valType>
    class IncreaseGreaterThanX : Predicate<valType>{
        public:
            /// \brief The class constructor.
            /// \param xp The value to be used.
            IncreaseGreaterThanX(valType x) : x(x) {}
            virtual ~IncreaseGreaterThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            valType x;
    };

    /// \class DifferentThanParent
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with values different than their
    /// parent values.
    template <class valType>
    class DifferentThanParent : public Predicate<valType>{
        public:
            DifferentThanParent() {}
            virtual ~DifferentThanParent() {}
            bool operator()(valType myValue, valType parentValue) const;
    };

    /// \class GreaterThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with values greater than a
    /// preset threshold.
    template <class valType>
    class GreaterThanX : public Predicate<valType>{
        public:
            /// \brief The class constructor.
            /// \param x The threshold to be used.
            GreaterThanX(valType x) : x(x) {}
            virtual ~GreaterThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            valType x;
    };

    /// \class ParentGreaterThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with parent values greater than a
    /// preset threshold.
    template <class valType>
    class ParentGreaterThanX : public Predicate<valType>{
        public:
            /// \copydoc GreaterThanX::GreaterThanX()
            ParentGreaterThanX(valType x) : gtx(x) {}
            virtual ~ParentGreaterThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            GreaterThanX<valType> gtx;
    };


    /// \class LessThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with values less than a
    /// preset threshold.
    template <class valType>
    class LessThanX : public Predicate<valType>{
        public:
            /// \copydoc GreaterThanX::GreaterThanX()
            LessThanX(valType x) : x(x) {}
            virtual ~LessThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            valType x;
    };

    /// \class ParentLessThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with parent values less than a
    /// preset threshold.
    template <class valType>
    class ParentLessThanX : public Predicate<valType>{
        public:
            /// \copydoc GreaterThanX::GreaterThanX()
            ParentLessThanX(valType x) : ltx(x) {}
            virtual ~ParentLessThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            LessThanX<valType> ltx;
    };

    /// \class LessEqualThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with values less or equal to a
    /// preset threshold.
    template <class valType>
    class LessEqualThanX : public Predicate<valType>{
        public:
            /// \copydoc GreaterThanX::GreaterThanX()
            LessEqualThanX(valType x) : gtx(x) {}
            virtual ~LessEqualThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            GreaterThanX<valType> gtx;
    };

    /// \class ParentLessEqualThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with parent values less or
    /// equal to a preset threshold.
    template <class valType>
    class ParentLessEqualThanX : public Predicate<valType>{
        public:
            /// \copydoc GreaterThanX::GreaterThanX()
            ParentLessEqualThanX(valType x) : letx(x) {}
            virtual ~ParentLessEqualThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            LessEqualThanX<valType> letx;
    };

    /// \class GreaterEqualThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with values greater or equal to a
    /// preset threshold.
    template <class valType>
    class GreaterEqualThanX : public Predicate<valType>{
        public:
            /// \copydoc GreaterThanX::GreaterThanX()
            GreaterEqualThanX(valType x) : ltx(x) {}
            virtual ~GreaterEqualThanX() {}
            bool operator()(valType myValue, valType parentValue=0) const;
        private:
            LessThanX<valType> ltx;
    };

    /// \class ParentGreaterEqualThanX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) with parent values greater or
    /// equal to a preset threshold.
    template <class valType>
    class ParentGreaterEqualThanX : public Predicate<valType>{
        public:
            /// \copydoc GreaterThanX::GreaterThanX()
            ParentGreaterEqualThanX(valType x) : getx(x) {}
            virtual ~ParentGreaterEqualThanX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            GreaterEqualThanX<valType> getx;
    };

    /// \class AlphaPredicateX
    ///
    /// \brief A `Predicate` accepting (the `Node`s) which would remain in an
    /// alpha-tree hierarchy if attempting an image reconstruction for
    /// alpha = X.
    ///
    /// \note Equivalent to `ParentGreaterThanX<valType>()`
    template <class valType>
    class AlphaPredicateX : public Predicate<valType>{
        public:
            /// \copydoc GreaterThanX::GreaterThanX()
            AlphaPredicateX(valType x) : pgtx(x) {}
            ///AlphaPredicateX(valType x) : x(x) {}
            virtual ~AlphaPredicateX() {}
            bool operator()(valType myValue, valType parentValue) const;
        private:
            ParentGreaterThanX<valType> pgtx;
            //valType x;
    };

}

#include "predicate.tpp"

#endif // PREDICATE_H
