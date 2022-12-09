/*
	A waveform is an array of WF_LEN floating-point values between 0 and 1.
*/
#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include<string>

const float PI = 2 * acos(0.0);
const float TWO_PI = 2 * PI;
const float ROOT_2 = sqrt(2);
const int MAX_BIT_DEPTH = 1024;
const float MAX_BIT_DEPTH_F = (float)MAX_BIT_DEPTH;


//=======================================
// Utilities
//=======================================

float clamp(float val, float min, float max) {
	if (val > max) {
		return max;
	}
	if (val < min) {
		return min;
	}
	return val;
}

int sgn(double v) {
	if (v < 0) return -1;
	if (v > 0) return 1;
	return 0;
}

float ease_side_max = 5;

float ease_side(float sample, float amount) {
	sample = (sample + 1) / 2;	// map to [0, 1]
	if (amount == 0) {
		amount = 1;
	}
	else if (sgn(amount) == 1) {
		amount = (amount * ease_side_max) + 1;	// map to [1, ease_side_max]
	}
	else {
		amount = 1 + (amount + 1/ease_side_max);
	}
	sample = pow(sample, amount);
	sample = (sample * 2) - 1;	// map back to [-1, 1]
	return sample;
}

float ease_centre(float sample, float amount) {
	int sign = sgn(sample);
	sample = abs(sample);

	amount = (amount + 1) / 2;
	amount *= 5;

	sample = abs(pow(sample, amount));
	sample *= sign;

	return sample;
}

float scaler(float in, float out) {
	float scale = 2*(0.5 - abs(abs(in) - 0.5));
	return in + scale * out;
}

float expando(float in, float out) {
	return in * abs(out);
}

float fold(float sample) {
	if (sample > 0) {
		if (sample > 2) {
			sample -= floor(sample - 1);
		}
		if (sample > 1) {
			sample = 1 - (sample - 1);
		}
	}
	else if(sample < 0) {
		if (sample < -2) {
			sample += floor(abs(sample) - 1);
		}
		if (sample < -1) {
			sample = -1 - (sample + 1);
		}
	}
	return sample * 0.95;
}

//=======================================
// Functions
//=======================================

float w_identity(float sample) {
	return sample;
}

float w_cosine(float sample) {
	return cos(3 * PI / 2 + sample * PI / 2);
}

float w_tanh(float sample) {
	return tanh(sample * PI);
}

float w_scoop(float sample) {
	return scaler(sample, abs(sin(1.5 * PI * (sample + 1) / 2)) * 2 - 1) * 0.85;
}

float w_asin(float sample) {
	return 2 * asin(sample) / PI;
}

float w_cosstep(float sample) {
	return scaler(sample, cos(2*sample)/2);
}

float w_sinstep(float sample) {
	return scaler(sample, sin(-2 * sample) / 2);
}

float w_sinfold(float sample) {
	return fold(scaler(sample, abs(sin(4*sample))));
}

float w_bigcos(float sample) {
	float a = cos(9 * sample * sample);
	return fold(scaler(sample, a));
}

float w_shcos(float sample) {
	float a = sinh(sample) + cos(9 * sample * sample);
	return fold(scaler(sample, a));
}

float w_thcos(float sample) {
	float a = cos(9 * sample * sample) - tanh(5*sample);
	return fold(scaler(sample, a));
}

float w_fm1(float sample) {
	float a = 2*cos(sin(4*sample) + cos(2*sample - 1));
	return fold(scaler(sample, a));
}

float w_xp_sin(float sample) {
	float a = abs(2*sin(2*PI*sample)) - 1;
	return expando(sample, a);
}

float w_xp_cos(float sample) {
	float a = abs(1.5 * cos(2 * PI * sample)) - 1;
	return expando(sample, a);
}

float w_xp_sin_cos(float sample) {
	float a = abs(pow(sin(pow(cos(2 * PI * sample), 2)), 3)) * 1.6;
	return expando(sample, a);
}

float w_quadscale(float sample) {
	float a = scaler(sample, 2*sample*sample + sample);
	return fold(a);
}

float w_blancmange(float sample) {
	float a = scaler(-1*abs(0.3*sample), sin(-1*abs(0.3*sample))+0.5*pow(-1*abs(sample) + 2, 2)) * 6;
	return fold(a);
}

