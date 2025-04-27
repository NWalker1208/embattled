#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform int renderWidth;
uniform int renderHeight;
uniform float blurSize;

void main() {
  int intBlurSize = int(ceil(abs(blurSize)));
  float stdDev = blurSize * 0.4;

  vec4 weightedAverage = vec4(0,0,0,0);
  float totalWeight = 0.0;

  for (int offset = -intBlurSize; offset <= intBlurSize; offset++) {
    float temp = offset / stdDev;
    float weight = exp(-(temp * temp));

    weightedAverage += weight * texture2D(texture0, vec2(fragTexCoord.x + float(offset)/renderWidth, fragTexCoord.y));
    totalWeight += weight;
  }

  finalColor = (weightedAverage / totalWeight) * colDiffuse * fragColor;
}
