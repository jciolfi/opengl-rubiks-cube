#include "Object.hpp"
#include "Camera.hpp"
#include "Error.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>


Object::Object(){
}

Object::~Object(){
    
}


// Initialization of object as a 'quad'
//
// This could be called in the constructor or
// otherwise 'explicitly' called this
// so we create our objects at the correct time
void Object::MakeTexturedQuad(std::string fileName){

        // Setup geometry
        // We are using a new abstraction which allows us
        // to create triangles shapes on the fly
        // Position and Texture coordinate 
        m_geometry.AddVertex(-1.0f,-1.0f, 0.0f, 0.0f, 0.0f);
        m_geometry.AddVertex( 1.0f,-1.0f, 0.0f, 1.0f, 0.0f);
    	m_geometry.AddVertex( 1.0f, 1.0f, 0.0f, 1.0f, 1.0f);
        m_geometry.AddVertex(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
            
        // Make our triangles and populate our
        // indices data structure	
        m_geometry.MakeTriangle(0,1,2);
        m_geometry.MakeTriangle(2,3,0);

        // This is a helper function to generate all of the geometry
        m_geometry.Gen();

        // Create a buffer and set the stride of information
        // NOTE: How we are leveraging our data structure in order to very cleanly
        //       get information into and out of our data structure.
        m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                        m_geometry.GetIndicesSize(),
                                        m_geometry.GetBufferDataPtr(),
                                        m_geometry.GetIndicesDataPtr());

        // Load our actual texture
        // We are using the input parameter as our texture to load
        m_textureDiffuse.LoadTexture(fileName.c_str());
        
        // Setup shaders
        std::string vertexShader = m_shader.LoadShader("./shaders/vert.glsl");
        std::string fragmentShader = m_shader.LoadShader("./shaders/frag.glsl");
        // Actually create our shader
        m_shader.CreateShader(vertexShader,fragmentShader);
}

// TODO: In the future it may be good to 
// think about loading a 'default' texture
// if the user forgets to do this action!
void Object::LoadTexture(std::string fileName){
        // Load our actual textures
        m_textureDiffuse.LoadTexture(fileName);
}

// Bind everything we need in our object
// Generally this is called in update() and render()
// before we do any actual work with our object
void Object::Bind(){
        // Make sure we are updating the correct 'buffers'
        m_vertexBufferLayout.Bind();
        // Diffuse map is 0 by default, but it is good to set it explicitly
        m_textureDiffuse.Bind(0);
        // Select our appropriate shader
        m_shader.Bind();
}

void Object::Update(unsigned int screenWidth, unsigned int screenHeight){
        // Call our helper function to just bind everything
        Bind();
        // TODO: Read and understand
        // For our object, we apply the texture in the following way
        // Note that we set the value to 0, because we have bound
        // our texture to slot 0.
        m_shader.SetUniform1i("u_DiffuseMap",0);  
         // Here we apply the 'view' matrix which creates perspective.
        // The first argument is 'field of view'
        // Then perspective
        // Then the near and far clipping plane.
        // Note I cannot see anything closer than 0.1f units from the screen.
        // TODO: In the future this type of operation would be abstracted away
        //       in a camera class.
        m_projectionMatrix = glm::perspective(glm::radians(45.0f),((float)screenWidth)/((float)screenHeight),0.1f,20.0f);

        // Set the uniforms in our current shader
        // Set the MVP Matrix for our object
        // Send it into our shader
        m_shader.SetUniformMatrix4fv("model", &m_transform.GetInternalMatrix()[0][0]);
        m_shader.SetUniformMatrix4fv("view", &Camera::Instance().GetWorldToViewmatrix()[0][0]);
        m_shader.SetUniformMatrix4fv("projection", &m_projectionMatrix[0][0]);

        // Create a first 'light'
        m_shader.SetUniform3f("pointLights[0].lightColor",1.0f,1.0f,1.0f);
        m_shader.SetUniform3f("pointLights[0].lightPos",
           Camera::Instance().GetEyeXPosition() + Camera::Instance().GetViewXDirection(),
           Camera::Instance().GetEyeYPosition() + Camera::Instance().GetViewYDirection(),
           Camera::Instance().GetEyeZPosition() + Camera::Instance().GetViewZDirection());
        m_shader.SetUniform1f("pointLights[0].ambientIntensity",0.5f);
        m_shader.SetUniform1f("pointLights[0].specularStrength",0.5f);
        m_shader.SetUniform1f("pointLights[0].constant",1.0f);
        m_shader.SetUniform1f("pointLights[0].linear",0.09f);
        m_shader.SetUniform1f("pointLights[0].quadratic",0.032f);

}

