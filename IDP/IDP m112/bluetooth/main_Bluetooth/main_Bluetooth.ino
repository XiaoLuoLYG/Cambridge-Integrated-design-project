//String data_in;
//String data_out;
//void setup() {
//     pinMode(NINA_RESETN, OUTPUT);         
//     digitalWrite(NINA_RESETN, LOW);
//     
//     Serial.begin(115200);
//     SerialNina.begin(115200);
//}
//void loop() {
//  if (SerialNina.available()) data_in = SerialNina.readStringUntil("\n");
//  if (data_in != "") Serial.println(data_in);
//  data_in = "";
//  // do stuff with the data_in, which is a string.
//
//  SerialNina.write(Serial.read()); //data_out should be a string
//  SerialNina.write("\n");
//  delay(10);
//}

void setup() {
     pinMode(NINA_RESETN, OUTPUT);         
     digitalWrite(NINA_RESETN, LOW);
     
     Serial.begin(115200);
     SerialNina.begin(115200);
}
void loop() {
  if (Serial.available()) {
  SerialNina.write(Serial.read());}
  if (SerialNina.available()) {
  Serial.write(SerialNina.read());}
}
