/*
* Copyright (c) 2015, Marcus Hultman
*/
#pragma once

struct TwVars{
	double	info_fps;
	float	anim_speed = 0.6f;
	float	sim_gravity = 0;
	bool	sim_enable = true;
	bool	sim_step = false;
	bool	blend_useWeights = true;
	float	blend_multiplier = .8f;
	float	constraint_stiffness = 1.0f;
	bool	render_wireframe = true;
	bool	render_fill = true;
	bool	render_distMag = false;
};