// ISXMPL4420ex3 - Arduino UART Current Loop Receiver Using the IS4420 Module
// https://inacks.com/is4420
//
// The IS4420 sends one text line on its TX pad about every 100 ms
// (115200 baud, 8N1), for example:
//
//   12.34mA_+052.1%_+025deg_IS4420M1_ver0      (followed by CR LF)
//
// This sketch reads each line, takes out the current, the percentage of
// the 4-20 mA span and the module temperature, and prints them on the
// Serial Monitor (115200 baud).
//
// Wiring (Arduino Uno):
//   IS4420 3V3 -> Uno 3.3V
//   IS4420 GND -> Uno GND
//   IS4420 TX  -> Uno pin 0 (RX)
//   IS4420 CIN -> current loop (the loop current returns to GND)
//
// Notes:
// - TX needs a pull-up. On the Uno, pin 0 is already pulled up to 5 V by
//   the board's USB-serial chip through a 1 kOhm resistor, so no extra
//   resistor is needed.
// - Pin 0 is shared with the USB connection: disconnect the TX wire while
//   uploading the sketch, and do not send anything from the Serial Monitor.

void setup() {
  Serial.begin(115200);                 // same speed as the IS4420 line
}

void loop() {
  // Read one line, up to the LF at its end
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

  // Module temperature, characters 17 to 20: "+025"
  int temperature = line.substring(16, 20).toInt();

  // Current above the range: EOVERmA_+EOVER%_+025deg_IS4420M1_ver0
  if (line.startsWith("EOVER")) {
    Serial.print("Over-range");
  } else {
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
