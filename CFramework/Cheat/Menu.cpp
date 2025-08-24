#include "CFramework.h"

// ImGui::Combo/ImGui::List等で使う文字列群
const char* AimBoneList[] = { "Head", "Neck", "Chest", "Spine" };
const char* AimKeyModeList[] = { "None Key", "and", "or" };
const char* BoxTypeList[] = { "Simple", "Cornered" };
const char* CrosshairList[] = { "Cross", "Circle" };
std::vector<const char*> MenuStringList = { "AimBot", "Visual", "Misc", "Setting" };
std::vector<const char*> MenuIconList = { ICON_FA_CROSSHAIRS, ICON_FA_EYE, ICON_FA_BARS, ICON_FA_GEAR };

// Menu, Config
int Index = 0;
int BindingID = 0;
int FileNum = 0;
bool DeleteFlag = false;

// チートのメニュー
void CFramework::RenderMenu()
{
    // Setup
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    //ImGui::SetNextWindowBgAlpha(0.975f);
    ImGui::SetNextWindowSize(ImVec2(725.f, 450.f));
    ImGui::Begin("Counter-Strike 2 [ EXTERNAL ]", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    //---// Clild 0 //-----------------------------------//
    ImGui::BeginChild("##SelectChild", ImVec2(150.f, ImGui::GetContentRegionAvail().y), false);

    ImGui::SetCursorPosY(25.f);

    ImGui::PushFont(icon);

    for (int i = 0; i < MenuIconList.size(); i++) {
        if (ImGui::CustomButton(MenuIconList[i], MenuStringList[i], ImVec2(ImGui::GetContentRegionAvail().x, 35.f), Index == i ? true : false))
            Index = i;
    }

    ImGui::PopFont();

    ImGui::EndChild();
    //---// Clild 0 End //-------------------------------//

    ImGui::SameLine();

    //---// Clild 1 //-----------------------------------//
    ImGui::BeginChild("##MainChild", ImVec2(ImGui::GetContentRegionAvail()), false);

    //---// Left //--------------------------------------//
    ImGui::BeginChild("##LeftChild", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - (style.WindowPadding.x * 2), ImGui::GetContentRegionAvail().y), false);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));

    switch (Index)
    {
    case 0:
        ImGui::BeginChild("##C000", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 3.f), true);
        ImGui::Text("Visual");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("AimBot", &g.bAimBotEnable);

        ImGui::EndChild();
        ImGui::BeginChild("##C001", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("AimBot Setting");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::CustomSliderInt("Aim FOV", "##a_fov", &g.iAimFov, 30, 150);
        ImGui::CustomSliderFloat("Smooth", "##a_smt", &g.flAimSmooth, 1.f, 30.f);
        ImGui::CustomSliderInt("MaxDistance", "##a_dist", &g.iAimMaxDistance, 15, 150);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Combo("TargetBone", &g.iAimTargetBone, AimBoneList, IM_ARRAYSIZE(AimBoneList));

        ImGui::EndChild();
        break;
    case 1: // visual
        ImGui::BeginChild("##C010", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 3.f), true);
        ImGui::Text("Visual");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("ESP", &g.bVisualEnable);
        ImGui::Checkbox("2D Radar", &g.bShowRadar);
        ImGui::Checkbox("Team ESP", &g.bShowTeam);

        ImGui::EndChild();
        ImGui::BeginChild("##C011", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("ESP Options");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Box", &g.bBox);
        ImGui::Checkbox("BoxFilled", &g.bFilled);
        ImGui::Checkbox("Line", &g.bLine);
        ImGui::Checkbox("Skeleton", &g.bSkeleton);
        ImGui::Checkbox("HealthBar", &g.bHealth);
        ImGui::Checkbox("Name", &g.bName);
        ImGui::Checkbox("Distance", &g.bDistance);
        ImGui::Checkbox("Weapon", &g.bWeapon);

        ImGui::EndChild();
        break;
    case 2: { // misc
        /*
        ImGui::BeginChild("##C020", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.5f), true);

        ImGui::Checkbox("BunnyHop", &g.g_bHop);
        ImGui::Checkbox("RCS", &g.g_RCS);

        ImGui::EndChild();
        ImGui::BeginChild("##C021", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.f), true);
        
        ImGui::Text("RCS");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("RCS", &g.g_RCS);
        ImGui::CustomSliderFloat("Scale", "##RCS_Scale", &g.g_RCS_Scale, 0.f, 1.f, "%.2f");

        ImGui::EndChild();
        */
    } break;
    case 3: // system
        ImGui::BeginChild("##C030", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 1.75f), true);

        ImGui::Text("System");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("Crosshair", &g.bCrosshairEnable);

        ImGui::NewLine();

        ImGui::CustomSliderInt("MaxFramerate", "##MaxFrame", &g.iMaxFramerate, 30, 500);

        ImGui::EndChild();
        ImGui::BeginChild("##C031", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("Crosshair");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Crosshair", &g.bCrosshairEnable);
        ImGui::CustomSliderInt("CrosshairSize", "##SizeCH", &g.CrosshairSize, 1, 10);
        ImGui::ColorEdit4("Color##C", &g.Color_Crosshair.Value.x);
        ImGui::Combo("Type##C", &g.CrosshairType, CrosshairList, IM_ARRAYSIZE(CrosshairList));

        ImGui::EndChild();
        break;
    default:
        break;
    }

    ImGui::PopStyleColor();

    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::SameLine();

    //---// Right //--------------------------------------//
    ImGui::BeginChild("##RightChild", ImVec2(ImGui::GetContentRegionAvail()), false);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));

    switch (Index)
    {
    case 0:
        ImGui::BeginChild("##C100", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.7f), true);

        ImGui::Text("FOV Setting");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Draw FOV", &g.bShowFOV);
        ImGui::Checkbox("Rainbow FOV", &g.bRainbowFOV);
        ImGui::ColorEdit3("Color", &g.Color_AimFOV.Value.x);
        ImGui::CustomSliderInt("Aim FOV", "##a_fov", &g.iAimFov, 30, 300);

        ImGui::EndChild();
        ImGui::BeginChild("##101", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("KeyBind");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Combo("KeyMode", &g.iAimKeyMode, AimKeyModeList, IM_ARRAYSIZE(AimKeyModeList));

        ImGui::NewLine();

        ImGui::Text("1st Key");
        if (ImGui::Button(BindingID == 1 ? "< Press Any Key >" : utils::KeyNames[g.dwAimKey0], ImVec2(215.f, 22.5f))) {
            BindingID = 1;
            std::thread([&]() { utils::KeyBinder(g.dwAimKey0, BindingID); }).detach();
        }

        ImGui::Spacing();

        ImGui::Text("2nd Key");
        if (ImGui::Button(BindingID == 2 ? "< Press Any Key >" : utils::KeyNames[g.dwAimKey1], ImVec2(215.f, 22.5f))) {
            BindingID = 2;
            std::thread([&]() { utils::KeyBinder(g.dwAimKey1, BindingID); }).detach();
        }

        // 重複防止
        if (g.dwAimKey0 == g.dwAimKey1)
            g.dwAimKey1 = NULL;

        ImGui::EndChild();
        break;
    case 1: // visual
        ImGui::BeginChild("##C110", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.75f), true);

        ImGui::Text("ESP Setting");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::CustomSliderFloat("Radar scale", "##R_SCALE", &g.flRadarScale, 1.f, 20.f);
        ImGui::CustomSliderInt("Distance", "##Distance", &g.flVisualMaxDistance, 10, 100);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Combo("BoxType", &g.iBoxType, BoxTypeList, IM_ARRAYSIZE(BoxTypeList));

        ImGui::EndChild();
        ImGui::BeginChild("##C111", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("ESP Colors");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::ColorEdit3("Enemy", &g.Color_ESP_Enemy.Value.x);
        ImGui::ColorEdit3("AimTarget", &g.Color_ESP_AimTarget.Value.x);
        ImGui::ColorEdit3("Team", &g.Color_ESP_Team.Value.x);
        ImGui::ColorEdit3("Shadow", &g.Color_ESP_Shadow.Value.x, ImGuiColorEditFlags_DisplayRGB);

        ImGui::NewLine();

        ImGui::SeparatorText("Alpha");
        ImGui::CustomSliderFloat("Global", "##GlobalA", &g.m_flGlobalAlpha, 0.3f, 1.0f, "%.2f");
        ImGui::CustomSliderFloat("Shadow", "##ShadowA", &g.m_flShadowAlpha, 0.1f, 0.3f, "%.2f");

        ImGui::EndChild();
        break;
    case 2: // misc
        ImGui::BeginChild("##120", ImVec2(ImGui::GetContentRegionAvail()), true);

        

        ImGui::EndChild();
        break;
    case 3: // system
    {
        ImGui::BeginChild("##130", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 1.25f), true);

        ImGui::Text("Config");
        ImGui::Separator();
        ImGui::Spacing();

        // 設定ファイルのリストを作成
        auto vec = config.GetFileList();
        const char** FileList = new const char* [vec.size()];

        for (size_t j = 0; j < vec.size(); j++)
            FileList[j] = vec[j].c_str();

        if (!g.bGenerateFlag)
        {
            if (ImGui::Button("Generate ConfigFile", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
            {
                g.bGenerateFlag = true;
                g.newConfigName.clear();
                utils::EnableKeyboardHook();
            }
        }

        if (g.bGenerateFlag)
        {
            ImGui::TextColored(ImColor(1.f, 0.f, 0.f, 1.f), "New config name :");
            ImGui::SameLine();
            ImGui::Text("%s.json", g.newConfigName.c_str());

            if (ImGui::Button("Generate", ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 20.f)) || utils::IsKeyDown(VK_RETURN))
            {
                if (g.newConfigName.size() > 1) {
                    std::thread([&]() { config.CreateNewConfig(g.newConfigName); }).join();
                }

                g.bGenerateFlag = false;
                g.newConfigName.clear();
                utils::DisableKeyboardHook();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
            {
                g.bGenerateFlag = false;
                g.newConfigName.clear();
                utils::DisableKeyboardHook();
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::ListBox("##List", &FileNum, FileList, vec.size());

        if (!g.bGenerateFlag && !DeleteFlag)
        {
            // Button
            if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x / 3.f - 4.f, 20.f))) {
                std::thread([&]() { config.SaveSetting(FileList[FileNum]); }).join();
            }
            ImGui::SameLine();
            if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 4.f, 20.f)) && vec.size() != 0) {
                std::thread([&]() { config.LoadSetting(FileList[FileNum]); }).join();
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)) && vec.size() != 0) {
                DeleteFlag = true;
            }
        }

        if (DeleteFlag)
        {
            ImGui::Text("Delete this file?");

            if (ImGui::Button("OK", ImVec2(90.f, 20.f))) {
                std::thread([&]() { config.DeleteConfig(FileList[FileNum]); }).join();
                DeleteFlag = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(90.f, 20.f)))
                DeleteFlag = false;
        }

        delete[] FileList;

        ImGui::EndChild();
        ImGui::BeginChild("##131", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("Exit");
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Exit", ImVec2(ImGui::GetContentRegionAvail().x, 30.f)))
            g_ApplicationActive = false;

        ImGui::EndChild();
    }   break;
    default:
        break;
    }

    ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::End();
}