float w_foldscale(float sample) {
	float a = scaler(sample, sample);
	return fold(a);
}

float w_xpandoscale(float sample) {
	float a = expando(2*sample, scaler(sample, sample));
	return fold(a);
}

float w_archer1(float sample) {
	float a = scaler(-abs(sample), sample) * -1 * sgn(sample);
	return fold(a);
}

float w_archer2(float sample) {
	float a = scaler(abs(1.7*sample), 2*sample*sample)*0.9;
	return fold(a);
}

float w_archer3(float sample) {
	float s = 0.7 * sample + 0.2;
	float a = scaler(2*s, 1/cos(s));
	return fold(expando(sample, a));
}

float w_biscaler1(float sample) {
	float a = scaler(sample*sample*sample, scaler(sample, sample));
	return fold(a);
}

float w_biscaler2(float sample) {
	float a = scaler(-abs(sample), scaler(sample, cos(sample)));
	return fold(a);
}

float w_biscaler3(float sample) {
	float a = expando(sample, cos(sample * TWO_PI));
	return fold(a);
}

float w_rhizome(float sample) {
	float a = scaler(abs(sample), 2.5*sample);
	return fold(a);
}

float w_cadmium(float sample) {
	float a = expando(sample, scaler(abs(sample), 2 * sample / cos(sample + PI)));
	return fold(a);
}

float w_flotilla(float sample) {
	float a = scaler(sample, abs(expando(sample + 2, sample - 1)));
	return fold(a);
}

float w_quartic(float sample) {
	float a = scaler(sample, abs(expando(sample + 2, sample - 1)));
	return fold(a);
}

float w_cubic(float sample) {
	float a = scaler(sample * sample * sample, sample);
	return fold(a);
}

float w_cubiccos(float sample) {
	float a = scaler(sample*sample, cos(sample)/pow(sample + 2, 2)) * 2 - 1;
	return fold(expando(sample, a));
}

float w_cubicquad(float sample) {
	float a = scaler(sample * sample * sample, 1 - sample*sample);
	return fold(a);
}

float w_cubicratio1(float sample) {
	float a = scaler(abs(sample*sample*sample), 2/(2 + sample * sample));
	return fold(a);
}

float w_cubicratio2(float sample) {
	float a = scaler(2*abs(sample * sample), sample/4) * 2 - 1;
	return fold(expando(sample, a));
}

float w_cubicratio3(float sample) {
	float a = scaler(sample * sample, sample / pow(sample + 1.5, 2)) * -2 + 1;
	return fold(expando(sample, a));
}

float w_cubicratio4(float sample) {
	float a = scaler(0.5 * sample, sample + 3) -0.5;
	return fold(expando(sample, a));
}

//=======================================
// Function Pointer list
//======================================

const int NUM_WFs = 31;
float (*ptr[NUM_WFs]) (float sample);
bool INITIALIZED = false;

void initialize_waveforms() {
	if (INITIALIZED) {
		return;
	}
	ptr[0] = w_identity;
	ptr[1] = w_cosine;
	ptr[2] = w_tanh;
	ptr[3] = w_asin;
	ptr[4] = w_cosstep;
	ptr[5] = w_sinstep;
	ptr[6] = w_archer1;
	ptr[7] = w_biscaler1;
	ptr[8] = w_cubic;
	ptr[9] = w_cubicquad;
	ptr[10] = w_cadmium;
	ptr[11] = w_biscaler3;
	ptr[12] = w_sinfold;
	ptr[13] = w_scoop;
	ptr[14] = w_bigcos;
	ptr[15] = w_shcos;
	ptr[16] = w_thcos;
	ptr[17] = w_xp_sin;
	ptr[18] = w_xp_cos;
	ptr[19] = w_xp_sin_cos;
	ptr[20] = w_quadscale;
	ptr[21] = w_blancmange;
	ptr[22] = w_foldscale;
	ptr[23] = w_xpandoscale;
	ptr[24] = w_archer3;
	ptr[25] = w_biscaler2;
	ptr[26] = w_rhizome;
	ptr[27] = w_flotilla;
	ptr[28] = w_cubicratio1;	
	ptr[29] = w_cubicratio2;
	ptr[30] = w_cubicratio4;
}

