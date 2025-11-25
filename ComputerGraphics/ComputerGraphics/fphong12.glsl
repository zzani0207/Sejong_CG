#version 330

in  vec4 color;
in vec4 pos;
in vec3 normal;

out vec4 fColor;

void
main()
{
	vec4 LPos = vec4(2, 2, 2, 1);
	vec4 LColor = vec4(1, 1, 1, 1);
	vec4 ka = vec4(0.1, 0.1, 0.1, 1);
	vec4 kd = color;
	vec4 ks = vec4(0.3, 0.3, 0.3, 1);
	float sh = 80;

	vec4 Amb = ka * LColor;

	vec3 L3 = normalize((LPos - pos).xyz);
	vec3 N3 = normalize(normal);
	float NL = max(dot(L3, N3), 0);

	if (NL > 0.5) NL = 1;
	else if (NL > 0.3) NL = 0.5;
	else NL = 0.2;

	vec4 Dif = kd * LColor * NL;

	vec3 R3 = normalize(2 * dot(N3, L3) * N3 - L3);
	vec3 V3 = normalize((vec4(0, 0, 0, 1) - pos).xyz);
	float RV = pow(max(dot(R3, V3), 0), sh);

	if (RV > 0.5) RV = 1;
	else RV = 0;

	vec4 Spec = ks * LColor * RV;

	fColor = Amb + Dif + Spec;

	if (abs(dot(V3, N3)) < 0.1)
	{
		fColor = vec4(0, 0, 0, 1);
	}

}
