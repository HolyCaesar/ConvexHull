#ifndef Quaternion_h
#define Quaternion_h

#include <d3dx11.h>
#include <d3d11.h>
#include <D3DX10math.h>

struct _Quaternion
{
	float q[ 4 ];
}; 


class Quaternion
{
public:
	Quaternion(){};
	~Quaternion(){};

public:
	static _Quaternion g_SpongeRotation;

	inline static D3DXMATRIX QuaternionToMatrix( const _Quaternion& quat )
	{
		D3DXMATRIX out;
		float yy2 = 2.0f * quat.q[1] * quat.q[1];
		float xy2 = 2.0f * quat.q[0] * quat.q[1];
		float xz2 = 2.0f * quat.q[0] * quat.q[2];
		float yz2 = 2.0f * quat.q[1] * quat.q[2];
		float zz2 = 2.0f * quat.q[2] * quat.q[2];
		float wz2 = 2.0f * quat.q[3] * quat.q[2];
		float wy2 = 2.0f * quat.q[3] * quat.q[1];
		float wx2 = 2.0f * quat.q[3] * quat.q[0];
		float xx2 = 2.0f * quat.q[0] * quat.q[0];
		out.m[0][0] = - yy2 - zz2 + 1.0f;
		out.m[0][1] = xy2 + wz2;
		out.m[0][2] = xz2 - wy2;
		out.m[0][3] = 0;
		out.m[1][0] = xy2 - wz2;
		out.m[1][1] = - xx2 - zz2 + 1.0f;
		out.m[1][2] = yz2 + wx2;
		out.m[1][3] = 0;
		out.m[2][0] = xz2 + wy2;
		out.m[2][1] = yz2 - wx2;
		out.m[2][2] = - xx2 - yy2 + 1.0f;
		out.m[2][3] = 0;
		out.m[3][0] = out.m[3][1] = out.m[3][2] = 0;
		out.m[3][3] = 1;

		return out;
	}
};


#endif