
/*Monitoreo de CO2 y apertura de bentana con sensores de abierta y cerrada
 * Envio de datos de los sensores MQ135 (CO2)
 * por: Jose Luis Oviedo Barriga, David Garcia Sarmiento
 * Fecha: 29 de junio de 2022
 * 
 * Este programa lee los sensores MQ135 (CO2) y Pir Hcsr501 (presencia) por MQTT
 *            CO2                        PRESENCIA                    
 * ESP32      MQ135PinAtras      ESP32      Pir Hcsr501   ESP32      Ventilador
 * I012 ------    1 Datos        GND ------ 1 GND         I013 ------1 Datos    
 * GND -------    3 GND          I016 ------2 Datos        
 * VCC -------    4 Vin          VCC -------3 Vin                                                                  
 *        
 * Asignacion de pines DISPONIBLES ESP32CAM
 * IO4    Abrir     En 1 Abrir ventana
 * IO2    Cerrar    En 1 Cerrar ventana
 * IO14   Sv        Sensor de ventana activada
 * IO15   Venti     Rele Ventilador  
 * IO12   CO2       Sensor CO2
 * 
 */

 
// Constantes*************************************************************************
float ValorCO2;     // variable para guardar el valor analógico del sensor
int ValorSv=0;       // variable para guardar el valor digital del sensor de completamente abierto
int EdoVen=0;       //Condicion inicial Ventana cerrada  EdoVen=1 Abierta // EdoVen=0 Cerrada

// Variables**************************************************************************
long timeNow, timeLast; // Variables de control de tiempo no bloqueante
int data = 0; // Contador
int wait = 5000;  // Indica la espera cada 5 segundos para envío de mensajes MQTT

// Definición de objetos**************************************************************

  #define Sv 14
  #define Venti 15
  #define CO2 12
  #define Abrir 4
  #define Cerrar 2
  

// Condiciones iniciales - Se ejecuta sólo una vez al energizar***********************
void setup() {
  
     pinMode(Abrir,OUTPUT);
     pinMode(Cerrar,OUTPUT);
     pinMode(Venti,OUTPUT);
     pinMode(Sv,INPUT);

     digitalWrite(Abrir,LOW);
     digitalWrite(Cerrar,LOW);
     digitalWrite(Venti,LOW);
     
     Serial.begin (115200);
     Serial.println("El sensor de gas se esta pre-calentando");
     delay(5000); // Espera a que el sensor se caliente durante 20 segundos

     timeLast = millis (); // Inicia el control de tiempo     

}// Fin de void setup*****************************************************************


void loop(){     //VOID LOOP**********************************************************///////////////////

    timeNow = millis(); // Control de tiempo para esperas no bloqueantes
      if (timeNow - timeLast > wait) { // Manda un mensaje por MQTT cada cinco segundos
      timeLast = timeNow; // Actualización de seguimiento de tiempo
      COdos();                              //Funcion deteccion de CO2
    }// fin del if (timeNow - timeLast > wait)

}// Fin de void loop*****************************************************************///////////////////




// Funcioes del usuario
//----------------------- CO2---------------------------

  void COdos(){                //Esta funcion realiza el sensado de presencia
               ValorCO2 = analogRead(CO2); // lectura de la entrada analogica "A0""
               Serial.print("Valor detectado por el sensor: ");
               Serial.print(ValorCO2);
  
                    if(ValorCO2 > 600)   // La OMS sugiere de 400 a 600
                       {
                       Serial.print("  ¡Se ha detectado CO2!  ");
                        digitalWrite(Venti,HIGH);  //Enciende Ventilador
                           if(EdoVen==0){Open();}                     //Abre Ventana
                        delay(1000);
                      }
                     else{
                       Serial.print("");
                       digitalWrite (Venti, LOW);
                           if(EdoVen==1){Close();}                     //Cierra Ventana
                       delay(1000);
                     }
                     
               Serial.println("");
              } 


//------------------------- Abrir Ventana  -----------------------------
  void Open(){                            //Esta funcion abre la ventana
              digitalWrite(Abrir,HIGH);
              digitalWrite(Cerrar,LOW);
              Serial.print(" º ABRIENDO º");
              delay(9000);
              
                do{
                    digitalWrite(Abrir,HIGH);
                    digitalWrite(Cerrar,LOW);
                    Serial.println(" º ABRIENDO º");
                    ValorSv=digitalRead(Sv);
                   }while(ValorSv);
                   
              EdoVen=1;    
              digitalWrite(Abrir,LOW);
              digitalWrite(Cerrar,LOW);      
              Serial.print(" º DESACTIVADO º");        
             }

//--------------------------- Cerrar Ventana  -------------------------------
  void Close(){                            //Esta funcion cierra la ventana
               digitalWrite(Abrir,LOW);
               digitalWrite(Cerrar,HIGH);
               Serial.print(" º CERRANDO º");
               delay(5000);
               
                do{
                    digitalWrite(Abrir,LOW);
                    digitalWrite(Cerrar,HIGH);
                    Serial.println(" º CERRANDO º");
                    ValorSv=digitalRead(Sv);
                  if(!ValorSv){delay(7500);}    //Tiempo para que termine de cerrar la ventana (el sensor esta antes del cierre total)
                  }while(ValorSv);
                  
              EdoVen=0;    
              digitalWrite(Abrir,LOW);
              digitalWrite(Cerrar,LOW); 
              Serial.print(" º DESACTIVADO º"); 
             }


  