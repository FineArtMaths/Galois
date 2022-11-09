/*
	A waveform is an array of WF_LEN floating-point values between 0 and 1.
*/
#define _USE_MATH_DEFINES
#include <cmath>
#include<string>

const int WF_LEN = 1024;
const double WF_LEN_DOUBLE = (double)(WF_LEN);
const float PI = 2 * acos(0.0);
const double PI_STEP = 2 * PI / WF_LEN;
int IS_INITIALIZED = 0;

//===================================================
/// UTILITY FUNCTIONS
//===================================================

int sample_to_WFPosn(const float sample, const int time_scale) {
	int val = floor((sample + 1) * (WF_LEN / 2));
	if (time_scale % 2 == 1) {
		val = (val * time_scale) % WF_LEN;
	}
	else {
		val = (WF_LEN - (val * time_scale) % WF_LEN) - 1;
	}
	return val;
}

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

//===================================================
/// WAVEFORM DEFINITIONS
//===================================================

// LINEAR
const float* build_linear() {
	float* wf = new float[WF_LEN];
	for (int i = 0; i < WF_LEN; i++) {
		double i_d = static_cast<double>(i);
		float val = 2.0f * (i_d - WF_LEN_DOUBLE/2.0f) / WF_LEN_DOUBLE;
		//float val = 2 * (i_d / WF_LEN_DOUBLE) - 1;
		wf[i] = val;
	}
	return wf;
}

// COSINE
const float* build_cosine() {
	float* wf = new float[WF_LEN];
	for (int i = 0; i < WF_LEN; i++) {
		wf[i] = -1 * cos(i * PI_STEP / 2);
	}
	return wf;
}

// QUADRATIC
const float* build_quadratic() {
	float* wf = new float[WF_LEN];
	for (int i = 0; i < WF_LEN; i++) {
		double i_d = static_cast<double>(i);
		float val = 2 * pow(i_d / WF_LEN_DOUBLE, 2) - 1;
		val = clamp(val, -1, 1);
		wf[i] = val;
	}
	return wf;
}

// TANH
const float* build_tanh() {
	float* wf = new float[WF_LEN];
	for (int i = 0; i < WF_LEN; i++) {
		wf[i] = tanh(i * PI_STEP - PI);
	}
	return wf;
}

// QUAD HARM VARIANTS
const float* build_qharm(int freq) {
	float* wf = new float[WF_LEN];
	for (int i = 0; i < WF_LEN; i++) {
		double i_d = static_cast<double>(i);
		float val = (2 * pow(i_d / WF_LEN, 2) - 1) * 0.95f;
		float amp = 1 - pow(abs(val), 2);
		val += cos(PI_STEP * i * freq) * 0.2 * amp;
		wf[i] = val;
	}
	return wf;
}

// SCOOP HARM VARIANTS
const float* build_sharm(int freq) {
	float* wf = new float[WF_LEN];
	for (int i = 0; i < WF_LEN; i++) {
		double i_d = static_cast<double>(i);
		float val = abs(sin(1.5f * PI_STEP * i / 2.0f)) * 2.0f - 1;
		if (freq > 0) {
			float amp = 1 - pow(abs(val), 2);
			val += cos(PI_STEP * i * freq) * 0.2 * amp;
		}
		wf[i] = val;
	}
	return wf;
}

// ASIN
const float* build_asin() {
	float* wf = new float[WF_LEN];
	for (int i = 0; i < WF_LEN; i++) {
		wf[i] = asin((2 * i / (WF_LEN - 1)) - 1) / (PI / 2);
	}
	return wf;
}

const int NUM_WFs = 15;
const float** waveforms = 0;
char* names[NUM_WFs] = { 
	"Linear", "Cosine", "Quadratic", "Tanh", 
	"QHarm 1", "QHarm 2", "QHarm 4", "QHarm 8", 
	"Scoop", "SHarm 2", "SHarm 4", "SHarm 8", "SHarm 16", "SHarm 32",
	"Asin"
};

void init() {
	waveforms = new const float* [NUM_WFs];
	waveforms[0] = build_linear();
	waveforms[1] = build_cosine();
	waveforms[2] = build_quadratic();
	waveforms[3] = build_tanh();
	waveforms[4] = build_qharm(1);
	waveforms[5] = build_qharm(2);
	waveforms[6] = build_qharm(4);
	waveforms[7] = build_qharm(8);
	waveforms[8] = build_sharm(0);
	waveforms[9] = build_sharm(2);
	waveforms[10] = build_sharm(4);
	waveforms[11] = build_sharm(8);
	waveforms[12] = build_sharm(16);
	waveforms[13] = build_sharm(32);
	waveforms[14] = build_asin();
	IS_INITIALIZED = 1;
}

const float map_to_wf(const float sample, const int time_scale, const int wfi) {
	if (IS_INITIALIZED == 0) {
		init();
	}
	const float* wf = waveforms[wfi];
	float val = wf[sample_to_WFPosn(sample, time_scale)];
	return clamp( val, -1, 1);
}

