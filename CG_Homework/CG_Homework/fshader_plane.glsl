#version 330

uniform float uTime;

in vec4 color;
in vec4 position;
out vec4 fColor;

void main()
{   
	fColor = color;
	vec4 orange = vec4(1.0, 0.5, 0.0, 1.0); // 생주황
	vec4 blue =vec4(0.0, 0.0, 1.0, 1.0);   // 생파랑
	
	
	// 웨이브 높이에 비례하는 0.0 ~ 1.0 (또는 그 이상)의 강도(t)를 계산합니다.
	// clamp를 쓰지 않으므로, t가 1.0을 넘어갈 수도 있습니다.
	float max_height = 0.15;
	float t = abs(position.z) / max_height;
	if(t > 1.0) t = 1.0; // t가 1.0을 넘지 않도록 제한 (선택 사항))

	if( position.z < 0.0) 
	{
		fColor = color + orange * t;
	}
	else if(position.z > 0.0) 
	{
		// 기본 색상(color)에 파란색을 t만큼 더합니다.
		fColor = color + blue * t;
	}
	else
	{
		// z가 0일 때는 원래 색상 유지
		fColor = color;
	}
}