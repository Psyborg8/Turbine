uniform sampler2D texture;
uniform vec4 color;
uniform float alpha;

void main() 
{
	vec4 pixel = texture2D( texture, gl_TexCoord[0].xy );
	pixel.a = pixel.r;
	pixel = pixel * color;

	gl_FragColor = pixel;
}