#include "stdafx.h"
#include "Alien.h"
#include "Phys.h"
#include "Entity.h"
#include "EntityRegister.h"

#include "Debug.h"
#include "Render\Renderer.h"

Alien::Alien()
{
	bounding_box = { 0, 0, 16, 12 };
	weight = 1.0;
}

Vector2D normaliseVector(Vector2D v, double* magnitude);
Vector2D normaliseVector(Vector2D v, double* magnitude)
{
	double m = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
	Vector2D v_out = { v.x / m, v.y / m };
	if (magnitude)
		*magnitude = m;
	return v_out;
}

void Alien::runAI(const Entity* target)
{
	//this->current_state.vel.x = 16;
	//this->current_state.vel.y = 0;
	this->current_state.thrust.x = 100;
	Vector2D vec_alien = { this->current_state.pos.x + (this->bounding_box.w / 2), this->current_state.pos.y + (this->bounding_box.h/2) };
	Vector2D vec_target = { target->current_state.vel.x, target->current_state.vel.y };
	Vector2D vec_a_to_t = { target->current_state.pos.x - this->current_state.pos.x, target->current_state.pos.y - this->current_state.pos.y };

	double magnitude = 0.0;
	Vector2D vec_a_to_t_norm = normaliseVector(vec_a_to_t, &magnitude);
	debug->set("a_to_t magnitude", magnitude);
	debug->set("vec_a_to_t.x", vec_a_to_t.x);
	debug->set("vec_a_to_t.y", vec_a_to_t.y);

	attack_vector_n = vec_a_to_t_norm;

	Vector2D vec_alien_norm = normaliseVector(vec_alien, &magnitude);

	double dot_product = vec_a_to_t_norm.x * vec_alien_norm.x + vec_a_to_t_norm.y * vec_alien_norm.y;
	debug->set("dot_product", dot_product);
	double angle_a_to_t = std::acos(dot_product);
	debug->set("angle_a_to_t", angle_a_to_t);
	// Algorithm: thrust directly towards target

}