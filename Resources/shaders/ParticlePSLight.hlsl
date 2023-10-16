#include "ParticleLight.hlsli"

Texture2D<float4> tex : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

struct PSOutput
{
    float4 col : SV_TARGET0; //�ʏ�̃����_�����O
    float4 col2 : SV_TARGET1; //�@��
    float4 highLumi : SV_TARGET2; //���P�x
};


PSOutput main(GSOutput input)
{
    PSOutput output;

	// �e�N�X�`���}�b�s���O
    float4 texcolor = tex.Sample(smp, input.uv);

	// ����x
    const float SHININESS = 20.0f;
	// ���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// �����ˌ�
    float3 ambient = float3(ambientColor * m_ambient);

	// �V�F�[�f�B���O�ɂ��F
    float4 shadecolor = { 0, 0, 0, m_alpha };
    
    //�X�y�L�����}�b�v�̐F
    float4 specularMapCol = { 0, 0, 0, 0 };
    
    //���[���h�̖@��
    float3 wNormal = input.normal;
    
    float3 lightNormal = wNormal;
        
	//���s����
    for (int i = 0; i < S_DIRLIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(dirLights[i].lightv, lightNormal);
			// ���ˌ��x�N�g��
            float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * lightNormal);
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);

			// �g�U���ˌ�
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// ���ʔ��ˌ�
            specular = specular * m_specular * specularColor;

			// �S�ĉ��Z����
            shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
        }
    }

	//�_����
    for (int i = 0; i < S_POINTLIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
			//���C�g�ւ̃x�N�g��
            float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			//�x�N�g���̒���
            float d = length(lightv);
			//���K�����A�P�ʃx�N�g���ɂ���
            lightv = normalize(lightv);
			//���������W��
            float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d +
				pointLights[i].lightatten.z * d * d);
                       
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, lightNormal);
            
			// ���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * lightNormal);
                        
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);

			// �g�U���ˌ�
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// ���ʔ��ˌ�
            specular = specular * m_specular * specularColor;
            
			// �S�ĉ��Z����
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
        }
    }
    

	//�X�|�b�g���C�g
    for (int i = 0; i < S_SPOTLIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
			//���C�g�ւ̕����x�N�g��
            float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
            float d = length(lightv);
            lightv = normalize(lightv);
			//���������W��
            float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y
				* d + spotLights[i].lightatten.z * d * d));
			//�p�x����
            float cos = dot(lightv, spotLights[i].lightv);
			//�����J�n�p�x����A�����I���p�x�ɂ����Č���
			//�����J�n�p�x�̓����́A1�{ �����I���p�x�̊O����0�{�̋P�x
            float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y,
				spotLights[i].lightfactoranglecos.x, cos);
			//�p�x��������Z
            atten *= angleatten;
			//���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, lightNormal);
			//���ˌ��x�N�g���@
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * lightNormal);
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);

			// �g�U���ˌ�
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// ���ʔ��ˌ�
            specular = specular * m_specular * specularColor;
			//�S�ĉ��Z����
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
        }
    }

	//�ۉe
    for (int i = 0; i < S_CIRCLESHADOW_NUM; i++)
    {
        if (circleShadows[i].active)
        {
			//���C�g�ւ̕����x�N�g��
            float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;
			//���e�����ł̋���
            float d = dot(casterv, circleShadows[i].dir);
			//���������W��
            float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y
				* d + circleShadows[i].atten.z * d * d));
			//�������}�C�i�X�Ȃ�0�ɂ���
            atten *= step(0, d);
			//���z���C�g�̍��W
            float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir
				* circleShadows[i].distanceCasterLight;
			//�I�u�W�F�N�g�\�ʂ��烉�C�g�ւ̃x�N�g���i�P�ʃx�N�g���j
            float3 lightv = normalize(lightpos - input.worldpos.xyz);
			//�p�x����
            float cos = dot(lightv, circleShadows[i].dir);
			//�����J�n�p�x����A�����I���p�x�ɂ����Č���
			//�����J�n�p�x�̓����́A1�{ �����I���p�x�̊O����0�{�̋P�x
            float angleatten = smoothstep(circleShadows[i].factorAngleCos.y,
				circleShadows[i].factorAngleCos.x, cos);
			//�p�x��������Z
            atten *= angleatten;

			//�S�Č��Z����
            shadecolor.rgb -= atten;
        }
    }

	//����
    shadecolor.rgb += ambient;

	// �V�F�[�f�B���O�ɂ��F�ŕ`��
    float4 RGBA = (shadecolor * texcolor * input.color);
    
	//�ꖇ�ڂ̈��
    output.col = RGBA;

    return output;
}