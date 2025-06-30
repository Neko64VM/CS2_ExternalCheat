#include "CFramework.h"
#include <cmath>
#define M_PI 3.14159265358979323846

CEntity lastTarget = CEntity();

bool InScreen(const BoundingBox* box)
{
    return !(box->top == 0 && box->bottom == 0 && box->left == 0 && box->right == 0);
}

void CFramework::RenderInfo()
{
    // FPS
    String(Vector2(3.f, 3.f), TEXT_COLOR, 1.f, std::to_string((int)ImGui::GetIO().Framerate).c_str());

    // FOV Circle
    if (g.AimBotEnable && g.bShowFOV)
    {
        DrawCircle(Vector2((g.rcSize.right / 2.f), (g.rcSize.bottom / 2.f)), g.AimFOV, g.bRainbowFOV ? GenerateRainbow() : g.Color_AimFOV, 0.35f);
    }
    
    // Crosshair
    if (g.CrosshairEnable)
    {
        switch (g.CrosshairType)
        {
        case 0: {
            ImVec2 Center = ImVec2(g.rcSize.right / 2, g.rcSize.bottom / 2);
            ImColor crosshair_col = WithAlpha(g.Color_Crosshair, g.m_flGlobalAlpha);

            DrawLine(Vector2(Center.x - g.CrosshairSize, Center.y), Vector2((Center.x + g.CrosshairSize) + 1, Center.y), crosshair_col, 1.f);
            DrawLine(Vector2(Center.x, Center.y - g.CrosshairSize), Vector2(Center.x, (Center.y + g.CrosshairSize) + 1), crosshair_col, 1.f);
        }   break;
        case 1:
            DrawCircleFilled(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), 0.85f); // 0.85f == CrosshairAlpha
            DrawCircleFilled(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.CrosshairSize, g.Color_Crosshair, 0.85f);
            break;
        }
    }
}

