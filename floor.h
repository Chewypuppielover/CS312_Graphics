#ifndef FLOOR_H
#define FLOOR_H

#include "SDL2/SDL.h"
#include "gl.h"

	int floorTextureID;
    int uFTextureHandle;
    int uFThresholdHandle;
    int uFMatrixHandle;
    int aFPositionHandle;
    int aFUVHandle;
    unsigned int FVBO;
    mat4 Fmvp;

void setupFMVP(mat4 & mvp)
{
	mat4 proj = glm::perspective(glm::radians(60.0f), SCREEN_W / SCREEN_H, 0.1f, 200.0f);  // Perspective matrix
	mat4 view = glm::mat4(1.0);
	view = 		glm::rotate(view,	glm::radians(-myCam.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	view = 		glm::rotate(view, 	glm::radians(-myCam.yaw), glm::vec3(0.0, 1.0f, 0.0));
	view = 		glm::translate(view,glm::vec3(-myCam.camX, -myCam.camY, -myCam.camZ));
	mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0, 30, -10));
	model = glm::rotate(model, glm::radians(-65.0f), glm::vec3(1.0f,0.0f,0.0f));
    model = glm::scale(model, glm::vec3(100.0f));
	mvp = proj * view * model;
}

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
    success &= loadTexture("checker.bmp", floorTextureID);
    
	
	float vertices[] = {
        -1, -1,  0,   0, 0,
		 1, -1,  0,   1, 0,
		 1,  1, -1,   1, 1,

		 1,  1, -1,   0, 0, //1, 1,
		-1,  1, -1,   0, 1, //0, 1
        -1, -1,  0,   1, 1, //0, 0,
    };
    glGenBuffers(1, &FVBO);
    glBindBuffer(GL_ARRAY_BUFFER, FVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    uFTextureHandle = glGetUniformLocation(programID, "u_Texture");
	uFThresholdHandle = glGetUniformLocation(programID, "u_Threshold");
	uFMatrixHandle = glGetUniformLocation(programID, "u_Matrix");
	aFPositionHandle = glGetAttribLocation(programID, "a_Position");
	aFUVHandle = glGetAttribLocation(programID, "a_UV");
    
    return success;
}
void drawFloor(int &programID)
{
    // floor
    // Setup Program, Attach appropriate buffer
    glUseProgram(programID);
    glBindBuffer(GL_ARRAY_BUFFER, FVBO);
    glEnableVertexAttribArray(aFPositionHandle);
    glVertexAttribPointer(aFPositionHandle, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(aFUVHandle);
    glVertexAttribPointer(aFUVHandle, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glActiveTexture(GL_TEXTURE0 + 1); // + "i" to change texture chosen
    glBindTexture(GL_TEXTURE_2D, floorTextureID);
    glUniform1f(uFThresholdHandle, threshold);
    glUniform1i(uFTextureHandle, 1);
    setupFMVP(Fmvp);
    glUniformMatrix4fv(uFMatrixHandle, 1, false, &Fmvp[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

/*
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