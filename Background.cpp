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

void Background::Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle, const bool& boss)
{
	count+=0.05f;

	float scale = 0;
	//点滅
	scale = (sinf(count)+5.0f) / 4.0f;

	for (int i = 0; i < _countof(back) / 4; i++)
	{
		//ボスと戦闘時、おしゃれな奴動かす
		if (boss)
		{
			if (i % 2 == 0)
			{
				back[i * 4].worldMat->scale.x = scale;
				back[i * 4 + 1].worldMat->scale.x = scale;
				back[i * 4 + 2].worldMat->scale.x = scale;
				back[i * 4 + 3].worldMat->scale.x = scale;
			}
			else
			{
				back[i * 4].worldMat->scale.y = scale;
				back[i * 4 + 1].worldMat->scale.y = scale;
				back[i * 4 + 2].worldMat->scale.y = scale;
				back[i * 4 + 3].worldMat->scale.y = scale;
			}

			back[i * 4].worldMat->SetWorld();
			back[i * 4 + 1].worldMat->SetWorld();
			back[i * 4 + 2].worldMat->SetWorld();
			back[i * 4 + 3].worldMat->SetWorld();
		}
		else
		{
			//ゆっくり元の大きさに戻す
			if (back[i * 4].worldMat->scale.x > 1.0f)
			{
				back[i * 4].worldMat->scale.x -= 0.01f;
				back[i * 4 + 1].worldMat->scale.x -= 0.01f;
				back[i * 4 + 2].worldMat->scale.x -= 0.01f;
				back[i * 4 + 3].worldMat->scale.x -= 0.01f;
			}

			if (back[i * 4].worldMat->scale.y > 1.0f)
			{
				back[i * 4].worldMat->scale.y -= 0.01f;
				back[i * 4 + 1].worldMat->scale.y -= 0.01f;
				back[i * 4 + 2].worldMat->scale.y -= 0.01f;
				back[i * 4 + 3].worldMat->scale.y -= 0.01f;
			}
		}

		XMFLOAT4 color;

		if (boss)
			color = { 1.0f,0.0f,0.0f,0.9f };
		else
			color = { 1.0f,1.0f,1.0f,1.0f / (back[i * 4].worldMat->trans.z / 130.0f) };

		back[i * 4].DrawLine(pos[0], pos[1], back[i * 4].worldMat, &view, &projection, color, { texHundle[0] });
		back[i * 4 + 1].DrawLine(pos[1], pos[2], back[i * 4 + 1].worldMat, &view, &projection, color, { texHundle[0] });
		back[i * 4 + 2].DrawLine(pos[2], pos[3], back[i * 4 + 2].worldMat, &view, &projection, color, { texHundle[0] });
		back[i * 4 + 3].DrawLine(pos[3], pos[0], back[i * 4 + 3].worldMat, &view, &projection, color, { texHundle[0] });
	}
}
