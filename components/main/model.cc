// components/main/model.cc
#include <cstdint>

// symbols produced by xxd -i in model_data.cc
extern unsigned char sine_model_int8_tflite[];
extern unsigned int sine_model_int8_tflite_len;

// Provide canonical names expected by TinyML examples
unsigned char *g_model = sine_model_int8_tflite;
unsigned int g_model_len = (unsigned int)sine_model_int8_tflite_len;
