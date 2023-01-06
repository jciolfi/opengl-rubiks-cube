
/** @file Cube.hpp
 *  @brief Draw a simple cube primitive.
 *
 *  Draws a simple cube, that is derived
 *  from the Object class.
 *
 *  @author John C.
 *  @bug No known bugs.
 */

#include "VertexBufferLayout.hpp"
#include "Geometry.hpp"
#include <cmath>

class Cube : public Object{
    public:
        Cube();
        void Init();
};

Cube::Cube(){
    Init();
}

void Cube::Init(){
    // * make 8 vertices of cube
    // front four (z=1): 0,1,2,3
    m_geometry.AddVertex(-1.0f,-1.0f, 1.0f, 0.0f, 0.0f); // 0
    m_geometry.AddVertex( 1.0f,-1.0f, 1.0f, 1.0f, 0.0f); // 1
    m_geometry.AddVertex( 1.0f, 1.0f, 1.0f, 1.0f, 1.0f); // 2
    m_geometry.AddVertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f); // 3

    // back four (z=-1): 4,5,6,7
    m_geometry.AddVertex(-1.0f,-1.0f, -1.0f, 0.0f, 0.0f); // 4
    m_geometry.AddVertex( 1.0f,-1.0f, -1.0f, 1.0f, 0.0f); // 5
    m_geometry.AddVertex( 1.0f, 1.0f, -1.0f, 1.0f, 1.0f); // 6
    m_geometry.AddVertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f); // 7

    // * add faces with triangles
    // front face
    m_geometry.MakeTriangle(0,1,2);
    m_geometry.MakeTriangle(2,3,0);
    // back face
    m_geometry.MakeTriangle(4,5,6);
    m_geometry.MakeTriangle(6,7,4);
    // left face
    m_geometry.MakeTriangle(0,4,7);
    m_geometry.MakeTriangle(7,3,0);
    // right face
    m_geometry.MakeTriangle(1,5,6);
    m_geometry.MakeTriangle(6,2,1);
    // top face
    m_geometry.MakeTriangle(3,2,6);
    m_geometry.MakeTriangle(6,7,3);
    // bottom face
    m_geometry.MakeTriangle(0,1,5);
    m_geometry.MakeTriangle(5,4,0);

    m_geometry.Gen();

    m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                        m_geometry.GetIndicesSize(),
                                        m_geometry.GetBufferDataPtr(),
                                        m_geometry.GetIndicesDataPtr());
}