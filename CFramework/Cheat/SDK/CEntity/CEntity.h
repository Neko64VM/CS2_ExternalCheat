#pragma once
#include "../GameSDK/GameSDK.h"

struct alignas(0x20) Collision {
	Vector3 vecMin{};
	Vector3 vecMax{};
};

struct alignas(0x10) BoundingBox {
	int left, right, top, bottom;
};

struct alignas(0x20) CBone {
	Vector3 position{}; 
	uint64_t address[2]{};
};

struct CSkeletonArray {
	CBone bone[28]{};
};

class CEntity
{
public:
	// pointer
	uintptr_t m_address{ 0 };
	uintptr_t m_pCSPlayerPawn{ 0 };
	uintptr_t m_pGameSceneNode{ 0 };
	uintptr_t m_pCollision{ 0 };
	uintptr_t m_pClippingWeapon{ 0 };
	uintptr_t m_pBoneArray{ 0 };

	// data
	int m_iTeamNum{ 0 };
	int m_iHealth{ 0 };
	int m_iMaxHealth{ 0 };
	std::string m_szPlayerName{};
	std::string m_szWeaponName{};
	Vector3 m_vOldOrigin{ 0.f, 0.f, 0.f };

    // functions
	bool Update();
	bool UpdateStaticData(const uintptr_t& entitylist);

	bool IsAlive();
	float GetYaw();
	uint32_t GetFlag();
	Collision GetCollision();
	BoundingBox GetBoundingBoxData(Matrix& ViewMatrix);
	Vector3 GetBoneByID(const int ID);
	CSkeletonArray GetBoneList();
	Vector2 GetEyeAngle();
	std::string GetEntityClassName();
	Vector3 GetCameraPosition();
};