#include "definitions.h"
#include "myFunctions.h"

#ifndef PERSONAL_PROJECT_1_H
#define PERSONAL_PROJECT_1_H

const int X = 10;
const int Y = 30;
static bool isSetup = true;
    static int iter = 5;
void flipSetup() { isSetup = !isSetup; }

static Vertex quad[4];
static double coordinates[4][2];
static Attributes imageUniforms;
Attributes imageAttributesA[3];
Attributes imageAttributesB[3];
FragmentShader fragImg;
VertexShader vertImg;
    static BufferImage myImage("marshmallow.bmp");
    static BufferImage myImage2("marshmallowSide.bmp");
static Matrix model;
static Matrix view = Matrix().camera(myCam.x, myCam.y, myCam.z, myCam.yaw, myCam.pitch, myCam.roll);
static Matrix oposite;
static Matrix proj = Matrix().perspective(60.0, 1, 1, 200); //FOV, Astpect, Near, Far

void wormMotion(Buffer2D<PIXEL> & target)
{
    static bool clockwise = true;
    model = model * Matrix().trans(0,1,0);
    oposite = oposite * Matrix().trans(0,1,0);
    if(clockwise) 
    {
        model = model * Matrix().rotate(0, 0, -5);
        oposite = oposite * Matrix().rotate(0, 0, 5);
    }
    else 
    {
        model = model * Matrix().rotate(0, 0, 5);
        oposite = oposite * Matrix().rotate(0, 0, -5);
    }
    iter++;
    if(iter == 10)
    {
        iter = 0;
        clockwise = !clockwise;
    }

    Attributes imageUniforms2;
    imageUniforms2.addPtr((void*)&myImage2);
    imageUniforms2.addPtr((void*)&oposite);
    imageUniforms2.addPtr((void*)&view);
    imageUniforms2.addPtr((void*)&proj);

    static Vertex verticesImgA[3] = { quad[0], quad[1], quad[2] };
    static Vertex verticesImgB[3] = { quad[2], quad[3], quad[0] };
    Attributes imageAttributesA2[3];
    Attributes imageAttributesB2[3];
        imageAttributesA2[0].addDouble(coordinates[1][0], coordinates[1][1]);
        imageAttributesA2[1].addDouble(coordinates[0][0], coordinates[0][1]);
        imageAttributesA2[2].addDouble(coordinates[3][0], coordinates[3][1]);
        imageAttributesB2[0].addDouble(coordinates[3][0], coordinates[3][1]);
        imageAttributesB2[1].addDouble(coordinates[2][0], coordinates[2][1]);
        imageAttributesB2[2].addDouble(coordinates[1][0], coordinates[1][1]);
        //SDL_Delay(1000);
    // Draw image triangle
    DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA2, &imageUniforms2, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB2, &imageUniforms2, &fragImg, &vertImg);
}

void TestMarshmallowFrag(Buffer2D<PIXEL> & target)
{
    //SDL_Event e;
    //while(SDL_PollEvent(&e)) if(e.key.keysym.sym == 'g' && e.type == SDL_KEYDOWN) isSetup = !isSetup;
    static int counter = 0;
    static double rotate = 0;
    if(isSetup)
    {
        quad[0] = {-20, -25, 50, 1};
        quad[1] = { 20, -25, 50, 1};
        quad[2] = { 20,  25, 50, 1};
        quad[3] = {-20,  25, 50, 1};
        coordinates[0][0] = 0; coordinates[0][1] = 0; coordinates[1][0] = 1; coordinates[1][1] = 0;
        coordinates[2][0] = 1; coordinates[2][1] = 1; coordinates[3][0] = 0; coordinates[3][1] = 1;
        fragImg.FragShader = ImageFragShader;
        vertImg.VertShader = ProjectVertexShader;
        model = Matrix();
        oposite = Matrix().rotate(0, 0, 90) * Matrix().trans(3*X, 0, 0);
            imageUniforms.clear();
            imageUniforms.addPtr((void*)&myImage);
            imageUniforms.addPtr((void*)&model);
            imageUniforms.addPtr((void*)&view);
            imageUniforms.addPtr((void*)&proj);
            imageAttributesA[0].clear();
            imageAttributesA[1].clear();
            imageAttributesA[2].clear();
            imageAttributesB[0].clear();
            imageAttributesB[1].clear();
            imageAttributesB[2].clear();
            imageAttributesA[0].addDouble(coordinates[0][0], coordinates[0][1]);
            imageAttributesA[1].addDouble(coordinates[1][0], coordinates[1][1]);
            imageAttributesA[2].addDouble(coordinates[2][0], coordinates[2][1]);
            imageAttributesB[0].addDouble(coordinates[2][0], coordinates[2][1]);
            imageAttributesB[1].addDouble(coordinates[3][0], coordinates[3][1]);
            imageAttributesB[2].addDouble(coordinates[0][0], coordinates[0][1]);
        counter = 0;
        rotate = 0;
        iter = 5;
        //isSetup = !isSetup;
    }
    if(!isSetup)
    {
        switch (counter)
        {
            case 0:
                /*quad[0] = {-X, -Y, 80, 1};
                quad[1] = { X, -Y, 80, 1};
                quad[2] = { X,  Y, 80, 1};
                quad[3] = {-X,  Y, 80, 1};*/
                quad[0] = {0, 0, 80, 1};
                quad[1] = {2*X, 0, 80, 1};
                quad[2] = {2*X,  2*Y, 80, 1};
                quad[3] = {0, 2*Y, 80, 1};
                model = model * Matrix().trans(-Y, -Y, 0);
                SDL_Delay(500);
                break;
            case 1:
                imageUniforms.clear();
                imageUniforms.addPtr((void*)&myImage2);
                imageUniforms.addPtr((void*)&model);
                imageUniforms.addPtr((void*)&view);
                imageUniforms.addPtr((void*)&proj);
                SDL_Delay(500);
                break;
            case 2:
                rotate++;
                if (rotate < 18) counter--;
                model = Matrix().trans(-Y, -Y+(1.5*rotate), 0) * Matrix().rotate(0, 0, -5*rotate);
                break;
            case 3:
                quad[0] = {-X, -Y, 80, 1};
                quad[1] = { X, -Y, 80, 1};
                quad[2] = { X,  Y, 80, 1};
                quad[3] = {-X,  Y, 80, 1};
                model = Matrix().trans(0, -X, 0) * Matrix().rotate(0,0,-90);
            default:
                wormMotion(target);
                break;
        }
        counter++;
    }
    
    Vertex verticesImgA[3] = { quad[0], quad[1], quad[2] };
    Vertex verticesImgB[3] = { quad[2], quad[3], quad[0] };
    
    // Draw image triangle
    DrawPrimitive(TRIANGLE, target, verticesImgA, imageAttributesA, &imageUniforms, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, target, verticesImgB, imageAttributesB, &imageUniforms, &fragImg, &vertImg);
}

#endif
