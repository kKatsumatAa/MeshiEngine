#include "Background.h"

Background::Background()
{
	for (int i = 0; i < _countof(back)/4; i++)
	{
		back[i * 4].worldMat->trans.z = 0 + distance * (float)i;
		back[i * 4 + 1].worldMat->trans.z = 0 + distance * (float)i;
		back[i * 4 + 2].worldMat->trans.z = 0 + distance * (float)i;
		back[i * 4 + 3].worldMat->trans.z = 0 + distance * (float)i;

		back[i * 4].worldMat->SetWorld();
		back[i * 4 + 1].worldMat->SetWorld();
		back[i * 4 + 2].worldMat->SetWorld();
		back[i * 4 + 3].worldMat->SetWorld();
	}
}

void Background::Update()
{
	for (int i = 0; i < _countof(back); i++)
	{
		back[i].worldMat->trans.z -= 1.0f;
		back[i].worldMat->SetWorld();
		if (back[i].worldMat->trans.z <= -15.0f)
		{
			back[i].worldMat->trans.z = distance * (float)backNum;
		}
	}
}

void Background::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle)
{
	for (int i = 0; i < _countof(back) / 4; i++)
	{
		back[i * 4].DrawLine(pos[0], pos[1], back[i * 4].worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f / (back[i * 4].worldMat->trans.z / 130.0f) }, { texHundle[0] });
		back[i * 4 + 1].DrawLine(pos[1], pos[2], back[i * 4 + 1].worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f / (back[i * 4].worldMat->trans.z / 130.0f) }, { texHundle[0] });
		back[i * 4 + 2].DrawLine(pos[2], pos[3], back[i * 4 + 2].worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f / (back[i * 4].worldMat->trans.z / 130.0f) }, { texHundle[0] });
		back[i * 4 + 3].DrawLine(pos[3], pos[0], back[i * 4 + 3].worldMat, &view, &projection, { 1.0f,1.0f,1.0f,1.0f / (back[i * 4].worldMat->trans.z / 130.0f) }, { texHundle[0] });
	}
}
