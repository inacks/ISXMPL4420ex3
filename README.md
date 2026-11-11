# Arduino UART Current Loop Receiver Using the IS4420 Module

<p align="center">
  <a href="https://inacks.com/product/4-20ma-receiver-is4420-m1/">
    <img src="https://inacks.com/wp-content/uploads/IS4420-M1-I2C-UART-Current-Loop-Receiver-1.jpg" alt="IS4420-M1 I2C 4-20 mA current loop receiver module" width="250">
  </a>
</p>

This example is powered by the **IS4420-M1**, a 4-20 mA current loop receiver module that sends every measurement as a line of text on its TX pad. Any board with a serial receive pin can read a current loop.

No analog circuitry to design. No extra libraries. No calibration needed: INACKS calibrates every module at 4 mA and 20 mA.  
The IS4420-M1 contains the complete measurement side of the receiver: the loop current flows into its CIN pad, and about every 100 ms the module sends the current in mA, the percentage of the 4-20 mA span and the module temperature.

Perfect for reading pressure, temperature, flow, level and weight transmitters from an Arduino, for adding 4-20 mA inputs to your own controllers and data loggers, and for monitoring a current loop from a PC through a USB-to-UART adapter.

### 🛒 [Buy the IS4420-M1 and learn more](https://inacks.com/product/4-20ma-receiver-is4420-m1/)

---

## What this example does

- Reads the text line that the IS4420-M1 sends on its TX pad, at 115200 baud.
- Prints the current, the percentage of the 4-20 mA span and the module temperature on the Serial Monitor.
- Prints `Over-range` when the current is above 21.50 mA, and `No measurement` when the module has no valid measurement.
- Skips incomplete lines, such as the one caught halfway when the sketch starts.

