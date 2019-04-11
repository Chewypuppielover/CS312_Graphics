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
    if(sample.r == 163 && sample.g == 73 && sample.b == 164) discard;
	gl_FragColor = sample; 
}