#version 330

// C++에서 전달받는 uniform 변수 (모든 정점이 동일한 값을 사용)
uniform float uTime;
uniform int uIsWave;

// C++에서 전달받는 정점별 attribute 변수
in vec4 vPosition; // 정점의 초기 위치 (로컬 좌표)
in vec4 vColor;    // 정점의 초기 색상

// 프래그먼트 셰이더로 전달할 out 변수
out vec4 color;
out vec4 position;

void main()
{
	vec4 wavePos = vPosition; // 최종 위치를 저장할 변수 (초기값은 로컬 좌표)

	if(uIsWave == 1)
	{
		// 1. 평면의 중심(0,0)에서 현재 정점까지의 거리를 계산합니다.
		float dist = length(vPosition.xy);
		
		// 2. 거리와 시간을 이용해 Z축 방향의 웨이브 높이를 계산합니다.
		//    시간이 지남에 따라 웨이브가 중심에서 바깥으로 퍼져나갑니다.
		float waveHeight = -0.15 * sin(dist * 15.0 - uTime * 5.0);

		// 3. 감쇠 계수(Attenuation Factor)를 계산합니다.
		// 평면의 크기가 1x1이므로, 중심에서 가장 먼 모서리까지의 거리는 약 0.707입니다.
		// smoothstep(start, end, value)는 value가 start에서 end로 변할 때 0에서 1로 부드럽게 변하는 값을 반환합니다.
		// 여기서는 거리가 0.3에서 0.7로 멀어질수록 감쇠 효과를 적용합니다.
		float attenuation = 1.0 - smoothstep(0.3, 0.7, dist);

		// 4. 계산된 높이에 감쇠 계수를 곱하여 최종 높이를 결정합니다.
		wavePos.z += waveHeight * attenuation;
	}

	// 1. 앞으로 기울이기 위한 X축 회전 행렬 (고정값)
	float tiltAngle = -50.0f; // 기울임 각도 (도 단위)
	float tiltRad = tiltAngle / 180.0f * 3.141592f;
	mat4 tiltX = mat4(1.0);
	tiltX[1][1] = cos(tiltRad);
	tiltX[2][1] = -sin(tiltRad);
	tiltX[1][2] = sin(tiltRad);
	tiltX[2][2] = cos(tiltRad);

	// 2. 시간에 따라 Z축을 중심으로 회전하는 행렬
	float angleZ = uTime * 30.0f;
	float radZ = angleZ / 180.0f * 3.141592f;
	mat4 rotZ = mat4(1.0);
	rotZ[0][0] = cos(radZ);
	rotZ[1][0] = -sin(radZ);
	rotZ[0][1] = sin(radZ);
	rotZ[1][1] = cos(radZ);

	// 3. 변환 적용: Z축 회전 순서로 적용-> 기울임 -> 웨이브
	vec4 pos = rotZ *  wavePos * tiltX;

	// 4. 프래그먼트 셰이더로 데이터 전달
	color = vColor;           // 입력받은 색상을 그대로 전달
	position = wavePos;     // 변환되지 않은 '원본 위치'를 전달
	//position = pos;		  // 주석 처리됨: 만약 활성화하면 '변환된 위치'를 전달

	// 5. 정점의 최종 클립 공간 위치 결정 (필수)
	gl_Position = pos;
}