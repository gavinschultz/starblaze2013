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
	double x;
	double y;
	Point2D(double x = 0.0, double y = 0.0) : x(x), y(y) {};
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
	double x;
	double y;
	Vector2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}
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

struct Rect
{
	double x, y;
	double w, h;
	Rect(double x = 0.0, double y = 0.0, double w = 0.0, double h = 0.0) : x(x), y(y), w(w), h(h) {}
};

struct State2D
{
	Point2D pos;
	Vector2D vel;
	Vector2D acc;
	Vector2D thrust;
};

struct TemporalState2D
{
	State2D current;
	State2D prev;
	Point2D interpolated;
};