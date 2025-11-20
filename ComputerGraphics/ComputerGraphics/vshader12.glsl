#version 330

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vNormal;
out vec4 color;

uniform mat4 uMat;
uniform vec4 uColor;

void main()
{
	gl_Position = uMat * vPosition;
	gl_Position *= vec4(1, 1, -1, 1);	// z축 방향이 반대임

	//color = vec4(vNormal, 1);
	color = uColor;
}