To read the module through the I2C-serial interface instead, see [ISXMPL4420ex4](https://github.com/inacks/ISXMPL4420ex4).

---

## What you need

- An [IS4420-M1 module](https://inacks.com/product/4-20ma-receiver-is4420-m1/)
- An Arduino Uno R3 (for an Uno R4, see [Notes](#notes))
- A 4-20 mA transmitter (2-wire or 3-wire) and its loop power supply, or a 4-20 mA current source
- The Arduino IDE. The sketch doesn't need any library.

---

## Wiring

| IS4420-M1 pad | Connect to |
|---|---|
| 3V3 | Arduino 3.3V |
| GND | Arduino GND and the loop power supply 0 V |
| TX | Arduino pin 0 (RX) |
| CIN | Current loop input: the loop current flows into CIN and returns through GND |
| SDA, SCL, SPD, ADR | Not used in this example |

- The IS4420-M1 takes its power (3.2 to 3.5 V) from the Arduino 3.3V pin.
- The TX pad only pulls the line low, so it needs a pull-up. On the Uno R3, pin 0 is already pulled up to 5 V through a 1 kΩ resistor to the board's USB-to-serial chip, so you don't need an extra resistor. On other boards, fit a 2.2 kΩ pull-up from TX to 3.3 V or 5 V.
- With a 2-wire transmitter: loop power supply + to the transmitter +, the transmitter - to CIN, and the loop power supply 0 V to GND.
- With a 3-wire transmitter: the transmitter output to CIN, and the transmitter 0 V and the loop power supply 0 V to GND.
- The current loop input isn't isolated: the loop power supply 0 V, the IS4420-M1 GND and the Arduino GND are connected together.

---

## Running the example

1. Wire the circuit, but leave the TX wire disconnected for now.
2. Open the sketch in the Arduino IDE.
3. Select **Arduino Uno** and its port, and upload the sketch.
4. Connect the TX wire to pin 0.
5. Open the Serial Monitor at 115200 baud.

You should see a new line about every 100 ms. With 12 mA in the loop:

```
12.00 mA   50.0 %   26 degC
12.00 mA   50.0 %   26 degC
12.00 mA   50.0 %   26 degC
...
```

The percentage is the position of the current in the 4-20 mA span:

| Loop current | Percentage |
|:---:|:---:|
| Below 4 mA | Negative |
| 4 mA | 0.0 % |
| 12 mA | 50.0 % |
| 20 mA | 100.0 % |
| 21.50 mA | 109.4 % |
| Above 21.50 mA | `Over-range` |

---

## How it works

The IS4420-M1 sends one line per measurement, about every 100 ms, at 115200 baud, 8 data bits, no parity, 1 stop bit (8N1):

```
12.34mA_+052.1%_+025deg_IS4420M1_ver0
```

Every line has exactly 37 characters, followed by CR LF (39 bytes in total):

| Characters | Content | Example |
|:---:|---|:---:|
| 1–5 | Current in mA | `12.34` |
| 6–8 | `mA_` | |
| 9–14 | Percentage of the 4-20 mA span, with its sign | `+052.1` |
| 15–16 | `%_` | |
| 17–20 | Module temperature in °C, with its sign | `+025` |
| 21–24 | `deg_` | |
| 25–32 | Model | `IS4420M1` |
| 33–37 | `_ver` and the module revision | `_ver0` |

Two special lines keep the same length:

| Line | Meaning |
|---|---|
| `EOVERmA_+EOVER%_+025deg_IS4420M1_ver0` | The current is above 21.50 mA (over-range). The temperature is still valid. |
| `ERRORmA_+ERROR%_+ERRdeg_IS4420M1_ver0` | The module has no valid measurement. |

The sketch keeps a line only if exactly 37 characters come before its CR LF, and then takes each value from its position:

```cpp
String line = Serial.readStringUntil('\n');
line.trim();                                          // remove the CR
if (line.length() != 37) return;                      // skip incomplete lines

float current   = line.substring(0, 5).toFloat();     // "12.34"
float percent   = line.substring(8, 14).toFloat();    // "+052.1"
int temperature = line.substring(16, 20).toInt();     // "+025"
```

Any microcontroller with a UART can read the same line, and so can a PC through a USB-to-UART adapter.

---

## Notes

- **Uploading:** pin 0 is shared with the Uno R3's USB connection. Disconnect the TX wire while you upload a sketch, and don't send anything from the Serial Monitor.
- **Why pin 0:** 115200 baud is too fast for SoftwareSerial to receive reliably on a 16 MHz board, so the example uses the hardware serial port.
- **Uno R4 and boards with a second serial port:** on an Arduino Uno R4, `Serial` is the USB port, so the sketch as it is doesn't read pin 0. On an Uno R4, Mega 2560 or Leonardo, connect TX to the RX pin of `Serial1` (pin 0 on the Uno R4 and the Leonardo, pin 19 on the Mega 2560) with a 2.2 kΩ pull-up, add `Serial1.begin(115200);` to `setup()` and read with `Serial1.readStringUntil('\n')`. The USB connection then stays free for uploading and for the Serial Monitor.
- **Temperature:** it's the temperature inside the module, which can be above the ambient temperature.
- **Protection:** CIN has no protection. Never connect the loop power supply straight to CIN, and never reverse the loop: both destroy the module.

---

## Troubleshooting

| Serial Monitor shows | What to check |
|---|---|
| Nothing | The Serial Monitor speed (115200 baud), the TX wire on pin 0, and the 3V3 and GND connections. The first line comes about 0.6 s after power-up. On an Uno R4, see [Notes](#notes). |
| About `0.00 mA   -25.0 %` | There's no loop current. Check the CIN connection, the transmitter and its loop power supply, and that the loop power supply 0 V goes to GND. |
| `Over-range` | The loop current is above 21.50 mA. Check the transmitter. |
| `No measurement` | The module couldn't measure during the last 100 ms. Check the 3V3 supply (3.2 to 3.5 V). |

If the upload fails, disconnect the TX wire from pin 0 and try again.

---

## Get the IS4420-M1

- 🛒 **[Buy the IS4420-M1](https://inacks.com/product/4-20ma-receiver-is4420-m1/)**: I2C 4-20 mA Current Loop Receiver Module

ℹ️ For more information: [www.inacks.com](https://www.inacks.com)
