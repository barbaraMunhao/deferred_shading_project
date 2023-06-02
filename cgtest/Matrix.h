#include "../cuda/include/CUDAHelper.h"

namespace cg
{

	template <typename T, typename Allocator = ArrayAllocator>
	class Matrix
	{
	public:
		using type = Matrix<T, Allocator>;
		using value_type = T;

		static auto New(int m, int n)
		{
			type mat;

			mat._m = m; // column
			mat._n = n; // row
			mat._data = Allocator::template allocate<T>(m * n);
			return mat;
		}

		void destroy()
		{
			Allocator::template free<T>(_data);
			_data = nullptr;
			_m = _n = 0;
		}

		HOST DEVICE
			auto m() const
		{
			return _m;
		}

		HOST DEVICE
			auto n() const
		{
			return _n;
		}

		HOST DEVICE
			const T& operator ()(int y, int x) const
		{
			return _data[y * _m + x];
		}

		HOST DEVICE
			T& operator ()(int y, int x)
		{
			return _data[y * _m + x];
		}

		HOST DEVICE
			operator const T* () const
		{
			return _data;
		}

		HOST DEVICE
			operator T* ()
		{
			return _data;
		}

	private:
		int _m{};
		int _n{};
		T* _data{};

	}; // Matrix

	using Matrixf = Matrix<float>;

}