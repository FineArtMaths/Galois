#include <math.h>
#include <stdlib.h>

#ifndef M_LN2
#define M_LN2	   0.69314718055994530942
#endif

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

/* filter types */
enum {
    LPF, /* low pass filter */
    HPF, /* High pass filter */
    BPF, /* band pass filter */
    NOTCH, /* Notch Filter */
    PEQ, /* Peaking band EQ filter */
    LSH, /* Low shelf filter */
    HSH /* High shelf filter */
};

class Biquad
{
public:
    //==============================================================================
    float apply(float sample) {
        float result;
        if (!initialized) {
            return sample;
        }

        /* compute result */
        result = biquad_a0 * sample 
            + biquad_a1 * biquad_x1 
            + biquad_a2 * biquad_x2 
            - biquad_a3 * biquad_y1 
            - biquad_a4 * biquad_y2;

        /* shift x1 to x2, sample to x1 */
        biquad_x2 = biquad_x1;
        biquad_x1 = sample;

        /* shift y1 to y2, result to y1 */
        biquad_y2 = biquad_y1;
        biquad_y1 = result;

        return result;
    }

    void recalculate(float sample_rate, float frequency, float bandwidth, float gain, int type) {
        float A, omega, sn, cs, alpha, beta;
        float a0, a1, a2, b0, b1, b2;

        /* setup variables */
        A = pow(10, gain / 40);
        omega = 2 * M_PI * frequency / sample_rate;
        sn = sin(omega);
        cs = cos(omega);
        alpha = sn * sinh(M_LN2 / 2 * bandwidth * omega / sn);
        beta = sqrt(A + A);

        switch (type) {
        case LPF:
            b0 = (1 - cs) / 2;
            b1 = 1 - cs;
            b2 = (1 - cs) / 2;
            a0 = 1 + alpha;
            a1 = -2 * cs;
            a2 = 1 - alpha;
            break;
        case HPF:
            b0 = (1 + cs) / 2;
            b1 = -(1 + cs);
            b2 = (1 + cs) / 2;
            a0 = 1 + alpha;
            a1 = -2 * cs;
            a2 = 1 - alpha;
            break;
        case BPF:
            b0 = alpha;
            b1 = 0;
            b2 = -alpha;
            a0 = 1 + alpha;
            a1 = -2 * cs;
            a2 = 1 - alpha;
            break;
        case NOTCH:
            b0 = 1;
            b1 = -2 * cs;
            b2 = 1;
            a0 = 1 + alpha;
            a1 = -2 * cs;
            a2 = 1 - alpha;
            break;
        case PEQ:
            b0 = 1 + (alpha * A);
            b1 = -2 * cs;
            b2 = 1 - (alpha * A);
            a0 = 1 + (alpha / A);
            a1 = -2 * cs;
            a2 = 1 - (alpha / A);
            break;
        case LSH:
            b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
            b1 = 2 * A * ((A - 1) - (A + 1) * cs);
            b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
            a0 = (A + 1) + (A - 1) * cs + beta * sn;
            a1 = -2 * ((A - 1) + (A + 1) * cs);
            a2 = (A + 1) + (A - 1) * cs - beta * sn;
            break;
        case HSH:
            b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
            b1 = -2 * A * ((A - 1) + (A + 1) * cs);
            b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
            a0 = (A + 1) - (A - 1) * cs + beta * sn;
            a1 = 2 * ((A - 1) - (A + 1) * cs);
            a2 = (A + 1) - (A - 1) * cs - beta * sn;
            break;
        }

        /* precompute the coefficients */
        biquad_a0 = b0 / a0;
        biquad_a1 = b1 / a0;
        biquad_a2 = b2 / a0;
        biquad_a3 = a1 / a0;
        biquad_a4 = a2 / a0;

        /* zero initial samples */
        //biquad_x1 = biquad_x2 = 0;
        //biquad_y1 = biquad_y2 = 0;

        initialized = true;
    }

private:
    float biquad_a0 = 0, biquad_a1 = 0, biquad_a2 = 0, biquad_a3 = 0, biquad_a4 = 0;
    float biquad_x1 = 0, biquad_x2 = 0, biquad_y1 = 0, biquad_y2 = 0;
    bool initialized = false;

};