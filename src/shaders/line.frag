#version 410 core

in vec3 v_color;

out vec4 out_color;

// Axis/grid colours are flat UI colours authored in display space: no lighting is
// evaluated for them, so decoding and re-encoding would cancel out. Written as is.
void main() {
    out_color = vec4(v_color, 1.0);
}
