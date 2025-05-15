#version 330 core
layout (triangles) in; 
layout (triangle_strip, max_vertices = 12) out; 

in VS_OUT {
    vec2 texCoord;
} gs_in[];
out vec2 texCoord;
out float timeFactor;

uniform float time;
uniform bool enableExplotion; //爆炸
uniform bool enableDither;  //發抖
uniform bool enableBurning; //燃燒

vec4 explode(vec4 position, vec3 normal, float speed){
    vec3 direction = normal * time * speed;
    return position + vec4(direction, 0.0);
}

vec3 GetNormal(){
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
}

void main() {
    if (enableExplotion) {
        vec3 normal = GetNormal();
        vec3 offset3;
        for (int i = 0; i < 3; i++) {
            float noiseX = sin(gl_in[i].gl_Position.x * 10.0 + time * 5.0) * 1;
            float noiseY = cos(gl_in[i].gl_Position.y * 10.0 + time * 5.0) * 2;
            offset3 = vec3(noiseX, noiseY, 0.0); 

            gl_Position = explode(gl_in[i].gl_Position, normal + offset3, 15*i+30); 
            texCoord = gs_in[i].texCoord;  
            EmitVertex(); 
        }
        return;
    }
    else if (enableDither) {
                for (int i = 0; i < 3; i++) {
                        vec4 center = vec4(0.0); 
                for (int i = 0; i < 3; i++) {
                    center += gl_in[i].gl_Position;
                }
                center /= 3.0; 
		        vec4 direction = normalize(gl_in[i].gl_Position - center); 
		
		        float offsetStrength = sin(time * 3.0 + float(i)) * 0.1;
		        vec4 offset4 = direction * offsetStrength;
		
		        float noiseX = sin(gl_in[i].gl_Position.x * 10.0 + time * 5.0) * 1;
		        float noiseY = cos(gl_in[i].gl_Position.y * 10.0 + time * 5.0) * 2;
		        offset4 += vec4(noiseX, noiseY, 0.0, 0.0); 
		
		        gl_Position = gl_in[i].gl_Position + offset4; 
		        texCoord = gs_in[i].texCoord;
                // if(enableBurning) timeFactor = abs(offsetStrength);
		       
		        EmitVertex();
		    }
    EndPrimitive();
        return;
    }
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        texCoord = gs_in[i].texCoord;
        EmitVertex();
    }
    EndPrimitive();
}

