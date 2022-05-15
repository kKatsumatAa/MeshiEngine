#pragma once

class M4
{
private:
	

public:
	//çsóÒ
	float m[4][4];

	//ä÷êî
	M4();
	M4(const float (*other)[4]);
	/*void Normalize();*/

	//
	M4& operator=(const float (*other)[4]);
};

const float normalM[4][4] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
};

const float papa[4][4] = {
		{1,1,1,1},
		{2,2,2,2},
		{3,3,3,3},
		{4,4,4,4}
};

