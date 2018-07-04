
attribute vec3 a_Position;
attribute vec3 a_Color;
attribute mat4 a_Transform;

uniform mat4 u_ComposedMatrix;

varying vec4 v_Color;

void main()
{
	gl_Position = u_ComposedMatrix * a_Transform * vec4(a_Position, 1.0);
	v_Color = vec4(a_Color, 1.0);
}
