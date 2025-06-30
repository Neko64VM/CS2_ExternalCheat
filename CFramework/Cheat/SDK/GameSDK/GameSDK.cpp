#include "GameSDK.h"

CGameAddress g_game;

bool Vec2_Empty(const Vector2& value)
{
    return value == Vector2();
}

bool Vec3_Empty(const Vector3& value)
{
    return value == Vector3();
}

void NormalizeAngles(Vector2& angle)
{
    while (angle.x > 89.0f)
        angle.x -= 180.f;

    while (angle.x < -89.0f)
        angle.x += 180.f;

    while (angle.y > 180.f)
        angle.y -= 360.f;

    while (angle.y < -180.f)
        angle.y += 360.f;
}

Vector2 CalcAngle(const Vector3& src, const Vector3& dst)
{
    Vector2 angle{};
    Vector3 delta = Vector3((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

    double distance = sqrt(delta.x * delta.x + delta.y * delta.y);

    angle.x = atan(delta.z / distance) * (180.0f / 3.1415926535);
    angle.y = atan(delta.y / delta.x) * (180.0f / 3.1415926535);

    if (delta.x >= 0.0) angle.y += 180.0f;

    return angle;
}

bool WorldToScreen(Matrix ViewMatrix, RECT Size, Vector3 vIn, Vector2& vOut)
{
    float w = ViewMatrix.m[3][0] * vIn.x + ViewMatrix.m[3][1] * vIn.y + ViewMatrix.m[3][2] * vIn.z + ViewMatrix.m[3][3];

    if (w < 0.001)
        return false;

    vOut.x = ViewMatrix.m[0][0] * vIn.x + ViewMatrix.m[0][1] * vIn.y + ViewMatrix.m[0][2] * vIn.z + ViewMatrix.m[0][3];
    vOut.y = ViewMatrix.m[1][0] * vIn.x + ViewMatrix.m[1][1] * vIn.y + ViewMatrix.m[1][2] * vIn.z + ViewMatrix.m[1][3];

    float invw = 1.f / w;

    vOut.x *= invw;
    vOut.y *= invw;

    float x = Size.right / 2;
    float y = Size.bottom / 2;

    x += 0.5 * vOut.x * Size.right + 0.5;
    y -= 0.5 * vOut.y * Size.bottom + 0.5;

    vOut.x = x;
    vOut.y = y;

    return true;
}