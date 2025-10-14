#version 330

uniform float uTime;

in vec4 color;
in vec4 position;
out vec4 fColor;

void main()
{   
	fColor = color;
	vec4 orange = vec4(1.0, 0.647, 0.0, 1.0);
	vec4 blue =vec4(0.4, 0.7, 1.0, 1.0);
	
	// 웨이브 높이에 비례하는 0.0 ~ 1.0 (또는 그 이상)의 강도(t)를 계산
	float max_height = 0.15;
	float t = abs(position.z) / max_height;

	if( position.z < 0.0) 
	{
		// 기본 색상의 비율은 줄이고 주황의 비율을 높임
		fColor = color * (1 - t) + orange * t;
	}
	else if(position.z > 0.0) 
	{
		// 기본 색상의 비율은 줄이고 파랑의 비율을 높임
		fColor = color * (1 - t) + blue * t;
	}
	else
	{
		// z가 0일 때는 원래 색상 유지
		fColor = color;
	}
}