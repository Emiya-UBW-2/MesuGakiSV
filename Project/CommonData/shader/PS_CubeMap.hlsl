// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float4 Position : SV_POSITION; // ���W
    float4 DiffuseColor : COLOR0; // �f�B�t���[�Y�J���[
    float4 SpecularColor : COLOR1; // �X�y�L�����J���[
    float2 TextureCoord0 : TEXCOORD0; // �e�N�X�`�����W�O
    float2 TextureCoord1 : TEXCOORD1; // �e�N�X�`�����W�P
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT {
	float4 color0           : SV_TARGET0;	// �F
};

// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE {
	float4		FactorColor;			// �A���t�@�l��

	float		MulAlphaColor;			// �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
	float		AlphaTestRef;			// �A���t�@�e�X�g�Ŏg�p�����r�l
	float2		Padding1;

	int		    AlphaTestCmpMode;		// �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
	int3		Padding2;

	float4		IgnoreTextureColor;	// �e�N�X�`���J���[���������p�J���[
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE				: register(b1) {
	DX_D3D11_PS_CONST_BUFFER_BASE		g_Base;
};


// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3) {
	float4 caminfo;
}

cbuffer cbLIGHTCAMERA_MATRIX : register(b4)
{
    matrix g_LightViewMatrix; // ���C�g�̃��[���h�@���@�r���[�s��
    matrix g_LightProjectionMatrix; // ���C�g�̃r���[�@�@���@�ˉe�s��
};


//�e�N�X�`��
SamplerState g_Register0MapSampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_Register0MapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

SamplerState g_Register1MapSampler : register(s1); // �@���}�b�v�T���v��
Texture2D g_Register1MapTexture : register(t1); // �@���}�b�v�e�N�X�`��

SamplerState g_Register2MapSampler : register(s2); // �[�x�}�b�v�T���v��
Texture2D g_Register2MapTexture : register(t2); // �[�x�}�b�v�e�N�X�`��


SamplerState dynamicCubeMapSampler : register(s3);
TextureCube dynamicCubeMapTexture : register(t3);

//�֐�
float4 GetTexColor1(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register1MapTexture.Sample(g_Register1MapSampler, texCoord, offset);
}
float4 GetTexColor2(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register2MapTexture.Sample(g_Register2MapSampler, texCoord, offset);
}

float3 DisptoProjNorm(float2 screenUV) {
    screenUV /= 0.5f;
    screenUV.x = screenUV.x - 1.f;
    screenUV.y = 1.f - screenUV.y;

	float3 position;
    position.x = screenUV.x * caminfo.z * dispsize.x / dispsize.y;
    position.y = screenUV.y * caminfo.z;
	position.z = 1.f;

	return position;
}

PS_OUTPUT main(PS_INPUT PSInput) {
	//�߂�l
	PS_OUTPUT PSOutput;
	//���˂��ǂꂾ�������邩
    float uvFactor = 2.0 * length(PSInput.TextureCoord0 - float2(0.5, 0.5));
    uvFactor *= uvFactor;
    uvFactor *= uvFactor;
    float edge = max(0.0, 1.0 - uvFactor);
    float Per = GetTexColor2(PSInput.TextureCoord0).g * edge;
	//�m�[�}�����W�擾
    float3 normal = GetTexColor1(PSInput.TextureCoord0).xyz * 2.f - 1.f;
    
	//����
    float4 lWorldPosition;
	//�L���[�u�}�b�v����̔���
    lWorldPosition.xyz = DisptoProjNorm(PSInput.TextureCoord0);
    lWorldPosition.w = 0.f;
	
	// ���[���h���W���ˉe���W�ɕϊ�
    float4 LPPosition1 = mul(g_LightViewMatrix, lWorldPosition);

    lWorldPosition.xyz = normal;
    lWorldPosition.w = 0.f;


    float4 LPPosition2 = mul(g_LightViewMatrix, lWorldPosition);
    float3 LPPosition3 = reflect(LPPosition1.xyz, LPPosition2.xyz);
    
    float4 RefColor = dynamicCubeMapTexture.Sample(dynamicCubeMapSampler, LPPosition3);

    PSOutput.color0 = lerp(float4(0.f, 0.f, 0.f, 0.f), RefColor, Per);
	return PSOutput;
}
