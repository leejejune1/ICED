#define PIN_LED 7
void setup() {
  pinMode(PIN_LED, OUTPUT);

}

void loop() {
  int i = 0;
  
  digitalWrite(PIN_LED, HIGH); 
  delay(1000);  
  digitalWrite(PIN_LED, LOW); 
  delay(1000); 
  
  while(i<5){
    digitalWrite(PIN_LED, HIGH); 
    delay(200);  
    digitalWrite(PIN_LED, LOW); 
    delay(200);  
    i++;
  }
  
  while(1){
    digitalWrite(PIN_LED, HIGH); 
  }
}
