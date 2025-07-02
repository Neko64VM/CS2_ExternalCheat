#include "CFramework.h"

float Renderer::GetHueFromTime(float speed) {
    float t = static_cast<float>(ImGui::GetTime());
    return fmodf(t / speed, 1.0f);
}

ImColor Renderer::GenerateRainbow(float speed) {
    float hue = GetHueFromTime(speed);
    return ImColor::HSV(hue, 1.0f, 1.0f);
}

ImVec2 Renderer::ToImVec2(const ImVec2& value) {
    return ImVec2((int)value.x, (int)value.y);
}

ImVec2 Renderer::ToImVec2(const Vector2& value) {
    return ImVec2((int)value.x, (int)value.y);
}

ImColor Renderer::ApplyAlpha(const ImColor& color, const float& alpha) {
    return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
}

void Renderer::Line(const Vector2 p1, const Vector2 p2, ImColor color, float thickness)
{
    ImGui::GetBackgroundDrawList()->AddLine(ToImVec2(p1), ToImVec2(p2), color, thickness);
}

void Renderer::Circle(const Vector2 pos, float size, ImColor color)
{
    ImGui::GetBackgroundDrawList()->AddCircle(ToImVec2(pos), size, color);
}

void Renderer::CircleA(const Vector2 pos, float size, ImColor color, float alpha)
{
    ImGui::GetBackgroundDrawList()->AddCircle(ToImVec2(pos), size, ApplyAlpha(color, alpha));
}

void Renderer::CircleFilled(const Vector2 pos, float size, ImColor color, float alpha)
{
    ImGui::GetBackgroundDrawList()->AddCircleFilled(ToImVec2(pos), size, ApplyAlpha(color, alpha));
}

void Renderer::Rect(Vector2 min, Vector2 max, ImColor color)
{
    ImGui::GetBackgroundDrawList()->AddRect(ToImVec2(Vector2(min)), ToImVec2(Vector2(max)), color);
}

void Renderer::RectFilled(int x0, int y0, int x1, int y1, ImColor color)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(x0, y0)), ToImVec2(Vector2(x1, y1)), color);
}

void Renderer::RectFilled(int x0, int y0, int x1, int y1, ImColor color, float alpha)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(x0, y0)), ToImVec2(Vector2(x1, y1)), ApplyAlpha(color, alpha));
}

void Renderer::HealthBar(int x, int y, int w, int h, int value, int v_max, ImColor shadow, float global_alpha)
{
    RectFilled(x - 1, y + 1, x + w + 1, y + h - 1, shadow);
    RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), ImColor(min(510 * (v_max - value) / 100, 255), min(510 * value / 100, 255), 25, (int)(255 * global_alpha)), global_alpha);
}

void Renderer::String(const Vector2 pos, ImColor color, float alpha, const char* text)
{
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ToImVec2(pos), ApplyAlpha(color, alpha), text, text + strlen(text), 512);
}

void Renderer::StringEx(Vector2 pos, ImColor shadow_color, float global_alpht, float font_size, const char* text)
{
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), font_size, ToImVec2(Vector2(pos.x + 1.f, pos.y + 1.f)), shadow_color, text, text + strlen(text), 512, 0); // Shadow
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ToImVec2(pos), ApplyAlpha(TEXT_COLOR, global_alpht), text, text + strlen(text), 512); // Text
}