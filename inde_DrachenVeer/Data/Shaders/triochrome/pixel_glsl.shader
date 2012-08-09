//Triochrome Shader. Modified From: http://r3dux.org/2011/05/anaglyphic-3d-in-glsl/
//For Green/Magenta Glasses.
#ifndef PREC
#define PREC
#endif //PREC

uniform bool use_tex;
uniform sampler2D tex;
uniform bool use_tex_matrix;
uniform PREC mat4 tex_matrix;
varying PREC vec4 varying_color;
varying PREC vec2 varying_texcoord;
uniform float offset;

void main(){
  PREC vec4 tmpL=varying_color;
  PREC vec4 tmpR=varying_color;
  PREC vec4 tmp=varying_color;
  PREC vec4 coord;
  PREC vec4 coordL;
  PREC vec4 coordR;
  PREC mat3 DuboisL;
  PREC mat3 DuboisR;
  coordL = vec4(varying_texcoord.x - offset, varying_texcoord.y, 1.0, 1.0);
  coordR = vec4(varying_texcoord.x + offset, varying_texcoord.y, 1.0, 1.0);
  coord = vec4(varying_texcoord, 1.0, 1.0);
  if (use_tex_matrix) {
     PREC vec4 sample;

     coordL = coordL * tex_matrix; 
     sample = texture2D(tex, coordL.xy);
     tmpL *= sample;

     coordR = coordR * tex_matrix; 
     sample = texture2D(tex, coordR.xy);
     tmpR *= sample;

     coord = coord * tex_matrix; 
     sample = texture2D(tex, coord.xy);
     tmp *= sample;
  }
  else if (use_tex) {
     PREC vec4 sample;

     sample = texture2D(tex, coordL.xy);
     tmpL *= sample;

     sample = texture2D(tex, coordR.xy);
     tmpR *= sample;

     sample = texture2D(tex, coord.xy);
     tmp *= sample;
  }

  if(tmp.r == tmp.b && tmp.g == 0.0) {
      gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
      return;
  } 

  tmpL.r = 1.0;
  tmpL.g = tmp.g;
  tmpL.b = 1.0;
  tmpR.g = 1.0;

/*
  //DuboisL = mat3(vec3(0.4561,-0.0400822,-0.0152161),vec3(0.500484,-0.0378246,-0.0205971),vec3(0.176381,-0.0157589,-0.00546856));
  //DuboisR = mat3(vec3(-0.0434706,0.378476,-0.0721527),vec3(-0.0879388,0.73364,-0.112961),vec3(-0.00155529,-0.0184503,1.2264));
  DuboisL = mat3(vec3(-0.062, -0.158, -0.039),
		vec3(0.284, 0.668, 0.143),
		vec3(-0.015,-0.027,0.021));
  DuboisR = mat3(vec3(0.529,0.705,0.024),
		vec3(-0.016,-0.015,-0.065),
		vec3(0.009,0.075,0.937));
  tmpL.rgb = clamp(DuboisL * tmpL.rgb,0.0,1.0);
  tmpR.rgb = clamp(DuboisR * tmpR.rgb,0.0,1.0);
  //tmp.rgb = clamp(0.001953125 + tmpL.rgb + tmpR.rgb,0.0,1.0);
  tmp.rgb = clamp(pow(tmpL.rgb + tmpR.rgb,1.15),0.0,1.0);

  gl_FragColor=vec4(tmp.rgb, tmp.a);
*/
  gl_FragColor=vec4(tmpL.rgb * tmpR.rgb, tmp.a);
}

