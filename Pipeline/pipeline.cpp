#include "definitions.h"
#include "coursefunctions.h"
#include "myFunctions.h"
#include "personalProject1.h"
using namespace std;


bool DEBUG = true;
/***********************************************
 * CLEAR_SCREEN
 * Sets the screen to the indicated color value.
 **********************************************/
void clearScreen(Buffer2D<PIXEL> & frame, PIXEL color = 0xff000000)
{
    int h = frame.height();
    int w = frame.width();
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            frame[y][x] = color;
        }
    }
}

/************************************************************
 * UPDATE_SCREEN
 * Blits pixels from RAM to VRAM for rendering.
 ***********************************************************/
void SendFrame(SDL_Texture* GPU_OUTPUT, SDL_Renderer * ren, SDL_Surface* frameBuf) 
{
    SDL_UpdateTexture(GPU_OUTPUT, NULL, frameBuf->pixels, frameBuf->pitch);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, GPU_OUTPUT, NULL, NULL);
    SDL_RenderPresent(ren);
}

/*************************************************************
 * POLL_CONTROLS
 * Updates the state of the application based on:
 * keyboard, mouse, touch screen, gamepad inputs. 
 ************************************************************/
void processUserInputs(bool & running)
{
    SDL_Event e;
    int mouseX;
    int mouseY;
    while(SDL_PollEvent(&e)) 
    {
        if(e.type == SDL_QUIT) running = false;
        if(e.key.keysym.sym == 'q' && e.type == SDL_KEYDOWN) running = false;
        if(e.key.keysym.sym == 'g' && e.type == SDL_KEYDOWN) flipSetup();
        if(e.type == SDL_MOUSEMOTION)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                double mouseX = e.motion.xrel;
                double mouseY = e.motion.yrel;

                myCam.yaw += mouseX * 0.02;
                myCam.pitch += mouseY * 0.02;
            }
        }
        if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            else
            {
                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }

        // Translation
        if(e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN)
        {
            double yawR = myCam.yaw * (M_PI/180.0);
            myCam.x -= sin(yawR) * 0.5;
            myCam.z += cos(yawR) * 0.5;
        }
        if(e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN)
        {
            double yawR = myCam.yaw * (M_PI/180.0);
            myCam.x += sin(yawR) * 0.5;
            myCam.z -= cos(yawR) * 0.5;
        }
        if(e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN)
        {
            double yawR = myCam.yaw * (M_PI/180.0);
            myCam.x -= cos(yawR) * 0.5;
            myCam.z -= sin(yawR) * 0.5;
        }
        if(e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN)
        {
            double yawR = myCam.yaw * (M_PI/180.0);
            myCam.x += cos(yawR) * 0.5;
            myCam.z += sin(yawR) * 0.5;
        }
    }
}

/****************************************
 * DRAW_POINT
 * Renders a point to the screen with the
 * appropriate coloring.
 ***************************************/
void DrawPoint(Buffer2D<PIXEL> & target, Vertex* v, Attributes* attrs, Attributes * const uniforms, FragmentShader* const frag)
{
    //target[(int)v[0].y][(int)v[0].x] = attrs[0].color;

    //frag callback -> coloring the fragment(in this case pixel)
    frag -> FragShader(target[(int)v[0].y][(int)v[0].x], *attrs, *uniforms);
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    //bounding box to limit the amount of checking
    int maxX = MAX3(triangle[0].x,  triangle[1].x,  triangle[2].x);
    int minX = MIN3(triangle[0].x,  triangle[1].x,  triangle[2].x);
    int maxY = MAX3(triangle[0].y,  triangle[1].y,  triangle[2].y);
    int minY = MIN3(triangle[0].y,  triangle[1].y,  triangle[2].y);
    
    /*bounding edges of the triangle
     *vectors that are a measuremtnt of the edge from triangle[0] to triangle[1] 
     *and from triangle[1] to triangle[2] and from triangle[2] to triangle[0] */
    double edge0[] = {(triangle[2].x - triangle[1].x),  (triangle[2].y - triangle[1].y)};
    double edge1[] = {(triangle[0].x - triangle[2].x),  (triangle[0].y - triangle[2].y)};
    double edge2[] = {(triangle[1].x - triangle[0].x),  (triangle[1].y - triangle[0].y)};

    //area of the whole triangle
    double area = determinant(-edge1[0], edge0[0], -edge1[1], edge0[1]);

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x ++)
        {
            //area of each segment
            double area0 = determinant(triangle[2].x - x, edge0[0], triangle[2].y - y, edge0[1])/area;
            double area1 = determinant(triangle[0].x - x, edge1[0], triangle[0].y - y, edge1[1])/area;
            double area2 = determinant(triangle[1].x - x, edge2[0], triangle[1].y - y, edge2[1])/area;

            if ( (area0 >= 0) && (area1 >= 0) && (area2 >= 0) )
            {
                //Interpolate attributes
                double Z = 1/((triangle[0].w*area0) + (triangle[1].w*area1) + (triangle[2].w*area2));
                Attributes interAttr(attrs, area0, area1, area2, Z);
                int h = target.height(); int w = target.width();
                //frag callback -> coloring the fragment(in this case pixel)
                //if(x >= w) x -= (w-2);
                if(x < h && y < w && x > 0 && y > 0)
                    frag -> FragShader(target[y][x], interAttr, *uniforms);
            }
        }
    }
}

