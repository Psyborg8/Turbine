uniform sampler2D texture;
uniform float alpha;

void main() {
	vec4 color = sampler2D( texture, gl_TexCoord[0].xy );

	color.a = color.r;
	
	gl_FragColor = color;
}