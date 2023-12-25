/*
* @file Util.h
* @brief 便利な機能詰め合わせ
*/

#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include "Vec4.h"
#include "M4.h"
#include "Quaternion.h"
#include"TextureManager.h"

namespace Constant
{
	//0
	static const float ZERO = 0;
	//0.5
	static const float HALF = 0.5f;
	//2.0
	static const float TWICE = 2.0f;
	//1.0f
	static const float RATE_MAX_ = 1.0f;
}

//自作
//vec4とm4の掛け算をして代入
void Vec4xM4(Vec4& v, const M4& m4);
//vec3とm4の掛け算をして代入
void Vec3xM4(Vec3& v, const M4& m4, bool w);
//vec3とm4の掛け算
Vec3 GetVec3xM4(const Vec3& v, const M4& m4, bool w);
//vec3とm4の掛け算をして、出たベクトルを割る
void Vec3xM4andDivisionW(Vec3& v, const M4& m4, bool w);

//--------------------------------------------------------------------------------------------------
//ラジアン系
//角度をラジアンに変換
float AngletoRadi(float angle);
//ラジアンを角度に変換
float RaditoAngle(float radian);
//ベクトルのなす角度を取得
float GetRadianVec3(const Vec3& v1, const Vec3& v2);

//--------------------------------------------------------------------------------------------------
//線形補完
float Lerp(float v1, float v2, float t);
//線形補完(二次元ベクトル)
Vec2 LerpVec2(const Vec2& v1, const Vec2& v2, float t);
//線形補完(三次元ベクトル)
Vec3 LerpVec3(const Vec3& v1, const Vec3& v2, float t);
//球面線形補完(三次元ベクトル)
Vec3 SlerpVec3(const Vec3& v1, const Vec3& v2, float t);

//スプライン補間
Vec3 SplinePosition(const std::vector<Vec3>& points, int32_t startIndex, float t);

//イージング---------------------------------------------------------------------------------------
//イーズイン
float EaseIn(float t);
//イーズアウト
float EaseOut(float t);
//イーズアウトバック
float EaseInOutBack(float t);
//イーズインアウト
float EaseInOut(float t);

//--------------------------------------------------------------------------------------------------
//円同士の判定
bool CollisionCircleCircle(const Vec3& pos1, float r1,
	const Vec3& pos2, float r2);
//円とレイの判定
bool CollisionRayCircle(const Vec3& sv, const Vec3& ev, float r, const Vec3& pos, float r2);
//四角形同士の判定
bool CollisionBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t r1, int32_t r2);
//四角形同士の判定(Vec2を使った)
bool CollisionBox(Vec2 leftUp1, Vec2 rightBottom1, Vec2 leftUp2, Vec2 rightBottom2);

//--------------------------------------------------------------------------------------------------
//便利
/// <summary>
/// 1か‐1で返す（0は0）
/// </summary>
/// <returns></returns>
float sign(float num);

//--------------------------------------------------------------------------------------------------
//3次元を2次元に変換
Vec2 Vec3toVec2(const Vec3& v, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
/// <summary>
/// 2次元を3次元に変換(distanceは、projectionのnearPosからの位置!!
/// </summary>
/// <param name="v"></param>
/// <param name="view"></param>
/// <param name="projection"></param>
/// <param name="distance"></param>
/// <returns></returns>
Vec3 Vec2toVec3(const Vec2& v, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, float distance);

/// <summary>
/// スクリーン座標をprojectionのnear,farの位置に変換してreturn...Posに代入
/// </summary>
/// <param name="v"></param>
/// <param name="returnNearPos"></param>
/// <param name="returnFarPos"></param>
/// <param name="view"></param>
/// <param name="projection"></param>
void Vec2toNearFarPos(const Vec2& pos, Vec3& returnNearPos, Vec3& returnFarPos, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);

//--------------------------------------------------------------------------------------------------
//アライメントにそろえたサイズを返す
int32_t AligmentSize(int32_t size, int32_t aligment);

//-----------------------------------------------------------
//文字変換系
//char* を const wchar_t* に変換
void ConstCharToWcharT(const char* string, wchar_t(&wString)[128]);

//const wchar_t* を char* に変換
void ConstWCharTToChar(const wchar_t* wString, char(&string)[128]);

//文字列からfindChar以外を抜き取る(その文字から後ろのみ)
std::string MySubstr(const std::string& str, char findChar);

//ディレクトリを含んだファイルパスからファイル名(***.pmg)を抽出する
std::string ExtractFileName(const std::string& path);

//-------------------------------------------------------------
//スムースステップ
float SmoothStep(float min, float max, float v);

//クランプ
float Clamp(float v, float min, float max);

//ランダム
float GetRand(float min, float max);

//------------------------------------------------------------------------
//回転行列から角度を求める（クォータニオンから）
Vec3 GetRotFromQuaternion(Quaternion q);
//回転行列から角度を求める(行列から)
Vec3 GetRotFromMat(M4 m);
//ほぼ等しいかどうか
bool Approximately(float a, float b);

//ベクトルを角度を使ってクォータニオンで回転
Vec3 GetTurnVec3UseQuaternionAndRot(const Vec3& vec, const Vec3& rot);

//-----------------------------------------------------------------------
//フォルダ系
bool GetFileNames(std::string folderPath, std::vector<std::string>& file_names);

//--------------------------------------------------------------------------------------------------
//XMFLOAT4同士の足し算
const DirectX::XMFLOAT4 operator+(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs);
//XMFLOAT4同士の引き算
const DirectX::XMFLOAT4 operator-(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs);
//XMFLOAT3同士の足し算
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
//XMFLOAT3同士の引き算
const DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
//XMFLOAT2同士の足し算
const DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);
//XMFLOAT2同士の引き算
const DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);
//XMFLOAT4をfloatで割る
const DirectX::XMFLOAT4 operator/(const DirectX::XMFLOAT4& lhs, float RHS);
//XMFLOAT3をfloatで掛ける
const DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, float rhs);
//XMFLOAT2をfloatで掛ける
const DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, float rhs);