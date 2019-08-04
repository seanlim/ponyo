#pragma once

#include <d3d9.h>
#include <d3dx9.h>

namespace Vec2NS
{
static float Vector2Length(const Vec2* v) { return D3DXVec2Length(v); }

static Vec2 Vector2Difference(Vec2 v1, Vec2 v2)
{
  return Vec2(v1.x - v2.x, v1.y - v2.y);
}

static float Vector2Dot(const Vec2* v1, const Vec2* v2)
{
  return D3DXVec2Dot(v1, v2);
}

static void Vector2Normalize(Vec2* v) { D3DXVec2Normalize(v, v); }

static Vec2* Vector2Transform(Vec2* v, D3DXMATRIX* m)
{
  return D3DXVec2TransformCoord(v, v, m);
}
} // namespace Vec2NS