#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// inputs from vertex shader
in vec2 tc;	// used for texture coordinate visualization

// output of the fragment shader
out vec4 fragColor;

uniform vec4 custom_color;
uniform sampler2D	TEX;
uniform int color_type;

void main()
{
	if (color_type == 1) fragColor = texture( TEX, tc );
	else if (color_type == 2) fragColor = texture( TEX, tc ) * custom_color;
	else if (color_type == 3) fragColor = custom_color;
	else fragColor = vec4(tc.xy, 0, 1);
}