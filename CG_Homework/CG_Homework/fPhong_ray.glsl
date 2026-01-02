#version 330

in vec3 N3; 
in vec3 L3; 
in vec3 V3; 
in vec3 wV;
in vec3 wP;
in vec3 wN;

out vec4 fColor;

struct Material {
	vec4  k_d;	// diffuse coefficient
	vec4  k_s;	// specular coefficient
	float n;	// specular exponent
};

struct Sphere {
	vec4     center;
	float    radius;
	Material mtl;
};

struct Ray {
	vec3 pos;
	vec3 dir;
};

struct HitInfo {
	float    t;
	vec4     position;
	vec3     normal;
	Material mtl;
};

uniform mat4 uModelMat; 
uniform mat4 uViewMat; 
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uLIntensity;
uniform vec4 uAmb; 
uniform vec4 uDif; 
uniform vec4 uSpc; 
uniform float uShininess; 
uniform samplerCube uCube;
uniform vec4 uEPos;
uniform int uNumSphere;
uniform Sphere uSpheres[20];
uniform int uBounceLimit;
uniform int uDrawingMode;



bool IntersectRay( inout HitInfo hit, Ray ray );

// Shades the given point and returns the computed color.
vec4 Shade( Material mtl, vec4 position, vec3 normal, vec3 view )
{
	vec4 color = vec4(0,0,0,1);
	
	// 광원 방향 벡터
	vec3 lightDir = normalize(uLPos.xyz - position.xyz);
	float distToLight = length(uLPos.xyz - position.xyz);

	// Ambient
	vec4 ambient = uAmb * mtl.k_d; 
	color += ambient;

	// TO-DO: Check for shadows
	bool inShadow = false;
	Ray shadowRay;
	// Self-shadowing 방지를 위해 표면에서 아주 살짝 띄움
	shadowRay.pos = position.xyz + normal * 0.001; 
	shadowRay.dir = lightDir;
	
	HitInfo shadowHit;
	// 광원까지 가는 길에 물체가 있는지 확인
	if (IntersectRay(shadowHit, shadowRay)) {
		// 충돌한 물체가 광원보다 가까이 있으면 그림자
		if (shadowHit.t < distToLight) {
			inShadow = true;
		}
	}

	// TO-DO: If not shadowed, perform shading using the diffuse color only (and Specular)
	if (!inShadow) {
		// Diffuse
		float diff = max(dot(normal, lightDir), 0.0);
		vec4 diffuse = mtl.k_d * uLIntensity * diff;
		
		// Specular
		vec3 halfDir = normalize(lightDir + view);
		float spec = pow(max(dot(normal, halfDir), 0.0), mtl.n);
		vec4 specular = mtl.k_s * uLIntensity * spec;

		color += diffuse + specular;
	}

	return color;
}

// Intersects the given ray with all spheres in the scene
// and updates the given HitInfo using the information of the sphere
// that first intersects with the ray.
// Returns true if an intersection is found.
bool IntersectRay( inout HitInfo hit, Ray ray )
{
	hit.t = 1e30;
	bool foundHit = false;
	
	for ( int i=0; i<uNumSphere; ++i ) {
		// TO-DO: Test for ray-sphere intersection
		vec3 oc = ray.pos - uSpheres[i].center.xyz;
		float b = dot(ray.dir, oc);
		float c = dot(oc, oc) - uSpheres[i].radius * uSpheres[i].radius;
		
		// 판별식 계산: b^2 - c
		float discriminant = b * b - c;
		
		if (discriminant > 0.0) {
			float sqrtDisc = sqrt(discriminant);
			float t = -b - sqrtDisc; // 더 가까운 교차점
			
			// t가 0보다 크고(광선 앞쪽), 현재 기록된 hit.t보다 가까우면 갱신
			if (t > 0.001 && t < hit.t) {
				hit.t = t;
				hit.position = vec4(ray.pos + t * ray.dir, 1.0);
				hit.normal = normalize(hit.position.xyz - uSpheres[i].center.xyz);
				hit.mtl = uSpheres[i].mtl;
				foundHit = true;
			}
		}
	}
	return foundHit;
}

// Given a ray, returns the shaded color where the ray intersects a sphere.
// If the ray does not hit a sphere, returns the environment color.
vec4 RayTracer( Ray ray )
{
	HitInfo hit;
	if ( IntersectRay( hit, ray ) ) {
		vec3 view = normalize( -ray.dir );
		vec4 clr = Shade( hit.mtl, hit.position, hit.normal, view );

		// Compute reflections
		vec4 k_s = hit.mtl.k_s; // 현재까지 누적된 반사 계수
		
		
		for ( int bounce=0; bounce<uBounceLimit; bounce++ ) {

			if ( hit.mtl.k_s.r + hit.mtl.k_s.g + hit.mtl.k_s.b <= 0.0 ) break;
			
			Ray r;	// this is the reflection ray
			HitInfo h;	// reflection hit info
						
			// TO-DO: Initialize the reflection ray
			// 반사 벡터 계산: reflect
			// 입사벡터는 이전 ray.dir
			vec3 reflectDir = reflect(ray.dir, hit.normal);
			
			r.pos = hit.position.xyz + hit.normal * 0.001; // bias 적용
			r.dir = normalize(reflectDir);
			
			if ( IntersectRay( h, r ) ) {
				// TO-DO: Hit found, so shade the hit point
				vec3 newView = -r.dir;
				vec4 localColor = Shade( h.mtl, h.position, h.normal, newView );
				
				// 색상 누적: (현재까지의 반사율 * 쉐이딩 결과)
				clr += k_s * localColor;
				
				// TO-DO: Update the loop variables for tracing the next reflection ray
				// 다음 반사를 위해 반사율 감쇠
				k_s *= h.mtl.k_s;
				
				// 다음 루프에서 사용할 기준점을 현재 충돌 지점으로 갱신
				hit = h;
				ray = r; // 다음 반사각 계산을 위해 입사 광선도 갱신
				
			} else {
				// The reflection ray did not intersect with anything,
				// so we are using the environment color
				clr += k_s * texture(uCube, vec3(1,-1,1)*r.dir);
				break;	// no more reflections
			}
		}

		return clr;	// return the accumulated color, including the reflections
	} else {
		return texture(uCube, vec3(1,-1,1)*ray.dir);	// return the environment color
	}
}

void main()
{
	if(uDrawingMode == 0) 
	{
		vec3 N = normalize(N3); 
		vec3 L = normalize(L3); 
		vec3 V = normalize(V3); 
		vec3 H = normalize(V+L); 

		float NL = max(dot(N, L), 0); 
		float VR = pow(max(dot(H, N), 0), uShininess); 

		fColor = uAmb + uLIntensity*uDif*NL + uLIntensity*uSpc*VR; 
		fColor.w = 1; 

		vec3 viewDir = wP - wV;
		vec3 dir = reflect(viewDir, wN);

		fColor += uSpc*texture(uCube, vec3(1,-1,1)*dir);
	}
	else if(uDrawingMode == 1)
	{
		Ray r;
		r.pos = wV;
		r.dir = normalize(wP - wV);
		fColor = RayTracer (r);
	}
}
