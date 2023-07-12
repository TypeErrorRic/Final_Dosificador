#include <Config.h>
#include <Memoria.h>

/********************* IMPLEMENTACIÓN DE IMPRECIÓN EN LCD **************************/

volatile unsigned char *REGENTRADAS = new unsigned char();

volatile unsigned short mostrarMensaje = 0;

static bool ejecutarMenu = true;

//Variables para el cambio de la pantalla de la LCD:
bool Cambio = true;

LiquidCrystal_I2C  Lcd(0x27,16,2);

void setLCD()
{
    Wire.begin();
    Lcd.begin(16,2);
    Lcd.backlight();
    Lcd.setCursor(0, 0);
    Lcd.print("Ready");
}

static void escribirEstado(short estado, void (*alarma)(short, bool), bool& realizar, void (*MenuPrincipal)(bool))
{
    if(Estado == 5 && ejecutarMenu)
    {
      MenuPrincipal(true);
      ejecutarMenu = false;
    }
    if (!mostrarMensaje)
    {
      //Apagar alarmas
      digitalWrite(7, LOW);
      Lcd.clear();
      Lcd.setCursor(0, 0);
      Lcd.print("Estado:");
      Lcd.print(estado);
      Lcd.setCursor(11, 0);
      Lcd.print("E1:");
      Lcd.print(*(NUM_ENVASES[*NUM_CICLO_FINAL])[0]);
      Lcd.setCursor(0, 1);
      Lcd.print("E2:");
      Lcd.print(*(NUM_ENVASES[*NUM_CICLO_FINAL])[1]);
      Lcd.setCursor(11, 1);
      Lcd.print("E3:");
      Lcd.print(*(NUM_ENVASES[*NUM_CICLO_FINAL])[2]);
    }
    else
    {
      alarma(estado, realizar);
    }
}

LiquidCrystal_I2C  &getLcd()
{
  return Lcd;
}

/********************* IMPLEMENTACIÓN DE MODO OPERACIÓN **************************/
static unsigned long timer = 0; //Compartida con Memoria.
static short count = 0; //Compartida con Memoria.
static bool crash = true; //Compartida con Memoria.

#define DELAY_MESSAGE         3000UL
#define TIEMPO_ESPERA         5000UL

//Función de inicialización de los modos de operación del sistema:
static bool Modo_seleccion()
{
    if(*VARIABLE_REST) return true;
    unsigned int time = millis();
    bool aux = true;
    while (millis() - time <= TIEMPO_ESPERA)
    {
        if (CONMUTADOR == aux)
        {
            aux = false;
            count++;
        }
        if (count >= 6)
        {
          count = 0;
          timer = millis();
          return true;
        }
    }
    count = 0;
    timer = millis() - DELAY_MESSAGE;
    return false;
}

//Función de inicialización de los modos de operación del sistema:
short Modo_Configuracion()
{
  if (Modo_seleccion())
  {
    ++(VARIABLE_REST = 1);
    while(crash)
    {
      switch (count)
      {
      case 0:
        if ((millis() - timer) > DELAY_MESSAGE)
        {
          escribirLcd<String>("Modo de", 0, 0, true);
          escribirLcd<String>("Operacion.", 1, 0);
          timer = millis();
          count++;
        }
        break;
      case 1:
        if ((millis() - timer) > DELAY_MESSAGE)
        {
          escribirLcd<String>("1. Done", 0, 0, true);
          escribirLcd<String>("2. Config", 1, 0);
          timer = millis();
          count++;
        }
        break;
      case 2:
        if ((millis() - timer) > DELAY_MESSAGE)
        {
          escribirLcd<String>("3. Memoria.", 0, 0, true);
          timer = millis();
          count++;
        }
        break;
      default:
        count = 0;
        break;
      }  
      if((Serial.available() > 0) && crash)
      {
        switch ((char)Serial.read())
        {
        case '1':
          MODO_OPERACION = 0;
          crash = false;
          break;
        case '2':
          MODO_OPERACION = 1;
          crash = false;
          break;
        case '3':
          MODO_OPERACION = 2;
          crash = false;
          break;
        default:
          break;
        }
      }      
    }
    if(!crash)
    {
      ++MODO_OPERACION;
      if (MODO_OPERACION != 1) {++(VARIABLE_REST = 0);}
      if (MODO_OPERACION == 1) {crash = true;}
      count = 0; 
    } 
  }
  if (MODO_OPERACION != 1) {crash = false;}
  timer = millis();
  return *MODO_OPERACION;
}

/**************** IMPLEMENTACIÓN DE FUNCIONES PARA EL MANEJO DE LA MEMORIA ****************/

