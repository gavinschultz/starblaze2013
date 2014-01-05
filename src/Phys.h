#pragma once
#include <stdint.h>

struct Window
{
public:
	unsigned int w;
	unsigned int h;
	Window(unsigned int w = 0, unsigned int h = 0) : w(w), h(h) {};
};

struct Point2D
{
	float x;
	float y;
	Point2D(float x = 0.0, float y = 0.0) : x(x), y(y) {};
};

struct Point2Di
{
	int x;
	int y;
	Point2Di(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Point2Du
{
	unsigned int x;
	unsigned int y;
	Point2Du(unsigned int x = 0, unsigned int y = 0) : x(x), y(y) {}
};

struct Vector2D
{
	float x;
	float y;
	Vector2D(float x = 0.0, float y = 0.0) : x(x), y(y) {}
};

struct Vector2Di
{
	int x;
	int y;
	Vector2Di(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Vector2Du
{
	unsigned int x;
	unsigned int y;
	Vector2Du(unsigned int x = 0, unsigned int y = 0) : x(x), y(y) {}
};

// A simple X/Y boolean pair
struct Bool2D
{
	bool x;
	bool y;
	Bool2D(bool x = false, bool y = false) : x(x), y(y) {}
};

struct Rect
{
	float x, y;
	float w, h;
	Rect(float x = 0.0, float y = 0.0, float w = 0.0, float h = 0.0) : x(x), y(y), w(w), h(h) {}
};

struct State2D
{
	Point2D pos;
	Vector2D vel;
	Vector2D acc;
	State2D(Point2D pos = { 0.0, 0.0 }, Vector2D vel = { 0.0, 0.0 }, Vector2D acc = { 0.0, 0.0 }) : pos(pos), vel(vel), acc(acc) {}
};