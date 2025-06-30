#include "CC4.h"

bool CC4::IsPlanted()
{
	return m.Read<bool>(m.m_dwClientBaseAddr + g_game.dwPlantedC4 - 0x8);
}

float CC4::GetC4Blow()
{
	return  m.Read<float>(m_address + 0xFC0); // m_flC4Blow
}

int CC4::GetBombSite()
{
	return m.Read<int>(m_address + 0xF94); // m_nBombSite
}

Vector3 CC4::GetPosition()
{
	return m.Read<Vector3>(m_pGameSceneNode + 0xD0); // m_pGameSceneNode->m_vecPrevAbsOrigin
}