#pragma once
#include "../Framework/ImGui/imgui.h"
#include "../Framework/ImGui/imgui_impl_win32.h"
#include "../Framework/ImGui/imgui_impl_dx11.h"
#include "../Framework/ImGui/Fonts/fa.h"
#include "../Framework/ImGui/Fonts/IconsFontAwesome6.h"
#include "../Framework/ImGui/Custom.h"
#include "SDK/CEntity/CEntity.h"
#include "SDK/CC4/CC4.h"
#include <mutex>
#pragma comment(lib, "freetype.lib")

class CFramework
{
public:
    ImFont* icon{ nullptr };

    void UpdateList();

	void RenderInfo();
	void RenderMenu();
	void RenderESP();
private:
    // Thread safe.
    std::mutex list_mutex;
    std::mutex c4_mutex;

    CC4 entityC4{};
    std::vector<CEntity> EntityList{};

    std::vector<CEntity> GetEntityList() {
        std::lock_guard<std::mutex> lock(list_mutex);
        return EntityList;
    }

    CC4 GetEntityC4() {
        std::lock_guard<std::mutex> lock(c4_mutex);
        return entityC4;
    }

    // AimBot KeyChecker
    bool AimBotKeyCheck(DWORD& AimKey0, DWORD& AimKey1, int AimKeyMode);

    ImColor TEXT_COLOR{ 1.f, 1.f, 1.f, 1.f };

    // Render.cpp
    ImVec2 ToImVec2(const ImVec2& value); // Convert to int
    ImVec2 ToImVec2(const Vector2& value);
    ImColor WithAlpha(const ImColor& color, const float& alpha);
    float GetHueFromTime(float speed = 5.0f);
    ImColor GenerateRainbow(float speed = 5.0f);
    void DrawLine(const Vector2 p1, const Vector2 p2, ImColor color, float thickness = 1.f);
    void DrawBox(int right, int left, int top, int bottom, ImColor color, float thickness = 1.f);
    void DrawCircle(const Vector2 pos, float size, ImColor color);
    void DrawCircle(const Vector2 pos, float size, ImColor color, float alpha);
    void DrawCircleFilled(const Vector2 pos, float size, ImColor color, float alpha);
    void RectFilled(int x0, int y0, int x1, int y1, ImColor color);
    void RectFilled(int x0, int y0, int x1, int y1, ImColor color, float alpha);
    void HealthBar(int x, int y, int w, int h, int value, int v_max, ImColor shadow, float global_alpha);
    void String(const Vector2 pos, ImColor color, float alpha, const char* text);
    void StringEx(Vector2 pos, ImColor shadow_color, float global_alpht, float font_size, const char* text);
};