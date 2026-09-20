const int GREEN_LED = 9; 
const int ORANGE_LED = 11; 
const int RED_LED = 13; 

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(ORANGE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  digitalWrite(GREEN_LED, HIGH); 
  delay(1000);            
  digitalWrite(GREEN_LED, LOW);  

  digitalWrite(ORANGE_LED, HIGH);  
  delay(1000);            
  digitalWrite(ORANGE_LED, LOW);  

  digitalWrite(RED_LED, HIGH);  
  delay(1000);            
  digitalWrite(RED_LED, LOW);  

  delay(1500);
}