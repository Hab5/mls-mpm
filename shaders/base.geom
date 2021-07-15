# version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4  view;
uniform mat4  projection;
uniform float particle_size;

in vec3 Velocity[];
out vec3 fVelocity;

void main() {
    
    for (int i = 0; i < gl_in.length(); i++) {
        vec4 center = projection * view * gl_in[i].gl_Position;
        
        fVelocity = Velocity[i];

        gl_Position = center + vec4(particle_size, particle_size, 0, 0);
        EmitVertex();
        gl_Position = center + vec4(particle_size, -particle_size, 0, 0);
        EmitVertex();
        gl_Position = center + vec4(-particle_size, particle_size, 0, 0);
        EmitVertex();
        gl_Position = center + vec4(-particle_size, -particle_size, 0, 0);
        EmitVertex();
    }
    EndPrimitive();
}