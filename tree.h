#ifndef TREE_H
#define TREE_H

#include "SDL2/SDL.h"
#include "gl.h"

	int textureID;
    int uTTextureHandle;
    int uTThresholdHandle;
    int uTProHandle;
    int uTModelHandle;
    int uTViewHandle;
    int uTMatrixHandle;

    int aTPositionHandle;
    int aTUVHandle;
    unsigned int TreeVBO;

//void setupTMVP(mat4 & view, mat4 & model, mat4 & projection)
void setupTMVP(mat4 & mvp)
{
	mat4 projection = glm::perspective(glm::radians(60.0f), SCREEN_W / SCREEN_H, 0.1f, 200.0f);  // Perspective matrix
	mat4 view = glm::mat4(1.0);
	view = 		glm::rotate(view,	glm::radians(-myCam.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	view = 		glm::rotate(view, 	glm::radians(-myCam.yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	view = 		glm::translate(view,glm::vec3(-myCam.camX, -myCam.camY, -myCam.camZ));
	mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0, -10, -85));
    mat4 modelView = view * model;
    modelView[0][0] = 2.0; modelView[0][1] = 0.0;  modelView[0][2] = 0.0;
    modelView[1][0] = 0.0;  modelView[1][1] = 2.0; modelView[1][2] = 0.0;
    modelView[2][0] = 0.0;  modelView[2][1] = 0.0;  modelView[2][2] = 2.0;
    mvp = projection * modelView;
}
bool setupTree(int &programID)
{
	bool success = true;
	string treeVertexShader;
	string treeFragmentShader;
	int treeVertexID;
	int treeFragID;
	success &= parseFile((char*)"vertex.vs", treeVertexShader);
	success &= parseFile((char*)"fragmentT.fs"  , treeFragmentShader);
	success &= compileShader(treeVertexShader.c_str(), GL_VERTEX_SHADER, treeVertexID);
	success &= compileShader(treeFragmentShader.c_str(), GL_FRAGMENT_SHADER, treeFragID);
	success &= compileProgram(treeVertexID, treeFragID, programID);
    success &= loadTexture("tree.bmp", textureID);
    
	float vertices[] = {
        // positions  // texcoords
         -1, -1, -1,       0, 0,
          1, -1, -1,       1, 0,
          1,  1, -1,       1, 1,

          1,  1, -1,       1, 1,
         -1,  1, -1,       0, 1,
         -1, -1, -1,       0, 0,
    };
    glGenBuffers(1, &TreeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, TreeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    uTTextureHandle = glGetUniformLocation(programID, "u_Texture");
	uTThresholdHandle = glGetUniformLocation(programID, "u_Threshold");
	uTProHandle = glGetUniformLocation(programID, "u_Projection");
	uTModelHandle = glGetUniformLocation(programID, "u_Model");
	uTViewHandle = glGetUniformLocation(programID, "u_View");
	uTMatrixHandle = glGetUniformLocation(programID, "u_Matrix");

	aTPositionHandle = glGetAttribLocation(programID, "a_Position");
	aTUVHandle = glGetAttribLocation(programID, "a_UV");
    
    return success;
}
void drawTree(int &programID)
{
    mat4 model, view, projection;
    mat4 mvp;
    // tree
    // Setup Program, Attach appropriate buffer
    glUseProgram(programID);
    glBindBuffer(GL_ARRAY_BUFFER, TreeVBO);
    glEnableVertexAttribArray(aTPositionHandle);
    glVertexAttribPointer(aTPositionHandle, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(aTUVHandle);
    glVertexAttribPointer(aTUVHandle, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glActiveTexture(GL_TEXTURE0 + 1); // + "i" to change texture chosen
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1f(uTThresholdHandle, threshold);
    glUniform1i(uTTextureHandle, 1);
    setupTMVP(mvp);
    glUniformMatrix4fv(uTMatrixHandle, 1, false, &mvp[0][0]);
    //setupTMVP(view, model, projection);
    //glUniformMatrix4fv(uTViewHandle, 1, false,  &view[0][0]);
    //glUniformMatrix4fv(uTModelHandle, 1, false,  &model[0][0]);
    //glUniformMatrix4fv(uTProHandle, 1, false,  &projection[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
#endif