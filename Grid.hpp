#pragma once

#include <stdint.h>
#include <vector>
#include <exception>

template<typename T, typename _size_type = uint32_t, typename _holder_type = std::vector<T>>
class Grid
{
	using size_type = _size_type;

	using data_type = T;
	using holder_type = _holder_type;

public:
	Grid() : x_size(size_type{}), y_size(size_type{}), data({})
	{}

	Grid (size_type x_size, size_type y_size) : x_size (x_size), y_size (y_size), data (holder_type (((size_t)x_size)* y_size))
	{
	}

	Grid (size_type x_size, size_type y_size, data_type default_value) : x_size (x_size), y_size (y_size), data (holder_type (((size_t)x_size)* y_size))
	{
		std::fill_n(data.begin(), ((size_t)x_size) * y_size, default_value);
	}

	Grid(size_type x_size, holder_type data) : x_size(x_size), data(data)
	{
		const double y_s = data.size() * 1.0 / x_size;
		y_size = y_s == 0 ? 0 : std::ceil(y_s); //prevent unaligned data loses
	}

	[[nodiscard]]
	inline const size_type get_x_size() const noexcept
	{
		return x_size;
	}

	[[nodiscard]]
	inline const size_type get_y_size() const noexcept
	{
		return y_size;
	}

	[[nodiscard]]
	inline holder_type& get_data() noexcept
	{
		return data;
	}

	[[nodiscard]]
	inline const holder_type& get_data() const noexcept
	{
		return data;
	}

	[[nodiscard]]
	inline data_type& at(const size_type index) noexcept
	{
		check_index(index);
		return data[index];
	}

	[[nodiscard]]
	inline const data_type& at(const size_type index) const noexcept
	{
		check_index(index);
		return data[index];
	}

	[[nodiscard]]
	inline data_type& at(const size_type x, const size_type y) noexcept
	{
		check_x(x);
		check_y(y);
		return data[to_1_d(x,y)];
	}

	[[nodiscard]]
	inline const data_type& at(const size_type x, const size_type y) const noexcept
	{
		check_x(x);
		check_y(y);
		return data[to_1_d(x, y)];
	}

	//unchecked

	[[nodiscard]]
	inline data_type& at_unchecked(const size_type index) noexcept
	{
		return data[index];
	}

	[[nodiscard]]
	inline const data_type& at_unchecked(const size_type index) const noexcept
	{
		return data[index];
	}

	[[nodiscard]]
	inline data_type& at_unchecked(const size_type x, const size_type y) noexcept
	{
		return data[to_1_d(x, y)];
	}

	[[nodiscard]]
	inline const data_type& at_unchecked(const size_type x, const size_type y) const noexcept
	{
		return data[to_1_d(x, y)];
	}

	[[nodiscard]]
	inline void assign (const size_type x, const size_type y, const data_type value) noexcept
	{
		check_x (x);
		check_y (y);
		data[to_1_d (x, y)] = value;
	}

	[[nodiscard]]
	inline void assign_unchecked (const size_type x, const size_type y, const data_type value) noexcept
	{
		data[to_1_d (x, y)] = value;
	}

	[[nodiscard]]
	inline void assign (const size_type index, const data_type value) noexcept
	{
		check_index (index);
		data[index] = value;
	}

	[[nodiscard]]
	inline void assign_unchecked (const size_type index, const data_type value) noexcept
	{
		data[index] = value;
	}

	//operators

	[[nodiscard]]
	inline data_type& operator[](const size_type x) noexcept
	{
		check_x(x);
		return Row(&data, x, x_size, y_size);
	}

	[[nodiscard]]
	inline const data_type& operator[](const size_type x) const noexcept
	{
		check_x(x);
		return Row{ &data, x, x_size, y_size };
	}

	//iterator

	[[nodiscard]]
	auto begin() noexcept
	{
		return data.begin();
	}

	[[nodiscard]]
	auto begin() const noexcept
	{
		return data.begin();
	}

	[[nodiscard]]
	auto end() noexcept
	{
		return data.end();
	}

	[[nodiscard]]
	auto end() const noexcept
	{
		return data.end();
	}

	//helpers

	const size_type to_1_d(const size_type x, const size_type y) const noexcept
	{
		return x + y * x_size;
	}

	void for_each(std::function<data_type(size_type, size_type)> operation)
	{
		for (size_type x = 0; x < x_size; x++)
		{
			for (size_type y = 0; y < y_size; y++)
			{
				data[to_1_d(x, y)] = operation(x, y);
			}
		}
	}

	void for_each(std::function<void(size_type, size_type, const data_type)> operation) const
	{
		for (size_type x = 0; x < x_size; x++)
		{
			for (size_type y = 0; y < y_size; y++)
			{
				operation(x, y, data[to_1_d (x, y)]);
			}
		}
	}

private:
	struct Row
	{
		holder_type* data;
		size_type x;
		size_type x_size;
		size_type y_size;

		[[nodiscard]]
		inline data_type& operator[](const size_type y) noexcept
		{
			check_y(y);
			return data->operator[](Row::to_1_d(x, y));
		}

		[[nodiscard]]
		inline const data_type& operator[](const size_type y) const noexcept
		{
			check_y(y);
			return data->operator[](Row::to_1_d(x, y));
			//return (*data)[Row::to_1_d(x, y)];
		}

		size_type to_1_d(const size_type x, const size_type y)
		{
			return x + y * x_size;
		}
	};

	static void raise_out_of_bounds_exception(const size_type value, const size_type bound)
	{
		throw new std::exception(std::string("Out of bounds exception: x {" + std::to_string(value) + "}, while x_size {" + std::to_string(bound) + "}").c_str());
	}

public:
	void check_index (const size_type index)
	{
		const size_type bound = x_size * y_size;
		if ( index >= bound )
		{
			raise_out_of_bounds_exception (index, bound);
		}
	}

	void check_x (const size_type x)
	{
		if ( x >= x_size )
		{
			raise_out_of_bounds_exception (x, x_size);
		}
	}

	void check_y (const size_type y)
	{
		if ( y >= y_size )
		{
			raise_out_of_bounds_exception (y, y_size);
		}
	}


private:
	size_type x_size;
	size_type y_size;

	holder_type data;
};