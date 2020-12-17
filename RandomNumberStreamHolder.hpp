#pragma once

#ifndef _RANDOM_NUMBER_STREAM_HOLDER_HPP_
#define _RANDOM_NUMBER_STREAM_HOLDER_HPP_

#include <stdint.h>
#include <random>

#include "Grid.hpp"
#include "UtilsRandom.hpp"

template<
    typename _value_type,
    typename _size_type = uint32_t,
    typename _seed_type = uint32_t,
    typename _random_engine_type = std::default_random_engine
>
class RandomNumberStreamHolder
{
    using value_type = _value_type;
    using size_type = _size_type;
    using seed_type = _seed_type;

    using random_engine_type = _random_engine_type;

    using initial_seed_grid = Grid<seed_type, size_type>;
    using sequence_grid = Grid<_random_engine_type, size_type>;
    using last_value_grid = Grid<value_type, size_type>;

private:
    initial_seed_grid init_seed;
    sequence_grid sequence;
    last_value_grid last_value;
    value_type min;
    value_type max;

public:

    RandomNumberStreamHolder (std::seed_seq& seed_sequence, value_type min, value_type max, size_type amount)
        : min (min), max (max),
        init_seed (initial_seed_grid{ 1, amount }),
        sequence (sequence_grid{ 1, amount }),
        last_value (last_value_grid{ 1, amount })
    {
        seed_sequence.generate (init_seed.begin (), init_seed.end ());
        _create_sequnce_and_last_value_based_on_init_seeds ();
    }

    RandomNumberStreamHolder (std::seed_seq& seed_sequence, value_type min, value_type max, size_type  size_x, size_type size_y)
        : min (min), max (max),
        init_seed (initial_seed_grid{ size_x, size_y }),
        sequence (sequence_grid{ size_x, size_y }),
        last_value (last_value_grid{ size_x, size_y })
    {
        seed_sequence.generate (init_seed.begin (), init_seed.end ());
        _create_sequnce_and_last_value_based_on_init_seeds ();
    }

    RandomNumberStreamHolder (initial_seed_grid& grid, value_type min, value_type max) : min (min), max (max),
        init_seed (grid),
        sequence (sequence_grid{ grid.get_x_size(), grid .get_y_size()}),
        last_value (last_value_grid{ grid.get_x_size (), grid.get_y_size () })
    {
        _create_sequnce_and_last_value_based_on_init_seeds ();
    }

    RandomNumberStreamHolder (size_type amount, value_type min, value_type max, value_type initial_seed = {})
        : min (min), max (max),
        init_seed (initial_seed_grid{ 1, amount, initial_seed }),
        sequence (sequence_grid{ 1, amount }),
        last_value (last_value_grid{ 1, amount })
    {
        _create_sequnce_and_last_value_based_on_init_seeds ();
    }

    RandomNumberStreamHolder (size_type size_x, size_type size_y, value_type min, value_type max, value_type initial_seed = {})
        : min (min), max (max),
        init_seed (initial_seed_grid{ size_x, size_y, initial_seed }),
        sequence (sequence_grid{ size_x, size_y }),
        last_value (last_value_grid{ size_x, size_y })
    {
        _create_sequnce_and_last_value_based_on_init_seeds ();
    }

private:

    random_engine_type _create_random_engine (seed_type seed) const
    {
        const std::seed_seq ss{ seed };
        return random_engine_type{ ss };
    }

    random_engine_type _get_random_engine (size_type index) const
    {
        return sequence.at_unchecked (index);
    }

    random_engine_type _get_random_engine (size_type x, size_type y) const
    {
        return sequence.at_unchecked (x, y);
    }

    void _create_sequnce_and_last_value_based_on_init_seeds ()
    {
        init_seed.for_each ([this](size_type x, size_type y, const seed_type value)
                            {
                                const random_engine_type eng = _create_random_engine ( value );
                                this->sequence.assign_unchecked (x, y, eng);
                                this->last_value.assign_unchecked (x, y, utils_random::rand (eng, this->min, this->max));
                            });
    }

    void _create_sequnce_based_on_init_seeds ()
    {
        init_seed.for_each ([this](size_type x, size_type y, const seed_type value)
                            {
                                this->sequence.assign_unchecked (x, y, _create_random_engine ( value ));
                            });
    }

    void _create_last_values_based_on_sequence ()
    {
        sequence.for_each ([this](size_type x, size_type y, const std::seed_seq value)
                            {
                               this->last_value.assign_unchecked (x, y, utils_random::rand(_get_random_engine (x, y), this->min, this->max));
                            });
    }

    const value_type _get_random_for (size_type index, bool return_last = true) const
    {
        if ( return_last )
        {
            return last_value.at_unchecked (index);
        }
        else
        {
            return utils_random::rand (_get_random_engine (index), min, max);
        }
    }

    const value_type _get_random_for (size_type x, size_type y, bool return_last = true) const
    {
        if ( return_last )
        {
            return last_value.at_unchecked (x, y);
        }
        else
        {
            return utils_random::rand (_get_random_engine(x, y), min, max);
        }
    }

    const value_type _next_random_for (size_type index)
    {
        const value_type value = _get_random_for(index, false);
        last_value.assign_unchecked (index, value);
        return value;
    }

    const value_type _next_random_for (size_type x, size_type y)
    {
        const value_type value = _get_random_for (x, y, false);
        last_value.assign_unchecked (x, y, value);
        return value;
    }

public:

    /**
     * @brief get last generated value
     * @param index index of value
     * @return last generated value
    */
    value_type get (const size_type index) const
    {
        init_seed.check_index (index);
        return _get_random_for (index);
    }

    /**
     * @brief get last generated value
     * @param x x coord of value
     * @param y y coord of value
     * @return last generated value
    */
    value_type get (const size_type x, const size_type y) const
    {
        init_seed.check_x (x);
        init_seed.check_y (y);
        return _get_random_for (x, y);
    }

    /**
     * @brief generate new value and get it
     * @param index index of value
     * @return generated value
    */
    value_type next (const size_type index)
    {
        init_seed.check_index (index);
        return _next_random_for (index);
    }

    /**
     * @brief generate new value and get it
     * @param x x coord of value
     * @param y y coord of value
     * @return generated value
    */
    value_type next (const size_type x, const size_type y)
    {
        init_seed.check_x (x);
        init_seed.check_y (y);
        return _next_random_for (x, y);
    }

    /**
     * @brief initial seed
     * @return initial seed
    */
    const initial_seed_grid get_initial_seed () const
    {
        return init_seed;
    }

    /**
     * @brief sequence
     * @return current value generators
    */
    const sequence_grid get_sequence () const
    {
        return sequence_grid;
    }

    /**
     * @brief last generated values
     * @return last generated values
    */
    const last_value_grid get_last_value () const
    {
        return last_value;
    }

    /**
     * @brief reset whole values to initial
    */
    void reset ()
    {
        _create_sequnce_and_last_value_based_on_init_seeds ();
    }
};

#endif // !_RANDOM_NUMBER_STREAM_HOLDER_HPP_

