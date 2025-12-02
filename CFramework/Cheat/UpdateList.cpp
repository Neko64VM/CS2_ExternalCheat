#include "CFramework.h"

constexpr int ReadCount{ 64 };

void CFramework::UpdateList()
{
    while (g_ApplicationActive)
    {
        bool skip = false;

        auto pEntityList = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwEntityList);

        if (pEntityList == NULL)
            skip = true;

        CEntity local = CEntity();
        local.m_address = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwLocalPlayerController);

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

            CEntity p = CEntity();
            p.m_address = m.Read<uintptr_t>(entity_entry + 0x70 * (i & 0x1FF));

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