const char* wf_names[NUM_WFs] = {
	"Identity",
	"Cosine",
	"Tanh",
	"Arcsine",
	"Mondegreen",
	"Xanthoria",		//5
	"Natronomonas",
	"Gorgonian",
	"Vassago",
	"Phenex",
	"Squamous",			//10
	"Varahi",
	"Deltoid",
	"Strix",
	"Ergot",
	"Miasma",			//15
	"Inglip",
	"Qiupalong",
	"Archery",
	"Larkspur",			
	"Dolmen",
	"Rhizome",
	"Cadmium",			
	"Coldharbour",
	"Atabeg",
	"Nocebo",
	"Bakelite",
	"Irvine",			
	"Calliope",
	"Arctic",
	"Voile"
};

//=======================================
// Waveform Calculation
//=======================================

float apply_power(float val, float power) {
	if (power == 0) {
		power = 1;
	}
	else {
		power *= -1;
		if (power < 0) {
			power = 1 + power;
		}
		else {
			power *= 10;
			power += 1;
		}
		float pval = pow(abs(val), power);
		if (sgn(val) != sgn(pval)) {
			val = pval * -1;
		}
		else {
			val = pval;
		}
	}
	return val;
}

float apply_harmonics(float sample, float val, float harm_freq, float harm_amp) {
	if (harm_amp > 0) {
		float amp = (1 - pow(abs(val), 2)) * harm_amp;
		val += sin(sample * PI * harm_freq) * 0.2 * amp;
	}
	else if (harm_amp < 0) {
		harm_amp *= -1;
		float amp = (1 - pow(1 - abs(val), 2)) * harm_amp;
		val += sin(sample * PI * harm_freq) * 0.2 * amp;
	}
	return val;
}

float apply_bit_reduction(float val, float bit_depth, int mask) {
	// Bit reduction
	if (bit_depth > 2) {
		float bd = MAX_BIT_DEPTH_F - abs(bit_depth) + 2;
		bd /= MAX_BIT_DEPTH_F;
		bd = bd * bd * bd;
		bd *= MAX_BIT_DEPTH_F;
		val = sgn(val) * float(floor(abs(val) * bd) / bd);
	}

	// Bit mask
	int intval = floor(abs(val) * MAX_BIT_DEPTH);
	if (mask > 0) {
		intval ^= mask;
		val = sgn(val) * (float)intval / MAX_BIT_DEPTH_F;
	}
	else if (mask < 0) {
		mask = -1 * mask;
		mask = MAX_BIT_DEPTH - 1 - mask;
		intval &= mask;
		val = sgn(val) * (float)intval / MAX_BIT_DEPTH_F;
	}
	return val;
}

float apply_fold(float val, float fold_amt) {
	if (fold_amt > 0) {
		fold_amt += 1;
		val = fold(val * fold_amt);
	}
	else if (fold_amt < 0) {
		fold_amt -= 0.5;
		fold_amt *= 3;
		val = fold(sin(-val * fold_amt));
	}
	return val;
}

enum {
	ALGO_WF,
	ALGO_POWER,
	ALGO_HARMONICS,
	ALGO_BIT,
	ALGO_FOLD
};

float remap_sample(
	float sample, 
	int wf, 
	float power,
	float harm_freq, float harm_amp, float bit_depth,
	float fold_amt,
	int mask,
	int* algorithm
) {	

	if (sample == 0) {
		return 0;	// All waveforms should do this, but some glitch out at 0 with extreme values.
	}

	// Base waveform
//	float val = (*ptr[wf])(sample);

//	val = apply_power(val, power);
//	val = apply_harmonics(sample, val, harm_freq, harm_amp);
//	val = apply_bit_reduction(val, bit_depth);
//	val = apply_fold(val, fold_amt);

	float val = sample;
	for (int i = 0; i < 5; ++i) {
		switch (algorithm[i]) {
		case ALGO_WF:
			val = (*ptr[wf])(val);
			break;
		case ALGO_POWER:
			val = apply_power(val, power);
			break;
		case ALGO_HARMONICS:
			val = apply_harmonics(sample, val, harm_freq, harm_amp);
			break;
		case ALGO_BIT:
			val = apply_bit_reduction(val, bit_depth, mask);
			break;
		case ALGO_FOLD:
			val = apply_fold(val, fold_amt);
			break;
		}
	}

	return val;
}