/**************************************************************
 * VERTEX_SHADER_EXECUTE_VERTICES
 * Executes the vertex shader on inputs, yielding transformed
 * outputs. 
 *************************************************************/
void VertexShaderExecuteVertices(const VertexShader* vert, Vertex const inputVerts[], Attributes const inputAttrs[], const int& numIn, 
                                 Attributes* const uniforms, Vertex transformedVerts[], Attributes transformedAttrs[])
{
    // Defaults to pass-through behavior
    if(vert == NULL)
    {
        for(int i = 0; i < numIn; i++)
        {
            transformedVerts[i] = inputVerts[i];
            transformedAttrs[i] = inputAttrs[i];
        }
        return;
    }
    for(int i = 0; i < numIn; i++)
    {
        vert->VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], *uniforms);
    }
}

void intersectAgainstYLimit(double & along, const double & Limit, const double & segStart, const double & segEnd)
{
    along = -1;
    double segDiff = segEnd - segStart;
    if(segDiff == 0) return;
    along = (Limit - segStart) / segDiff;
}
void intersectAtPositiveLine(double & along, const double & segStartX, const double & segStartY, 
                             const double & segEndX, const double & segEndY)
{
    along = -1;
    double segDiffX = segEndX - segStartX;
    double segDiffY = segEndY - segStartY;
    if(segDiffX == segDiffY) return;
    along = (segStartY - segStartX) / (segDiffX - segDiffY);
}
void intersectAtNegativeLine(double & along, const double & segStartX, const double & segStartY, 
                             const double & segEndX, const double & segEndY)
{
    along = -1;
    double segDiffX = segEndX - segStartX;
    double segDiffY = segEndY - segStartY;
    if(segDiffX == segDiffY) return;
    along = (segStartY + segStartX) / (-segDiffX - segDiffY);
}
Vertex VertexBetweenVerts(const Vertex & vertA, const Vertex & vertB, const double & along)
{
    Vertex rv;
    rv.x = vertA.x + ((vertB.x-vertA.x) * along);
    rv.y = vertA.y + ((vertB.y-vertA.y) * along);
    rv.z = vertA.z + ((vertB.z-vertA.z) * along);
    rv.w = vertA.w + ((vertB.w-vertA.w) * along);
    return rv;
}

