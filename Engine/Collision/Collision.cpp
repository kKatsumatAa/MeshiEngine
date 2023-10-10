#include "Collision.h"
#include "Util.h"

using namespace DirectX;

bool Collision::CheckSphere2Sphere(const Sphere& sphere, const Sphere& sphere2, DirectX::XMVECTOR* inter,
	DirectX::XMVECTOR* reject)
{
	XMVECTOR length = sphere.center - sphere2.center;
	Vec3 vec = { length.m128_f32[0],length.m128_f32[1],length.m128_f32[2] };

	if (inter)
	{
		// A縺ｮ蜊雁ｾ・′0縺ｮ譎ょｺｧ讓吶・B縺ｮ荳ｭ蠢・B縺ｮ蜊雁ｾ・′0縺ｮ譎ょｺｧ讓吶・A縺ｮ荳ｭ蠢・縺ｨ縺ｪ繧九ｈ縺・｣懷ｮ・
		float t = sphere2.iRadius / (sphere.iRadius + sphere2.iRadius);
		*inter = XMVectorLerp(sphere.center, sphere2.center, t);
	}

	if (powf(vec.GetLength(), 2.0f) <= powf(sphere.iRadius + sphere2.iRadius, 2.0f))
	{
		//謚ｼ縺怜・縺吶・繧ｯ繝医Ν繧定ｨ育ｮ・
		if (reject)
		{
			float rejectLen = sphere.iRadius + sphere2.iRadius - sqrtf(vec.GetLength());//
			*reject = XMVector3Normalize(sphere.center - sphere2.center);
			*reject *= rejectLen;
		}

		return true;
	}

	return false;
}

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter)
{
	//"蠎ｧ讓咏ｳｻ縺ｮ蜴溽せ"縺九ｉ逅・・荳ｭ蠢・ｺｧ讓吶∈縺ｮ霍晞屬
	XMVECTOR distV = XMVector3Dot(sphere.center, plane.iNormal);
	//蟷ｳ髱｢縺ｮ蜴溽せ霍晞屬繧呈ｸ帷ｮ励☆繧九％縺ｨ縺ｧ縲∝ｹｳ髱｢縺ｨ逅・・荳ｭ蠢・→縺ｮ霍晞屬縺悟・繧・
	float dist = distV.m128_f32[0] - plane.iDistance;
	//霍晞屬縺ｮ邨ｶ蟇ｾ蛟､縺悟濠蠕・ｈ繧雁､ｧ縺阪￠繧後・蠖薙◆縺｣縺ｦ縺・↑縺・
	if (fabsf(dist) > sphere.iRadius)return false;

	//逍台ｼｼ莠､轤ｹ繧定ｨ育ｮ・
	if (inter)
	{
		//蟷ｳ髱｢荳翫・譛霑第磁轤ｹ繧偵∫桝莨ｼ莠､轤ｹ縺ｨ縺吶ｋ
		*inter = -dist * plane.iNormal + sphere.center;
	}

	return true;
}

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest)
{
	// point縺継0縺ｮ螟門・縺ｮ鬆らせ鬆伜沺縺ｮ荳ｭ縺ｫ縺ゅｋ縺九←縺・°繝√ぉ繝・け
	XMVECTOR p0_p1 = triangle.iP1 - triangle.iP0;
	XMVECTOR p0_p2 = triangle.iP2 - triangle.iP0;
	XMVECTOR p0_pt = point - triangle.iP0;

	XMVECTOR d1 = XMVector3Dot(p0_p1, p0_pt);
	XMVECTOR d2 = XMVector3Dot(p0_p2, p0_pt);

	if (d1.m128_f32[0] <= 0.0f && d2.m128_f32[0] <= 0.0f)
	{
		// p0縺梧怙霑大ｍ
		*closest = triangle.iP0;
		return;
	}

	// point縺継1縺ｮ螟門・縺ｮ鬆らせ鬆伜沺縺ｮ荳ｭ縺ｫ縺ゅｋ縺九←縺・°繝√ぉ繝・け
	XMVECTOR p1_pt = point - triangle.iP1;

	XMVECTOR d3 = XMVector3Dot(p0_p1, p1_pt);
	XMVECTOR d4 = XMVector3Dot(p0_p2, p1_pt);

	if (d3.m128_f32[0] >= 0.0f && d4.m128_f32[0] <= d3.m128_f32[0])
	{
		// p1縺梧怙霑大ｍ
		*closest = triangle.iP1;
		return;
	}

	// point縺継0_p1縺ｮ霎ｺ鬆伜沺縺ｮ荳ｭ縺ｫ縺ゅｋ縺九←縺・°繝√ぉ繝・け縺励√≠繧後・point縺ｮp0_p1荳翫↓蟇ｾ縺吶ｋ蟆・ｽｱ繧定ｿ斐☆
	float vc = d1.m128_f32[0] * d4.m128_f32[0] - d3.m128_f32[0] * d2.m128_f32[0];
	if (vc <= 0.0f && d1.m128_f32[0] >= 0.0f && d3.m128_f32[0] <= 0.0f)
	{
		float v = d1.m128_f32[0] / (d1.m128_f32[0] - d3.m128_f32[0]);
		*closest = triangle.iP0 + v * p0_p1;
		return;
	}

	// point縺継2縺ｮ螟門・縺ｮ鬆らせ鬆伜沺縺ｮ荳ｭ縺ｫ縺ゅｋ縺九←縺・°繝√ぉ繝・け
	XMVECTOR p2_pt = point - triangle.iP2;

	XMVECTOR d5 = XMVector3Dot(p0_p1, p2_pt);
	XMVECTOR d6 = XMVector3Dot(p0_p2, p2_pt);
	if (d6.m128_f32[0] >= 0.0f && d5.m128_f32[0] <= d6.m128_f32[0])
	{
		*closest = triangle.iP2;
		return;
	}

	// point縺継0_p2縺ｮ霎ｺ鬆伜沺縺ｮ荳ｭ縺ｫ縺ゅｋ縺九←縺・°繝√ぉ繝・け縺励√≠繧後・point縺ｮp0_p2荳翫↓蟇ｾ縺吶ｋ蟆・ｽｱ繧定ｿ斐☆
	float vb = d5.m128_f32[0] * d2.m128_f32[0] - d1.m128_f32[0] * d6.m128_f32[0];
	if (vb <= 0.0f && d2.m128_f32[0] >= 0.0f && d6.m128_f32[0] <= 0.0f)
	{
		float w = d2.m128_f32[0] / (d2.m128_f32[0] - d6.m128_f32[0]);
		*closest = triangle.iP0 + w * p0_p2;
		return;
	}

	// point縺継1_p2縺ｮ霎ｺ鬆伜沺縺ｮ荳ｭ縺ｫ縺ゅｋ縺九←縺・°繝√ぉ繝・け縺励√≠繧後・point縺ｮp1_p2荳翫↓蟇ｾ縺吶ｋ蟆・ｽｱ繧定ｿ斐☆
	float va = d3.m128_f32[0] * d6.m128_f32[0] - d5.m128_f32[0] * d4.m128_f32[0];
	if (va <= 0.0f && (d4.m128_f32[0] - d3.m128_f32[0]) >= 0.0f && (d5.m128_f32[0] - d6.m128_f32[0]) >= 0.0f)
	{
		float w = (d4.m128_f32[0] - d3.m128_f32[0]) / ((d4.m128_f32[0] - d3.m128_f32[0]) + (d5.m128_f32[0] - d6.m128_f32[0]));
		*closest = triangle.iP1 + w * (triangle.iP2 - triangle.iP1);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.iP0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle,
	DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	XMVECTOR p;
	//蠑ｾ縺ｮ荳ｭ蠢・↓蟇ｾ縺吶ｋ譛霑第磁轤ｹ縺ｧ縺ゅｋ荳芽ｧ貞ｽ｢荳翫↓縺ゅｋ轤ｹp繧定ｦ九▽縺代ｋ
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	//轤ｹp縺ｨ逅・・荳ｭ蠢・・蟾ｮ蛻・・繧ｯ繝医Ν
	XMVECTOR v = p - sphere.center;
	//霍晞屬縺ｮ莠御ｹ励ｒ豎ゅａ繧・
	//・亥酔縺倥・繧ｯ繝医Ν蜷悟｣ｫ縺ｮ蜀・ｩ阪・荳牙ｹｳ譁ｹ縺ｮ螳夂炊縺ｮ繝ｫ繝ｼ繝亥・驛ｨ縺ｮ蠑上→荳閾ｴ縺吶ｋ・・
	v = XMVector3Dot(v, v);
	//逅・→荳芽ｧ貞ｽ｢縺ｮ霍晞屬縺悟濠蠕・ｻ･荳九↑繧牙ｽ薙◆縺｣縺ｦ縺・↑縺・
	if (v.m128_f32[0] > sphere.iRadius * sphere.iRadius)
	{
		return false;
	}
	//逍台ｼｼ莠､轤ｹ繧定ｨ育ｮ・
	if (inter)
	{
		//荳芽ｧ貞ｽ｢荳翫・譛霑第磁轤ｹp繧堤桝莨ｼ莠､轤ｹ縺ｨ縺吶ｋ
		*inter = p;
	}
	//謚ｼ縺怜・縺吶・繧ｯ繝医Ν繧定ｨ育ｮ・
	if (reject)
	{
		float ds = XMVector3Dot(sphere.center, triangle.iNormal).m128_f32[0];
		float dt = XMVector3Dot(triangle.iP0, triangle.iNormal).m128_f32[0];
		//逅・・蜊雁ｾ・逅・→荳芽ｧ貞ｽ｢縺ｮ霍晞屬
		float recectLen = dt - ds + sphere.iRadius;
		*reject = triangle.iNormal * recectLen;
	}

	return true;
}

bool Collision::CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance, DirectX::XMVECTOR* inter)
{
	const float EPSILON = 1.0e-5f; //隱､蟾ｮ蜷ｸ蜿守畑縺ｮ蠕ｮ蟆上↑蛟､
	//髱｢豕慕ｷ壹→繝ｬ繧､縺ｮ譁ｹ蜷代・繧ｯ繝医Ν縺ｮ蜀・ｩ・
	float d1 = XMVector3Dot(plane.iNormal, ray.dir).m128_f32[0];
	//陬城擇縺ｫ縺ｯ蠖薙◆繧峨↑縺・
	if (d1 > -EPSILON) { return false; }
	//蟋狗せ縺ｨ蜴溽せ縺ｮ霍晞屬・亥ｹｳ髱｢縺ｮ豕慕ｷ壽婿蜷托ｼ・
	//髱｢豕慕ｷ壹→蜴溽せ縺ｮ蟋狗せ蠎ｧ讓呻ｼ井ｽ咲ｽｮ繝吶け繝医Ν・峨・蜀・ｩ・
	float d2 = XMVector3Dot(plane.iNormal, ray.start).m128_f32[0];
	//蟋狗せ縺ｨ蟷ｳ髱｢縺ｮ霍晞屬・亥ｹｳ髱｢縺ｮ豕慕ｷ壽婿蜷托ｼ・
	float dist = d2 - plane.iDistance;
	//蟋狗せ縺ｨ蟷ｳ髱｢縺ｮ霍晞屬・医Ξ繧､譁ｹ蜷托ｼ・
	float t = dist / -d1;
	//莠､轤ｹ縺悟ｧ狗せ繧医ｊ蠕後ｍ縺ｫ縺ゅｋ縺ｮ縺ｧ縲∝ｽ薙◆繧峨↑縺・
	if (t < 0) { return false; }
	//霍晞屬繧呈嶌縺崎ｾｼ繧
	if (distance) { *distance = t; }
	//莠､轤ｹ繧定ｨ育ｮ・
	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance, DirectX::XMVECTOR* inter)
{
	//荳芽ｧ貞ｽ｢縺御ｹ励▲縺ｦ縺・ｋ蟷ｳ髱｢繧堤ｮ怜・
	Plane plane;
	XMVECTOR interPlane;
	plane.iNormal = triangle.iNormal;
	plane.iDistance = XMVector3Dot(triangle.iNormal, triangle.iP0).m128_f32[0];

	//繝ｬ繧､縺ｨ蟷ｳ髱｢縺悟ｽ薙◆縺｣縺ｦ縺・↑縺代ｌ縺ｰ縲∝ｽ薙◆縺｣縺ｦ縺・↑縺・
	if (!CheckRay2Plane(ray, plane, distance, &interPlane)) { return false; }

	//繝ｬ繧､縺ｨ蟷ｳ髱｢縺後≠縺｣縺溘※縺・◆縺ｮ縺ｧ縲∬ｷ晞屬縺ｨ莠､轤ｹ縺梧嶌縺崎ｾｼ縺ｾ繧後◆
	//繝ｬ繧､縺ｨ蟷ｳ髱｢縺ｮ莠､轤ｹ縺御ｸ芽ｧ貞ｽ｢縺ｮ蜀・・縺ｫ縺ゅｋ縺句愛螳・
	const float EPSILON = 1.0e-5f;//隱､蟾ｮ蜷ｸ蜿守畑縺ｮ蠕ｮ蟆上↑蛟､
	XMVECTOR m;

	//霎ｺp0_p1縺ｫ縺､縺・※
	XMVECTOR pt_p0 = triangle.iP0 - interPlane;
	XMVECTOR p0_p1 = triangle.iP1 - triangle.iP0;
	m = XMVector3Cross(pt_p0, p0_p1);
	//霎ｺ縺ｮ螟門・縺ｧ縺ゅｌ縺ｰ蠖薙◆縺｣縺ｦ縺・↑縺・・縺ｧ蛻､螳壹ｒ謇薙■蛻・ｋ
	if (XMVector3Dot(m, triangle.iNormal).m128_f32[0] < -EPSILON) { return false; }

	//霎ｺp1_p2縺ｫ縺､縺・※
	XMVECTOR pt_p1 = triangle.iP1 - interPlane;
	XMVECTOR p1_p2 = triangle.iP2 - triangle.iP1;
	m = XMVector3Cross(pt_p1, p1_p2);
	//霎ｺ縺ｮ螟門・縺ｧ縺ゅｌ縺ｰ蠖薙◆縺｣縺ｦ縺・↑縺・・縺ｧ蛻､螳壹ｒ謇薙■蛻・ｋ
	if (XMVector3Dot(m, triangle.iNormal).m128_f32[0] < -EPSILON) { return false; }

	//霎ｺp2_p0縺ｫ縺､縺・※
	XMVECTOR pt_p2 = triangle.iP2 - interPlane;
	XMVECTOR p2_p0 = triangle.iP0 - triangle.iP2;
	m = XMVector3Cross(pt_p2, p2_p0);
	//霎ｺ縺ｮ螟門・縺ｧ縺ゅｌ縺ｰ蠖薙◆縺｣縺ｦ縺・↑縺・・縺ｧ蛻､螳壹ｒ謇薙■蛻・ｋ
	if (XMVector3Dot(m, triangle.iNormal).m128_f32[0] < -EPSILON) { return false; }

	//縺薙％縺ｾ縺ｧfalse縺罫eturn縺輔ｌ縺ｪ縺代ｌ縺ｰ縲∝・蛛ｴ縺ｪ縺ｮ縺ｧ蠖薙◆縺｣縺ｦ縺・ｋ
	if (inter) {
		*inter = interPlane;
	}

	return true;
}

