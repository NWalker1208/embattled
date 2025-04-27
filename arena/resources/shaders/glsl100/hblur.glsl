#version 100

precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform int renderWidth;
uniform int renderHeight;
uniform float blurSize;

const int MAX_BLUR_SIZE = 10;

void main() {
  int intBlurSize = int(ceil(abs(blurSize)));
  float stdDev = blurSize * 0.4;

  vec4 weightedAverage = vec4(0,0,0,0);
  float totalWeight = 0.0;

  for (int i = 0; i <= 2 * MAX_BLUR_SIZE; i++) {
    if (i > 2 * intBlurSize) { break; }
    int offset = i - intBlurSize;
    float temp = float(offset) / stdDev;
    float weight = exp(-(temp * temp));

    weightedAverage += weight * texture2D(texture0, vec2(fragTexCoord.x + float(offset)/float(renderWidth), fragTexCoord.y));
    totalWeight += weight;
  }

  gl_FragColor = (weightedAverage / totalWeight) * colDiffuse * fragColor;
}
