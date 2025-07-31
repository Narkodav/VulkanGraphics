#include "Camera.h"

namespace Graphics {

	CameraBase::CameraBase(glm::mat4 projection) noexcept :
		m_position(glm::vec3(0.0f, 0.0f, 0.0f)),
		m_camFront(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_camUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		m_camRight(glm::vec3(1.0f, 0.0f, 0.0f)),
		m_worldUpVector(glm::vec3(0.0f, 1.0f, 0.0f)),
		m_camForward(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_pitch(0.0f), m_yaw(-glm::pi<float>() / 2.0f),
		m_view(glm::lookAt(m_position, m_position + m_camFront, m_worldUpVector)),
		m_viewWithoutTransposition(glm::mat4(glm::mat3(m_view))),
		m_projection(projection)
	{
		m_projection[1][1] *= -1;
	}

	CameraBase::CameraBase(glm::vec3 worldUpVector, glm::vec3 position,
		float pitch, float yaw, glm::mat4 projection) noexcept :
		m_projection(projection),
		m_pitch(pitch),
		m_yaw(yaw),
		m_worldUpVector(worldUpVector),
		m_position(position)
	{
		m_projection[1][1] *= -1;
		if (m_pitch > 89.f)
			m_pitch = 89.f;
		if (m_pitch < -89.f)
			m_pitch = -89.f;

		m_camFront.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
		m_camFront.y = glm::sin(glm::radians(m_pitch));
		m_camFront.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
		m_camFront = glm::normalize(m_camFront);

		m_camRight = glm::normalize(glm::cross(m_camFront, m_worldUpVector));
		m_camUp = glm::normalize(glm::cross(m_camRight, m_camFront));
		m_camForward = glm::normalize(glm::cross(m_worldUpVector, m_camRight));

		m_view = glm::lookAt(m_position, m_position + m_camFront, m_worldUpVector);
		m_viewWithoutTransposition = glm::mat4(glm::mat3(m_view));
	}


	void CameraBase::move(glm::vec3 deltaPos)
	{
		m_position += deltaPos;
		m_view = glm::translate(m_view, -deltaPos);
	}

	void CameraBase::rotate(float deltaPitch, float deltaYaw)
	{
		m_pitch += deltaPitch;
		m_yaw += deltaYaw;

		if (m_pitch > 89.f)
			m_pitch = 89.f;
		if (m_pitch < -89.f)
			m_pitch = -89.f;

		if (m_yaw > 360.f)
			m_yaw -= 360.f;
		else if (m_yaw < 0)
			m_yaw += 360;

		m_camFront.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
		m_camFront.y = glm::sin(glm::radians(m_pitch));
		m_camFront.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
		m_camFront = glm::normalize(m_camFront);
		m_camRight = glm::normalize(glm::cross(m_camFront, m_worldUpVector));
		m_camUp = glm::normalize(glm::cross(m_camRight, m_camFront));
		m_camForward = glm::normalize(glm::cross(m_worldUpVector, m_camRight));

		m_view = glm::lookAt(m_position, m_position + m_camFront, m_worldUpVector);
		m_viewWithoutTransposition = glm::mat4(glm::mat3(m_view));
	}
} // namespace Graphics