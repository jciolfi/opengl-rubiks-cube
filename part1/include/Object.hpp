/** @file Object.hpp
 *  @brief Sets up an OpenGL camera..
 *  
 *  More...
 *
 *  @author Mike
 *  @bug No known bugs.
 */
#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glad/glad.h>

#include <vector>
#include <string>
#include <map>

#include "Shader.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Geometry.hpp"

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Purpose:
// An abstraction to create multiple objects
//
//
class Object{
public:
    // Object Constructor
    Object();
    // Object destructor
    ~Object();
    // Load a texture
    void LoadTexture(std::string fileName);
    // Create a textured quad
    void MakeTexturedQuad(std::string fileName);
    // Updates and transformatinos applied to object
    void Update(unsigned int screenWidth, unsigned int screenHeight);
    // How to draw the object
    void Render();
    // Returns an objects transform
    Transform& GetTransform();
    // Load textured quad from .obj
    void LoadTextureQuad(std::string objFilePath, std::string ppmFilePath);
    
protected:
	// Helper method for when we are ready to draw or update our object
	void Bind();

    // Object vertices
    std::vector<GLfloat> m_vertices;
    // Object indices
    std::vector<GLuint> m_indices;

    // For now we have one shader per object.
    Shader m_shader;
    // For now we have one buffer per object.
    VertexBufferLayout m_vertexBufferLayout;
    // For now we have one texture per object
    Texture m_textureDiffuse;
    // Store the objects transformations
    Transform m_transform; 
    // Store the 'camera' projection
    glm::mat4 m_projectionMatrix;
    // Store the objects Geometry
	Geometry m_geometry;

private:
    // parse obj File for v, vt, vn, f, mtllib
    void LoadObjData(std::string objFilePath);
    // populate m_vertices, m_indices based on face data
    void LoadFaceData(std::vector<GLfloat> vertices,
    std::vector<GLfloat> textures,
    std::vector<GLfloat> normals,
    std::vector<std::string> faces);

    // Map faces to indices
    std::map<std::string, int> m_fToIdxMap;
};


#endif
