## DSP Project for Multi-Signal NanoEdge Outlier Detection
Features: CMSIS/DSP functions applied to actual data from the microcontroler, reduced memory footprint with reusable buffers, const qualifier to place the big inference data in SRAM, NanoEdge trained Outlier mulit-signal model, different modes of execution.

**Reusable Buffers · CMSIS-DSP · STM32**

---

Each signal below represents a potential *dimension* in a NanoEdge AI "Outlier Detection" model. Signals are derived from real hardware sensors or internal metrics and processed using CMSIS-DSP or basic math.

| Signal Source                         | Processing / Techniques                        |
| ------------------------------------- | ---------------------------------------------- |
| **Voltage (from ADC)**                | FFT, filtering, RMS, envelope (CMSIS-DSP)      |
| **Temperature (internal sensor)**     | Averaging, smoothing filters (FIR/IIR)         |
| **CPU Load** (cycle counter, systick) | Trend analysis, moving average, basic stats    |
| **RAM Usage** (heap/stack monitor)    | Threshold detection, low-pass filtering        |
| **Power Draw** (via Vref + shunt)     | Smoothing, RMS, transient detection            |
| **MCU Core Temp**                     | CMSIS-DSP smoothing, anomaly detection         |
| **Loop Time / Jitter**                | Variance, stddev, FFT for periodic noise       |
| **UART Traffic / Packet Rate**        | Histogram analysis, idle vs. overload behavior |
| **ADC Noise Floor** (idle)            | FFT-based fingerprinting                       |
| **Internal Clock Drift**              | Time-domain deltas vs. RTC, anomaly detection  |

---



#### Mode 1: **Verbose DSP logging: SIGNAL_FORMAT NOT defined in dsp_test.h**

* Prints all data via `printf`
* Used during development/debugging
* May output raw ADC, FFT bins, drift values

#### Mode 2: **Data Collection for Training: SIGNAL_FORMAT defined in DSP_test.h**

* Outputs one signal per row
* Each row = 128 elements (space-separated floats or integers)
* Targeted at NanoEdge AI Studio data format
* Used to generate datasets for multi-dimensional model training

> *Still deciding*: whether each test signal should output a single row, or run for 1000 iterations and collate output offline (e.g. in Python).

#### Mode 3: **Inference Mode**

* Once the NanoEdge outlier model is trained and deployed
* Will run real-time anomaly detection using onboard model
* Minimal output (anomaly score, status LED, etc.)

---
COM Port Reader: https://users.ece.utexas.edu/~valvano/edX/download.html
### 🧠 Planned Features

* **Mode selection** (compile-time flag or runtime command)
* **Reusable DSP buffers** to minimize memory usage
* **NanoEdge-compatible output formatting**
* **Optional: batch capture logic (e.g. 1000 rows at once)**