void clipVerticies(Vertex const transformedVerts[], Attributes const transformedAttrs[],
                     const int & numIn, Vertex clippedVerts[], Attributes clippedAttrs[], int & numClipped)
{
    /*/Pass-through
    numClipped = numIn;
    for(int i = 0; i < numClipped; i++)
    {
        clippedVerts[i] = transformedVerts[i];
        clippedAttrs[i] = transformedAttrs[i];
    }
    */
    //TMP clip buffers
    int num;
    int numOut;
    bool inBounds[MAX_VERTICES];
    Vertex tempVertA[MAX_VERTICES];
    Vertex tempVertB[MAX_VERTICES];
    Attributes tempAttrA[MAX_VERTICES];
    Attributes tempAttrB[MAX_VERTICES];

    Vertex const * srcVerts;
    Vertex* sinkVerts;
    Attributes const * srcAttrs;
    Attributes* sinkAttrs;
    
    //Setup Pointers for the first round of clipping
    srcVerts = transformedVerts;
    srcAttrs = transformedAttrs;
    sinkAttrs = tempAttrA;
    sinkVerts = tempVertA;
    num = numIn;
    numOut = 0;

    // Clip on each side against wLimit
    double wLimit = 0.001;
    for(int i = 0; i < num; i++) inBounds[i] = (srcVerts[i].w > wLimit);
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];
        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else {;}
    }
    
    //Clip against X
    //Set pointers
    srcVerts = tempVertA;
    srcAttrs = tempAttrA;
    sinkVerts = tempVertB;
    sinkAttrs = tempAttrB;
    num = numOut;
    numOut = 0;

    //Clip against X=W
    for(int i = 0; i < num; i++) inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];
        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else {;}
    }

    //Setup Pointers
    srcVerts = tempVertB;
    srcAttrs = tempAttrB;
    sinkVerts = tempVertA;
    sinkAttrs = tempAttrA;
    num = numOut;
    numOut = 0;
    
    //Clip against -X=W
    for(int i = 0; i < num; i++) inBounds[i] = (-srcVerts[i].x < srcVerts[i].w);
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];
        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else {;}
    }

    //Clip against Y
    //Setup pointers
    srcVerts = tempVertA;
    srcAttrs = tempAttrA;
    sinkVerts = tempVertB;
    sinkAttrs = tempAttrB;
    num = numOut;
    numOut = 0;

    //Clip against Y=W
    for(int i = 0; i < num; i++) inBounds[i] = (srcVerts[i].y < srcVerts[i].w);
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];
        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else {;}
    }

    //Setup Pointers
    srcVerts = tempVertB;
    srcAttrs = tempAttrB;
    sinkVerts = tempVertA;
    sinkAttrs = tempAttrA;
    num = numOut;
    numOut = 0;
    
    //Clip against -Y=W
    for(int i = 0; i < num; i++) inBounds[i] = (-srcVerts[i].y < srcVerts[i].w);
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];
        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else {;}
    }

    //Clip against Z
    //Setup pointers
    srcVerts = tempVertA;
    srcAttrs = tempAttrA;
    sinkVerts = tempVertB;
    sinkAttrs = tempAttrB;
    num = numOut;
    numOut = 0;

    //Clip against Z=W
    for(int i = 0; i < num; i++) inBounds[i] = (srcVerts[i].z < srcVerts[i].w);
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];
        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else {;}
    }

    //Setup Pointers
    srcVerts = tempVertB;
    srcAttrs = tempAttrB;
    sinkVerts = clippedVerts;
    sinkAttrs = clippedAttrs;
    num = numOut;
    numOut = 0;
    
    //Clip against -Z=W
    for(int i = 0; i < num; i++) inBounds[i] = (-srcVerts[i].z < srcVerts[i].w);
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];
        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else {;}
    }

    //Final number of output verticies
    numClipped = numOut;//
}

void normalizeVerticies(Vertex clippedVerts[], Attributes clippedAttrs[], int const &numClipped)
{
    for(int i = 0; i < numClipped; i++)
    {
        // Normalize X,Y,Z components of homogeneous coordinates
        clippedVerts[i].x /= clippedVerts[i].w;
        clippedVerts[i].y /= clippedVerts[i].w;
        clippedVerts[i].z /= clippedVerts[i].w;
        // Setup W value for depth interpolation
        double z = clippedVerts[i].w;
        clippedVerts[i].w = 1.0/z;
        // Setup Attributes
        int size = clippedAttrs[i].attrAry.size();
        for(int j = 0; j < size; j++)
        {
            double temp = clippedAttrs[i][j].d;
            clippedAttrs[i][j].d /= z;
        }
    }
}

void viewPortTransform(Buffer2D<PIXEL> & target, Vertex clippedVerts[], const int numClipped)
{
    // Move from -1 to 1 space in X,Y to screen coordinates
    int w = target.width();
    int h = target.height();
    for(int i = 0; i < numClipped; i++)
    {
        clippedVerts[i].x = round(((clippedVerts[i].x + 1) / 2.0 *w));
        clippedVerts[i].y = round(((clippedVerts[i].y + 1) / 2.0 *h));
    }
}

/***************************************************************************
 * DRAW_PRIMITIVE
 * Processes the indicated PRIMITIVES type through pipeline stages of:
 *  1) Vertex Transformation
 *  2) Clipping
 *  3) Normalization
 *  4) ViewPort transform
 *  5) Rasterization & Fragment Shading
 **************************************************************************/
