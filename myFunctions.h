#include "definitions.h"

#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

// Image Fragment Shader 
void ImageFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    BufferImage* ptr = (BufferImage*)uniforms[0].ptr;
    int w = (ptr -> width()-1); int h = (ptr -> height()-1);
    int x = vertAttr[0].d * w;
    int y = vertAttr[1].d * h;
    double fr = (*ptr)[y][x];
    //4294901760 (red) 4294967295 (white)
    if(x<0 || x > w || y<0 || y > h) fragment = 0xff00ff00;
    else { fragment = (*ptr)[y][x]; }
}
void ProjectVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, 
                        const Attributes & attrIn, const Attributes & uniforms)
{
        //Uniforms
        // [0] -> Image referance
        // [1] -> Model transform
        // [2] -> View transform
        // [3] -> Projection transform
    Matrix* model = (Matrix*)uniforms[1].ptr;
    Matrix* view = (Matrix*)uniforms[2].ptr;
    Matrix* proj = (Matrix*)uniforms[3].ptr;
    Matrix temp = (*proj) * (*view) * (*model);
    vertOut = temp * vertIn;
    attrOut = attrIn;
}
// My Fragment Shader for color interpolation
void ColorFragShader(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    // creating a full color from rgb parts
    PIXEL color = 0xff000000;
    color += (unsigned int)(vertAttr[0].d *0xff) << 16;
    color += (unsigned int)(vertAttr[1].d *0xff) << 8;
    color += (unsigned int)(vertAttr[2].d *0xff) << 0;
    fragment = color;
}

//My Vertex Shader
void BasicVertexShader(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, const Attributes & vertAttr, const Attributes & uniforms)
{
    Matrix* mat = (Matrix*)uniforms.attrAry[0].ptr;
    vertOut = (*mat) * vertIn;
    attrOut = vertAttr;
}

void BasicVertexShader2(Vertex & vertOut, Attributes & attrOut, const Vertex & vertIn, 
                        const Attributes & attrIn, const Attributes & uniforms)
{
        //Uniforms
        // [0] -> Image referance
        // [1] -> Model transform
        // [2] -> View transform
        // [3] -> Projection transform
    Matrix* model = (Matrix*)uniforms[1].ptr;
    Matrix* view = (Matrix*)uniforms[2].ptr;
    Matrix* proj = (Matrix*)uniforms[3].ptr;
    Matrix temp = (*proj) * (*view) * (*model);
    vertOut = temp * vertIn;
    attrOut = attrIn;
}
void ColorFragShader2(PIXEL & fragment, const Attributes & vertAttr, const Attributes & uniforms)
{
    PIXEL color = uniforms[0].d;
    fragment = color;
}
/****************************************
 * DETERMINANT
 * Find the determinant of a matrix with
 * components A, B, C, D from 2 vectors.
 ***************************************/
inline double determinant(const double & A, const double & B, const double & C, const double & D)
{ return (A*D - B*C); }
/*********************************************************************
 * CROSS_PRODUCT
 * helper functions to calculate the cross product of two vertecies
 ********************************************************************/
inline double crossProduct(double* v1, double* v2) { return (v1[0] * v2[1]) - (v1[1] * v2[0]); }

