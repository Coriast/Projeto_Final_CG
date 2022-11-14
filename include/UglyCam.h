#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

enum cameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

enum Mode { DEBUG, PLAY };

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 15.5f;
const float SENSITIVITY = 0.1f;

class UglyCam {

public:
	Mode camMode;
	// Attributes
	glm::vec3 Position, Front, Up, Right, WorldUp;
	vector<vector<glm::vec3>> CollisionPos;

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

	void checkCollision(Model model, glm::mat4 matrix) {
		if (camMode == PLAY) {

			glm::vec3 fPos = glm::vec3(matrix * glm::vec4(model.meshes[0].vertices[0].Position, 1.0f));

			glm::vec3 top, bottom;
			top.x = fPos.x;
			top.z = fPos.z;;
			bottom.x = fPos.x;
			bottom.z = fPos.z;

			for (int i = 0; i < model.meshes.size(); i++) {
				Mesh mesh = model.meshes[i];

				for (int i = 0; i < mesh.vertices.size(); i++) {
					glm::vec3 Vpos = glm::vec3(matrix * glm::vec4(mesh.vertices[i].Position, 1.0f));

					if (Vpos.x > top.x) {
						top.x = Vpos.x;
					}
					else if (Vpos.x < bottom.x) {
						bottom.x = Vpos.x;
					}

					if (Vpos.z > top.z) {
						top.z = Vpos.z;
					}
					else if (Vpos.z < bottom.z) {
						bottom.z = Vpos.z;
					}
				}
				CollisionPos.push_back({ top, bottom });
			}

			
		}
	}

	void checkCollisionGround(Model model, float deltaTime, glm::mat4 matrix) {
		if (camMode == PLAY) {
			Mesh mesh = model.meshes[0];
			Vertex PlayerVertice = mesh.vertices[0];

			glm::vec3 adaptV = glm::vec3(matrix * glm::vec4((PlayerVertice.Position), 1.0f)); // Fazemos a pr�pria multiplica��o da Matriz com o v�rtice
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

			float camHeight = (finalV.y + 7.0f);

			if (Position.y < camHeight)
				Position.y += fabs(Position.y - camHeight) * 5.0f * deltaTime;
			if (Position.y > camHeight)
				Position.y -= fabs(Position.y - camHeight) * 5.0f * deltaTime;
		}
	}

	bool checkKeyCollision(float checkPosX, float checkPosZ, cameraMovement direction) {
		float PosX = -40.0, PosZ = -40.0;
		switch (direction) {
		case FORWARD:
			PosX = Position.x + checkPosX;
			PosZ = Position.z + checkPosZ;
			break;
		case BACKWARD:
			PosX = Position.x - checkPosX;
			PosZ = Position.z - checkPosZ;
			break;
		case LEFT:
			PosX = Position.x - checkPosX;
			PosZ = Position.z - checkPosZ;
			break;
		case RIGHT:
			PosX = Position.x + checkPosX;
			PosZ = Position.z + checkPosZ;
			break;
		default:
			break;
		}

		for (int i = 0; i < CollisionPos.size(); i++) {
			if (PosX < CollisionPos[i][0].x && PosX > CollisionPos[i][1].x) {
				if (PosZ < CollisionPos[i][0].z && PosZ > CollisionPos[i][1].z) {
					return true;
				}
			}
		}
		return false;
	}

	void ProcessKeyboard(cameraMovement direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		float checkPosX, checkPosZ;
		if (direction == FORWARD) {
			if (camMode == PLAY) {
				checkPosX = cos(glm::radians(Yaw)) * velocity;
				checkPosZ = sin(glm::radians(Yaw)) * velocity;

				if (!checkKeyCollision(checkPosX, checkPosZ, FORWARD)) {
					Position.x += checkPosX;
					Position.z += checkPosZ;
				}
			}
			else
				Position += Front * 2.0f * velocity;
		}
		if (direction == BACKWARD) {
			if (camMode == PLAY) { // -=
				checkPosX = cos(glm::radians(Yaw)) * velocity;
				checkPosZ = sin(glm::radians(Yaw)) * velocity;

				if (!checkKeyCollision(checkPosX, checkPosZ, BACKWARD)) {
					Position.x -= checkPosX;
					Position.z -= checkPosZ;
				}
			}
			else
				Position -= Front * 2.0f * velocity;
		}
		if (direction == LEFT) {
			if (camMode == PLAY) { // -=
				checkPosX = Right.x * velocity;
				checkPosZ = Right.z * velocity;

				if (!checkKeyCollision(checkPosX, checkPosZ, LEFT)) {
					Position.x -= checkPosX;
					Position.z -= checkPosZ;
				}
			}
			else
				Position -= Right * 2.0f * velocity;
		}
		if (direction == RIGHT) {
			if (camMode == PLAY) { // += 
				checkPosX = Right.x * velocity;
				checkPosZ = Right.z * velocity;

				if (!checkKeyCollision(checkPosX, checkPosZ, RIGHT)) {
					Position.x += checkPosX;
					Position.z += checkPosZ;
				}
			}
			else
				Position += Right * 2.0f * velocity;

		}
		if (direction == UP && camMode == DEBUG)
			Position.y += 2.0 * velocity;
		if (direction == DOWN && camMode == DEBUG)
			Position.y -= 2.0 * velocity;


	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch -= yoffset;

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

		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
