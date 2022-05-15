#include "M4.h"

M4::M4()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = 0;
		}
	}
}

M4::M4(const float(*other)[4])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = other[i][j];
		}
	}
}

M4& M4::operator=(const float (*other)[4])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = other[i][j];
		}
	}

	return *this;
}

//void M4::Normalize()
//{
//}
