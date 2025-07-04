
## ⚙️ DSP Project: Multi-Signal NanoEdge™ AI Outlier Detection

**CMSIS-DSP · Embedded Inference · STM32F401RE · Reusable Buffers**

This project demonstrates real-time anomaly detection on an STM32F401RE using a NanoEdge™-trained outlier model. It streams live sensor data, applies lightweight DSP preprocessing, and runs inference—all under severe memory constraints.

Key features:

- 📉 CMSIS-DSP processing on actual MCU signals
- ♻️ Reusable buffer architecture to reduce SRAM usage
- 🔒 `const`-qualified inference data in Flash (bypassing `.data` overflows)
- 🧠 NanoEdge™ AI multi-signal outlier detection
- ⚡ Multiple execution modes for development, data generation, and deployment

---

### 📡 Signal Sources & Processing Techniques

Each signal contributes a dimension to the anomaly model. They’re derived from live hardware measurements or synthetic runtime metrics:

| Signal Source                         | Processing Techniques                           |
| ------------------------------------- | ----------------------------------------------- |
| **Voltage (from ADC)**                | FFT, filtering, RMS, envelope (CMSIS-DSP)       |
| **Temperature (internal sensor)**     | Averaging, FIR/IIR smoothing                    |
| **CPU Load** (cycle counter/systick)  | Trend analysis, moving average                  |
| **RAM Usage** (heap/stack monitor)    | Threshold detection, low-pass filter            |
| **Power Draw** (via shunt or Vref)    | Smoothing, transient detection, RMS             |
| **MCU Core Temp**                     | Windowed smoothing, baseline deviation          |
| **Loop Time / Jitter**                | Stddev, FFT for noise profiling                 |
| **UART Traffic**                      | Idle time analysis, packet rate histogram       |
| **ADC Noise Floor** (idle)            | FFT fingerprinting                              |
| **Internal Clock Drift**              | Time-domain deltas, anomaly detection           |

---

> **Note on Signal Structure:**  
> In this project, each signal type (e.g. voltage, temperature, power draw) is processed individually and treated as a separate input to the NanoEdge model. While the term "multi-signal" is used to describe the broader capability of the system, signals are not fused into a single vector per inference window. Instead, each mode operates independently per signal, allowing modular capture, transformation, and evaluation.

---

| Mode              | Purpose                                           | Signal Handling                 |
|------------------|---------------------------------------------------|----------------------------------|
| `DSP_MODE`        | Visualize signals & DSP pre-processing            | Per-signal, printed to UART     |
| `SIGNAL_FORMAT`   | Generate rows for training                        | One signal at a time            |
| `INFERENCE_TEST`  | Run inference on fixed dataset                    | Full multi-signal array         |
| `SELF_DIAG_MODE`  | Live inference with minimal output                | One active signal per run       |


### 🔁 Execution Modes

#### 🔹 Mode 1: **Verbose DSP Logging**
**When:** `DSP_TEST` is **defined** in `dsp_test.h`  
**Purpose:** Development & debugging  
**Behavior:**
- Logs full signal outputs via `printf`
- Can include raw ADC snapshots, FFT bin magnitudes, and jitter measurements

---

#### 🔸 Mode 2: **Signal Capture for Training**
**When:** `SIGNAL_FORMAT` is **defined**  
**Purpose:** Create datasets for NanoEdge AI Studio  
**Behavior:**
- Each row = one time window with 128 elements
- Outputs plain text rows (float or integer)
- Suited for collecting hundreds of rows to train the anomaly model

---

#### 🧠 Mode 3: **Inference Mode**
**Purpose:** Real-time anomaly detection  
**Behavior:**
- Uses NanoEdge’s `neai_oneclass()` on incoming signals
- Prints classification result, row index, and outlier flag
- Processes up to 495 rows of inference data with only ~512B RAM footprint

---

### 🧰 Utilities & Tools

**Recommended COM port reader:**  
🖥️ TExaSdisplay by Dr. Jonathan Valvano  
📥 [Download here](https://users.ece.utexas.edu/~valvano/edX/download.html)

---
🧠 Inference Data Note
For demonstration purposes, the current inference phase runs directly on the same signal dataset used during NanoEdge AI model training. This ensures predictable results and makes it easy to validate the inference loop and memory management.
If you’d like to test the model with new or live data, you can:
- Stream fresh vectors via UART (see NanoEdge_Client)
- Enable USARTx_IRQHandler() to receive and store incoming vectors dynamically
- Swap the inference_data.c contents with fresh captured vectors (formatted as const float signal_data[...][128])

----
License GPL v.2
