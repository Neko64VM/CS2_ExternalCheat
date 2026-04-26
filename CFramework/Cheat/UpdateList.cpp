#include "CFramework.h"

constexpr int ReadCount{ 64 };

struct CGlobalVars
{
    float m_realtime;
    __int32 m_framecount;
    float m_frametime;
    float m_abs_frametime;
    __int32 m_maxclients;
    char pad_0014[28]; //0x0014
    float m_frametime2; //0x0030
    float m_curtime; //0x0034
    float m_curtime2; //0x0038
    char pad_003C[20]; //0x003C
    __int32 m_tickcount; //0x0050
};

void CFramework::UpdateList()
{
    while (g_ApplicationActive)
    {
        bool skip = false;

        auto pEntityList = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwEntityList);

        if (pEntityList == NULL)
            skip = true;

        uintptr_t local_address = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwLocalPlayerController);
        CEntity local{ CEntity(local_address) };

        // Optional
        if (!local.IsAlive())
            skip = true;

        if (skip) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        // Update LocalPlayer
        if (local.UpdateStaticData(pEntityList))
        {
            if (local.Update()) {
                std::lock_guard<std::mutex> lock(m_mutex);
                localplayer = local;
            }
        }

        // Player
        std::vector<CEntity> temp_entitylist{};

        for (int i = 0; i < ReadCount; i++)
        {
            uintptr_t entity_entry = m.Read<uintptr_t>(pEntityList + (0x8 * (i & 0x7FFF) >> 9) + 0x10);

            if (entity_entry == NULL)
                continue;
            else if (entity_entry == local.m_address)
                continue;

			uintptr_t entity_address = m.Read<uintptr_t>(entity_entry + 0x70 * (i & 0x1FF));
            CEntity p{ CEntity(entity_address) };

            if (!p.IsAlive())
                continue;

            // player check
            if (!p.GetEntityClassName().compare("cs_player_controller"))
            {
                // some checks
                if (!p.UpdateStaticData(pEntityList))
                    continue;
                else if (p.m_iTeamNum == local.m_iTeamNum)
                    continue;

                temp_entitylist.push_back(p);
                continue;
            }
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        localplayer = local;
        entitylist = temp_entitylist;
    }
}