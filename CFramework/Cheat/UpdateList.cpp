#include "CFramework.h"

constexpr int ReadCount{ 64 };

void CFramework::UpdateList()
{
    while (g_ApplicationActive)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // EntityList found?
        auto pEntityList = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwEntityList);

        if (pEntityList == NULL)
            continue;

        CEntity local = CEntity();
        local.m_address = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwLocalPlayerController);

        // Optional
        if (!local.IsAlive())
            continue;

        // Update LocalPlayer
        if (local.UpdateStaticData(pEntityList))
        {
            if (local.Update()) {
                std::lock_guard<std::mutex> lock(m_mtxLocal);
                localplayer = local;
            }
        }

        // Player
        std::vector<CEntity> list_result{};

        for (int i = 0; i < ReadCount; i++)
        {
            uintptr_t entity_entry = m.Read<uintptr_t>(pEntityList + (0x8 * (i & 0x7FFF) >> 9) + 0x10);

            if (entity_entry == NULL)
                continue;
            else if (entity_entry == local.m_address)
                continue;

            CEntity p = CEntity();
            p.m_address = m.Read<uintptr_t>(entity_entry + 120 * (i & 0x1FF));

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

                list_result.push_back(p);
                continue;
            }
        }

        std::lock_guard<std::mutex> lock(list_mutex);
        EntityList = list_result;
    }
}