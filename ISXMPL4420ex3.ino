// ISXMPL4420ex3 - Arduino UART Current Loop Receiver Using the IS4420 Module
// https://inacks.com/is4420
//
// The IS4420-M1 sends one text line on its TX pad per measurement, about
// every 100 ms (115200 baud, 8N1), for example:
//
//   12.34mA_+052.1%_+025deg_IS4420M1_ver0      (followed by CR LF)
//
// This sketch reads each line, extracts the current, the percentage of
// the 4-20 mA span and the module temperature, and prints them on the
// Serial Monitor (115200 baud).
//
// Wiring (Arduino Uno R3):
//   IS4420-M1 3V3 -> Uno 3.3V
//   IS4420-M1 GND -> Uno GND and the loop power supply 0 V
//   IS4420-M1 TX  -> Uno pin 0 (RX)
//   IS4420-M1 CIN -> current loop (the loop current returns through GND)
//   IS4420-M1 SDA, SCL, SPD and ADR are not used in this example.
//
// Notes:
// - TX only pulls the line low, so it needs a pull-up. On the Uno R3, pin 0
//   is already pulled up to 5 V by the board's USB-to-serial chip through a
//   1 kOhm resistor, so no extra resistor is needed.
// - Pin 0 is shared with the USB connection: disconnect the TX wire while
//   uploading the sketch, and do not send anything from the Serial Monitor.
// - On an Uno R4, Serial is the USB port and pin 0 belongs to Serial1, with
//   no pull-up: fit a 2.2 kOhm pull-up, call Serial1.begin(115200) and read
//   from Serial1 instead.

void setup() {
  Serial.begin(115200);                 // same speed as the IS4420-M1 line
}

void loop() {
  // Read one line, up to the LF at its end. readStringUntil() also returns
  // after a 1 s timeout, with whatever arrived until then.
  String line = Serial.readStringUntil('\n');
  line.trim();                          // remove the CR

  // A complete line has exactly 37 characters. Skip anything else,
  // for example a line caught halfway when the sketch starts.
  if (line.length() != 37) return;

  // No measurement available: ERRORmA_+ERROR%_+ERRdeg_IS4420M1_ver0
  if (line.startsWith("ERROR")) {
    Serial.println("No measurement");
    return;
  }

  // Module temperature, characters 17 to 20: "+025". The over-range line
  // carries it too, so it is read before the check below.
  int temperature = line.substring(16, 20).toInt();

  // Current above 21.50 mA (over-range):
  // EOVERmA_+EOVER%_+025deg_IS4420M1_ver0
  if (line.startsWith("EOVER")) {
    Serial.print("Over-range");
  } else {
    // The percentage is negative below 4 mA, for example "-024.6"
    float current = line.substring(0, 5).toFloat();   // characters 1 to 5:  "12.34"
    float percent = line.substring(8, 14).toFloat();  // characters 9 to 14: "+052.1"
    Serial.print(current, 2);
    Serial.print(" mA   ");
    Serial.print(percent, 1);
    Serial.print(" %");
  }

  Serial.print("   ");
  Serial.print(temperature);
  Serial.println(" degC");
}
