#version 330

uniform float uAngle1;
uniform float uAngle2;
uniform float utime;

in vec4 vPosition;
in vec4 vColor;

out vec4 color;
out vec4 position;

void main()
{
	float angle1 = utime * 30.0f; // degrees
	float angle2 = utime * 20.0f; // degrees
	float rad1 = angle1 / 180.0f * 3.141592f;
	float rad2 = angle2 / 180.0f * 3.141592f;

	mat4 rot1 = mat4(1);
	mat4 rot2 = mat4(1);
	/*
	rot[i][j] : i = col, j = row : col-major
	rot[0][0];		rot[1][0];		rot[2][0];		rot[3][0];
	rot[0][1];		rot[1][1];		rot[2][1];		rot[3][1];
	rot[0][2];		rot[1][2];		rot[2][2];		rot[3][2];
	rot[0][3];		rot[1][3];		rot[2][3];		rot[3][3];
	*/

	rot1[0][0] = cos(rad1);	rot1[2][0] = -sin(rad1);
	rot1[0][2] = sin(rad1);	rot1[2][2] = cos(rad1);

	rot2[1][1] = cos(-rad2);	rot2[2][1] = -sin(-rad2);
	rot2[1][2] = sin(-rad2);	rot2[2][2] = cos(-rad2);
	
	vec4 pos = rot2 * rot1 * vPosition;

	color = vColor;
	position = vPosition; // local coordinates
	//position = pos; // screen coordinates

	gl_Position = pos;
}
