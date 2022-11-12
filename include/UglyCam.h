#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

enum cameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

enum Mode {DEBUG, PLAY};

const float YAW		= -90.0f;
const float PITCH	= 0.0f;
const float SPEED	=10.5f;
const float SENSITIVITY = 0.1f;

class UglyCam {

public:
	Mode camMode;
	// Attributes
	glm::vec3 Position, Front, Up, Right, WorldUp;

	// Euler Angles
	float Yaw, Pitch;

	float MovementSpeed, MouseSensitivity;

	UglyCam(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0), float yaw = YAW, float pitch = PITCH, Mode mode = PLAY)
		: Front(glm::vec3(0.0, 0.0, -1.0)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		camMode = mode;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}

	void checkCollision(Model model, float deltaTime, glm::mat4 matrix) {
		if (camMode == PLAY) {
			Mesh mesh = model.meshes[0];
			Vertex PlayerVertice = mesh.vertices[0];

			glm::vec3 adaptV = glm::vec3(matrix * glm::vec4((PlayerVertice.Position), 1.0f)); // Fazemos a própria multiplicação da Matriz com o vértice
			glm::vec3 finalV;

			float prevDiff = fabs(fabs(adaptV.x) - fabs(Position.x)) + fabs(fabs(adaptV.z) - fabs(Position.z));

			for (int i = 0; i < model.meshes.size(); i++) {
				mesh = model.meshes[i];
				for (int j = 0; j < mesh.vertices.size(); j++) {
					Vertex tempV = model.meshes[i].vertices[j];
					adaptV = glm::vec3(matrix * glm::vec4((tempV.Position), 1.0f));
					float diff = fabs(fabs(adaptV.x) - fabs(Position.x)) + fabs(fabs(adaptV.z) - fabs(Position.z));
					if (diff < prevDiff) {
						PlayerVertice = tempV;
						prevDiff = diff;
						finalV = adaptV;
					}
				}
			}
			if (Position.y < (finalV.y + 7.0f))
				Position.y += 2.0f * deltaTime;
			if (Position.y > (finalV.y + 7.0f))
				Position.y -= 2.0f * deltaTime;
		}
	}

	void ProcessKeyboard(cameraMovement direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD) {
			if (camMode == PLAY) {
				Position.x += cos(glm::radians(Yaw)) * velocity;
				Position.z += sin(glm::radians(Yaw)) * velocity;
			} else 
				Position += Front * velocity;
		}
		if (direction == BACKWARD) {
			if (camMode == PLAY) {
				Position.x -= cos(glm::radians(Yaw)) * velocity;
				Position.z -= sin(glm::radians(Yaw)) * velocity;
			} else
				Position -= Front * velocity;
		}
		if (direction == LEFT) {
			if (camMode == PLAY) {
				Position.x -= Right.x * velocity;
				Position.z -= Right.z * velocity;
			} else 
				Position -= Right * velocity;
		}
		if (direction == RIGHT) {
			if (camMode == PLAY) {
				Position.x += Right.x * velocity;
				Position.z += Right.z * velocity;
			} else 
				Position += Right * velocity;

		}
		if (direction == UP && camMode == DEBUG)
			Position.y += 2.0 * velocity;
		if (direction == DOWN && camMode == DEBUG)
			Position.y -= 2.0 * velocity;


	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw		+= xoffset;
		Pitch	-= yoffset;

		if (constrainPitch) {
			if (Pitch > 89.0) Pitch = 89.0;
			else if (Pitch < -89.0) Pitch = -89.0;
		}

		updateCameraVectors();
	}

	void SwitchMode() {
		if (camMode == DEBUG)
			camMode = PLAY;
		else
			camMode = DEBUG;
		std::cout << camMode << std::endl;
	}

private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		Right	= glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
