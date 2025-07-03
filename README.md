I'm still working on this.  
DSP project
multi-signal NanoEdge Outlayer
resuable buffers

This part is done:
Data Source (Real Signals)	Relevant DSP / Processing Libs or Techniques
Voltage (from ADC)	- CMSIS-DSP for filtering, FFT, RMS, envelope detection
Temperature (internal sensor)	- Simple averaging/filtering (CMSIS-DSP FIR/IIR)
CPU Load (cycle counter, systick)	- Trend analysis, moving average, basic stats
RAM Usage (heap/stack monitor)	- Low-pass filter, threshold detection
Power draw (if Vref & shunt)	- CMSIS-DSP for smoothing, RMS, transient detection
MCU Core Temp	- CMSIS-DSP for smoothing, anomaly detection if trend spikes
Loop Time / Jitter / Timing	- CMSIS-DSP for variance/standard deviation, frequency domain for periodic noise
UART Traffic / Packet Rate	- CMSIS-DSP histogram/stats; detect idle, overload, bursty behavior
ADC Noise Floor (idle readings)	- FFT (CMSIS-DSP) for spectral fingerprinting, anomaly detection
Internal Clock Drift	- Time-domain analysis, basic math (delta vs. RTC), anomaly detection on delta drift

I just completed printing to uart with the result of the SDP operations. Next, I have to think about modes of operation,
having each signal print 128 elements, space delimited for NanoEdge Studio.  I plan to use 10 or more signals to create
a multi-dimensional Outlayer Detection model.  Then there is an inference mode, once the model is trained and deployed.
