// ==================================================================
#version 330 core
// Read in our attributes stored from our vertex buffer object
// We explicitly state which is the vertex information
// (The first 3 floats are positional data, we are putting in our vector)
layout(location=0)in vec3 position; // We explicitly state which is the vertex information (The first 3 floats are positional data, we are putting in our vector)
layout(location=1)in vec3 normals; // Our second attribute - normals.
layout(location=2)in vec2 texCoord; // Our third attribute - texture coordinates.
layout(location=3)in vec3 tangents; // Our third attribute - texture coordinates.
layout(location=4)in vec3 bitangents; // Our third attribute - texture coordinates.

// If we are applying our camera, then we need to add some uniforms.
// Note that the syntax nicely matches glm's mat4!
//
// For our objects, we can now have model space which are the objects position
// We also have a camera which is the 'view space' now
// And finally the 'projection' which will transform our vertices into our chosen projection (i.e. for us, a perspective view).
uniform mat4 model; // Object space
uniform mat4 view; // View space
uniform mat4 projection; // Projection space

// Export our normal data, and read it into our frag shader
out vec3 myNormal;
// Export our Fragment Position computed in world space
out vec3 FragPos;
// If we have texture coordinates we will need to pass these into the 
// fragment shader. 
// We create a 'vec2' and the 'out' qualifier implies that we will 
// read this variable 'in' a later stage of the graphics pipeline (i.e. our fragment shader)
out vec2 v_texCoord;


void main()
{

	// gl_Position is a special glsl variable that tells us what
	// position to put things in.
	// It takes in exactly 4 things.
	// Note that 'w' (the 4th dimension) should be 1.
	gl_Position = projection * view * model * vec4(position, 1.0f);

    myNormal = normals;
    // Transform normal into world space
    FragPos = vec3(model* vec4(position,1.0f));

  	// Store the texture coordinaets which we will output to
  	// the next stage in the graphics pipeline.
  	v_texCoord = texCoord;
}
// ==================================================================