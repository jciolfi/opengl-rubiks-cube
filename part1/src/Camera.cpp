#include "Camera.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <iostream>

Camera& Camera::Instance(){
    static Camera* instance = new Camera();
    return *instance;
}

void Camera::MouseLook(int mouseX, int mouseY){
    glm::vec3 rightVector = glm::cross(m_viewDirection, m_upVector);
    m_viewDirection = glm::rotate(m_viewDirection, -.0075f * (mouseX - m_oldMousePosition.x), m_upVector);
    m_viewDirection = glm::rotate(m_viewDirection, -.0075f * (mouseY - m_oldMousePosition.y), rightVector);

    m_oldMousePosition.x = (float) mouseX;
    m_oldMousePosition.y = (float) mouseY;
}

void Camera::MoveForward(float speed){
    m_eyePosition += speed * m_viewDirection;
}

void Camera::MoveBackward(float speed){
    m_eyePosition -= speed * m_viewDirection;
}

void Camera::MoveLeft(float speed){
    glm::vec3 rightVector = glm::cross(m_viewDirection, m_upVector);
    m_eyePosition -= speed * rightVector;
}

void Camera::MoveRight(float speed){
    glm::vec3 rightVector = glm::cross(m_viewDirection, m_upVector);
    m_eyePosition += speed * rightVector;
}

void Camera::MoveUp(float speed){
    m_eyePosition += speed * m_upVector;
}

void Camera::MoveDown(float speed){
    m_eyePosition -= speed * m_upVector;
}

void Camera::SetCameraEyePosition(float x, float y, float z){
    m_eyePosition.x = x;
    m_eyePosition.y = y;
    m_eyePosition.z = z;
}

void Camera::Reset(){
    m_eyePosition = glm::vec3(0.0f,0.0f,8.0f);
	// Looking down along the z-axis initially.
	// Remember, this is negative because we are looking 'into' the scene.
    m_viewDirection = glm::vec3(0.0f,0.0f, -1.0f);
	// For now--our upVector always points up along the y-axis
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

float Camera::GetEyeXPosition(){
    return m_eyePosition.x;
}

float Camera::GetEyeYPosition(){
    return m_eyePosition.y;
}

float Camera::GetEyeZPosition(){
    return m_eyePosition.z;
}

float Camera::GetViewXDirection(){
    return m_viewDirection.x;
}

float Camera::GetViewYDirection(){
    return m_viewDirection.y;
}

float Camera::GetViewZDirection(){
    return m_viewDirection.z;
}



Camera::Camera(){
	Reset();
}

glm::mat4 Camera::GetWorldToViewmatrix() const{
    // Think about the second argument and why that is
    // setup as it is.
    return glm::lookAt( m_eyePosition,
                        m_eyePosition + m_viewDirection,
                        m_upVector);
}
