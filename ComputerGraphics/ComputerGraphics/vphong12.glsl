#version 330

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vNormal;
out vec4 color;

uniform mat4 uProjMat;
uniform mat4 uModelMat;

void main()
{
	gl_Position = uProjMat * (uModelMat * vPosition);
	gl_Position *= vec4(1, 1, -1, 1);	// z           ݴ   

	vec4 LPos = vec4(2, 2, 2, 1); //in camera coord
	vec4 LColor = vec4(1, 1, 1, 1);	
	vec4 pos = uModelMat * vPosition; // in cameera coord
	vec4 N4 = uModelMat * vec4((vNormal), 0); // in camera coord

	vec4 Amb = vec4(0.1, 0.1, 0.1, 1) * LColor;

	vec3 L3 = (LPos - pos).xyz;
	L3 = normalize(L3);
	vec3 N3 = N4.xyz;
	N3 = normalize(N3);

	float NL = max(dot(L3, N3) , 0);
	vec4 Diff = vColor * LColor * NL;

	vec3 R3 = 2 * dot(N3, L3) * N3 - L3;
	R3 = normalize(R3);
	vec3 V3 = (vec4(0, 0, 0, 1) - pos).xyz; // camera is located at (0,0,0)
	V3 = normalize(V3);

	float Shineness = 10;
	float RV = pow(max(dot(R3, V3), 0), Shineness); //cos^n (R V)

	vec4 Spec = vec4(0.2, 0.2, 0.2, 1) * LColor*RV;


	vec4 phong = Amb + Diff + Spec;


	//color = vColor;
	//color = vec4(vNormal, 1);
	//color = N4;
	color = phong;
}
