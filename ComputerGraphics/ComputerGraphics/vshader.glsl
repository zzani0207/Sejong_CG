#version 330

in vec4 vPosition;
in vec4 vColor;

out vec4 color;

void main()
{
	float angle = 30;
	float theta = angle / 360.0f * 2.0f * 3.1415926f; // convert to radians

	mat4 rot = mat4(1);

	// i = row, j = column: row major
	// i = column, j = row: column major
	// openGL uses column major
	
	// rotation around y axis
	rot[0][0] = cos(theta); rot[2][0] = -sin(theta);
	rot[0][2] = sin(theta); rot[2][2] = cos(theta);

	// rotation around x axis
	// rot[1][1] = cos(theta); rot[2][1] = sin(theta);
	// rot[1][2] = -sin(theta); rot[2][2] = cos(theta);

	// rotation around z axis
	// rot[0][0] = cos(theta); rot[1][0] = sin(theta);
	// rot[0][1] = -sin(theta); rot[1][1] = cos(theta);

	vec4 pos = rot * vPosition;

	color = vColor;

	gl_Position = pos;
}
