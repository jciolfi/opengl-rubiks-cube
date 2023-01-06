#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "ObjectManager.hpp"
#include "Cube.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h):m_screenWidth(w),m_screenHeight(h){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = NULL;
	// rotation state
    rotationState = Rotation::NONE;

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		m_window = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                m_screenWidth,
                                m_screenHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		m_openGLContext = SDL_GL_CreateContext( m_window );
		if( m_openGLContext == NULL){
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if(!InitGL()){
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	// GetOpenGLVersionInfo();

    LoadCubes();
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    // Reclaim all of our objects
    ObjectManager::Instance().RemoveAll();

    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

	return success;
}


// Update OpenGL
void SDLGraphicsProgram::Update(){
    // variable for rotating sub cubes
    static float rot = 0;

    // when actively rotating, update rot
    if (rotationState != Rotation::NONE) {
        rot += M_PI_2/40;
    }
    
    // link absolute position to current subcube
    for(int i=0; i<NUM_SUB_CUBES; i++) {
        glm::vec3 cubePos = idxToPositionMap.at(i);
        int subCubeIdx = subCubePositions[i];

        // note: this is the identity + identity rotation already
        ObjectManager::Instance().GetObject(subCubeIdx).GetTransform().LoadIdentity();

        std::vector<int> rotIdxs = rotationMap.at(rotationState);

        if (rotationState != Rotation::NONE && std::find(rotIdxs.begin(), rotIdxs.end(), i) != rotIdxs.end()) {
            // if actively rotating -> rotate then translate to achieve "orbit" effect
            ObjectManager::Instance().GetObject(subCubeIdx).GetTransform().Rotate(rotationDirection*rot, rotationTypeMap.at(rotationAxes.at(rotationState)));
        }

        // translate the sub cube to the absolute cub position
        ObjectManager::Instance().GetObject(subCubeIdx).GetTransform().Translate(cubePos);
        // perform final rotations to make sure cube is facing right direction when rotation completes
        ObjectManager::Instance().GetObject(subCubeIdx).GetTransform().ApplyIdentityRotation();
        // scale the cubes
        ObjectManager::Instance().GetObject(subCubeIdx).GetTransform().Scale(.49f,.49f,.49f);
    }

    // reached M_PI/2 radians (90 degrees) => rotation has finished
    // reset rotation state & rot, update subCubePositions
    if (rot >= M_PI_2) {
        UpdateSubCubePositions();
        rotationState = Rotation::NONE;
        rot = 0;
    }

    // Update all of the objects
    ObjectManager::Instance().UpdateAll(m_screenWidth,m_screenHeight);
}



// Render
// The render function gets called once per loop
void SDLGraphicsProgram::Render(){
    // What we are doing, is telling opengl to create a depth(or Z-buffer) 
    // for us that is stored every frame.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D); 

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );

    // Clear color buffer and Depth Buffer
    // Remember that the 'depth buffer' is our
    // z-buffer that figures out how far away items are every frame
    // and we have to do this every frame!
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Nice way to debug your scene in wireframe!
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    // Render all of our objects in a simple loop
    ObjectManager::Instance().RenderAll();
 
	// Delay to slow things down just a bit!
    SDL_Delay(15); 
}


//Loops forever!
void SDLGraphicsProgram::Loop(){
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();

    // Set the camera speed for how fast we move.
    float cameraSpeed = 1.0f;
    Camera::Instance().Reset();

    // While application is running
    while(!quit){
     	     	 //Handle events on queue
		while(SDL_PollEvent( &e ) != 0){
        	// User posts an event to quit
	        // An example is hitting the "x" in the corner of the window.
    	    if(e.type == SDL_QUIT){
        		quit = true;
	        }
            // Handle keyboard input for the camera class
            if(e.type==SDL_MOUSEMOTION){
                // Handle mouse movements
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                Camera::Instance().MouseLook(mouseX, mouseY);
            }
            if (e.type==SDL_KEYDOWN) {
                // Handle keyboard presses
                switch(e.key.keysym.sym){
                    // WASD to move around
                    case SDLK_a:
                        Camera::Instance().MoveLeft(cameraSpeed);
                        break;
                    case SDLK_d:
                        Camera::Instance().MoveRight(cameraSpeed);
                        break;
                    case SDLK_w:
                        Camera::Instance().MoveForward(cameraSpeed);
                        break;
                    case SDLK_s:
                        Camera::Instance().MoveBackward(cameraSpeed);
                        break;
                    // R/F to move up/down
                    case SDLK_r:
                        Camera::Instance().MoveUp(cameraSpeed);
                        break;
                    case SDLK_f:
                        Camera::Instance().MoveDown(cameraSpeed);
                        break;
                    // ESCAPE to reset camera
                    case SDLK_ESCAPE:
                        Camera::Instance().Reset();
                        break;
                    // TILDE to update rotation direction
                    case SDLK_BACKQUOTE:
                        rotationDirection *= -1;
                        break;
                    // 1-3 to update roll
                    case SDLK_1:
                        UpdateRotationState(Rotation::FRONT_Z);
                        break;
                    case SDLK_2:
                        UpdateRotationState(Rotation::MID_Z);
                        break;
                    case SDLK_3:
                        UpdateRotationState(Rotation::BACK_Z);
                        break;
                    // 4-6 to update pitch
                    case SDLK_4:
                        UpdateRotationState(Rotation::TOP_Y);
                        break;
                    case SDLK_5:
                        UpdateRotationState(Rotation::MID_Y);
                        break;
                    case SDLK_6:
                        UpdateRotationState(Rotation::BOTTOM_Y);
                        break;
                    // 7-9 to update yaw
                    case SDLK_7:
                        UpdateRotationState(Rotation::LEFT_X);
                        break;
                    case SDLK_8:
                        UpdateRotationState(Rotation::MID_X);
                        break;
                    case SDLK_9:
                        UpdateRotationState(Rotation::RIGHT_X);
                        break;
                    // quit project
                    case SDLK_q:
                        quit = true;
                        break;
                }
      	    } // End SDL_PollEvent loop.
        }
		// Update our scene
		Update();
		// Render using OpenGL
	    Render();
      	SDL_GL_SwapWindow(GetSDLWindow());
	}
    //Disable text input
    SDL_StopTextInput();
}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}


