#pragma once
#include "../Framework/ImGui/imgui.h"
#include "../Framework/ImGui/imgui_impl_win32.h"
#include "../Framework/ImGui/imgui_impl_dx11.h"
#include "../Framework/ImGui/Fonts/fa.h"
#include "../Framework/ImGui/Fonts/IconsFontAwesome6.h"
#include "../Framework/ImGui/Custom.h"
#include "SDK/CEntity/CEntity.h"
#include <mutex>
#pragma comment(lib, "freetype.lib")

class CFramework
{
public:
    bool Init();
    void UpdateList();
	void RenderInfo();
	void RenderMenu();
	void RenderESP();
private:
    ImFont* icon{ nullptr };
    ImColor TEXT_COLOR_DEFAULT{ 1.f, 1.f, 1.f, 1.f };
    ImColor TEXT_COLOR_ATTENTION{ 1.f, 1.f, 0.f, 1.f };
    ImColor TEXT_COLOR_WARNING{ 1.f, 0.f, 0.f, 1.f };

    std::mutex m_mutex;
    CEntity localplayer{ CEntity(NULL) };
    std::vector<CEntity> entitylist;

    bool AimBotKeyCheck(DWORD& AimKey0, DWORD& AimKey1, int AimKeyMode);
};

class Renderer
{
public:
    ImColor TEXT_COLOR{ 1.f, 1.f, 1.f, 1.f };

    // Render.cpp
    ImVec2 ToImVec2(const ImVec2& value); // Convert to int
    ImVec2 ToImVec2(const Vector2& value);
    ImColor ApplyAlpha(const ImColor& color, const float& alpha);
    float GetHueFromTime(float speed = 5.0f);
    ImColor GenerateRainbow(float speed = 5.0f);
    void Line(const Vector2 p1, const Vector2 p2, ImColor color, float thickness = 1.f);
    void Circle(const Vector2 pos, float size, ImColor color);
    void CircleFilled(const Vector2 pos, float size, ImColor color, float alpha);
    void Rect(Vector2 min, Vector2 max, ImColor color);
    void CorneredBox(Vector2 min, Vector2 max, int scale, ImColor color);
    void RectFilled(int x0, int y0, int x1, int y1, ImColor color);
    void RectFilled(int x0, int y0, int x1, int y1, ImColor color, float alpha);
    void HealthBar(int x, int y, int w, int h, int value, int v_max, ImColor shadow, float global_alpha);
    void String(const Vector2 pos, ImColor color, float alpha, const char* text);
    void StringEx(Vector2 pos, ImColor shadow_color, float global_alpht, float font_size, const char* text);
};