void testCAD(Buffer2D<PIXEL> & topLeft, Buffer2D<PIXEL> & topRight, Buffer2D<PIXEL> & botLeft, Buffer2D<PIXEL> & botRight)
{
        /*
        static BufferImage checkerImage("checker.bmp");
        static BufferImage foothillsImage("foothills.bmp");
        static BufferImage sharkImage("great-white-shark.bmp");
        static BufferImage marshmallowImage("marshmallow.bmp");
        
        Attributes mainImageUniforms0;
        mainImageUniforms0.addPtr((void*)&checkerImage);
        mainImageUniforms0.addPtr((void*)&model);
        mainImageUniforms0.addPtr((void*)&view);
        mainImageUniforms0.addPtr((void*)&proj);
        Attributes mainImageUniforms1 = mainImageUniforms0;
        //mainImageUniforms1.replacePtr(0, (void*)&foothillsImage);
        Attributes mainImageUniforms2 = mainImageUniforms0;
        //mainImageUniforms2.replacePtr(0, (void*)&checkerImage);
        Attributes otherUniforms0 = mainImageUniforms0;
        otherUniforms0.replacePtr(3, (void*)&orthog);
        Attributes otherUniforms1 = otherUniforms0;
        //otherUniforms1.replacePtr(0, (void*)&checkerImage);
        Attributes otherUniforms2 = otherUniforms0;
        //otherUniforms2.replacePtr(0, (void*)&checkerImage);
        */
        static BufferImage checkerImage("checker.bmp");
        static BufferImage foothillsImage("foothills.bmp");
        static BufferImage sharkImage("great-white-shark.bmp");
        static BufferImage marshmallowImage("marshmallow.bmp");
        
        double coordinates[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
        Attributes imageAttributesA[3];
        Attributes imageAttributesB[3];
        imageAttributesA[0].addDouble(coordinates[0][0], coordinates[0][1]);
        imageAttributesA[1].addDouble(coordinates[1][0], coordinates[1][1]);
        imageAttributesA[2].addDouble(coordinates[2][0], coordinates[2][1]);
        imageAttributesB[0].addDouble(coordinates[2][0], coordinates[2][1]);
        imageAttributesB[1].addDouble(coordinates[3][0], coordinates[3][1]);
        imageAttributesB[2].addDouble(coordinates[0][0], coordinates[0][1]);

        Matrix model = Matrix();
        Matrix view = Matrix().camera(myCam.x, myCam.y, myCam.z, myCam.yaw, myCam.pitch, myCam.roll);
        Matrix viewF = view;
        Matrix viewT = view.rotate(-90, 0, 0) * Matrix().trans(0, -60, 0);
        Matrix viewS = view.rotate(0, 90, 0) * Matrix().trans(-60, 0, 0);
        double NEAR = 1; double FAR = 200;
        double LEFT = -60; double RIGHT = 60;
        double BOTTOM = -60; double TOP = 60;
        double orth[4][4] = { {2/(RIGHT-LEFT), 0, 0, -(RIGHT+LEFT)/(RIGHT-LEFT)},
                              {0,2/(TOP-BOTTOM),0, -(TOP+BOTTOM)/(TOP-BOTTOM)},
                              {0,0,2/(FAR-NEAR), -(FAR+NEAR)/(FAR-NEAR)}, {0,0,0,1} };
        Matrix orthog = Matrix(orth);
        Matrix proj = Matrix().perspective(abs(RIGHT-LEFT), 1, NEAR, FAR); //FOV, Aspect, Near, Far
        
        Attributes mainImageUniforms1;
        mainImageUniforms1.addDouble(0xffff0000);
        mainImageUniforms1.addPtr((void*)&model);
        mainImageUniforms1.addPtr((void*)&view);
        mainImageUniforms1.addPtr((void*)&proj);
        Attributes mainImageUniforms2;
        mainImageUniforms2.addDouble(0xff00ff00);
        mainImageUniforms2.addPtr((void*)&model);
        mainImageUniforms2.addPtr((void*)&view);
        mainImageUniforms2.addPtr((void*)&proj);
        Attributes mainImageUniforms5;
        mainImageUniforms5.addDouble(0xff0000ff);
        mainImageUniforms5.addPtr((void*)&model);
        mainImageUniforms5.addPtr((void*)&view);
        mainImageUniforms5.addPtr((void*)&proj);
        Attributes mainImageUniforms6;
        mainImageUniforms6.addDouble(0xff00ffff);
        mainImageUniforms6.addPtr((void*)&model);
        mainImageUniforms6.addPtr((void*)&view);
        mainImageUniforms6.addPtr((void*)&proj);
        
        Attributes otherUniformsF1;
        otherUniformsF1.addDouble(0xffff0000);
        otherUniformsF1.addPtr((void*)&model);
        otherUniformsF1.addPtr((void*)&viewF);
        otherUniformsF1.addPtr((void*)&orthog);
        Attributes otherUniformsF2;
        otherUniformsF2.addDouble(0xff00ff00);
        otherUniformsF2.addPtr((void*)&model);
        otherUniformsF2.addPtr((void*)&viewF);
        otherUniformsF2.addPtr((void*)&orthog);
        Attributes otherUniformsF5;
        otherUniformsF5.addDouble(0xff0000ff);
        otherUniformsF5.addPtr((void*)&model);
        otherUniformsF5.addPtr((void*)&viewF);
        otherUniformsF5.addPtr((void*)&orthog);
        Attributes otherUniformsF6;
        otherUniformsF6.addDouble(0xff00ffff);
        otherUniformsF6.addPtr((void*)&model);
        otherUniformsF6.addPtr((void*)&viewF);
        otherUniformsF6.addPtr((void*)&orthog);

        Attributes otherUniformsT;
        otherUniformsT.addDouble(0xff00ff00);
        otherUniformsT.addPtr((void*)&model);
        otherUniformsT.addPtr((void*)&viewT);
        otherUniformsT.addPtr((void*)&orthog);
        Attributes otherUniformsS;
        otherUniformsS.addDouble(0xff0000ff);
        otherUniformsS.addPtr((void*)&model);
        otherUniformsS.addPtr((void*)&viewS);
        otherUniformsS.addPtr((void*)&orthog);

        FragmentShader fragImg;
        fragImg.FragShader = ColorFragShader2;
        VertexShader vertImg;
        vertImg.VertShader = BasicVertexShader2;

    Vertex quad3D[] = { {-20,-20, 50, 1}, {20, -20, 50, 1}, {20, 20, 50, 1}, {-20,20, 50, 1}, 
                        {-20,-20, 150, 1}, {20, -20, 150, 1}, {20, 20, 150, 1}, {-20,20, 150, 1}};
    Vertex verticesImg1A[3] = { quad3D[0], quad3D[1], quad3D[2] };
    Vertex verticesImg1B[3] = { quad3D[2], quad3D[3], quad3D[0] };
    Vertex verticesImg2A[3] = { quad3D[3], quad3D[2], quad3D[5] };
    Vertex verticesImg2B[3] = { quad3D[5], quad3D[4], quad3D[3] };
    Vertex verticesImg3A[3] = { quad3D[4], quad3D[5], quad3D[6] };
    Vertex verticesImg3B[3] = { quad3D[6], quad3D[7], quad3D[4] };
    Vertex verticesImg4A[3] = { quad3D[7], quad3D[5], quad3D[1] };
    Vertex verticesImg4B[3] = { quad3D[1], quad3D[0], quad3D[7] };
    Vertex verticesImg5A[3] = { quad3D[1], quad3D[6], quad3D[5] };
    Vertex verticesImg5B[3] = { quad3D[5], quad3D[2], quad3D[1] };
    Vertex verticesImg6A[3] = { quad3D[7], quad3D[0], quad3D[3] };
    Vertex verticesImg6B[3] = { quad3D[3], quad3D[4], quad3D[7] };

    DrawPrimitive(TRIANGLE, topLeft, verticesImg1A, imageAttributesA, &mainImageUniforms1, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topLeft, verticesImg1B, imageAttributesB, &mainImageUniforms1, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topLeft, verticesImg2A, imageAttributesA, &mainImageUniforms2, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topLeft, verticesImg2B, imageAttributesB, &mainImageUniforms2, &fragImg, &vertImg);
    //DrawPrimitive(TRIANGLE, topLeft, verticesImg3A, imageAttributesA, &mainImageUniforms, &fragImg, &vertImg);
    //DrawPrimitive(TRIANGLE, topLeft, verticesImg3B, imageAttributesB, &mainImageUniforms, &fragImg, &vertImg);
    //DrawPrimitive(TRIANGLE, topLeft, verticesImg4A, imageAttributesA, &mainImageUniforms, &fragImg, &vertImg);
    //DrawPrimitive(TRIANGLE, topLeft, verticesImg4B, imageAttributesB, &mainImageUniforms, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topLeft, verticesImg5A, imageAttributesA, &mainImageUniforms5, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topLeft, verticesImg5B, imageAttributesB, &mainImageUniforms5, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topLeft, verticesImg6A, imageAttributesA, &mainImageUniforms6, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topLeft, verticesImg6B, imageAttributesB, &mainImageUniforms6, &fragImg, &vertImg);
    
    DrawPrimitive(TRIANGLE, topRight, verticesImg1A, imageAttributesA, &otherUniformsF1, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topRight, verticesImg1B, imageAttributesB, &otherUniformsF1, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topRight, verticesImg2A, imageAttributesA, &otherUniformsF2, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topRight, verticesImg2B, imageAttributesB, &otherUniformsF2, &fragImg, &vertImg);
    //DrawPrimitive(TRIANGLE, topRight, verticesImg3A, imageAttributesA, &otherUniformsF, &fragImg, &vertImg);
    //DrawPrimitive(TRIANGLE, topRight, verticesImg3B, imageAttributesB, &otherUniformsF, &fragImg, &vertImg);
    //DrawPrimitive(TRIANGLE, topRight, verticesImg4A, imageAttributesA, &otherUniformsF, &fragImg, &vertImg);
    //DrawPrimitive(TRIANGLE, topRight, verticesImg4B, imageAttributesB, &otherUniformsF, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topRight, verticesImg5A, imageAttributesA, &otherUniformsF5, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topRight, verticesImg5B, imageAttributesB, &otherUniformsF5, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topRight, verticesImg6A, imageAttributesA, &otherUniformsF6, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, topRight, verticesImg6B, imageAttributesB, &otherUniformsF6, &fragImg, &vertImg);

    DrawPrimitive(TRIANGLE, botLeft, verticesImg1A, imageAttributesA, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg1B, imageAttributesB, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg2A, imageAttributesA, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg2B, imageAttributesB, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg3A, imageAttributesA, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg3B, imageAttributesB, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg4A, imageAttributesA, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg4B, imageAttributesB, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg5A, imageAttributesA, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg5B, imageAttributesB, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg6A, imageAttributesA, &otherUniformsT, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botLeft, verticesImg6B, imageAttributesB, &otherUniformsT, &fragImg, &vertImg);

    DrawPrimitive(TRIANGLE, botRight, verticesImg1A, imageAttributesA, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg1B, imageAttributesB, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg2A, imageAttributesA, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg2B, imageAttributesB, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg3A, imageAttributesA, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg3B, imageAttributesB, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg4A, imageAttributesA, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg4B, imageAttributesB, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg5A, imageAttributesA, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg5B, imageAttributesB, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg6A, imageAttributesA, &otherUniformsS, &fragImg, &vertImg);
    DrawPrimitive(TRIANGLE, botRight, verticesImg6B, imageAttributesB, &otherUniformsS, &fragImg, &vertImg);
}


#endif