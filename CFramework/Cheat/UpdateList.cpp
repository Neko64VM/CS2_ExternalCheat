#include "CFramework.h"

constexpr int ReadCount = 64;

void CFramework::UpdateList()
{
    while (g_ApplicationActive)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(333));

        // EntityList found?
        auto pEntityList = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwEntityList);

        if (pEntityList == NULL)
            continue;

        // Get LocalPlayer
        CEntity local = CEntity();
        local.m_address = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwLocalPlayerController);

        if (!local.IsAlive())
            continue;

        if (!local.UpdateStaticData(pEntityList))
            continue;
        else if (!local.Update())
            continue;

        // C4
        CC4 tC4 = CC4();

        if (tC4.IsPlanted())
        {
            uintptr_t pEntityC4 = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwPlantedC4);

            if (pEntityC4 != NULL)
            {
                tC4.m_address = m.Read<uintptr_t>(pEntityC4);

                if (!Vec3_Empty(tC4.GetPosition()))
                {
                    std::lock_guard<std::mutex> lock(c4_mutex);
                    entityC4 = tC4;
                }
            }
        }

        std::vector<CEntity> list_result{};

        // Loop
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