// all rotations are relative to the initial camera position (0.0, 0.0, 7.0)
// i.e. the white face is the FRONT, orange face is the TOP, etc.
void SDLGraphicsProgram::LoadCubes(){
    std::cout<<"Loading...\n";

    // create all cube objects - indexed left-to-right, top-to-bottom, front-to-back
    for (int i=0; i<NUM_SUB_CUBES; i++){
        subCubePositions.push_back(i);
        Object* subCube = new Object();
        subCube->LoadTextureQuad("./cube/cube.obj", ("./cube/textures/cube" + std::to_string(i) + ".ppm"));
        ObjectManager::Instance().AddObject(subCube);
    }
    
    // informational messages
    std::cout<<"Done!\n\n";
    std::cout<<"CONTROLS============================================================================\n";
    std::cout<<" • Press ESCAPE to reset the camera.\n";
    std::cout<<" • Use WASD to move around, R to go up, and F to go down\n";
    std::cout<<" • You will travel in the direction you're looking. Use your mouse to look around.\n";
    std::cout<<" • Use the number keys [1-9] to rotate the cube.\n";
    std::cout<<" • Press tilde (~) to change the rotation direction.\n";
    std::cout<<" • Press q to quit.\n";
    std::cout<<"====================================================================================\n";
}

void SDLGraphicsProgram::UpdateSubCubePositions(){
    // update identity rotation for rotated cubes, keep temp map of absolute index (i) to sub cube index
    std::map<int,int> absToSubCubeMap;
    for(int i : rotationMap.at(rotationState)){
        int subCubeIdx = subCubePositions[i];
        absToSubCubeMap[i] = subCubeIdx;
        ObjectManager::Instance().GetObject(subCubeIdx).GetTransform()
            .UpdateIdentityRotation(rotationAxes.at(rotationState), rotationDirection*M_PI_2);
    }

    // update subcubepositions based on postRotationMap
    std::map<int,int> newPositions = GetUpdatedIndices(rotationMap.at(rotationState));

    for(int i : rotationMap.at(rotationState)){
        subCubePositions[i] = absToSubCubeMap[newPositions[i]];
    }
}

// update rotation state if not set
void SDLGraphicsProgram::UpdateRotationState(Rotation rotation){
    if (rotationState == Rotation::NONE) {
        rotationState = rotation;
    }
}

// rotate a flat 1D vector, these are the updated positions for the subcubes post-rotation
std::map<int, int> SDLGraphicsProgram::GetUpdatedIndices(std::vector<int> face){
    /*  
    think about "rotating" 2D array, then flatten
    0 1 2      6 3 0           
    3 4 5  ->  7 4 1 
    6 7 8      8 5 2
    OR
    0 1 2      2 5 8
    3 4 5  ->  1 4 7 
    6 7 8      0 3 6
    */
    std::map<int,int> result;
    if (rotationDirection == -1){
        result[face[0]] = face[6];
        result[face[1]] = face[3];
        result[face[2]] = face[0];
        result[face[3]] = face[7];
        result[face[4]] = face[4];
        result[face[5]] = face[1];
        result[face[6]] = face[8];
        result[face[7]] = face[5];
        result[face[8]] = face[2];
    } else if(rotationDirection == 1){
        result[face[0]] = face[2];
        result[face[1]] = face[5];
        result[face[2]] = face[8];
        result[face[3]] = face[1];
        result[face[4]] = face[4];
        result[face[5]] = face[7];
        result[face[6]] = face[0];
        result[face[7]] = face[3];
        result[face[8]] = face[6];
    }
    return result;
}