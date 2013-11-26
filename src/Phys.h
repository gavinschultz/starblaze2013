#pragma once
#include <stdint.h>

struct Point2D
{
	double x;
	double y;
	Point2D(double x = 0.0, double y = 0.0) : x(x), y(y) {};
};

struct Point2Di
{
	int32_t x;
	int32_t y;
	Point2Di(int32_t x = 0, int32_t y = 0) : x(x), y(y) {}
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
	int32_t x;
	int32_t y;
	Vector2Di(int32_t x = 0, int32_t y = 0) : x(x), y(y) {}
};

struct Vector2Du
{
	unsigned int x;
	unsigned int y;
	Vector2Du(unsigned int x = 0, unsigned int y = 0) : x(x), y(y) {}
};

struct State2D
{
	Point2D pos;
	Vector2D vel;
	Vector2D acc;
	Vector2D thrust;
	int loop_count{ 0 };
};