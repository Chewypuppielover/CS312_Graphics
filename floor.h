#ifndef FLOOR_H
#define FLOOR_H

#include "SDL2/SDL.h"
#include "gl.h"

	int textureID;
    int uTextureHandle;
    int uThresholdHandle;
    int uMatrixHandle;
    int aPositionHandle;
    int aUVHandle;
    unsigned int VBO;
    mat4 mvp;

bool setupFLoor(int &programID)
{
	bool success = true;
	string floorVertexShader;
	string floorFragmentShader;
	int floorVertexID;
	int floorFragID;
	success &= parseFile((char*)"vertex.vs", floorVertexShader);
	success &= parseFile((char*)"fragment.fs"  , floorFragmentShader);
	success &= compileShader(floorVertexShader.c_str(), GL_VERTEX_SHADER, floorVertexID);
	success &= compileShader(floorFragmentShader.c_str(), GL_FRAGMENT_SHADER, floorFragID);
	success &= compileProgram(floorVertexID, floorFragID, programID);
    success &= loadTexture("checker.bmp", textureID);
    
	float vertices[] = {
        // positions  // texcoords
         -1, -1, -1,       0, 0,
          1, -1, -1,       1, 0,
          1,  1, -1,       1, 1,

          1,  1, -1,       1, 1,
         -1,  1, -1,       0, 1,
         -1, -1, -1,       0, 0,
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    uTextureHandle = glGetUniformLocation(programID, "u_Texture");
	uThresholdHandle = glGetUniformLocation(programID, "u_Threshold");
	uMatrixHandle = glGetUniformLocation(programID, "u_Matrix");
	aPositionHandle = glGetAttribLocation(programID, "a_Position");
	aUVHandle = glGetAttribLocation(programID, "a_UV");
    
    return success;
}
void drawFloor(int &programID)
{
    // floor
    // Setup Program, Attach appropriate buffer
    glUseProgram(programID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(aPositionHandle);
    glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(aUVHandle);
    glVertexAttribPointer(aUVHandle, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glActiveTexture(GL_TEXTURE0 + 1); // + "i" to change texture chosen
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1f(uThresholdHandle, threshold);
    glUniform1i(uTextureHandle, 1);
    setupMVP(mvp);
    glUniformMatrix4fv(uFMatrixHandle, 1, false, &mvp[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
/*
void setupMVP(mat4 & mvp)
{
	mat4 proj = glm::perspective(glm::radians(60.0f), SCREEN_W / SCREEN_H, 0.1f, 200.0f);  // Perspective matrix
	mat4 view = glm::mat4(1.0);
	view = 		glm::rotate(view,	glm::radians(-myCam.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	view = 		glm::rotate(view, 	glm::radians(-myCam.yaw), glm::vec3(0.0, 1.0f, 0.0));
	view = 		glm::translate(view,glm::vec3(-myCam.camX, -myCam.camY, -myCam.camZ));
	mat4 model = glm::mat4(1.0);
    model = glm::scale(model, glm::vec3(400.0));
	mvp = proj * view * model;
}

uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec2 a_UV;

varying vec2 v_UV;

void main()
{
	v_UV = a_UV;
	gl_Position = u_Matrix * vec4(a_Position, 1.0);
}

uniform sampler2D u_Texture;
uniform float u_Threshold;

varying vec2 v_UV;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);	
    if(sample.r > u_Threshold)
    {
        discard;
    }
	gl_FragColor = sample;
}*/
#endif