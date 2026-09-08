#pragma once
#include "wled.h"

#ifdef ESP32  

class CpuFreqUsermod : public Usermod {
  private:
    unsigned long lastCheck = 0;
    uint8_t  currentFreq = 240;
    uint16_t maxFreq = 240;
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
      if (maxFreq != 160 && maxFreq != 240) maxFreq = 240;
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

    void addToConfig(JsonObject& root) override {
      JsonObject top = root.createNestedObject("CpuFreq");
      top["enabled"] = enabled;
      top["maxFreqMHz"] = maxFreq;
    }

    bool readFromConfig(JsonObject& root) override {
      JsonObject top = root["CpuFreq"];
      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top["enabled"], enabled, true);
      configComplete &= getJsonValue(top["maxFreqMHz"], maxFreq, 240);
      if (maxFreq != 160 && maxFreq != 240) maxFreq = 240;
      return configComplete;
    }
};

static CpuFreqUsermod cpu_freq_usermod;
REGISTER_USERMOD(cpu_freq_usermod)

#endif // ESP32
