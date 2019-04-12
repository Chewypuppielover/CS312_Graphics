uniform sampler2D u_Texture;
uniform float u_Threshold;

varying vec2 v_UV;

void main()
{
    float r, g, b;
    r = 163/255; g = 73/255; b = 164/255;
    vec4 sample = texture2D(u_Texture, v_UV);	
    if(sample.r > u_Threshold) discard;
    if(sample.r == r && sample.g == g && sample.b == b) discard;
    if(sample.r == 1 && sample.g == 1 && sample.b == 1) discard;
	gl_FragColor = sample; 
}