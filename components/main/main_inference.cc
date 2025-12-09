// components/main/main_inference.cc
#include <cstdio>
#include <cstdint>
#include <cmath>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/schema/schema_generated.h"

extern "C" {
  extern unsigned char *g_model;
  extern unsigned int g_model_len;
  void HandleOutput(float x_value, float y_value);
}

static const char* TAG = "tflite_infer";
constexpr int kTensorArenaSize = 60000;
static uint8_t tensor_arena[kTensorArenaSize];

extern "C" void run_inference_task(void *pvParameters) {
  static tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  const tflite::Model* model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    ESP_LOGE(TAG, "Model schema version %d not supported", model->version());
    vTaskDelete(NULL);
    return;
  }

  static tflite::MicroMutableOpResolver<3> resolver;
  if (resolver.AddFullyConnected() != kTfLiteOk) { ESP_LOGE(TAG,"AddFullyConnected failed"); vTaskDelete(NULL); return; }
  if (resolver.AddQuantize() != kTfLiteOk) { ESP_LOGE(TAG,"AddQuantize failed"); vTaskDelete(NULL); return; }
  if (resolver.AddDequantize() != kTfLiteOk) { ESP_LOGE(TAG,"AddDequantize failed"); vTaskDelete(NULL); return; }

  static tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, kTensorArenaSize, error_reporter);

  if (interpreter.AllocateTensors() != kTfLiteOk) {
    ESP_LOGE(TAG, "AllocateTensors() failed");
    vTaskDelete(NULL);
    return;
  }

  size_t used = interpreter.arena_used_bytes();
  ESP_LOGI(TAG, "Arena used bytes: %u", (unsigned)used);

  TfLiteTensor* input = interpreter.input(0);
  TfLiteTensor* output = interpreter.output(0);

  int cnt = 0;
  for (;;) {
    float angle_deg = static_cast<float>(cnt % 360);
    float x_val = angle_deg * (3.14159265f / 180.0f);

    if (input->type == kTfLiteInt8) {
      float in_scale = input->params.scale;
      int in_zp = input->params.zero_point;
      int32_t q = static_cast<int32_t>(std::lround(x_val / in_scale) + in_zp);
      if (q < -128) q = -128; if (q > 127) q = 127;
      input->data.int8[0] = static_cast<int8_t>(q);
    } else if (input->type == kTfLiteFloat32) {
      input->data.f[0] = x_val;
    } else {
      ESP_LOGE(TAG, "Unhandled input type %d", input->type);
    }

    if (interpreter.Invoke() != kTfLiteOk) {
      ESP_LOGE(TAG, "Invoke failed");
      vTaskDelay(pdMS_TO_TICKS(1000));
      cnt++;
      continue;
    }

    float y_val = 0.0f;
    if (output->type == kTfLiteInt8) {
      int8_t q_out = output->data.int8[0];
      float out_scale = output->params.scale;
      int out_zp = output->params.zero_point;
      y_val = (static_cast<int>(q_out) - out_zp) * out_scale;
    } else if (output->type == kTfLiteFloat32) {
      y_val = output->data.f[0];
    } else {
      ESP_LOGE(TAG, "Unhandled output type %d", output->type);
    }

    HandleOutput(x_val, y_val);
    ESP_LOGI(TAG, "x: %.4f, y: %.4f", x_val, y_val);

    cnt++;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  vTaskDelete(NULL);
}
