#version 330 core
out vec4 FragColor;
uniform bool enableFlash; 
uniform bool enableBurning;
uniform bool useTexture;
uniform vec3 heliColor;
uniform float time;  

in vec2 texCoord; 
in float timeFactor;
uniform sampler2D ourTexture; 

void main() {
    vec4 texColor;
    if(useTexture){
        texColor = texture(ourTexture, texCoord);
    }
    else texColor = vec4(heliColor/255.0, 1.0);
    

    if (enableFlash) {
        float intensity = abs(sin(time * 10.0)); 
        vec3 flashColor = vec3(1.0);
        texColor.rgb = mix(texColor.rgb, flashColor, intensity);
    }
    if (enableBurning) {
        float noise = sin(texCoord.y * 10.0 + time * 3.0) * 0.5 + 0.5; 
        float gradient = texCoord.y;

        vec3 flameColor = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.5, 0.0), gradient); 
        flameColor = mix(flameColor, vec3(1.0, 0.0, 0.0), noise); 
        flameColor = mix(flameColor, vec3(1.0, 1.0, 0.0), timeFactor); 

        vec3 finalColor = mix(texColor.rgb, flameColor, 0.8); 
        FragColor = vec4(finalColor, texColor.a);
    }
    else FragColor = texColor;

}