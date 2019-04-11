#ifndef BUNNY_H
#define BUNNY_H

#include "SDL2/SDL.h"
#include "gl.h"

bool setupBunny(int &programID)
{
	int aPositionHandle, aUVHandle, aNormalHandle;
	int uTextureHandle, uThresholdHandle;
    int uCameraHandle, uLightHandle;
	int uMaterialA, uMaterialD, uMaterialS, uMaterialShine;
	int uProHandle, uModelHandle, uViewHandle;

	int stride;
    int numDraw;
	unsigned int VBO;
    vector<int> textureIDs;	
	vector<material> materials;

	bool success = true;
	string bunnyVertexShader;
	string bunnyFragmentShader;
	int bunnyVertexID;
	int bunnyFragID;
	success &= parseFile((char*)"phongLighting.vs", bunnyVertexShader);
	success &= parseFile((char*)"phongLighting.fs"  , bunnyFragmentShader);
	success &= compileShader(bunnyVertexShader.c_str(), GL_VERTEX_SHADER, bunnyVertexID);
	success &= compileShader(bunnyFragmentShader.c_str(), GL_FRAGMENT_SHADER, programID);
	success &= compileProgram(bunnyVertexID, bunnyFragID, programID);


	/***************************************************************************************
	* OBJECT LOADER - Vertices, UVs, Normals, etc
	***************************************************************************************/  
	vector<vertexData> vertexBuffer;
	bool hasUV;
	bool hasNormal;  
	success &= getObjData("bunny.obj", materials, vertexBuffer, hasUV, hasNormal);
	//success &= getObjData("pot.obj", materials, vertexBuffer, hasUV, hasNormal);
	numDraw = vertexBuffer.size(); 

	// Build out a single array of float data 
	stride = 3 + (2*hasUV) + (3*hasNormal);
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
		if(hasNormal)
		{
			vertexBufferData[i++] = vertexBuffer[vb].normal[0];
			vertexBufferData[i++] = vertexBuffer[vb].normal[1];
			vertexBufferData[i++] = vertexBuffer[vb].normal[2];
		}
	}

	// Load in each texture 
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
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);

	float data[] = { -0.5, -0.5, 1.0, 
					 0.5,  -0.5, 1.0, 
					 0.0,  0.0,  1.0};
	unsigned int VBB; 
	glGenBuffers(1, &VBB);
	glBindBuffer(GL_ARRAY_BUFFER, VBB);
	glBufferData(GL_ARRAY_BUFFER, 9*4, data, GL_STATIC_DRAW);

	/***************************************************************************
	* ATTRIBUTE HANDLES - based on the way the shader is written; mapped to data 
	***************************************************************************/      
	aPositionHandle = glGetAttribLocation(programID, "a_Position");
	aUVHandle = glGetAttribLocation(programID, "a_UV");
	aNormalHandle = glGetAttribLocation(programID, "a_Normal");
	/***************************************************************************************
	* UNIFORM HANDLES - kept to update uniforms; mapped for each frame (could be pre-cached)
	***************************************************************************************/      
	// Establish shader-specific variables
	uTextureHandle = glGetUniformLocation(programID, "u_Texture");
	uThresholdHandle = glGetUniformLocation(programID, "u_Threshold");
	uCameraHandle = glGetUniformLocation(programID, "u_Camera");
	uLightHandle = glGetUniformLocation(programID, "u_LightPos");

	uMaterialA = glGetUniformLocation(programID, "u_MaterialAmbient");
	uMaterialD = glGetUniformLocation(programID, "u_MaterialDiffuse");
	uMaterialS = glGetUniformLocation(programID, "u_MaterialSpecular");
	uMaterialShine = glGetUniformLocation(programID, "u_MaterialShine");

	uProHandle = glGetUniformLocation(programID, "u_Projection");
	uModelHandle = glGetUniformLocation(programID, "u_Model");
	uViewHandle = glGetUniformLocation(programID, "u_View");
}

void drawBunny(int &programID)
{
	// MVP Data for transforming vertices
	mat4 view, model, projection;

    // Setup Program, Attach appropriate buffer
    glUseProgram(programID);
    // Clear buffers, setup/use program 
    glClearColor(0,0,0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /***************************************************************************
    * ATTRIBUTE HANDLES - based on the way the shader is written; mapped to data 
    ***************************************************************************/     
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(aPositionHandle);
    glVertexAttribPointer(aPositionHandle, 
        3, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)0);
    glEnableVertexAttribArray(aUVHandle);
    glVertexAttribPointer(aUVHandle, 
        2, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(aNormalHandle);
    glVertexAttribPointer(aNormalHandle, 
        3, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(5*sizeof(float)));

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
    setupMVP(view, model, projection);
    glUniformMatrix4fv(uViewHandle, 1, false,  &view[0][0]);
    glUniformMatrix4fv(uModelHandle, 1, false,  &model[0][0]);
    glUniformMatrix4fv(uProHandle, 1, false,  &projection[0][0]);

    // Output what we have
    glDrawArrays(GL_TRIANGLES, 0, numDraw);
}
#endif