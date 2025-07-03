

## DSP Project for Multi-Signal NanoEdge Outlier Detection

**Reusable Buffers · CMSIS-DSP · STM32**

I'm still working on this, but the core DSP infrastructure is now code-complete.

---

### ✅ Current Status

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

I just completed the UART printout of results from the DSP operations — it prints usable diagnostic output and FFT magnitudes.

---

### 🔧 Next Steps

The next design decision is how to organize *modes of operation*. These will control the verbosity, output formatting, and intent of the device.

#### Mode 1: **Verbose Diagnostic**

* Prints all data via `printf`
* Used during development/debugging
* May output raw ADC, FFT bins, drift values

#### Mode 2: **Data Collection for Training**

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

### 🧠 Planned Features

* **Mode selection** (compile-time flag or runtime command)
* **Reusable DSP buffers** to minimize memory usage
* **NanoEdge-compatible output formatting**
* **Optional: batch capture logic (e.g. 1000 rows at once)**


