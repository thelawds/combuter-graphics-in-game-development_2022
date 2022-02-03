#pragma once

#include "utils/error_handler.h"

#include <algorithm>
#include <linalg.h>
#include <vector>


using namespace linalg::aliases;

namespace cg
{
	template<typename T>
	class resource
	{
	public:
		resource(size_t size);
		resource(size_t x_size, size_t y_size);
		~resource();

		const T* get_data();
		T& item(size_t item);
		T& item(size_t x, size_t y);

		size_t get_size_in_bytes() const;
		size_t get_number_of_elements() const;
		size_t get_stride() const;

	private:
		std::vector<T> data;
		size_t item_size = sizeof(T);
		size_t stride;
	};
	template<typename T>
	inline resource<T>::resource(size_t size) : data(size), stride(0)
	{}

	template<typename T>
	inline resource<T>::resource(size_t x_size, size_t y_size) : stride(x_size), data(x_size * y_size)
	{}

	template<typename T>
	inline resource<T>::~resource() = default;

	template<typename T>
	inline const T* resource<T>::get_data()
	{
		return data.data();
	}

	template<typename T>
	inline T& resource<T>::item(size_t item)
	{
		return data[item];
	}

	template<typename T>
	inline T& resource<T>::item(size_t x, size_t y)
	{
		return data[x + y * stride];
	}

	template<typename T>
	inline size_t resource<T>::get_size_in_bytes() const
	{
		return data.size() * item_size;
	}

	template<typename T>
	inline size_t resource<T>::get_number_of_elements() const
	{
		return data.size();
	}

	template<typename T>
	inline size_t resource<T>::get_stride() const
	{
		return stride;
	}

	struct color
	{
		static color from_float3(const float3& in)
		{
			return color{in.x, in.y, in.z};
		}

		float3 to_float3() const
		{
			return float3{r, g, b};
		}

		float r;
		float g;
		float b;
	};

	struct unsigned_color
	{
		static unsigned_color from_color(const color& color)
		{
			return unsigned_color{
					static_cast<unsigned char>(color.r * 255),
					static_cast<unsigned char>(color.g * 255),
					static_cast<unsigned char>(color.b * 255)};
		};

		static unsigned_color from_float3(const float3& color)
		{
			return unsigned_color{
					static_cast<unsigned char>(color.x * 255),
					static_cast<unsigned char>(color.y * 255),
					static_cast<unsigned char>(color.z * 255)};
		}

		float3 to_float3() const
		{
			return float3{
					static_cast<float>(r / 255.),
					static_cast<float>(g / 255.),
					static_cast<float>(b / 255.)};
		};

		unsigned char r;
		unsigned char g;
		unsigned char b;
	};


	struct vertex
	{
		float x;
		float y;
		float z;

		float nx;
		float ny;
		float nz;

		float ambient_r;
		float ambient_g;
		float ambient_b;

		float diffuse_r;
		float diffuse_g;
		float diffuse_b;

		float emissive_r;
		float emissive_g;
		float emissive_b;

		float u;
		float v;

		vertex operator+(const vertex& other) const
		{
			vertex result{};
			result.x = this->x + other.x;
			result.y = this->y + other.y;
			result.z = this->z + other.z;
			result.nx = this->nx + other.nx;
			result.ny = this->ny + other.ny;
			result.nz = this->nz + other.nz;

			// TODO: Add remaining parameters
			// ----
			return result;
		}

		vertex operator*(const float value) const
		{
			vertex result{};
			result.x = this->x * value;
			result.y = this->y * value;
			result.z = this->z * value;
			result.nx = this->nx * value;
			result.ny = this->ny * value;
			result.nz = this->nz * value;

			// TODO: Add remaining parameters
			// ----

			return result;
		}
	};

}// namespace cg