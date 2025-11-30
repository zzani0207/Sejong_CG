#version 330

in vec4 color;
in vec3 N3;
in vec3 V3;
in vec3 L3;

out vec4 fColor;

uniform vec4 uLCol;
uniform vec4 uKAmb;
uniform vec4 uKDif;
uniform vec4 uKSpc;
uniform float uShine;

void main()
{
    vec3 N = normalize(N3);
    vec3 L = normalize(L3);
    vec3 V = normalize(V3);
    vec3 R = reflect(-L, N);

    vec4 ambient = uKAmb * uLCol * color;

    float NL = max(dot(N, L), 0.0);
    vec4 diffuse = uKDif * uLCol * color * NL;

    float RV = pow(max(dot(R, V), 0.0), uShine);
    vec4 specular = uKSpc * uLCol * RV;

    fColor = ambient + diffuse + specular;
    fColor.a = 1.0;
}