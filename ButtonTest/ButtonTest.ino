int IA = A2;
int OA = A3;
int IB = A4;
int OB = A5;

void setup() {
  Serial.begin(115200);
  pinMode(IA, INPUT);
  pinMode(OA, INPUT);
  pinMode(IB, INPUT);
  pinMode(OB, INPUT);

}

void loop() {
  Serial.print(digitalRead(IA));
  Serial.print("  ");
  Serial.print(digitalRead(OA));
  Serial.print("  ");
  Serial.print(digitalRead(IB));
  Serial.print("  ");
  Serial.println(digitalRead(OB));
}

 
