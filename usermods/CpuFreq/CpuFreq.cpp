#include "wled.h"

#ifdef ESP32

class CpuFreqUsermod : public Usermod {
  private:
    unsigned long lastCheck = 0;
    uint8_t currentFreq = 240;
    uint8_t maxFreq = 240;          // 80, 160 или 240
    bool enabled = true;

    bool allSegmentsIdle() const {
      if (bri == 0) return true;
      for (uint8_t i = 0; i < strip.getSegmentsNum(); i++) {
        const Segment &seg = strip.getSegment(i);
        if (!seg.isActive()) continue;
        if (seg.mode != FX_MODE_STATIC) return false;
      }
      return true;
    }

  public:
    void setup() override {
      if (maxFreq != 80 && maxFreq != 160 && maxFreq != 240) maxFreq = 240;
      if (enabled) {
        setCpuFrequencyMhz(maxFreq);
        currentFreq = maxFreq;
      }
      lastCheck = millis();
    }

    void loop() override {
      if (!enabled) return;
      if (millis() - lastCheck < 2000) return;
      lastCheck = millis();

      uint8_t target = allSegmentsIdle() ? 80 : maxFreq;
      if (target != currentFreq) {
        setCpuFrequencyMhz(target);
        currentFreq = target;
      }
    }

    uint16_t getId() override {
      return 0x1234;
    }

    // Без override – компилируется во всех версиях WLED
    void addToInfo(JsonObject& root) {
      if (!enabled) return;
      root["CPU Freq"] = String(currentFreq) + " MHz";
    }

    void addToConfig(JsonObject& root) override {
      JsonObject top = root.createNestedObject("CpuFreq");
      top["enabled"] = enabled;
      
      JsonObject freqObj = top.createNestedObject("maxFreqMHz");
      freqObj["label"] = "Макс. частота, МГц";
      freqObj["type"] = "select";
      freqObj["options"] = "80,160,240";
      freqObj["value"] = maxFreq;
    }

    bool readFromConfig(JsonObject& root) override {
      JsonObject top = root["CpuFreq"];
      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top["enabled"], enabled, true);
      
      int tempFreq = 240;
      configComplete &= getJsonValue(top["maxFreqMHz"], tempFreq, 240);
      if (tempFreq == 80 || tempFreq == 160 || tempFreq == 240) {
        maxFreq = (uint8_t)tempFreq;
      } else {
        maxFreq = 240;
      }
      return configComplete;
    }
};

static CpuFreqUsermod cpu_freq_usermod;
REGISTER_USERMOD(cpu_freq_usermod);

#endif // ESP32
