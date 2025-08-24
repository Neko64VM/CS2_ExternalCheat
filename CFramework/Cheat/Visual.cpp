#include "CFramework.h"
#include <cmath>

#define M_PI 3.14159265358979323846

CEntity lastTarget = CEntity();
auto g_gui = std::make_unique<Renderer>();

// 厳密には違うけどね
bool InScreen(const BoundingBox* box)
{
    return !(box->top == 0 && box->bottom == 0 && box->left == 0 && box->right == 0);
}

void MouseMove(int dx, int dy) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dx = dx;
    input.mi.dy = dy;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;

    SendInput(1, &input, sizeof(INPUT));
}

void CFramework::RenderInfo()
{
    // FPS
    g_gui->String(Vector2(3.f, 3.f), g_gui->TEXT_COLOR, 1.f, std::to_string((int)ImGui::GetIO().Framerate).c_str());

    // FOV Circle
    if (g.bAimBotEnable && g.bShowFOV) {
        ImColor rainbow{ g_gui->GenerateRainbow() };
        rainbow.Value.w = 0.35f;
        g_gui->Circle(Vector2((g.rcSize.right / 2.f), (g.rcSize.bottom / 2.f)), g.iAimFov, g.bRainbowFOV ? rainbow : g.Color_AimFOV);
    }
    
    // Crosshair
    if (g.bCrosshairEnable)
    {
        switch (g.CrosshairType)
        {
        case 0: {
            ImVec2 Center = ImVec2(g.rcSize.right / 2, g.rcSize.bottom / 2);
            ImColor crosshair_col = g_gui->ApplyAlpha(g.Color_Crosshair, g.m_flGlobalAlpha);

            g_gui->Line(Vector2(Center.x - g.CrosshairSize, Center.y), Vector2((Center.x + g.CrosshairSize) + 1, Center.y), crosshair_col, 1.f);
            g_gui->Line(Vector2(Center.x, Center.y - g.CrosshairSize), Vector2(Center.x, (Center.y + g.CrosshairSize) + 1), crosshair_col, 1.f);
        }   break;
        case 1:
            g_gui->CircleFilled(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), 0.85f); // 0.85f == CrosshairAlpha
            g_gui->CircleFilled(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.CrosshairSize, g.Color_Crosshair, 0.85f);
            break;
        }
    }
}

