#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "model_data.cc"

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "output_handler.h"

#define TENSOR_ARENA_SIZE 25 * 1024
static uint8_t tensor_arena[TENSOR_ARENA_SIZE];

void app_main(void)
{
    const tflite::Model* model = tflite::GetModel(g_sine_model_int8_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("Model schema %d does not match runtime %d\n",
               model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Required for quantized model
    static tflite::MicroMutableOpResolver<3> resolver;
    resolver.AddFullyConnected();
    resolver.AddQuantize();
    resolver.AddDequantize();

    // Interpreter
    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena,
                                         TENSOR_ARENA_SIZE);

    if (interpreter.AllocateTensors() != kTfLiteOk) {
        printf("AllocateTensors() failed\n");
        return;
    }

    TfLiteTensor* input = interpreter.input(0);
    TfLiteTensor* output = interpreter.output(0);

    float x = 0;
    while (true) {

        // Convert x to int8
        float input_scale = input->params.scale;
        int zero_point = input->params.zero_point;
        int8_t quant_x = (int8_t)(x / input_scale + zero_point);

        input->data.int8[0] = quant_x;

        if (interpreter.Invoke() != kTfLiteOk) {
            printf("Invoke failed\n");
            continue;
        }

        int8_t y_q = output->data.int8[0];

        // Convert back to float
        float y = (y_q - output->params.zero_point) * output->params.scale;

        HandleOutput(x, y);

        x += 0.1f;
        if (x > 2 * M_PI) x = 0;

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
