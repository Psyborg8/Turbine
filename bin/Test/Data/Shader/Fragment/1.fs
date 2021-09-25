uniform sampler2D texture;
uniform vec4 color;
uniform float alpha;

void main() 
{
	vec4 pixel = texture2D( texture, gl_TexCoord[0].xy );
	float alpha = pixel.r;
	pixel = color;
	pixel.a = pixel.a * alpha;

	gl_FragColor = pixel;
}