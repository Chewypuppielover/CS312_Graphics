#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "gl.h"
#include "tree.h"
#include "floor.h"

// To compile: g++ gl.cpp -lopengl32 -lglew32 -lSDL2 -lmingw32
int main(int argc, char **argv)
{  
	SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

	SDL_Window * win = SDL_CreateWindow("OpenGL Shaders", 200, 200, SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);
	SDL_GLContext cont = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, cont);
	glewInit();
	glEnable(GL_DEPTH_TEST);

	bool success = true;
	string bunnyVertexShader;
	string bunnyFragmentShader;
	int bunnyVertexID;
	int bunnyFragID;
	int bunnyProgram;
	success &= parseFile((char*)"phongLighting.vs", bunnyVertexShader);
	success &= parseFile((char*)"phongLighting.fs"  , bunnyFragmentShader);
	success &= compileShader(bunnyVertexShader.c_str(), GL_VERTEX_SHADER, bunnyVertexID);
	success &= compileShader(bunnyFragmentShader.c_str(), GL_FRAGMENT_SHADER, bunnyFragID);
	success &= compileProgram(bunnyVertexID, bunnyFragID, bunnyProgram);

	int floorProgram;
	success &= setupFLoor(floorProgram);
	int treeProgram;
	success &= setupTree(treeProgram);
	/***************************************************************************************
	* OBJECT LOADER - Vertices, UVs, Normals, etc
	***************************************************************************************/  
	vector<material> materials;
	vector<vertexData> vertexBuffer;
	bool hasUV;
	bool hasNormal;  
	success &= getObjData("bunny.obj", materials, vertexBuffer, hasUV, hasNormal);

	// Build out a single array of float data 
	int stride = 3 + (2*hasUV) + (3*hasNormal);
	int vertexBufferNumBytes = stride * vertexBuffer.size() * sizeof(float);
	float* vertexBufferData = (float*)(malloc(vertexBufferNumBytes));
	int i = 0;

	// Join data together into an interleaved buffer
	for(int vb = 0; vb < vertexBuffer.size(); vb++)
	{
		vertexBufferData[i++] = vertexBuffer[vb].vert[0];
		vertexBufferData[i++] = vertexBuffer[vb].vert[1];
		vertexBufferData[i++] = vertexBuffer[vb].vert[2];
		if(hasUV)
		{
			vertexBufferData[i++] = vertexBuffer[vb].uv[0];
			vertexBufferData[i++] = vertexBuffer[vb].uv[1];		
		}
		// Leave Normal data for later...
		if(hasNormal)
		{
			vertexBufferData[i++] = vertexBuffer[vb].normal[0];
			vertexBufferData[i++] = vertexBuffer[vb].normal[1];
			vertexBufferData[i++] = vertexBuffer[vb].normal[2];
		}
	}
	// Load in each texture 
	vector<int> textureIDs;	
	for(int mat = 0; mat < materials.size(); mat++)
	{
		int tmp;
		material m = materials[mat];
		success &= loadTexture(m.map_Kd, tmp);
		textureIDs.push_back(tmp);
	}
	validate(success, (char*)"Setup OpenGL Program");
	
	/***************************************************************************************
	* VERTEX BUFFER OBJECT - Vertex data AND Attributes
	***************************************************************************************/  
	// Create VBO - Vertex Buffer Object - Which will hold the data (interleaved) for our vertices 
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);

	/***************************************************************************
	* ATTRIBUTE HANDLES - based on the way the shader is written; mapped to data 
	***************************************************************************/      
	int aPositionHandle = glGetAttribLocation(bunnyProgram, "a_Position");
	int aUVHandle = glGetAttribLocation(bunnyProgram, "a_UV");
	int aNormalHandle = glGetAttribLocation(bunnyProgram, "a_Normal");

	/***************************************************************************************
	* UNIFORM HANDLES - kept to update uniforms; mapped for each frame (could be pre-cached)
	***************************************************************************************/      
	// Establish shader-specific variables
	int uTextureHandle = glGetUniformLocation(bunnyProgram, "u_Texture");
	int uThresholdHandle = glGetUniformLocation(bunnyProgram, "u_Threshold");
	int uCameraHandle = glGetUniformLocation(bunnyProgram, "u_Camera");
	int uLightHandle = glGetUniformLocation(bunnyProgram, "u_LightPos");

	int uMaterialA = glGetUniformLocation(bunnyProgram, "u_MaterialAmbient");
	int uMaterialD = glGetUniformLocation(bunnyProgram, "u_MaterialDiffuse");
	int uMaterialS = glGetUniformLocation(bunnyProgram, "u_MaterialSpecular");
	int uMaterialShine = glGetUniformLocation(bunnyProgram, "u_MaterialShine");

	int uMatrixHandle = glGetUniformLocation(bunnyProgram, "u_Matrix");