//Permite obtener todos los datos Guardados en la EPPROM:
void Imprimir_dato()
{
  if(*NUM_CICLO_FINAL == 5) 
      Serial.println("|...ACTUAL...|");
  for(unsigned short i = *NUM_CICLO_FINAL; i > (*NUM_CICLO_FINAL-5); i--)
  {
    Serial.print("Ciclo: ");
    Serial.println((*NUM_CICLO_FINAL - i) + 1);
    Serial.println("# Envase: ");
    for(unsigned int j = 0; j < NUM_ELEMENTOS_ARREGLO(1); j++) 
    {
      Serial.print(j+1);
      Serial.print(". ");
      Serial.println(*((NUM_ENVASES[(*NUM_CICLO_FINAL - i)])[j]));
    }
    Serial.print("Peso Total: ");
    Serial.println(*(PESO[*NUM_CICLO_FINAL - i]));
    if((((*NUM_CICLO_FINAL - i)) == ((*NUM_CICLO_FINAL- 1) % NUM_ELEMENTOS_ARREGLO(0))) && !(*NUM_CICLO_FINAL == 5)) 
      Serial.println("|...ACTUAL...|");
  }
  Serial.print("F. Celdad: ");
  Serial.println(*FACTOR_CELDADCARGA);
  Serial.print("[X2, X, A]=");
  Serial.print(*X2);
  Serial.print(',');
  Serial.print(*X);
  Serial.print(',');
  Serial.println(*A);
  Serial.print("# ciclos: ");
  Serial.println(*NUM_CICLO_FINAL - 5);
  Serial.println("Scale: ");
  Serial.println(*SCALE);
}
//Función para Visualización de datos alamancenados en la EEPROM.
void EjecucionMemoria()
{
  while(!crash)
  {
    switch (count)
    {
    case 0:
      if ((millis() - timer) > DELAY_MESSAGE)
      {
        escribirLcd<String>("Operacion", 0, 0, true);
        escribirLcd<String>("Memoria.", 1, 0);
        timer = millis();
        count++;
      }
      break;
    case 1:
      if ((millis() - timer) > DELAY_MESSAGE)
      {
        escribirLcd<String>("1. Datos", 0, 0, true);
        escribirLcd<String>("2. Reset. Celda", 1, 0);
        timer = millis();
        count++;
      }
      break;
    case 2:
      if ((millis() - timer) > DELAY_MESSAGE)
      {
        escribirLcd<String>("3. Limpiar.", 0, 0, true);
        escribirLcd<String>("4. Salir.", 1, 0);
        timer = millis();
        count++;
      }
      break;
    default:
      count = 0;
      break;
    }
    if((Serial.available() > 0) && !crash)
    {
      switch ((char)Serial.read())
      {
      case '1':
        Imprimir_dato();
        break;
      case '2':
        limpirSitioEnvasado();
        VARIABLE_REST = 1;
        crash = true;
        //Activar para Impresión T.
        //for(unsigned short i=0; i< MODO_OPERACION.size(); i++) 
          //{Serial.print(i); Serial.print(". "); Serial.println(MODO_OPERACION.M_DDR(i));}
        break;
      case '3':
        LIMPIAR();
        VARIABLE_REST = 1;
        crash = true;
        break;
      case '4':
        VARIABLE_REST = 1;
        crash = true;
        break;
      default:
        break;
      }
    }
  }
}

/*********** IMPLEMENTACIÓN DE FUNCIONES PARA EL MANEJO DEL FLUJO DEL PROGRAMA ************/

short Estado = 0; // Número de estados.
static short tipo = 0;   // Tipo de Envase.
static bool Alert = false;

// Revision de las diferentes entradas del sistema.
void RevisionSensoresInit()
{
    // Revisar cada entrada:
    *REGENTRADAS = 0x00;
}

// Revision de los sensores de cada sistema:
void Revision_variables(bool(*revisarTolva)(void), void(*llenarTolva)(void), 
                      bool(*revisarEnvase)(short &), bool(*llenado)(void), void(*alerta)(short type, bool state),
                      void (*initCeldad)(unsigned int num))
{
  switch (Estado)
  {
  case 0:
    Alert = true;
    if(crash == false)
    {
      ++(PESO[*NUM_CICLO_FINAL] = 0);
      NUM_ENVASES.Begin(*NUM_CICLO_FINAL);
      crash = true;
    }
    Cambio = true;
    break;
  case 1: // Revisa si hay un envase en la zona de envasado.
    if(revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(1);}
    else {MRECONOCIMIENTO_ENVASE(0); MCICLO_LLENADO(0);}
    Cambio = false;
    break;
  case 2: // Revisa si se ha quitado el envase de la zona de envasado.
    if(!revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(0); MCICLO_LLENADO(0);}
    break;
  case 3: // Revisa si la tolva dosificadora tiene suficiente material para realizar un llenado.
    if(revisarTolva()) {MSENSORTOLVA(1);}
    else MSENSORTOLVA(0) {;}
    Cambio = false;
    break;
  case 4: // Revisa si la tolva dosificadora ya se ha llenado.
    llenarTolva();
    break;
  case 5: // Revisa si se ha colocado un envase en la zona de envasado.
  	initCeldad(TIME_CONFIRMACION_CELDAD); //Inicializar la celdad de carga.
    if(revisarEnvase(tipo)) 
    {
      MRECONOCIMIENTO_ENVASE(1);
      MTIPO_ENVASE(tipo);
      if(llenado()) {MCICLO_LLENADO(1);}
    }
    break;
  case 6: // Revisa si el envase no reconocido se ha quitado de la zona de envasado.
    if(!revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(0)};
    break;
  case 7: // Revisa si el envase se ha llenado correctamente.
    if(!revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(0)};
    if(llenado()) {MCICLO_LLENADO(1);}
    break;
  case 8: // Revisa si se ha quitado el envase reconocido pero con material del ciclo de envasado.
    if(!revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(0); MCICLO_LLENADO(0);}
  default:
    break;
  }
}

