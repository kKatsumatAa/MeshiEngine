#include "Particle.h"
#include<random>

void Particle::Initialize(const Vec3& pos, const Vec3& vec, const int& timer, const float& scale, const Vec3& angle, const XMFLOAT4& color)
{
	this->vec3 = vec;
	this->timer = timer;
	this->scale = scale;
	this->color = color;
	particle.worldMat->trans = pos;
	particle.worldMat->rot = {angle};
	particle.worldMat->SetWorld();
}

void Particle::Update()
{
	vec3 -= vec3.GetNormalized();
	timer--;
	scale -= scale / 20.0f;

	particle.worldMat->trans += vec3 * 0.5f;
	particle.worldMat->scale = { scale,scale,scale };
	particle.worldMat->rot += {0.03f, 0.03f, 0.03f};
	particle.worldMat->SetWorld();

	if (timer <= 0 || scale <= 0)
	{
		isDead = true;
	}
}

void Particle::Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle)
{
	particle.DrawBox(particle.worldMat, &view, &projection, color, texHundle, 1);
}

bool Particle::GetIsDead()
{
	return isDead;
}



//------------------------------------------------------------------------------------------------------------

void ParticleManager::GenerateRandomParticle(const Vec3& pos, const int& deadTimer, const float& scale, const XMFLOAT4& color, int Num)
{
	//�����V�[�h������
	std::random_device seed_gen;
	//�����Z���k�c�C�X�^�[
	std::mt19937_64 engine(seed_gen());
	//vec�����_��
	std::uniform_real_distribution<float> vecDist(-7.0f, 7.0f);

	//Num�̐���������
	for (int i = 0; i < Num; i++)
	{
		

		Vec3 v = { vecDist(engine),vecDist(engine) ,vecDist(engine) };
		Vec3 rot = { vecDist(engine),vecDist(engine) ,vecDist(engine) };
		//v.Normalized();

		//�����A������
		std::unique_ptr<Particle> particle = std::make_unique<Particle>();
		particle->Initialize(pos, v * 1.0f, deadTimer, scale, rot, color);
		//�o�^
		particles.push_back(std::move(particle));
	}
}

void ParticleManager::Update()
{
	//particle
	for (std::unique_ptr<Particle>& particle : particles)
	{
		particle->Update();
	}

	//particle����
	particles.remove_if([](std::unique_ptr<Particle>& particle)
		{
			return (particle->GetIsDead());
		}
	);
}

void ParticleManager::Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle)
{
	//particle
	for (std::unique_ptr<Particle>& particle : particles)
	{
		particle->Draw(view, projection, texHundle);
	}
}
