Al estar usando un ESP8266 WEMOS D1 R2 MINI, HAY QUE DEFINIR LOS PINES CON LA SIGUIENTE ESTRUCTURA: 
#define LUZ_1 D4 -> SIEMPRE aclarando con la "D" que se va a hacer uso del pin digital, ya que sino el sistema colapsa y reinicia por el acceso a la memoria, debido que determinados pines (como el 1, 6, 7 y 8) se utilizan para el incio del uC