void DrawPrimitive(PRIMITIVES prim, 
                   Buffer2D<PIXEL>& target,
                   const Vertex inputVerts[], 
                   const Attributes inputAttrs[],
                   Attributes* const uniforms,
                   FragmentShader* const frag,                   
                   VertexShader* const vert,
                   Buffer2D<double>* zBuf)
{
    // Setup count for vertices & attributes
    int numIn = 0;
    switch(prim)
    {
        case POINT:
            numIn = 1;
            break;
        case LINE:
            numIn = 2;
            break;
        case TRIANGLE:
            numIn = 3;
            break;
    }
    
    // Vertex shader 
    Vertex transformedVerts[MAX_VERTICES];
    Attributes transformedAttrs[MAX_VERTICES];
    VertexShaderExecuteVertices(vert, inputVerts, inputAttrs, numIn, uniforms, transformedVerts, transformedAttrs);
    //Clipping
    Vertex clippedVerts[MAX_VERTICES];
    Attributes clippedAttrs[MAX_VERTICES];
    int numClipped = numIn;
    clipVerticies(transformedVerts, transformedAttrs, numIn, clippedVerts, clippedAttrs, numClipped);
    //Normalize
    normalizeVerticies(clippedVerts, clippedAttrs, numClipped);
    //Adapt viewport
    viewPortTransform(target, clippedVerts, numClipped);
    /*double numAttr4[6] = {clippedAttrs[0][0].d, clippedAttrs[0][1].d, 
                        clippedAttrs[1][0].d, clippedAttrs[1][1].d,
                        clippedAttrs[2][0].d, clippedAttrs[2][1].d};*/

    // Vertex Interpolation & Fragment Drawing
    switch(prim)
    {
        case POINT:
            DrawPoint(target, transformedVerts, transformedAttrs, uniforms, frag);
            break;
        case LINE:
            DrawLine(target, transformedVerts, transformedAttrs, uniforms, frag);
            break;
        case TRIANGLE:
            Vertex tri[3];
            Attributes vAttr[3];
            for(int i = 2; i < numClipped; i++)
            {
                tri[0] = clippedVerts[0];
                tri[1] = clippedVerts[i-1];
                tri[2] = clippedVerts[i];
                vAttr[0] = clippedAttrs[0];
                vAttr[1] = clippedAttrs[i-1];
                vAttr[2] = clippedAttrs[i];
                DrawTriangle(target, tri, vAttr, uniforms, frag);
            }
    }
}

/*************************************************************
 * MAIN:
 * Main game loop, initialization, memory management
 ************************************************************/
int main()
{
    // -----------------------DATA TYPES----------------------
    SDL_Window* WIN;               // Our Window
    SDL_Renderer* REN;             // Interfaces CPU with GPU
    SDL_Texture* GPU_OUTPUT;       // GPU buffer image (GPU Memory)
    SDL_Surface* FRAME_BUF;        // CPU buffer image (Main Memory) 

    // ------------------------INITIALIZATION-------------------
    SDL_Init(SDL_INIT_EVERYTHING);
    WIN = SDL_CreateWindow(WINDOW_NAME, 200, 200, S_WIDTH, S_HEIGHT, 0);
    REN = SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);
    FRAME_BUF = SDL_CreateRGBSurface(0, S_WIDTH, S_HEIGHT, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    FRAME_BUF = SDL_ConvertSurface(SDL_GetWindowSurface(WIN), SDL_GetWindowSurface(WIN)->format, 0);
    GPU_OUTPUT = SDL_CreateTextureFromSurface(REN, FRAME_BUF);
    BufferImage frame(FRAME_BUF);

    // Draw loop 
    bool running = true;
    while(running) 
    {           
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        // TODO Your code goes here
            TestMarshmallowFrag(frame);
            //CADView(frame);
            //TestPipeline(frame);
            //TestVertexShader(frame);
            //TestDrawPerspectiveCorrect(frame);
            //TestDrawFragments(frame);
            //TestDrawTriangle(frame);
            //TestDrawPixel(frame);
        // Push to the GPU
        SendFrame(GPU_OUTPUT, REN, FRAME_BUF);
    }

    // Cleanup
    SDL_FreeSurface(FRAME_BUF);
    SDL_DestroyTexture(GPU_OUTPUT);
    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(WIN);
    SDL_Quit();
    return 0;
}