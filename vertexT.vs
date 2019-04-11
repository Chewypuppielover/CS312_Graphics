uniform mat4 u_Projection;
uniform mat4 u_Model;
uniform mat4 u_View;

attribute vec3 a_Position;
attribute vec2 a_UV;

varying vec2 v_UV;

void main()
{
    v_UV = a_UV;
    mat4 modelView = u_View * u_Model;
    modelView[0][0] = 2.0; modelView[0][1] = 0.0;  modelView[0][2] = 0.0;
    modelView[1][0] = 0.0;  modelView[1][1] = 2.0; modelView[1][2] = 0.0;
    modelView[2][0] = 0.0;  modelView[2][1] = 0.0;  modelView[2][2] = 2.0;

    gl_Position = u_Projection * modelView * vec4(a_Position, 1.0);
}