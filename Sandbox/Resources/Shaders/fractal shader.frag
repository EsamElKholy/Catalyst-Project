#version 330

in mat4 MVP;
in vec2 coord;
out vec4 OutCol;

uniform vec3 color;

void main()
{
    float   real  = coord.x/200.0;
    float   imag  = coord.y/200.0;
    float   Creal = real;  
    float   Cimag = imag;  

    float r2 = 0.0;
	float iter;
    for ( iter = 0.0; iter < 500 && r2 < 4.0; ++iter)
    {
        float tempreal = real;

        real = (tempreal * tempreal) - (imag * imag) + Creal;
        imag = 2.0 * tempreal * imag + Cimag;
        r2   = (real * real) + (imag * imag);
    }

    vec3 col;

    if (r2 < 4.0)
        col = vec3(0.0f, 0.0f, 0.0f);
    else
        col = vec3(1.0f, 1.0f, 1.0f);

	col = col + color;


    OutCol = vec4(col, 1.0);
    OutCol = vec4(iter/5000.0,iter/5000.0,iter/5000.0,1);
}