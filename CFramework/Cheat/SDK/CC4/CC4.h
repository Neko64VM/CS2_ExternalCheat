#pragma once
#include "../GameSDK/GameSDK.h"

class CC4
{
public:
	// pointer
	uintptr_t m_address{ 0 };
	uintptr_t m_pGameSceneNode{ 0 };

    // functions
	bool IsPlanted();
	float GetC4Blow(); // m_flC4Blow
	int GetBombSite(); // m_nBombSite
	Vector3 GetPosition(); // m_pGameSceneNode->m_vecPrevAbsOrigin
};