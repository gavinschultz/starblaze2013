#include "stdafx.h"
#include <SDL.h>
#include "Camera.h"
#include "playfield.h"
#include "component.h"
#include "renderutil.h"
#include "mathutil.h"

class Camera::impl
{
public:
	SDL_Rect view_rect;
	SDL_Rect focus_rect;
	SDL_Rect prev_focus_rect;
	Point2Di focus_point;
	Vector2Di focus_point_vel;
	Point2Di prev_focus_point;

	void update(Camera& camera)
	{
		auto ship_id = db->getEntityIds(E::eship)[0];
		auto ship_state = (TemporalState2DComponent*)db->getComponentOfTypeForEntity(ship_id, C::ctemporalstate);
		auto ship_orient = (HorizontalOrientComponent*)db->getComponentOfTypeForEntity(ship_id, C::chorient);
		auto ship_phys = (PhysicalComponent*)db->getComponentOfTypeForEntity(ship_id, C::cphysical);
		auto playfield = db->getPlayField();
		auto playarea = playfield->getPlayArea(ship_phys->box);

		this->prev_focus_point = this->focus_point;
		this->prev_focus_rect = this->focus_rect;
		this->focus_point = { std::lround(ship_state->interpolated.x), std::lround(ship_state->interpolated.y) };

		this->focus_point_vel.x = std::lround(playfield->getRelativePosX(this->prev_focus_point.x, this->focus_point.x));

		int32_t turn_speed;
		if ((this->focus_point_vel.x < 0 && ship_orient->direction == HOrient::right) || (this->focus_point_vel.x > 0 && ship_orient->direction == HOrient::left))
			turn_speed = 8L;
		else
			turn_speed = (int)std::max(std::abs(lround(this->focus_point_vel.x * 0.3)), 8L);
		debug::set("cam_turn_speed", turn_speed);
		debug::set("cam focus_point.vel.x", this->focus_point_vel.x);
		int max_velocity{ 0 };
		int desired_x_abs{ 0 };
		if (ship_orient->direction == HOrient::right)
		{
			desired_x_abs = mathutil::abswrap(this->focus_point.x, playfield->w);
			max_velocity = this->focus_point_vel.x + turn_speed;
		}
		else
		{
			desired_x_abs = mathutil::abswrap(this->focus_point.x + ship_phys->box.w - this->focus_rect.w, playfield->w);
			max_velocity = this->focus_point_vel.x - turn_speed;
		}
		int proposed_x_abs = this->prev_focus_rect.x + max_velocity;

		if (ship_orient->direction == HOrient::right)
		{	
			if (playfield->isRightOf(desired_x_abs, proposed_x_abs))
				this->focus_rect.x = mathutil::abswrap(proposed_x_abs, playfield->w);
			else
				this->focus_rect.x = desired_x_abs;
		}
		else
		{
			if (playfield->isLeftOf(desired_x_abs, proposed_x_abs))
				this->focus_rect.x = mathutil::abswrap(proposed_x_abs, playfield->w);
			else
				this->focus_rect.x = desired_x_abs;
		}

		this->view_rect.x = mathutil::abswrap(this->focus_rect.x - playfield->boundaries.x, playfield->w);
	}
};

Camera::Camera(const SDL_Rect& view_rect, const SDL_Rect& focus_rect) : pi{ new impl{} }
{
	pi->view_rect = view_rect;
	pi->focus_rect = focus_rect;
	pi->focus_point = { 0, 0 };
}

Camera::~Camera() = default;

void Camera::update()
{
	pi->update(*this);
}

SDL_Rect Camera::getViewRect() const
{
	return pi->view_rect;
}

SDL_Rect Camera::getFocusRect() const
{
	return pi->focus_rect;
}