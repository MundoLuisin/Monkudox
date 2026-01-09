#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
	glm::vec3 position{ 0 };
	glm::vec3 rotation{ 0 };
	glm::vec3 scale{ 1 };

	glm::mat4 matrix{ 1.0f };

	// Update automatically the matrix when any property changes (IMPLEMENT THIS!!!)
	void UpdateMatrix() {
		glm::mat4 m = glm::mat4(1.0f);

		m = glm::translate(m, position);
		m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		m = glm::scale(m, scale);

		matrix = m;
	}

	glm::vec3 GetForwardVector() {
		glm::vec3 forward;
		forward.x = -sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		forward.y = sin(glm::radians(rotation.x));
		forward.z = -cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		return glm::normalize(forward);
	}

	glm::vec3 SetForward(glm::vec3 newForward) {
		newForward = glm::normalize(newForward);
		rotation.x = glm::degrees(asin(newForward.y));
		rotation.y = glm::degrees(atan2(-newForward.x, -newForward.z));
		return newForward;
	}
};