/*	int uProHandle = glGetUniformLocation(bunnyProgram, "u_Projection");
	int uModelHandle = glGetUniformLocation(bunnyProgram, "u_Model");
	int uViewHandle = glGetUniformLocation(bunnyProgram, "u_View");
*/
	// MVP Data for transforming vertices
	mat4 Bmvp;

	// Camera data 
	myCam.camX = myCam.camY = myCam.camZ = myCam.pitch = myCam.yaw = myCam.roll = 0.0;

	// Main Loop 
	int numDraw = vertexBuffer.size(); 
	bool running = true;
	printf("%s\n", glGetString(GL_VERSION));//_GetError());	
	while(running)
	{
		// Update input
		processUserInputs(running);      
		{
			// Setup Program, Attach appropriate buffer
			glUseProgram(bunnyProgram);
			// Clear buffers, setup/use program 
			glClearColor(0,0,0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			/***************************************************************************
			* ATTRIBUTE HANDLES - based on the way the shader is written; mapped to data 
			***************************************************************************/     
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glEnableVertexAttribArray(aPositionHandle);
			glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)0);
			glEnableVertexAttribArray(aUVHandle);
			glVertexAttribPointer(aUVHandle, 2, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(3*sizeof(float)));
			glEnableVertexAttribArray(aNormalHandle);
			glVertexAttribPointer(aNormalHandle, 3, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(5*sizeof(float)));

			// Update Texture - Assume that we want texture '0'
			glActiveTexture(GL_TEXTURE0 + 0); // + "i" to change texture chosen
			glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
			// Update Threshold
			glUniform1i(uTextureHandle, 0);
			glUniform1f(uThresholdHandle, threshold);
			glUniform3f(uCameraHandle, myCam.camX, myCam.camY, myCam.camZ);
			glUniform3f(uLightHandle, 0.0f, 0.0f, 0.0f);
			
			glUniform3f(uMaterialA, materials[0].Ka[0], materials[0].Ka[1], materials[0].Ka[2]);
			glUniform3f(uMaterialD, materials[0].Kd[0], materials[0].Kd[1], materials[0].Kd[2]);
			glUniform3f(uMaterialS, materials[0].Ks[0], materials[0].Ks[1], materials[0].Ks[2]);
			glUniform1f(uMaterialShine, materials[0].Ns);
			// Update MVP
			setupBMVP(Bmvp);
			glUniformMatrix4fv(uMatrixHandle, 1, false, &Bmvp[0][0]);
/*			setupMVP(view, model, projection);
			glUniformMatrix4fv(uViewHandle, 1, false,  &view[0][0]);
			glUniformMatrix4fv(uModelHandle, 1, false,  &model[0][0]);
			glUniformMatrix4fv(uProHandle, 1, false,  &projection[0][0]);
*/			// Output what we have
			glDrawArrays(GL_TRIANGLES, 0, numDraw);

			drawFloor(floorProgram);
			drawTree(treeProgram);
		}
		// Update SDL buffer
		SDL_GL_SwapWindow(win);
	}
	free(vertexBufferData);
	return 0;
}