// Flujo de Ejcución del sistema.
void flujo_ejecucion_programa(bool(*revisarTolva)(void), void(*llenarTolva)(void), void(*ApagarTolva)(void), 
                              bool(*revisarEnvase)(short &), bool(*llenado)(void), void(*doLlenado)(void),
                              float (*stopLlenado)(void), void(*alerta)(short type, bool state), 
                              void (*MenuPrincipal)(bool), void (*actualizar)(void), void (*initCeldad)(unsigned int num))
{
  actualizar();
  //Revision de todos los sensores:
  Revision_variables(revisarTolva, llenarTolva, revisarEnvase, llenado, alerta, initCeldad);
  //Flujo del programa:
  switch (Estado)
  {
  case 0: // Encender Conmutador
    if (CONMUTADOR) Estado = 1;
    else Estado = 0;
    ejecutarMenu = true;
    break;
  case 1: // ¿Hay un envase?
    if (CONMUTADOR)
    {
      //Mostrar que se finalizo el mensaje:
      escribirEstado(Estado, alerta, Alert, MenuPrincipal);
      //Reconocer a que estado se va:
      if (RECONOCIMIENTO_ENVASE) Estado = 2;
      else Estado = 3;
      Cambio = true;
    }
    else Estado = 0;
    break;
  case 2: // Alerta de Envase.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE) Estado = 2;
      else {Estado = 3; Alert = false;}
    }
    else Estado = 0;
    break;
  case 3: // ¿Cuál es el estado de la tolva?
    //Mostrar que se finalizo el mensaje:
    escribirEstado(Estado, alerta, Alert, MenuPrincipal);
    //Determinar que estado va:
    if (revisarTolva()) {Estado = 5; if(!SENSAR_TOLVA) {MSENSORTOLVA(1);}}
    else Estado = 4;
    break;
  case 4: // Encender Alimentador
    while (!revisarTolva()); // Revisar sensor de la tolva por polling.
    MSENSORTOLVA(1);// Se ha llenado.
    ApagarTolva(); // Apagar_Dispensador.
    Estado = 5;
  case 5: // Reconocimiento de envase
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE)
      {
        if (ENVASE_CORRECTO)
        {
          if (CICLO_LLENADO) Estado = 8;
          else 
          {
            doLlenado();
            Estado = 7;
          }
        }
        else Estado = 6;
      }
      else Estado = 5;
    }
    else Estado = 0;
    break;
  case 6: // Alerta de no reconocimiento del envase.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE) Estado = 6; // Revisar si el envase se quito. Polling
      else {Estado = 5; Alert = false;};
    }
    else Estado = 0;
    break;
  case 7: // Llenado
    if (!CICLO_LLENADO)
    {
      // Ya se lleno.
      if (RECONOCIMIENTO_ENVASE) Estado = 7;
      else
      {
        if (CONMUTADOR) Estado = 3;
        else Estado = 0;
      }
    }
    else
    {
      ++((PESO[*NUM_CICLO_FINAL]) += stopLlenado()); //Guarda el Peso del llenado.
      if(TIPO_ENVASE != 0) 
        {((NUM_ENVASES[*NUM_CICLO_FINAL])[TIPO_ENVASE - 1]) += 1;} //Guarda el número de envases.
      MSENSORTOLVA(0); //Se toma como vacia la tolva después del ciclo de llenado.
      if (CONMUTADOR) Estado = 1;
      else
      { 
        Estado = 0;
        NUM_CICLO_FINAL += 1; //Prepara el siguiente ciclo de llenado del siguiente día.
        crash = false;
      }
    }
    break;
  case 8: // Alerta de reconocimiento de envase ya dosificado.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE) Estado = 8;
      else {Estado = 5; Alert = false;};
    }
    else Estado = 0;
    break;
  default:
    break;
  }
  escribirEstado(Estado, alerta, Alert, MenuPrincipal);
  //Tiempo de ejecución:
  delay(DELAY_EJE);
}

/*********** IMPLEMENTACIÓN DE FUNCIÓN PARA OBTENER EL ESTADO DEL SISTEMA ****************/

const short &getEstado()
{
  return Estado;
}

const short getModoOperacion()
{
  return *MODO_OPERACION;
}