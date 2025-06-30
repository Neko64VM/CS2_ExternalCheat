#pragma once
#include "a2x/buttons.hpp"
#include "a2x/client_dll.hpp"
#include "a2x/offsets.hpp"
#include "../../../Framework/Config/Config.h"
using namespace cs2_dumper::schemas::client_dll;

namespace offset
{
	// Address
	constexpr auto dwViewAngles = cs2_dumper::offsets::client_dll::dwViewAngles;

	// Offsets
	constexpr auto m_hPlayerPawn = CCSPlayerController::m_hPlayerPawn;
	constexpr auto m_sSanitizedPlayerName = CCSPlayerController::m_sSanitizedPlayerName;
	constexpr auto m_fFlags = C_BaseEntity::m_fFlags;
	constexpr auto m_iHealth = C_BaseEntity::m_iHealth;
	constexpr auto m_iMaxHealth = C_BaseEntity::m_iMaxHealth;
	constexpr auto m_iTeamNum = C_BaseEntity::m_iTeamNum;
	constexpr auto m_pCollision = C_BaseEntity::m_pCollision;
	constexpr auto m_pGameSceneNode = C_BaseEntity::m_pGameSceneNode;
	constexpr auto m_vecLastClipCameraPos = C_CSPlayerPawnBase::m_vecLastClipCameraPos;
	constexpr auto m_angEyeAngles = C_CSPlayerPawnBase::m_angEyeAngles;
	constexpr auto m_pClippingWeapon = C_CSPlayerPawnBase::m_pClippingWeapon;
	constexpr auto m_vOldOrigin = C_BasePlayerPawn::m_vOldOrigin;
	constexpr auto m_modelState = CSkeletonInstance::m_modelState;
	constexpr auto m_bPawnIsAlive = CCSPlayerController::m_bPawnIsAlive;
};

class CGameAddress
{
public:
	bool InitOffset();

	uintptr_t dwViewAngles{ 0 };
	uintptr_t dwViewMatrix{ 0 };
	uintptr_t dwEntityList{ 0 };
	uintptr_t dwLocalPlayerController{ 0 };
	uintptr_t dwGlobalVars{ 0 };
	uintptr_t dwPlantedC4{ 0 };
};

enum CBoneID
{
	BONE_HEAD = 6,
	BONE_NECK = 5,
	BONE_SPINE = 4,
	BONE_SPINE_1 = 2,
	BONE_HIP = 0,
	BONE_LEFT_SHOULDER = 8,
	BONE_LEFT_ARM = 9,
	BONE_LEFT_HAND = 10,
	BONE_RIGHT_SHOULDER = 13,
	BONE_RIGHT_ARM = 14,
	BONE_RIGHT_HAND = 15,
	BONE_LEFT_HIP = 22,
	BONE_LEFT_KNEE = 23,
	BONE_LEFT_FEET = 24,
	BONE_RIGHT_HIP = 25,
	BONE_RIGHT_KNEE = 26,
	BONE_RIGHT_FEET = 27,
};

extern CGameAddress g_game;
extern bool Vec2_Empty(const Vector2& value);
extern bool Vec3_Empty(const Vector3& value);
extern void NormalizeAngles(Vector2& angle);
extern Vector2 CalcAngle(const Vector3& src, const Vector3& dst);
extern bool WorldToScreen(Matrix ViewMatrix, RECT Size, Vector3 vIn, Vector2& vOut);