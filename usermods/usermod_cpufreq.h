#pragma once
#include "wled.h"

class CpuFreqUsermod : public Usermod {
  private:
    unsigned long lastCheck = 0;
    uint8_t  currentFreq = 240;
    uint16_t maxFreq = 240; 

    bool allSegmentsIdle() {
      if (bri == 0) return true; 

      for (uint8_t i = 0; i < strip.getSegmentsNum(); i++) {
        Segment &seg = strip.getSegment(i);
        if (!seg.isActive()) continue;
        if (seg.mode != FX_MODE_STATIC) return false; 
      }
      return true; 
    }

  public:
    void setup() override {
      if (maxFreq != 160 && maxFreq != 240) maxFreq = 240;
      setCpuFrequencyMhz(maxFreq);
      currentFreq = maxFreq;
    }

    void loop() override {
      if (millis() - lastCheck < 2000) return;
      lastCheck = millis();

      uint8_t target = allSegmentsIdle() ? 80 : maxFreq;
      if (target != currentFreq) {
        setCpuFrequencyMhz(target);
        currentFreq = target;
      }
    }

    void addToConfig(JsonObject& root) override {
      JsonObject top = root.createNestedObject("CpuFreq");
      top["maxFreqMHz"] = maxFreq;
    }

    bool readFromConfig(JsonObject& root) override {
      JsonObject top = root["CpuFreq"];
      bool ok = !top.isNull();
      ok &= getJsonValue(top["maxFreqMHz"], maxFreq, 240);
      if (maxFreq != 160 && maxFreq != 240) maxFreq = 240;
      return ok;
    }
};

static CpuFreqUsermod cpu_freq_usermod;
REGISTER_USERMOD(cpu_freq_usermod);
