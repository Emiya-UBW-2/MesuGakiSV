// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float2 TexCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 LPPosition : TEXCOORD1; // ���C�g����݂����W( x��y�̓��C�g�̎ˉe���W�Az�̓r���[���W )
	float4 LPPosition2 : TEXCOORD2; // ���C�g����݂����W( x��y�̓��C�g�̎ˉe���W�Az�̓r���[���W )
    float4 Position : SV_POSITION; // ���W( �v���W�F�N�V������� )
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT
{
    float4 Color0 : SV_TARGET0; // �F
};

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^2
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3) {
	float4	g_param;
}

SamplerState g_DepthMapSampler : register(s1); // �[�x�o�b�t�@�e�N�X�`��
Texture2D g_DepthMapTexture : register(t1); // �[�x�o�b�t�@�e�N�X�`��

SamplerState g_DepthMapSampler2 : register(s2); // �[�x�o�b�t�@�e�N�X�`��
Texture2D g_DepthMapTexture2 : register(t2); // �[�x�o�b�t�@�e�N�X�`��


// main�֐�
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;

	float comp;
	int total;

	PSOutput.Color0.rgb = 0.f;
	PSOutput.Color0.a = 1.f;


	// �[�x�o�b�t�@�e�N�X�`������[�x���擾( +�␳�l )
	{
        float LightDepth = PSInput.LPPosition.z;
		comp = 0;
        if ((int) g_param.x == 3)
        {
            const int xs = 5;
    		[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                        float TextureDepth2 = g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.LPPosition.xy, int2(x, y)).r;
                        if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + 1.f))
                        {
                            comp++;
                        }
                }
            }
            PSOutput.Color0.r = comp / ((xs * 2 + 1) * (xs * 2 + 1));
        }
        if ((int) g_param.x == 2)
        {
            const int xs = 3;
    		[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                        float TextureDepth2 = g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.LPPosition.xy, int2(x, y)).r;
                        if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + 1.f))
                        {
                            comp++;
                        }
                        total++;
                }
            }
            PSOutput.Color0.r = comp / ((xs * 2 + 1) * (xs * 2 + 1));
        }
        if ((int) g_param.x == 1)
        {
            const int xs = 1;
    		[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                        float TextureDepth2 = g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.LPPosition.xy, int2(x, y)).r;
                        if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + 1.f))
                        {
                            comp++;
                        }
                }
            }
            PSOutput.Color0.r = comp / ((xs * 2 + 1) * (xs * 2 + 1));
        }
    }

	//*
	// �[�x�o�b�t�@�e�N�X�`������[�x���擾( +�␳�l )
	{
        if ((int) g_param.x == 3)
        {
            float LightDepth = PSInput.LPPosition2.z;
            comp = 0;
            total = 0;
            const int xs = 5;
	    	[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    if (abs(x) < (int) g_param.x && abs(y) < (int) g_param.x)
                    {
                        float TextureDepth2 = g_DepthMapTexture2.Sample(g_DepthMapSampler2, PSInput.LPPosition2.xy, int2(x, y)).r;
                        if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + 1.5f))
                        {
                            comp++;
                        }
                        total++;
                    }
                }
            }
            PSOutput.Color0.r = max(PSOutput.Color0.r, comp / total);
        }
        if ((int) g_param.x == 2)
        {
            float LightDepth = PSInput.LPPosition2.z;
            comp = 0;
            total = 0;
            const int xs = 3;
	    	[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    if (abs(x) < (int) g_param.x && abs(y) < (int) g_param.x)
                    {
                        float TextureDepth2 = g_DepthMapTexture2.Sample(g_DepthMapSampler2, PSInput.LPPosition2.xy, int2(x, y)).r;
                        if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + 1.5f))
                        {
                            comp++;
                        }
                        total++;
                    }
                }
            }
            PSOutput.Color0.r = max(PSOutput.Color0.r, comp / total);
        }
        if ((int) g_param.x == 1)
        {
            float LightDepth = PSInput.LPPosition2.z;
            comp = 0;
            total = 0;
            const int xs = 1;
	    	[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    if (abs(x) < (int) g_param.x && abs(y) < (int) g_param.x)
                    {
                        float TextureDepth2 = g_DepthMapTexture2.Sample(g_DepthMapSampler2, PSInput.LPPosition2.xy, int2(x, y)).r;
                        if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + 1.5f))
                        {
                            comp++;
                        }
                        total++;
                    }
                }
            }
            PSOutput.Color0.r = max(PSOutput.Color0.r, comp / total);
        }
    }

    if ((int) g_param.x > 2 && PSOutput.Color0.r < 128.f / 255.f)
    {
        PSOutput.Color0.r = 0.f;
    }
	//*/
	
    PSOutput.Color0.r = lerp(PSOutput.Color0.r, 0.f, saturate(PSInput.Position.w / g_param.y));

	// �o�̓p�����[�^��Ԃ�
    return PSOutput;
}
