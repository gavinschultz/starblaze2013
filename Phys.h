#pragma once

struct Point2D
{
	double x;
	double y;
	Point2D(double x = 0.0, double y = 0.0);
};

struct Vector2D
{
	double x;
	double y;
	Vector2D(double x = 0.0, double y = 0.0);
};

struct State2D
{
	Point2D pos;
	Vector2D vel;
	Vector2D acc;
	Vector2D thrust;
};