void CFramework::RenderESP()
{
    // AimBot
    float FOV{ 0.f };
    float MinFov{ FLT_MAX };
    float MinDistance{ FLT_MAX };
    CEntity target = CEntity();
    Vector2 ScreenMiddle{ g.rcSize.right / 2.f, g.rcSize.bottom / 2.f };

    // Local, ViewMatrix
    CEntity local = CEntity();
    CEntity* pLocal = &local;
    local.m_address = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwLocalPlayerController);
    uintptr_t entitylist = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwEntityList);

    if (!local.UpdateStaticData(entitylist))
        return;
    else if (!local.Update())
        return;

    Matrix ViewMatrix = m.Read<Matrix>(m.m_dwClientBaseAddr + g_game.dwViewMatrix);

    // Radar
    static Vector2 s_radar_size{ 250.f, 250.f };
    static Vector2 s_radar_pos{ 25.f, g.rcSize.bottom - (s_radar_size.y + 25.f) };
    static Vector2 s_radar_center{ s_radar_pos.x + s_radar_size.x / 2.f, s_radar_pos.y + s_radar_size.y / 2.f };

    if (g.ESP_Radar)
    {
        DrawLine(Vector2(s_radar_center.x, s_radar_pos.y), Vector2(s_radar_center.x, s_radar_pos.y + s_radar_size.y), ImColor(1.f, 1.f, 1.f, 1.f), 1.f);
        DrawLine(Vector2(s_radar_pos.x, s_radar_center.y), Vector2(s_radar_pos.x + s_radar_size.x, s_radar_center.y), ImColor(1.f, 1.f, 1.f, 1.f), 1.f);
        DrawCircleFilled(s_radar_center, 3.f, ImColor(0.f, 0.65f, 1.f, 1.f), 1.f);
    }

    // C4
    {
        CC4 pC4 = GetEntityC4();
        Vector3 vecC4Origin = pC4.GetPosition();

        if (!Vec3_Empty(vecC4Origin))
        {
            float flC4Blow = pC4.GetC4Blow();
            
            uintptr_t GlobalVars = m.Read<uintptr_t>(m.m_dwClientBaseAddr + g_game.dwGlobalVars);
            float flCurrentTime = m.Read<float>(GlobalVars + 0x34);

            if (flCurrentTime <= flC4Blow)
            {
                float result = flC4Blow - flCurrentTime;

                Vector2 vOut{};
                if (WorldToScreen(ViewMatrix, g.rcSize, vecC4Origin, vOut))
                {
                    std::string szC4 = "C4 [" + std::to_string((int)result) + "s]";
                    DrawCircleFilled(vOut, 2.f, ImColor(0.f, 1.f, 0.f, 1.f), 1.f);

                    // pC4.GetBombSite() == 0 ? "A" : "B";

                    String(Vector2(vOut.x - (ImGui::CalcTextSize(szC4.c_str()).x / 2.f), vOut.y + 1.f), ImColor(0.f, 1.f, 0.f, 1.f), ImGui::GetFontSize(), szC4.c_str());
                }
            }
        }
    }

    // ESP Loop
    for (auto& entity : this->GetEntityList())
    {
        CEntity* pEntity = &entity;

        if (!pEntity->IsAlive())
            continue;
        else if (!pEntity->Update())
            continue;

        const float fDistance = ((pLocal->m_vOldOrigin - pEntity->m_vOldOrigin).Length() * 0.01905f);

        if (g.ESP_MaxDistance < fDistance)
            continue;

        // Get Sizes
        const BoundingBox bbox = pEntity->GetBoundingBoxData(ViewMatrix);
        const int Height = bbox.bottom - bbox.top;
        const int Width = bbox.right - bbox.left;
        const int Center = (bbox.right - bbox.left) / 2.f;
        const int bScale = (bbox.right - bbox.left) / 3.f;

        // ToDo
        bool visible = false;

        ImColor shadow_color = WithAlpha(g.Color_ESP_Shadow, g.m_flShadowAlpha); // color + alpha
        ImColor tempColor = pLocal->m_iTeamNum == pEntity->m_iTeamNum ? g.Color_ESP_Team : g.Color_ESP_Enemy;
       
        if (pEntity->m_address == lastTarget.m_address) // aiming target?
            tempColor = g.Color_ESP_AimTarget;

        ImColor visualColor = WithAlpha(tempColor, g.m_flGlobalAlpha);

        if (InScreen(&bbox))
        {
            // Line
            if (g.bLine)
            {
                DrawLine(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom), Vector2(bbox.right - (Width / 2), bbox.bottom), visualColor, g.m_flGlobalAlpha);
            }

            // Box
            if (g.bBox)
            {
                // BoxFilled
                if (g.bFilled)
                    RectFilled(bbox.left, bbox.top, bbox.right, bbox.bottom, shadow_color, g.m_flShadowAlpha);

                /* Shadow
                DrawLine(Vector2(bbox.left - 1, bbox.top - 1), Vector2(bbox.right + 2, bbox.top - 1), shadow_color);
                DrawLine(Vector2(bbox.left - 1, bbox.top), Vector2(bbox.left - 1, bbox.bottom + 2), shadow_color);
                DrawLine(Vector2(bbox.right + 1, bbox.top), Vector2(bbox.right + 1, bbox.bottom + 2), shadow_color);
                DrawLine(Vector2(bbox.left - 1, bbox.bottom + 1), Vector2(bbox.right + 1, bbox.bottom + 1), shadow_color);
                */

                switch (g.ESP_BoxType)
                {
                case 0:
                    DrawBox(bbox.right, bbox.left, bbox.top, bbox.bottom, visualColor);
                    break;
                case 1:
                    DrawLine(Vector2(bbox.left, bbox.top), Vector2(bbox.left + bScale, bbox.top), visualColor); // Top
                    DrawLine(Vector2(bbox.right, bbox.top), Vector2(bbox.right - bScale, bbox.top), visualColor);
                    DrawLine(Vector2(bbox.left, bbox.top), Vector2(bbox.left, bbox.top + bScale), visualColor); // Left
                    DrawLine(Vector2(bbox.left, bbox.bottom), Vector2(bbox.left, bbox.bottom - bScale), visualColor);
                    DrawLine(Vector2(bbox.right, bbox.top), Vector2(bbox.right, bbox.top + bScale), visualColor); // Right
                    DrawLine(Vector2(bbox.right, bbox.bottom), Vector2(bbox.right, bbox.bottom - bScale), visualColor);
                    DrawLine(Vector2(bbox.left, bbox.bottom), Vector2(bbox.left + bScale, bbox.bottom), visualColor); // Bottom
                    DrawLine(Vector2(bbox.right + 1, bbox.bottom), Vector2(bbox.right - bScale, bbox.bottom), visualColor);
                    break;
                }
            }

            // Skeleton
            if (g.bSkeleton)
            {
                CSkeletonArray bArray = pEntity->GetBoneList();

                // レンダリング
                Vector2 pHead{}, pNeck{};
                if (WorldToScreen(ViewMatrix, g.rcSize, bArray.bone[BONE_HEAD].position, pHead) && WorldToScreen(ViewMatrix, g.rcSize, bArray.bone[BONE_NECK].position, pNeck))
                {
                    // 頭の円
                    DrawCircle(pHead, (pNeck.y - pHead.y) * 1.25, visualColor, g.m_flGlobalAlpha);

                    // 線を引くためのペアを作成する
                    const Vector3 skeleton_list[][2] = {
                        { bArray.bone[BONE_NECK].position, bArray.bone[BONE_HIP].position },
                        { bArray.bone[BONE_NECK].position, bArray.bone[BONE_LEFT_SHOULDER].position },
                        { bArray.bone[BONE_LEFT_SHOULDER].position, bArray.bone[BONE_LEFT_ARM].position },
                        { bArray.bone[BONE_LEFT_ARM].position, bArray.bone[BONE_LEFT_HAND].position },
                        { bArray.bone[BONE_NECK].position, bArray.bone[BONE_RIGHT_SHOULDER].position },
                        { bArray.bone[BONE_RIGHT_SHOULDER].position, bArray.bone[BONE_RIGHT_ARM].position },
                        { bArray.bone[BONE_RIGHT_ARM].position, bArray.bone[BONE_RIGHT_HAND].position },
                        { bArray.bone[BONE_HIP].position, bArray.bone[BONE_LEFT_KNEE].position },
                        { bArray.bone[BONE_LEFT_KNEE].position, bArray.bone[BONE_LEFT_FEET].position },
                        { bArray.bone[BONE_HIP].position, bArray.bone[BONE_RIGHT_KNEE].position },
                        { bArray.bone[BONE_RIGHT_KNEE].position, bArray.bone[BONE_RIGHT_FEET].position }
                    };

                    // WorldToScreenを行い各ペアをレンダリングする.
                    for (int j = 0; j < 11; j++)
                    {
                        Vector2 vOut0{}, vOut1{};
                        if (!WorldToScreen(ViewMatrix, g.rcSize, skeleton_list[j][0], vOut0) ||
                            !WorldToScreen(ViewMatrix, g.rcSize, skeleton_list[j][1], vOut1))
                            break;

                        DrawLine(vOut0, vOut1, visualColor);
                    }
                }
            }

            // Healthbar
            if (g.bHealth)
            {
                HealthBar(bbox.left - 3, bbox.bottom + 1, 1, -Height - 1, pEntity->m_iHealth, pEntity->m_iMaxHealth, shadow_color, g.m_flGlobalAlpha);
            }

            // Name
            if (g.bName)
            {
                StringEx(Vector2(bbox.right - Center - (ImGui::CalcTextSize(pEntity->m_szPlayerName.c_str()).x / 2.f), bbox.top - ImGui::GetFontSize()), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), pEntity->m_szPlayerName.c_str());
            }

            // Distance & Weapon
            if (g.bDistance || g.bWeapon)
            {
                std::string szResult{};

                if (g.bDistance)
                    szResult += "[ " + std::to_string((int)fDistance) + "m ]";

                if (g.bWeapon)
                    szResult += " " + pEntity->m_szWeaponName;

                // Rendering
                if (g.bDistance || g.bWeapon && szResult.size() > 0)
                    StringEx(Vector2(bbox.right - Center - (ImGui::CalcTextSize(szResult.c_str()).x / 2.f), bbox.bottom + 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), szResult.c_str());
            }
        }

        // 2D Radar
        if (g.ESP_Radar)
        {
            Vector3 delta = pEntity->m_vOldOrigin - pLocal->m_vOldOrigin;
            float yaw = pLocal->GetViewAngle().y * (M_PI / 180.f); // ToRadian
            float cosYaw = cosf(yaw);
            float sinYaw = sinf(yaw);

            Vector2 rotated{
                delta.y * cosYaw - delta.x * sinYaw,
                delta.y * sinYaw + delta.x * cosYaw
            };

            rotated /= g.ESP_RadarScale;
            rotated *= -1.f;
            rotated += s_radar_center;
            rotated.x = std::clamp(rotated.x, s_radar_pos.x, s_radar_pos.x + s_radar_size.x);
            rotated.y = std::clamp(rotated.y, s_radar_pos.y, s_radar_pos.y + s_radar_size.y);

            DrawCircleFilled(rotated, 3.f, visualColor, 1.f);
        }

        // AimBot
        if (InScreen(&bbox))
        {
            if (g.AimBotEnable && pLocal->m_iTeamNum != pEntity->m_iTeamNum)
            {
                if (fDistance > g.AimMaxDistance)
                    continue;

                for (int j = 0; j < 32; j++)
                {
                    CSkeletonArray bArray = pEntity->GetBoneList();

                    Vector2 BoneScreen{};
                    if (!WorldToScreen(ViewMatrix, g.rcSize, bArray.bone[j].position, BoneScreen))
                        break;

                    // In FOV?
                    FOV = abs((ScreenMiddle - BoneScreen).Length());

                    if (FOV < g.AimFOV)
                    {
                        switch (g.AimMode)
                        {
                        case 0: // Crosshair
                            if (MinFov > FOV) {
                                if (target.m_address == NULL || MinDistance > fDistance)
                                {
                                    target = entity;
                                    MinFov = FOV;
                                    MinDistance = fDistance;
                                }
                            }
                            break;
                        case 1: // Game Distance
                            if (MinDistance > fDistance) {
                                target = entity;
                                MinDistance = fDistance;
                            }
                            break;
                        }

                        break;
                    }
                }
            }
        }
    }

    // AimBot - ToDo
    if (target.m_address != NULL && AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.AimKeyMode))
    {
        int boneId = 1;
        switch (g.AimTargetBone)
        {
        case 0: boneId = BONE_HEAD; break;
        case 1: boneId = BONE_NECK; break;
        case 2: boneId = BONE_SPINE; break;
        case 3: boneId = BONE_HIP; break;
        default:
            break;
        }

        Vector2 Angle = CalcAngle(pLocal->GetCameraPosition(), target.GetBoneByID(boneId));
        Vector2 ViewAngle = pLocal->GetViewAngle();
        Vector2 Delta = Angle - ViewAngle;
        NormalizeAngles(Delta);
        Vector2 SmoothedAngle = ViewAngle + (Delta / g.AimSmooth);
        NormalizeAngles(SmoothedAngle);

        if (!Vec2_Empty(SmoothedAngle))
            m.Write<Vector2>(m.m_dwClientBaseAddr + g_game.dwViewAngles, SmoothedAngle);

        lastTarget = target;
    }
    else if (g.AimBotEnable) 
    {
        if (!AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.AimKeyMode))
            lastTarget = CEntity();
        else if (target.m_address == NULL)
            lastTarget = CEntity();
    }
}