void CFramework::RenderESP()
{
    // AimBot
    float MinFov{ FLT_MAX };
    float MinDistance{ FLT_MAX };
    CEntity target = CEntity();
    Vector2 screenCenter{ g.rcSize.right / 2.f, g.rcSize.bottom / 2.f };

    // Local
    CEntity local = GetLocalPlayer();

    if (!local.Update())
        return;

    // Update ViewMatrix
    Matrix ViewMatrix = m.Read<Matrix>(m.m_dwClientBaseAddr + g_game.dwViewMatrix);

    // Radar size and position
    static Vector2 s_radar_size{ 250.f, 250.f };
    static Vector2 s_radar_pos{ g.rcSize.right - (s_radar_size.x + 25.f) , 25.f};
    static Vector2 s_radar_center{ s_radar_pos.x + s_radar_size.x / 2.f, s_radar_pos.y + s_radar_size.y / 2.f };

    // Radar frame
    if (g.bShowRadar)
    {
        g_gui->Rect(Vector2(s_radar_pos), Vector2(s_radar_pos + s_radar_size), ImColor(1.f, 1.f, 1.f, 0.5f));
        g_gui->Line(Vector2(s_radar_center.x, s_radar_pos.y), Vector2(s_radar_center.x, s_radar_pos.y + s_radar_size.y), ImColor(1.f, 1.f, 1.f, 0.5f), 1.f);
        g_gui->Line(Vector2(s_radar_pos.x, s_radar_center.y), Vector2(s_radar_pos.x + s_radar_size.x, s_radar_center.y), ImColor(1.f, 1.f, 1.f, 0.5f), 1.f);
        g_gui->CircleFilled(s_radar_center, 3.f, ImColor(0.f, 0.65f, 1.f, 1.f), 1.f);

        // Circle
        for (int r = 1; 3 > r; r++) {
            g_gui->Circle(s_radar_center, 50.f * r, ImColor(1.f, 1.f, 1.f, 0.5f));
        }
    }

    // ESP Loop
    for (auto& entity : GetEntityList())
    {
        if (!entity.Update())
            continue;

        const float flDistance = ((local.m_vOldOrigin - entity.m_vOldOrigin).Length() * 0.01905f);

        if (g.flVisualMaxDistance < flDistance)
            continue;

        // ToDo
        bool visible = false;

        // Set color
        ImColor shadow_color = g_gui->ApplyAlpha(g.Color_ESP_Shadow, g.m_flShadowAlpha); // color + alpha
        ImColor tempColor = local.m_iTeamNum == entity.m_iTeamNum ? g.Color_ESP_Team : g.Color_ESP_Enemy;
       
        if (entity.m_address == lastTarget.m_address) // aiming target?
            tempColor = g.Color_ESP_AimTarget;

        ImColor visualColor = g_gui->ApplyAlpha(tempColor, g.m_flGlobalAlpha);

        // 2D Radar
        if (g.bShowRadar)
        {
            Vector3 delta = entity.m_vOldOrigin - local.m_vOldOrigin;
            float yaw = local.GetEyeAngle().y * (M_PI / 180.f); // ToRadian
            float cosYaw = cosf(yaw);
            float sinYaw = sinf(yaw);

            Vector2 rotated{
                delta.y * cosYaw - delta.x * sinYaw,
                delta.y * sinYaw + delta.x * cosYaw
            };

            rotated /= g.flRadarScale;
            rotated *= -1.f;
            rotated += s_radar_center;
            rotated.x = std::clamp(rotated.x, s_radar_pos.x, s_radar_pos.x + s_radar_size.x);
            rotated.y = std::clamp(rotated.y, s_radar_pos.y, s_radar_pos.y + s_radar_size.y);

            g_gui->CircleFilled(rotated, 3.f, visualColor, 1.f);
        }

        // Get Sizes #1
        const BoundingBox bbox = entity.GetBoundingBoxData(ViewMatrix);

        if (InScreen(&bbox))
        {
            // Get Sizes #2
            const int Height = bbox.bottom - bbox.top;
            const int Width = bbox.right - bbox.left;
            const int Center = (bbox.right - bbox.left) / 2.f;
            const int bScale = (bbox.right - bbox.left) / 3.f;

            // Line
            if (g.bLine)
            {
                g_gui->Line(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom), Vector2(bbox.right - (Width / 2), bbox.bottom), visualColor, g.m_flGlobalAlpha);
            }

            // Box
            if (g.bBox)
            {
                // Filled
                if (g.bFilled)
                    g_gui->RectFilled(bbox.left, bbox.top, bbox.right, bbox.bottom, shadow_color, g.m_flShadowAlpha);

                switch (g.iBoxType)
                {
                case 0:
                    g_gui->Rect(Vector2(bbox.left, bbox.top), Vector2(bbox.right, bbox.bottom), visualColor);
                    break;
                case 1:
                    g_gui->CorneredBox(Vector2(bbox.left, bbox.top), Vector2(bbox.right, bbox.bottom), bScale, visualColor);
                    break;
                }
            }

            // Skeleton
            if (g.bSkeleton)
            {
                CSkeletonArray bArray = entity.GetBoneList();

                // レンダリング
                Vector2 pHead{}, pNeck{};
                if (WorldToScreen(ViewMatrix, g.rcSize, bArray.bone[BONE_HEAD].position, pHead) && WorldToScreen(ViewMatrix, g.rcSize, bArray.bone[BONE_NECK].position, pNeck))
                {
                    // 頭の円
                    g_gui->Circle(pHead, (pNeck.y - pHead.y) * 1.25, visualColor);

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

                        g_gui->Line(vOut0, vOut1, visualColor);
                    }
                }
            }

            // Healthbar
            if (g.bHealth)
            {
                g_gui->HealthBar(bbox.left - 3, bbox.bottom + 1, 1, -Height - 1, entity.m_iHealth, entity.m_iMaxHealth, shadow_color, g.m_flGlobalAlpha);
            }

            // Name
            if (g.bName)
            {
                g_gui->StringEx(Vector2(bbox.right - Center - (ImGui::CalcTextSize(entity.m_szPlayerName.c_str()).x / 2.f), bbox.top - ImGui::GetFontSize()), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), entity.m_szPlayerName.c_str());
            }

            // Distance & Weapon
            if (g.bDistance || g.bWeapon)
            {
                std::string szResult{};

                if (g.bDistance)
                    szResult += "[ " + std::to_string((int)flDistance) + "m ]";

                if (g.bWeapon)
                    szResult += " " + entity.m_szWeaponName;

                // Rendering
                if (g.bDistance || g.bWeapon && szResult.size() > 0)
                    g_gui->StringEx(Vector2(bbox.right - Center - (ImGui::CalcTextSize(szResult.c_str()).x / 2.f), bbox.bottom + 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), szResult.c_str());
            }

            // AimBot
            if (g.bAimBotEnable && local.m_iTeamNum != entity.m_iTeamNum)
            {
                if (flDistance > g.iAimMaxDistance)
                    continue;

                int boneId = 1;
                switch (g.iAimTargetBone)
                {
                case 0: boneId = BONE_HEAD; break;
                case 1: boneId = BONE_NECK; break;
                case 2: boneId = BONE_SPINE; break;
                case 3: boneId = BONE_HIP; break;
                default:
                    break;
                }

                Vector2 BoneScreen{};
                if (!WorldToScreen(ViewMatrix, g.rcSize, entity.GetBoneByID(BONE_HEAD), BoneScreen))
                    break;

                // In FOV?
                float FOV = abs((screenCenter - BoneScreen).Length());

                if (FOV < g.iAimFov)
                {
                    if (target.m_address == NULL || MinFov > FOV)
                    {
                        target = entity;
                        MinFov = FOV;
                        MinDistance = flDistance;
                    }
                }

                // AIM target line
                if (lastTarget.m_address == entity.m_address)
                    g_gui->Line(screenCenter, BoneScreen, ImColor(1.f, 0.f, 0.f, 1.f));
            }
        }
    }

    // AimBot - ToDo
    if (target.m_address != NULL && AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.iAimKeyMode))
    {
        if (!target.IsAlive()) {
            target = CEntity();
            lastTarget = CEntity();
            return;
        }
        
        int boneId = 1;
        switch (g.iAimTargetBone)
        {
        case 0: boneId = BONE_HEAD; break;
        case 1: boneId = BONE_NECK; break;
        case 2: boneId = BONE_SPINE; break;
        case 3: boneId = BONE_HIP; break;
        default:
            break;
        }

        Vector2 bonePos{};
        if (WorldToScreen(ViewMatrix, g.rcSize, target.GetBoneByID(boneId), bonePos))
        {
            Vector2 relative = screenCenter - bonePos;
            relative *= -1;

            relative /= g.flAimSmooth;

            MouseMove(relative.x, relative.y);
        }

        /*
        Memory based aimbot.
        Vector2 Angle = CalcAngle(local.GetCameraPosition(), target.GetBoneByID(boneId));
        Vector2 ViewAngle = local.GetViewAngle();
        Vector2 Delta = Angle - ViewAngle;
        NormalizeAngles(Delta);
        Vector2 SmoothedAngle = ViewAngle + (Delta / g.AimSmooth);
        NormalizeAngles(SmoothedAngle);

        if (!Vec2_Empty(SmoothedAngle))
            m.Write<Vector2>(m.m_dwClientBaseAddr + g_game.dwViewAngles, SmoothedAngle);
        */

        lastTarget = target;
    }
    else if (g.bAimBotEnable) 
    {
        if (!AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.iAimKeyMode))
            lastTarget = CEntity();
        else if (target.m_address == NULL)
            lastTarget = CEntity();
    }
}