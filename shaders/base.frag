# version 460 core

out vec4 FragColor;
in vec3 fVelocity;

void main() {
    FragColor = vec4(mix(vec3(0.0588, 0.3137, 0.8667), abs(fVelocity), 0.4), 1.0);
}