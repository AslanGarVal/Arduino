/*
 --------------- Theremin con Arduino ---------------
 Prototipo de un theremin cuya frecuencia y volumen se controlan con la distancia 
 de las manos del usuario a dos sensores ultrasónicos de distancia. La frecuencia se 
 emite a la bocina del aparato con la funcion 'tone' y el volumen se controla con un 
 potenciómetro digital.
 
 A. García, C. Medina ---- Feb. 2021
*/


#define CS_Pin 4                 // Chip selector pot digital
#define INC_Pin 3                 // Increment pin pot digital
#define UD_Pin 2                  // Up-down pin pot digital

#define X9C_UP LOW                //Definimos como constantes el nivel máximo del digipot 
#define X9C_DOWN HIGH
#define X9C_MAX 99

#define ECHOPIN_volume 5          // Pin para recibir eco
#define TRIGPIN_volume 6          // Pin para enviar pulso ultrasonico
#define ECHOPIN_pitch 8           // Pin para recibir eco
#define TRIGPIN_pitch 7           // Pin para enviar pulso ultrasonico

#define SPEAKER_Pin  9            // Pin bocina

float frequency;                  // frecuencia 
float volume;                     // volumen
float distance_pitch;             // distancia para control de frecuencia
float distance_volume;            // distancia para control de vol
 
void setup()
   {
    Serial.begin(9600);
    //Inicializamos los pines del digipot
    pinMode(CS_Pin, OUTPUT);         
    pinMode(INC_Pin, OUTPUT);
    pinMode(UD_Pin, OUTPUT);
    //Inicializamos los pines de los sensores de movimiento
    pinMode(ECHOPIN_volume, INPUT);
    pinMode(TRIGPIN_volume, OUTPUT);
    pinMode(ECHOPIN_pitch, INPUT);
    pinMode(TRIGPIN_pitch, OUTPUT);
    //Inicializamos el pin de la bocina
    pinMode(SPEAKER_Pin, OUTPUT);
   }

void stepPot(int cant, int dir){
  /*
  Esta funcion mueve "cant" pasos en la dirección "dir"
  el potenciometro 
  */
  int cnt = (cant > X9C_MAX) ? X9C_MAX : cant; //Asegura que la cantidad < 100
  digitalWrite(UD_Pin, dir); //Seleccionamos el pin de direccion en la dir. especificada
  digitalWrite(CS_Pin, LOW); 
  while(cnt--){ //Hacemos un pulso de caida "cnt" veces para mover el wiper
    digitalWrite(INC_Pin, LOW);
    delayMicroseconds(1);
    digitalWrite(INC_Pin, HIGH);
    delayMicroseconds(1);
  }
  delayMicroseconds(100); //Tiempo de espera para afianzar el valor de la resistencia
}

void AjustarDigiPot(int cant) {
  /*
  Esta funcion coloca el potenciometro en el punto "cant" de la escalera de resistencias
  */
  stepPot(X9C_MAX+1, X9C_DOWN); //primero bajamos hasta 0
  stepPot(cant, X9C_UP); //luego subimos hasta "cant"
  digitalWrite(CS_Pin, HIGH);
}  



float sensorDistancia(int trigPin, int echoPin){
  digitalWrite(trigPin, LOW); //se asegura que este en LOW el sensor
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);//mandamos el pulso
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  float duracion = pulseIn(echoPin, HIGH) / 58.0; //recibimos el tiempo y convertimos a distancia
  return duracion;
}

//=====================
// Loop
//=====================

void loop()
   {
    //==========================
    //      Frecuencia
    //==========================
    distance_pitch = sensorDistancia(TRIGPIN_pitch, ECHOPIN_pitch); //Obtenemos la distancia para frec.

    frequency = 4000.0 / (distance_pitch + 1.0); //calculamos la frecuencia
    //Si la frecuencia es debajo o encima de nuestros umbrales, la ponemos en max o min.
    if (frequency < 30)
       {
        frequency = 0.0;        
       }
      
    if (frequency > 1400)
       {
        frequency = 1400.0;        
       }
  

    // ===============================
    //         Volumen
    // ===============================
    

    
    distance_volume = sensorDistancia(TRIGPIN_volume, ECHOPIN_volume);    // Obtenemos la distancia para el vol.
    
    volume = 100*(1-(exp(-distance_volume)*100)) ;  // Escala exponencial del volumen
    
    //ajustamos el volumen a estar en un cierto rango (es la senal para el pot digital)
    if (volume > 100.0)
       {
        volume = 100.0; 
       }    
    
    if (volume < 0)
       {
        volume = 0.0; 
       }
      
    Serial.print("d = ");
    Serial.print(distance_pitch,0);
    Serial.print("  f = ");
    Serial.print(frequency,0);
    Serial.print("  volume = ");
    Serial.println(volume,0);
              
    tone(SPEAKER_Pin,(int)frequency);   // Enviamos la frec. al pin de la bocina
    
    AjustarDigiPot((int)volume);       // Enviamos el vol. al pin del pot digital
       
    //delay(50);                          
    
}
