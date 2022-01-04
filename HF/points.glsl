//! \file bistro.glsl

#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;


uniform mat4 mvpMatrix;

/* decoration flat : le varying est un entier, donc pas vraiment interpolable... il faut le declarer explicitement */

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    
}

#endif


#ifdef FRAGMENT_SHADER

out vec4 fragment_color;


void main( )
{
    
    fragment_color= vec4(1., 0., 0., 1.);
}

#endif
