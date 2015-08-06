#version 330
#extension GL_ARB_gpu_shader5 : enable
#define FXAA_PC 1
#define FXAA_GLSL_130 1
#define FXAA_QUALITY_UNDERSCORE_PRESET 29
#define FXAA_GATHER4_ALPHA 0
//#define FXAA_GREEN_AS_LUMA 1
#include "FXAA3_11.h"

uniform sampler2D tex;
uniform vec2 invScreenSize;

noperspective in vec2 texCoord;
out vec4 color;


#ifndef FXAA_GLSL_120
// Fallback if "FXAA3_11.h" doesn't exist (This macro is defined to 0 within the
// header, but is defined nonetheless.)
vec4 FxaaPixelShader(vec2 pos,
                     vec4 fxaaConsolePosPos,
                     sampler2D tex,
                     sampler2D fxaaConsole360TexExpBiasNegOne,
                     sampler2D fxaaConsole360TexExpBiasNegTwo,
                     vec2 fxaaQualityRcpFrame,
                     vec4 fxaaConsoleRcpFrameOpt,
                     vec4 fxaaConsoleRcpFrameOpt2,
                     vec4 fxaaConsole360RcpFrameOpt2,
                     float fxaaQualitySubpix,
                     float fxaaQualityEdgeThreshold,
                     float fxaaQualityEdgeThresholdMin,
                     float fxaaConsoleEdgeSharpness,
                     float fxaaConsoleEdgeThreshold,
                     float fxaaConsoleEdgeThresholdMin,
                     vec4 fxaaConsole360ConstDir)
{
    return texture(tex, pos);
}
#endif


void main()
{
    color = FxaaPixelShader(texCoord,       // pos
                            vec4(0,0,0,0),
                            tex,            // tex
                            tex, tex,
                            invScreenSize,  // fxaaQualityRcpFrame
                            vec4(0,0,0,0),
                            vec4(0,0,0,0),
                            vec4(0,0,0,0),
                            1.0,            // fxaaQualitySubpix
                            0.063,          // fxaaQualityEdgeThreshold
                            0.0312,         // fxaaQualityEdgeThresholdMin
                            0.0, 0.0, 0.0,
                            vec4(0,0,0,0));

    color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}
