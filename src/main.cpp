/**
 * @file Proyecto de Taller de Ingeniería.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Este programa se encarga de describir el flujo de ejecución del programa
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 */
#include <Build.h>
/**
 * @brief Función que se encarga de configurar los parametros inciales de las funciones.
 */
void setup()
{
  // Inicializar trasmición:
  Serial.begin(VELOCIDA_TX);
  setLCD();
  // Configuracion Base del Sistema:
  // Inicialización de pines:
  pinMode(PORTCONMUT, INPUT_PULLUP);        // Pin del conmutador.
  // Inicialización de funciones
  RevisionSensoresInit(); // Revisar sensores.
  initRegresionCuadratica();
  initAlarma();
  setupCny();
  setupServomotor();
  // Mensaje de finalización de configuración.
}


/**
 * @brief Bucle incial de ejecución.
 */

void loop()
{
  switch (Modo_Configuracion())
  {
  case 0:
    while (1)
      flujo_ejecucion_programa(stateTolva, fillTolva, offTolva, revisarEnvase,
        revisarLLenado, llenandoEnvase, stopLllenadoEnvase, alarma, MostrarPantalla,
        Captura_dato, initCeldad, confirmarEnvase);
    break;
  case 1:
    //Configuración sistema:
    ConfigSistem();
    break;
  case 2:
    EjecucionMemoria();
    break;
  default:
    break;
  }
}

/**
 * @attention Cualquier inquietud informar al propietario del código.
 */