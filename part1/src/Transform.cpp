#include "Transform.hpp"
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

// By default, all transform matrices
// are also identity matrices
Transform::Transform(){
    LoadIdentity();
}

Transform::~Transform(){

}

// Resets the model transform as the identity matrix.
void Transform::LoadIdentity(){
    m_modelTransformMatrix = glm::mat4(1.0f);
}

void Transform::Translate(float x, float y, float z){
        // Here we apply our camera
        // This first transformation is applied to the object
        // This is the model transform matrix.
        // That is, 'how do I move our model'
        // Here we see I have translated the model -1.0f away from its original location.
        // We supply the first argument which is the matrix we want to apply
        // this transformation to (Our previous transformation matrix.
        m_modelTransformMatrix = glm::translate(m_modelTransformMatrix,glm::vec3(x,y,z));                            
}

void Transform::Translate(glm::vec3 pos){
    Translate(pos.x, pos.y, pos.z);
}

void Transform::Rotate(float radians, float x, float y, float z){
    m_modelTransformMatrix = glm::rotate(m_modelTransformMatrix, radians,glm::vec3(x,y,z));        
}

void Transform::Rotate(float radians, glm::vec3 rotationVector){
    Rotate(radians, rotationVector.x, rotationVector.y, rotationVector.z);
}

void Transform::Scale(float x, float y, float z){
    m_modelTransformMatrix = glm::scale(m_modelTransformMatrix,glm::vec3(x,y,z));        
}

// Returns the actual transform matrix
// Useful for sending 
GLfloat* Transform::GetTransformMatrix(){
    return &m_modelTransformMatrix[0][0];
}


// Get the raw internal matrix from the class
glm::mat4 Transform::GetInternalMatrix() const{
    return m_modelTransformMatrix;
}

void Transform::ApplyTransform(Transform t){
    m_modelTransformMatrix = t.GetInternalMatrix();
}



// Perform a matrix multiplication with our Transform
Transform& Transform::operator*=(const Transform& t) {
    m_modelTransformMatrix =  m_modelTransformMatrix * t.GetInternalMatrix();
    return *this;
}

// Perform a matrix addition with our Transform
Transform& Transform::operator+=(const Transform& t) {
    m_modelTransformMatrix =  m_modelTransformMatrix + t.GetInternalMatrix();
    return *this;
}

// Matrix assignment
Transform& Transform::operator=(const Transform& t) {
    m_modelTransformMatrix =  t.GetInternalMatrix();
    return *this;
}

Transform operator*(const Transform& lhs, const Transform& rhs){
    Transform result;

    result.m_modelTransformMatrix = lhs.GetInternalMatrix() * rhs.GetInternalMatrix();

    return result;
}

Transform operator+(const Transform& lhs, const Transform& rhs){
    Transform result;

    result.m_modelTransformMatrix = lhs.GetInternalMatrix() + rhs.GetInternalMatrix();

    return result;
}

void Transform::UpdateIdentityRotation(RotationType rotationType, float radians){
    appliedRotations.push_back(std::make_pair(rotationType, radians));
}

void Transform::ApplyIdentityRotation() {
    // ** axes are not absolute, they're relative to the model - need norms**
    glm::vec3 xNorm(1.0, 0.0f, 0.0);
    glm::vec3 yNorm(0.0, 1.0f, 0.0);
    glm::vec3 zNorm(0.0, 0.0f, 1.0);

    // important: rotations are not commutative, so store and apply them sequentially
    for (std::pair<RotationType, float> rotation : appliedRotations){
        switch(rotation.first){
            case RotationType::NONE:
                break;
            case RotationType::YAW:
                // rotate around x axis and update other norms
                m_modelTransformMatrix = glm::rotate(m_modelTransformMatrix, rotation.second, xNorm);
                yNorm = glm::rotate(yNorm, -rotation.second, xNorm);
                zNorm = glm::rotate(zNorm, -rotation.second, xNorm);
                break;
            case RotationType::PITCH:
                // rotate around y axis and update other norms
                m_modelTransformMatrix = glm::rotate(m_modelTransformMatrix, rotation.second, yNorm);
                xNorm = glm::rotate(xNorm, -rotation.second, yNorm);
                zNorm = glm::rotate(zNorm, -rotation.second, yNorm);
                break;
            case RotationType::ROLL:
                // rotate around z axis and update other norms
                m_modelTransformMatrix = m_modelTransformMatrix = glm::rotate(m_modelTransformMatrix, rotation.second, zNorm);
                xNorm = glm::rotate(xNorm, -rotation.second, zNorm);
                yNorm = glm::rotate(yNorm, -rotation.second, zNorm);
                break;
        }
    }
}