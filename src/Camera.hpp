#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/trigonometric.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

enum ECameraMovement {
	Forward,
	Backward,
	Left,
	Right
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 30.0f;
const float SENSITIVITY = 0.1f;

class Camera {
public:
	glm::vec3 Position{};
	glm::vec3 Front;
	glm::vec3 Up{};
	glm::vec3 Right{};
	glm::vec3 WorldUp{};

	bool LockRotation = false;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float LastX = 0, LastY = 0;

	explicit Camera(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW, float pitch = PITCH
	) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY) {
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}
	explicit Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		: Front(
			glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY) {
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}
	[[nodiscard]] glm::mat4 GetViewMatrix() const {
		return glm::lookAt(Position, Position + Front, Up);
	}

	[[nodiscard]] static glm::mat4 GetProjectionMatrix() {
		return glm::perspective(
			glm::radians(45.0f),
			(float)16 / 9,
			0.1f,
			5000.0f
		);
	}

	[[nodiscard]] glm::mat4 GetCameraMatrix() const {
		return GetProjectionMatrix() * GetViewMatrix();
	}

	void ProcessKeyboard(ECameraMovement direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		if (direction == ECameraMovement::Forward)
			Position += Front * velocity;
		if (direction == ECameraMovement::Backward)
			Position -= Front * velocity;
		if (direction == ECameraMovement::Left)
			Position -= Right * velocity;
		if (direction == ECameraMovement::Right)
			Position += Right * velocity;
	}

	void ProcessMouseMovement(float xPos, float yPos, bool constrainPitch = true) {
		float xoffset = xPos - LastX;
		float yoffset = LastY - yPos;

		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		LastY = yPos;
		LastX = xPos;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch) {
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		UpdateCameraVectors();
	}
private:
	void UpdateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};