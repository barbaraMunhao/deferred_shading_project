#include "CUDAHelper.h"
#include "Matrix.h"
#include "math/Vector2.h"

namespace cg
{
	struct vec2i
	{
		int x;
		int y;
	};

	class HeaterGen 
	{
	public:
		HeaterGen();

		static void makeSquaredHeater(vec2i& tl, int size, float temp, Matrixf& mat)
		{
			auto col = (tl.x + size < mat.n) ? tl.x + size : mat.n;
			auto row = (tl.y + size < mat.m) ? tl.y + size : mat.m;
			
			for (int i = tl.y; i < row; i++)
				for (int j = tl.x; j < col; j++)
					mat(i, j) = temp;
		}


	private:

	};
}

