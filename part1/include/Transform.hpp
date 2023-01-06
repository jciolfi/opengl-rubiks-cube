/** @file Transform.hpp
 *  @brief Responsible for holding matrix operations in model, view, and projection space..
 *  
 *  More...
 *
 *  @author Mike
 *  @bug No known bugs.
 */
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <map>
#include <utility>
#include <vector>
#include <glad/glad.h>
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

// The purpose of this class is to store
// transformations of 3D entities (cameras, objects, etc.)
class Transform{
public:
    // types of rotation
    enum class RotationType 
    {
        NONE,
        ROLL,
        PITCH,
        YAW
    };

    // Constructor for a new transform
    Transform();
    // Destructor for the transform
    ~Transform();
    // Resets matrix transformations to the identity matrix
    void LoadIdentity();
    // Perform a translation of an object
    void Translate(float x, float y, float z);
    void Translate(glm::vec3 pos);
    // Perform rotation about an axis
    void Rotate(float radians, float x, float y, float z);
    void Rotate(float radians, glm::vec3 rotationVector);
    // Perform rotation about an axis
    void Scale(float x, float y, float z);
    // Returns the transformation matrix
    GLfloat* GetTransformMatrix();
    // Apply Transform
    // Takes in a transform and sets internal
    // matrix.
    void ApplyTransform(Transform t);
    // Returns the transformation matrix
    glm::mat4 GetInternalMatrix() const;

    // Transform multiplicaiton
	Transform& operator*=(const Transform& t);
	// Transform addition
	Transform& operator+=(const Transform& t);
	// Transform =
	Transform& operator=(const Transform& t);
    // Multiplication
    friend Transform operator*(const Transform& lhs, const Transform& rhs);
    // Addition
    friend Transform operator+(const Transform& lhs, const Transform& rhs);
    // When a cube finishes rotating, keep track of its post-rotated state
    void UpdateIdentityRotation(RotationType rotationType, float radians);
    // Apply post-rotated state of sub cube (roll,pitch,yaw)
    void ApplyIdentityRotation();
private:
    // Stores the actual transformation matrix
    glm::mat4 m_modelTransformMatrix;
    // rotate identity per-axis
    std::vector<std::pair<RotationType, float>> appliedRotations;
};


#endif
