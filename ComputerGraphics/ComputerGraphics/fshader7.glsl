#version 330

in vec4 color;
in vec4 position;

out vec4 fColor;

void main()
{   
	float width = 0.05;
	fColor = color;

	if((abs(position.x) > 0.5 - width && abs(position.y) > 0.5 - width) || 
		(abs(position.y) > 0.5 - width && abs(position.z) > 0.5 - width)||
		(abs(position.z) > 0.5  - width&& abs(position.x) > 0.5 - width))
		fColor = vec4(0, 1, 0, 1); // Green

}