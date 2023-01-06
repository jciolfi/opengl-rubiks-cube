#ifndef SDLGRAPHICSPROGRAM_HPP
#define SDLGRAPHICSPROGRAM_HPP

// ==================== Libraries ==================
// Depending on the operating system we use
// The paths to SDL are actually different.
// The #define statement should be passed in
// when compiling using the -D argument.
// This gives an example of how a programmer
// may support multiple platforms with different
// dependencies.
#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif

// The glad library helps setup OpenGL extensions.
#include <glad/glad.h>
#include <vector>
#include <map>
#include "glm/gtx/transform.hpp"
#include "Transform.hpp"

// Purpose:
// This class sets up a full graphics program using SDL
class SDLGraphicsProgram{
public:
    // everything is relative to the front face (white) when program starts
    // roll = rotation around Z axis
    // pitch = rotation around Y axis
    // yaw = rotation around X axis
    enum class Rotation 
    {
        NONE,
        FRONT_Z,
        MID_Z,
        BACK_Z,
        TOP_Y,
        MID_Y,
        BOTTOM_Y,
        LEFT_X,
        MID_X,
        RIGHT_X
    };
    // Constructor
    SDLGraphicsProgram(int w, int h);
    // Destructor
    ~SDLGraphicsProgram();
    // Setup OpenGL
    bool InitGL();
    // Per frame update
    void Update();
    // Renders shapes to the screen
    void Render();
    // loop that runs forever
    void Loop();
    // Get Pointer to Window
    SDL_Window* GetSDLWindow();
    // Helper Function to Query OpenGL information.
    void GetOpenGLVersionInfo();

private:
    // load all cubes in order, populate subCubePositions
    void LoadCubes();
    // when rotation is finished, update subCubePositions to reflect changes
    void UpdateSubCubePositions();
    // update the rotation state if is NONE
    void UpdateRotationState(Rotation rotation);
    // get post-rotated indices
    std::map<int,int> GetUpdatedIndices(std::vector<int> face);

    // Screen dimension constants
    int m_screenWidth;
    int m_screenHeight;
    // The window we'll be rendering to
    SDL_Window* m_window ;
    // OpenGL context
    SDL_GLContext m_openGLContext;

    // ====== sub cube vars ======
    // number of sub cubes = (9*3)
    // Note: rendering the middle cube makes math easier even if it's not used
    static int const NUM_SUB_CUBES = 27;
    // mapping of rotation type to rotation axis
    const std::map<Rotation, Transform::RotationType> rotationAxes = {
        // None: no rotation axis
        {Rotation::NONE,       Transform::RotationType::NONE},
        // slices going back on z axis
        {Rotation::FRONT_Z,    Transform::RotationType::ROLL},
        {Rotation::MID_Z,      Transform::RotationType::ROLL},
        {Rotation::BACK_Z,     Transform::RotationType::ROLL},
        // slices going down on y axis
        {Rotation::TOP_Y,      Transform::RotationType::PITCH},
        {Rotation::MID_Y,      Transform::RotationType::PITCH},
        {Rotation::BOTTOM_Y,   Transform::RotationType::PITCH},
        // slices going across on x axis
        {Rotation::LEFT_X,     Transform::RotationType::YAW},
        {Rotation::MID_X,      Transform::RotationType::YAW},
        {Rotation::RIGHT_X,    Transform::RotationType::YAW}
    };

    // map from rotation type to absolute indices of affected sub cubes
    const std::map<Rotation, std::vector<int>> rotationMap ={
        {Rotation::NONE,       std::vector<int>{}},
        // slices going back on z axis
        {Rotation::FRONT_Z,    std::vector<int>{0,1,2, 3,4,5, 6,7,8}},
        {Rotation::MID_Z,      std::vector<int>{9,10,11, 12,13,14, 15,16,17}},
        {Rotation::BACK_Z,     std::vector<int>{18,19,20, 21,22,23, 24,25,26}},
        // slices going down on y axis
        {Rotation::TOP_Y,      std::vector<int>{18,19,20, 9,10,11, 0,1,2}},
        {Rotation::MID_Y,      std::vector<int>{21,22,23, 12,13,14, 3,4,5}},
        {Rotation::BOTTOM_Y,   std::vector<int>{24,25,26, 15,16,17, 6,7,8}},
        // slices going across on x axis
        {Rotation::LEFT_X,     std::vector<int>{6,3,0, 15,12,9, 24,21,18}},
        {Rotation::MID_X,      std::vector<int>{7,4,1, 16,13,10, 25,22,19}},
        {Rotation::RIGHT_X,    std::vector<int>{8,5,2, 17,14,11, 26,23,20}}
    };

    // map absolute cube indices to positions to transform in space
    const std::map<int, glm::vec3> idxToPositionMap = {
        {0, glm::vec3(-1.0f,1.0f,1.0f)},
        {1, glm::vec3(0.0f,1.0f,1.0f)},
        {2, glm::vec3(1.0f,1.0f,1.0f)},
        {3, glm::vec3(-1.0f,0.0f,1.0f)},
        {4, glm::vec3(0.0f,0.0f,1.0f)},
        {5, glm::vec3(1.0f,0.0f,1.0f)},
        {6, glm::vec3(-1.0f,-1.0f,1.0f)},
        {7, glm::vec3(0.0f,-1.0f,1.0f)},
        {8, glm::vec3(1.0f,-1.0f,1.0f)},
        {9, glm::vec3(-1.0f,1.0f,0.0f)},
        {10, glm::vec3(0.0f,1.0f,0.0f)},
        {11, glm::vec3(1.0f,1.0f,0.0f)},
        {12, glm::vec3(-1.0f,0.0f,0.0f)},
        {13, glm::vec3(0.0f,0.0f,0.0f)},
        {14, glm::vec3(1.0f,0.0f,0.0f)},
        {15, glm::vec3(-1.0f,-1.0f,0.0f)},
        {16, glm::vec3(0.0f,-1.0f,0.0f)},
        {17, glm::vec3(1.0f,-1.0f,0.0f)},
        {18, glm::vec3(-1.0f,1.0f,-1.0f)},
        {19, glm::vec3(0.0f,1.0f,-1.0f)},
        {20, glm::vec3(1.0f,1.0f,-1.0f)},
        {21, glm::vec3(-1.0f,0.0f,-1.0f)},
        {22, glm::vec3(0.0f,0.0f,-1.0f)},
        {23, glm::vec3(1.0f,0.0f,-1.0f)},
        {24, glm::vec3(-1.0f,-1.0f,-1.0f)},
        {25, glm::vec3(0.0f,-1.0f,-1.0f)},
        {26, glm::vec3(1.0f,-1.0f,-1.0f)}
    };

    // map type of rotation to absolute rotation axis
    const std::map<Transform::RotationType, glm::vec3> rotationTypeMap = {
        {Transform::RotationType::NONE,     glm::vec3(0.0f,0.0f,0.0f)},
        {Transform::RotationType::ROLL,     glm::vec3(0.0f,0.0f,1.0f)},
        {Transform::RotationType::PITCH,    glm::vec3(0.0f,1.0f,0.0f)},
        {Transform::RotationType::YAW,      glm::vec3(1.0f,0.0f,0.0f)}
    };

    // index pointers to absolute subcube positions (e.g. the front facing top left subcube = 0)
    // the index is the absolute position, the value is the sub cube position in the object manager
    std::vector<int> subCubePositions;
    // state of rotation updated by keypresses - None if static
    Rotation rotationState;
    // clockwise=-1 or counter-clockwise=1
    int rotationDirection = -1;
};

#endif
