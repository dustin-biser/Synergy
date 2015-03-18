// PerlinNoise.fs
#version 410

flat in float layer;

out float color;

uniform usamplerBuffer hashTable;

//---------------------------------------------------------------------------------------
double fade(double t) {
	// 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

//---------------------------------------------------------------------------------------
// Converts the least significant 4 bits of hash to one of
// into 12 gradient directions, which are the directions from center of the
// unit cube to each of it's edges:
//
// (1,1,0),(-1,1,0),(1,-1,0),(-1,-1,0),
// (1,0,1),(-1,0,1),(1,0,-1),(-1,0,-1),
// (0,1,1),(0,-1,1),(0,1,-1),(0,-1,-1)
//
// Then returns the dot product of one of these directions with the
// input vector (x,y,z).
double grad(int hash, double x, double y, double z) {
    switch(hash & 0xF)
    {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;

        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;

        default: return 0; // never happens
    }
}

//---------------------------------------------------------------------------------------
int p(int x) {
	return int(texelFetch(hashTable, x).r);
}

//---------------------------------------------------------------------------------------
// Outputs values in range [-1,1]
double perlinNoise2(double x, double y, double z) {
	//-- Determine unit cube that contains the point (x,y,z):
    int xi = int(x) & 255;
    int yi = int(y) & 255;
    int zi = int(z) & 255;

	//-- Determine relative poistion of point within unit cube:
    double xf = fract(x);
    double yf = fract(y);
    double zf = fract(z);

	//-- Compute fade curves for each x,y,z:
    double u = fade(xf);
    double v = fade(yf);
    double w = fade(zf);

    int A = p(xi  )+yi, AA = p(A)+zi, AB = p(A+1)+zi,
        B = p(xi+1)+yi, BA = p(B)+zi, BB = p(B+1)+zi;

    double x1, x2, y1, y2;

	//-- Add bleneded results from 8 corners of cube:
    x1 = mix(grad(p(AA), xf, yf, zf),   grad(p(BA), xf-1, yf, zf),   u);
    x2 = mix(grad(p(AB), xf, yf-1, zf), grad(p(BB), xf-1, yf-1, zf), u);

    y1 = mix(x1, x2, v);

	x1 = mix(grad(p(AA+1), xf, yf, zf-1),   grad(p(BA+1), xf-1, yf, zf-1),   u);
	x2 = mix(grad(p(AB+1), xf, yf-1, zf-1), grad(p(BB+1), xf-1, yf-1, zf-1), u);

    y2 = mix(x1, x2, v);

    return mix(y1, y2, w);
}

//---------------------------------------------------------------------------------------

void main () {
	color = float(perlinNoise2(gl_FragCoord.x, gl_FragCoord.y, layer));
}