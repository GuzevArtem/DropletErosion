#pragma once

#ifndef _RANGE_ITERATOR_HPP_
#define _RANGE_ITERATOR_HPP_

#include <iterator>
#include <concepts>
#include <type_traits>

namespace utils
{
    template<typename T>
    concept Discrete = std::is_integral<T>::value &&
        !std::is_same<T, bool>::value
        ;

    template<typename Discrete>
    class Range
    {
    public:
        Range(Discrete _from = 0, Discrete _to = 0) : from(_from), to(_to)
        {}

        class iterator
        {
        public:
            typedef iterator self_type;
            typedef Discrete value_type;
            typedef Discrete& reference;
            typedef Discrete* pointer;
            typedef std::forward_iterator_tag iterator_category;
            typedef int difference_type;

            iterator(Discrete _num = 0, Discrete _from = 0, Discrete _to = 0) : num(_num), from(_from), to(_to)
            {}
            self_type operator++()
            {
                self_type i = *this;
                num = to >= from ? num + 1 : num - 1;
                return i;
            }
            self_type operator++(int junk)
            {
                num = to >= from ? num + 1 : num - 1;
                return *this;
            }
            reference operator*()
            {
                return num;
            }
            bool operator==(const self_type& rhs)
            {
                return num == rhs.num;
            }
            bool operator!=(const self_type& rhs)
            {
                return num != rhs.num;
            }
        private:
            Discrete num;
            Discrete from;
            Discrete to;
        };

        iterator begin()
        {
            return iterator(from, from, to);
        }
        iterator end()
        {
            return iterator(to >= from ? to + 1 : to - 1, from, to);
        }

    private:
        const Discrete from;
        const Discrete to;
    };
}

#endif // !_RANGE_ITERATOR_HPP_