bool Collision::CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance, DirectX::XMVECTOR* inter)
{
	XMVECTOR m = ray.start - sphere.center;
	float b = XMVector3Dot(m, ray.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.iRadius * sphere.iRadius;
	//繝ｬ繧､縺ｮ蟋狗せ縺茎phere縺ｮ螟門・縺ｫ縺ゅｊ・・ > 0・峨√Ξ繧､縺茎phere縺九ｉ髮｢繧後※縺・￥譁ｹ蜷代ｒ縺輔＠縺ｦ縺・ｋ蝣ｴ蜷茨ｼ・ > 0・峨∝ｽ薙◆繧峨↑縺・
	if (c > 0.0f && b > 0.0f) { return false; }

	float discr = b * b - c;
	//雋縺ｮ蛻､蛻･蠑上・繝ｬ繧､縺檎帥繧貞､悶ｌ縺ｦ縺・ｋ縺薙→縺ｫ荳閾ｴ(d < 0 = 隗｣・井ｺ､轤ｹ縺後↑縺・ｼ・
	if (discr < 0.0f) { return false; }

	//繝ｬ繧､縺ｯ逅・→莠､蟾ｮ縺励※縺・ｋ
	//莠､蟾ｮ縺吶ｋ譛蟆上・蛟､繧定ｨ育ｮ・
	float t = -b - sqrtf(discr);
	//t縺御ｸ阪〒縺ゅｋ蝣ｴ蜷医√Ξ繧､縺ｯ逅・・蜀・・縺九ｉ髢句ｧ九＠縺ｦ縺・ｋ縺ｮ縺ｧt繧・縺ｫ繧ｯ繝ｩ繝ｳ繝・
	if (t < 0) { t = 0.0f; }
	if (distance) { *distance = t; }

	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}


// 2D

bool Collision::CheckCircle2Circle(const Circle& circle, const Circle& circle2, DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	Sphere sphere;
	Sphere sphere2;

	sphere.center = { circle.center.x,circle.center.y };
	sphere2.center = { circle2.center.x,circle2.center.y };

	sphere.iRadius = circle.iRadius;
	sphere2.iRadius = circle2.iRadius;

	//逅・・蛻､螳壹ｒ蛻ｩ逕ｨ・・縺ｯ0・・
	return CheckSphere2Sphere(sphere, sphere2, inter, reject);
}

