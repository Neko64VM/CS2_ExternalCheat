#include "CEntity.h"

// 毎フレーム更新する必要のある情報
bool CEntity::Update()
{
	if (!IsAlive())
		return false;

	m_vOldOrigin	  = m.Read<Vector3>(m_pCSPlayerPawn + offset::m_vOldOrigin);
	m_iHealth		  = m.Read<int>(m_pCSPlayerPawn + offset::m_iHealth);

	return true;
}

// 毎フレーム更新する必要のない情報
bool CEntity::UpdateStaticData(const uintptr_t& entitylist)
{
	// read pointer
	uint32_t hPlayerPawn = m.Read<uint32_t>(m_address + offset::m_hPlayerPawn);
	uintptr_t list = m.Read<uintptr_t>(entitylist + 0x8 * ((hPlayerPawn & 0x7FFF) >> 9) + 0x10);

	if (list == NULL)
		return false;

	// pointers
	m_pCSPlayerPawn = m.Read<uintptr_t>(list + 0x70 * (hPlayerPawn & 0x1FF));
	m_pCollision = m.Read<uintptr_t>(m_pCSPlayerPawn + offset::m_pCollision);
	m_pGameSceneNode = m.Read<uintptr_t>(m_pCSPlayerPawn + offset::m_pGameSceneNode);
	m_pBoneArray = m.Read<uintptr_t>(m_pGameSceneNode + (offset::m_modelState + 0x80));
	m_pClippingWeapon = m.Read<uintptr_t>(m_pCSPlayerPawn + offset::m_pClippingWeapon);

	// entity data
	m_iTeamNum = m.Read<int>(m_address + offset::m_iTeamNum);
	m_iMaxHealth = m.Read<int>(m_pCSPlayerPawn + offset::m_iMaxHealth);
	
	// Name
	uintptr_t pNameAddress = m.Read<uintptr_t>(m_address + offset::m_sSanitizedPlayerName);

	if (pNameAddress != NULL)
		m_szPlayerName = m.ReadStringA(pNameAddress);

	// Weapon name
	uintptr_t pWeaponEntity = m.ReadChain(m_pClippingWeapon, { 0x10, 0x20 });

	if (pWeaponEntity != NULL)
	{
		std::string szWName = m.ReadStringA(pWeaponEntity);

		if (szWName.find("weapon_") != std::string::npos)
			m_szWeaponName = szWName.substr(7, szWName.length());
	}

	return true;
}

bool CEntity::IsAlive()
{
	return m.Read<bool>(m_address + offset::m_bPawnIsAlive);
}

uint32_t CEntity::GetFlag()
{
	return m.Read<int>(m_address + offset::m_fFlags);
}

float CEntity::GetYaw()
{
	return m.Read<float>(m_address + offset::m_angEyeAngles + sizeof(float)); // Vector2(pitch, yaw)
}

Collision CEntity::GetCollision()
{
	return m.Read<Collision>(m_pCollision + 0x40);
}

BoundingBox CEntity::GetBoundingBoxData(Matrix& ViewMatrix)
{
	BoundingBox bbOut{};
	Collision collision = GetCollision();
	Vector3 min = collision.vecMin + this->m_vOldOrigin;
	Vector3 max = collision.vecMax + this->m_vOldOrigin;

	Vector2 flb, brt, blb, frt, frb, brb, blt, flt;

	Vector3 points[8] = { Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z),
				Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z),
				Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z) };

	if (!WorldToScreen(ViewMatrix, g.rcSize, points[3], flb) || !WorldToScreen(ViewMatrix, g.rcSize, points[5], brt) ||
		!WorldToScreen(ViewMatrix, g.rcSize, points[0], blb) || !WorldToScreen(ViewMatrix, g.rcSize, points[4], frt) ||
		!WorldToScreen(ViewMatrix, g.rcSize, points[2], frb) || !WorldToScreen(ViewMatrix, g.rcSize, points[1], brb) ||
		!WorldToScreen(ViewMatrix, g.rcSize, points[6], blt) || !WorldToScreen(ViewMatrix, g.rcSize, points[7], flt))
		return bbOut;

	Vector2 vec2_array[]{ flb, brt, blb, frt, frb, brb, blt, flt };
	bbOut = BoundingBox(flb.x, flb.x, flb.y, flb.y);

	for (auto j = 1; j < 8; ++j)
	{
		if (bbOut.left > vec2_array[j].x)
			bbOut.left = vec2_array[j].x;
		if (bbOut.bottom < vec2_array[j].y)
			bbOut.bottom = vec2_array[j].y;
		if (bbOut.right < vec2_array[j].x)
			bbOut.right = vec2_array[j].x;
		if (bbOut.top > vec2_array[j].y)
			bbOut.top = vec2_array[j].y;
	}

	return bbOut;
}

Vector3 CEntity::GetBoneByID(const int ID)
{
	return m.Read<Vector3>(m_pBoneArray + (ID * 0x20));
}

CSkeletonArray CEntity::GetBoneList()
{
	return m.Read<CSkeletonArray>(m_pBoneArray);
}

Vector2 CEntity::GetEyeAngle()
{
	return m.Read<Vector2>(m_pCSPlayerPawn + offset::m_angEyeAngles);
}

std::string CEntity::GetEntityClassName()
{
	uintptr_t pClassName = m.ReadChain(m_address, { 0x10, 0x20 });

	return m.ReadStringA(pClassName);
}

Vector3 CEntity::GetCameraPosition()
{
	return m.Read<Vector3>(m_pCSPlayerPawn + offset::m_vecLastClipCameraPos);
}