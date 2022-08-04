//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

// Custom effects using pixel shaders should use HLSL helper functions defined in
// d2d1effecthelpers.hlsli to make use of effect shader linking.
#define D2D_INPUT_COUNT 0           // The pixel shader is a source and does not take inputs.
#define D2D_REQUIRES_SCENE_POSITION // The pixel shader requires the SCENE_POSITION input.

// Note that the custom build step must provide the correct path to find d2d1effecthelpers.hlsli when calling fxc.exe.
#include "d2d1effecthelpers.hlsli"

cbuffer constants : register(b0)
{
    float dpi : packoffset(c0.x);
    float2 outputSize : packoffset(c0.y);
};

D2D_PS_ENTRY(main)
{
	float2 posScene = D2DGetScenePosition().xy;
    float2 pos = float2(posScene.x / outputSize.x, posScene.y / outputSize.y);

	float c = pos.x;				
	c = pow(c,0.45454);				
	c = c*0.5;						

	if (pos.y < 0.25f)
	{
		c = trunc(c * 256.0f) / 256.0f;			//8 bit per color
	}
	else if (pos.y > 0.25 && pos.y < 0.5f)
	{
		c = trunc(c * 256.0f) / 256.0f;			//8 bit per color //c = trunc(c * 1024.0f) / 1024.0f;			// 10 bit per color
	}
	else if (pos.y > 0.5f && pos.y < 0.75f)
	{
		c = trunc(c * 1024.0f) / 1024.0f;			// 10 bit per color /c = trunc(c * 256.0f) / 256.0f;			//8 bit per color
	}
	else if (pos.y > 0.75f)
	{
		c = trunc(c * 1024.0f) / 1024.0f;			// 10 bit per color
	}

	c = pow(c,2.2f);

	float4 color_temp = { c, c, c, 1.0f };
	//color_temp = { c, c, c, 1.0f };
	/*if (pos.y < 0.5f) {
		float4  color_temp = { 0, c, 0, 1.0f };
	}
	else {
		float4  color_temp = { c, c, c, 1.0f };
	}*/
	
	float4 color = color_temp;
    return color;
}