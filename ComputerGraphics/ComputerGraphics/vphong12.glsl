#version 330

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vNormal;

out vec4 color;
out vec3 normal;
out vec3 pos;

uniform mat4 uProjMat;
uniform mat4 uModelMat;

void main()
{
	gl_Position = uProjMat * (uModelMat * vPosition);
	gl_Position *= vec4(1, 1, -1, 1);	// z           ݴ   

	 pos = (uModelMat * vPosition).xyz; // in cameera coord
	 normal = normalize(uModelMat * vec4((vNormal), 0)).xyz; // in camera coord



	color = vColor;
	//color = vec4(vNormal, 1);
	//color = N4;
	//color = phong;
}