// Render our geometry
void Object::Render(){
    // Call our helper function to just bind everything
    Bind();

    int numIndices = m_geometry.GetIndicesSize() == 0 ? m_indices.size() : m_geometry.GetIndicesSize();

	//Render data
    glDrawElements(GL_TRIANGLES,
                   numIndices, // The number of indices, not triangles.
                   GL_UNSIGNED_INT,             // Make sure the data type matches
                        nullptr);               // Offset pointer to the data. 
                                                // nullptr because we are currently bound
}

// Returns the actual transform stored in our object
// which can then be modified
Transform& Object::GetTransform(){
    return m_transform; 
}

void Object::LoadTextureQuad(std::string objFilePath, std::string ppmFilePath){
        // setup geometry (parse obj file, set m_vertices and m_indices)
    LoadObjData(objFilePath);

    // Create a buffer and set the stride of information
    // NOTE: We are leveraging our data structure in order to very cleanly
    //       get information into and out of our data structure.
    m_vertexBufferLayout.CreateTextureBufferLayout(m_vertices.size(),m_indices.size(),m_vertices.data(),m_indices.data());

    // Load our actual texture
    // We are using the input parameter as our texture to load
    m_textureDiffuse.LoadTexture(ppmFilePath.c_str());
    
    // Setup shaders
    std::string vertexShader = m_shader.LoadShader("./shaders/texVert.glsl");
    std::string fragmentShader = m_shader.LoadShader("./shaders/texFrag.glsl");
    // Actually create our shader
    m_shader.CreateShader(vertexShader,fragmentShader);
}

void Object::LoadObjData(std::string objFileName) {
    // set variables to be extracted from obj file
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> textures;
    std::vector<GLfloat> normals;
    std::vector<std::string> faces;
    std::string mtlFileName;

    std::ifstream objFile;
    objFile.open(objFileName);
    if (objFile.is_open()) {
        std::string line;
        // get each line of file
        while (getline(objFile, line)) {
            // create stringstream to parse tokens
            std::stringstream ssLines(line);
            std::string token;
            ssLines >> token;
            // v -> vertex
            if (token == "v") {
                while (ssLines >> token) {
                    vertices.push_back(std::stof(token));
                }
            // vn -> normal vector
            } else if (token == "vn") {
                while (ssLines >> token) {
                    normals.push_back(std::stof(token));
                }
            // vt -> vertex texture
            } else if (token == "vt") {
                while (ssLines >> token) {
                    textures.push_back(std::stof(token));
                }
            // f -> face
            } else if (token == "f") {
                while (ssLines >> token) {
                    faces.push_back(token);
                }
            // assign mtl file
            } else if (token == "mtllib") {
                ssLines >> mtlFileName;
            }
        }
    }

    // read f data after fully parsing in case any v/vt/vn data comes after it
    LoadFaceData(vertices, textures, normals, faces);

    // close file
    objFile.close();
}

void Object::LoadFaceData(std::vector<GLfloat> vertices,
    std::vector<GLfloat> textures,
    std::vector<GLfloat> normals,
    std::vector<std::string> faces) {
        // iterate through raw face data
        int index = 0;
        for (int i=0; i<faces.size(); i++) {
            std::string face = faces.at(i);
            
            // if found same face, add that index to indices
            if (m_fToIdxMap.count(face) > 0) {
                m_indices.push_back(m_fToIdxMap[face]);
                continue;
            }

            // split slashes in v/vt/vn
            int split1 = face.find("/");
            int split2 = face.substr(split1 + 1).find("/") + split1 + 1;
            
            // get vertex data (x,y,z)
            int vIdx = 3 * (stoi(face.substr(0, split1)) - 1);
            for (int v=vIdx; v<vIdx+3; v++) {
                m_vertices.push_back(vertices[v]);
            }

            // get texture data (s,t)
            int vtIdx = 2 * (stoi(face.substr(split1+1, split2-split1-1)) - 1);
            for (int vt=vtIdx; vt<vtIdx+2; vt++) {
                m_vertices.push_back(textures[vt]);
            }

            // get normal data (i,j,k)
            // int vnIdx = 3 * (stoi(face.substr(split2 + 1)) - 1);
            // for (int vn=vnIdx; vn<vnIdx+3; vn++) {
            //     m_vertices.push_back(normals[vn]);
            // }
            
            // add index, add face to map, increment index
            m_indices.push_back(index);
            m_fToIdxMap[face] = index;
            index++;
        }
    }