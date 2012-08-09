//Anaglyphic Shader. Modified From: http://r3dux.org/2011/05/anaglyphic-3d-in-glsl/
//For Red/Cyan Glasses.
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
  tmpR.g = 1.0;
  tmpR.b = 1.0;

  gl_FragColor=vec4(tmpR.rgb * tmpL.rgb, tmp.a);
}

