uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec3 v_viewPos;
//varying vec3 v_lightPos;
uniform vec3 myCam;

void main()
{
    v_UV = a_UV;
    v_Normal = a_Normal;
    //v_lightPos = vec3(1.2f, 1.0f, 2.0f);
    //viewPos = vec3(myCam.camX, myCam.camY, myCam.camZ);

    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    v_FragPos = vec3(gl_Position);
}