//#pragma once
//#include <list>
//#include <iostream>
//#include "Object.h"
//
//
//class Particle
//{
//private:
//	Draw particle;
//	Vec3 vec3 = { 0,0,0 };
//	bool isDead = false;
//	int timer = 0;
//	float scale = 1.0f;
//	XMFLOAT4 color = { 0,0,0,0 };
//
//public:
//	void Initialize(const Vec3& pos, const Vec3& vec, const int& timer, const float& scale, const Vec3& angle, const XMFLOAT4& color);
//	void Update();
//	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle);
//	bool GetIsDead();
//};
//
//class ParticleManager
//{
//private:
//	
//
//public:
//	std::list<std::unique_ptr<Particle>> particles;
//
//	void Initialize();
//
//	void GenerateRandomParticle(const Vec3& pos, const int& deadTimer, const float& scale, const XMFLOAT4& color, int Num);
//	void Update();
//	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64& texHundle);
//};
//
