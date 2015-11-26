/*
* Copyright © 2015, Marcus Hultman
*/
#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

const glm::vec3 X_AXIS(1, 0, 0);
const glm::vec3 Y_AXIS(0, 1, 0);
const glm::vec3 Z_AXIS(0, 0, 1);

enum CameraMode{
	NONE,
	ARC,
	PAN
};
class Camera
{
public:
	Camera();
	Camera(glm::vec3 center);
	Camera(glm::vec3 center, glm::vec2 rotation);
	Camera(glm::vec3 center, glm::vec2 rotation, float zoom);
	~Camera();

	void pan(float dx, float dy);
	void pan(glm::vec2);
	void rotate(float dx, float dy);
	void rotate(glm::vec2);
	void zoom(float dz);

	glm::vec3	getCenter();
	void		setCenter(glm::vec3);
	glm::vec3	getPosition();
	void		setPosition(glm::vec3);
	float		getZoom();
	void		setZoom(float);

	glm::mat4	getView();

	CameraMode	getMode();
	void		setMode(CameraMode);
private:
	glm::vec3	m_center;
	glm::vec2	m_rotation;
	float		m_zoom;
	CameraMode	m_mode;
};

