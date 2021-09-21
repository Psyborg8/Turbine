#version 150

uniform sampler2D texture;
in vec2 texCoord

void main() 
{
	vec4 out = texture2D( texture, texCoord );
	gl_FragColor = out;
}