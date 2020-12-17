#pragma once

#ifndef _RNG_SERVICE_HPP_
#define _RNG_SERVICE_HPP_

#include <stdint.h>
#include <random>
#include <map>
#include <optional>

#include "RandomNumberStreamHolder.hpp"

template<typename _value_type, typename _size_type = uint32_t>
class RNGService
{
    using value_type = _value_type;
    using size_type = _size_type;
    using key_type = std::string;
    using rng_stream_type = RandomNumberStreamHolder<value_type, size_type>;

private:
    std::unordered_map<key_type, rng_stream_type> stream_holders_map;

public:

    rng_stream_type& create_sequence (const key_type& key, std::seed_seq& seed_sequence, value_type min, value_type max, size_type amount)
    {
        return (*(stream_holders_map
                .emplace (key, std::move (rng_stream_type{ seed_sequence, min, max, amount }))
                .first)) //iterator
            .second; //value
    }

    rng_stream_type& create_sequence (const key_type& key, std::seed_seq& seed_sequence, value_type min, value_type max, size_type size_x, size_type size_y)
    {
        return (*(stream_holders_map
                .emplace (key, std::move (rng_stream_type{ seed_sequence, min, max, size_x, size_y }))
                .first)) //iterator
            .second; //value
    }

    void put (const key_type& key, const rng_stream_type& value)
    {
        stream_holders_map.insert_or_assign(key, value);
    }

    rng_stream_type get_unchecked (const key_type& key) const
    {
        return stream_holders_map.at(key);
    }

    std::optional<rng_stream_type> get (const key_type& key) const noexcept
    {
        auto elem = stream_holders_map.find (key);
        if ( elem != stream_holders_map.end() )
        {
            return std::make_optional( elem->second );
        }
        
        return std::optional<rng_stream_type>();
    }

    void remove (const key_type& key) noexcept
    {
        stream_holders_map.erase (key);
    }

    void reset () noexcept
    {
        stream_holders_map.clear ();
    }
};

#endif // !_RNG_SERVICE_HPP_
