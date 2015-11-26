#include "Camera.h"

#define CAM_INITIAL_CENTER glm::vec3(0.0f, 0.0f, 0.0f)
#define CAM_INITIAL_ROTATION glm::vec2(0.0f, 0.0f)
#define CAM_INITIAL_ZOOM 1.75f

#define CAM_ZOOM_SPEED 0.25f
#define CAM_ZOOM_MIN 0.01f
#define CAM_ZOOM_MAX 10.0f

#define CAM_PAN_SPEED glm::vec2(-0.003f, 0.003f)
#define CAM_ARC_SPEED glm::vec2(0.003f, -0.003f)

Camera::Camera() : 
	Camera(CAM_INITIAL_CENTER, CAM_INITIAL_ROTATION, CAM_INITIAL_ZOOM)
{ }
Camera::Camera(glm::vec3 center) :
Camera(center, CAM_INITIAL_ROTATION, CAM_INITIAL_ZOOM)
{ }
Camera::Camera(glm::vec3 center, glm::vec2 rotation) :
Camera(center, rotation, CAM_INITIAL_ZOOM)
{ }
Camera::Camera(glm::vec3 center, glm::vec2 rotation, float zoom)
{
	m_center = center;
	m_rotation = rotation;
	m_zoom = zoom;
	m_mode = NONE;
}

Camera::~Camera()
{
}

void Camera::pan(float dx, float dy)
{
	pan(glm::vec2(dx, dy));
}
void Camera::pan(glm::vec2 offset)
{
	glm::vec3 dir = m_center - getPosition();
	glm::vec3 loc_x = glm::normalize(glm::cross<float,
		glm::highp>(dir, Y_AXIS));
	glm::vec3 loc_y = glm::normalize(glm::cross<float,
		glm::highp>(dir, loc_x));
	offset *= CAM_PAN_SPEED;
	m_center += offset.x * loc_x + offset.y * loc_y;
}
void Camera::rotate(float dx, float dy)
{
	rotate(glm::vec2(dx, dy));
}
void Camera::rotate(glm::vec2 offset)
{
	m_rotation += CAM_ARC_SPEED * offset;
}
void Camera::zoom(float dz)
{
	setZoom(m_zoom - CAM_ZOOM_SPEED * dz);
}
glm::vec3 Camera::getCenter()
{
	return m_center;
}
void Camera::setCenter(glm::vec3 center)
{
	m_center = center;
}
glm::vec3 Camera::getPosition()
{
	return m_center + (m_zoom * Z_AXIS) *
		glm::angleAxis(m_rotation.y, X_AXIS) *
		glm::angleAxis(m_rotation.x, Y_AXIS);
}
float Camera::getZoom()
{
	return m_zoom;
}
void Camera::setZoom(float zoom)
{
	m_zoom = std::max(CAM_ZOOM_MIN,
		std::min(zoom, CAM_ZOOM_MAX));
}

glm::mat4 Camera::getView()
{
	return glm::lookAt(getPosition(), m_center, Y_AXIS);
}

CameraMode Camera::getMode()
{
	return m_mode;
}
void Camera::setMode(CameraMode mode)
{
	m_mode = mode;
}