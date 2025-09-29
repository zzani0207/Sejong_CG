#version 330

uniform float uTime;

in vec4 color;
in vec4 position;
out vec4 fColor;

void main()
{   
	float width = abs(0.05*sin(uTime*3.141592f));

	fColor = color;
	
	if( (abs(position.x)>0.5-width && abs(position.y)>0.5-width) ||
		(abs(position.y)>0.5-width && abs(position.z)>0.5-width) ||
		(abs(position.x)>0.5-width && abs(position.z)>0.5-width))
		fColor = vec4(0,1,0,1);
		
}