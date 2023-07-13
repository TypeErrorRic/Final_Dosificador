/**
 * @file Proyecto de Taller de Ingeniería.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Este programa se encarga de describir el flujo de ejecución del programa
 * @version 1.0
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2022
 */

#ifndef BUILD_H
#define BUILD_H

// Librerias creadas:
#include <Config.h>               //Desarrollado por Ricardo Pabón Serna.
#include <Regresion_Cuadratica.h> //Desarrollado Por Ricardo Pabón Serna.
#include <Celdad_Carga.h>         //Desarrollado por Ricardo Pabón Serna.
#include <Alerta.h>               //Desarrollado por Dilxon Jhon Carlos Gaviria Ruiz y Carlos Pizarro.

// Archivos include con todos los puertos:
#include "C:\Users\ricardo\Desktop\Taller_Ing_II\include\Lista_puertos.h"

#define TIEMPO_REVISION 10 // El tiempo en que se va a efectuar la revisión de los sensor.

/************ FUNCIONES PARA EL CALCULO DE LA REGRESION CUADRATICA **************/

#define DISTANCIA_MINIMA 5

extern short puntos_busquedad[]; // Posiciones o modos de operación.

#define num_elements 5 // Elementos o modos de operación de la apertura de la tolva.

void initRegresionCuadratica();
void doRegresionCuadratica();

/**
 * @note Para la comprobación de los calculos realizador por el programa <<Regresion_Cuadratica.h>
 *       Debe descomentar lqas funcionalidades de test y Get_Matriz.
 */

/***************** FUNCIONES PARA EL MANEJO DEL DISPENSADOR *********************/

bool stateTolva(); // Devuelve el estado de la Tolva dispensadora. True llena / False Vacia.
void fillTolva();  // Activa la función de llenado de la Tolva dispensadora.
void offTolva();   // Dectiene la Función de llenado de la Tolva dispensadora.

/********************* FUNCIONES DEl SISTEMA DE ENVASADO ************************/

bool revisarEnvase(short &Tipo); // Función que permite conocer el tipo de envase. True si hay un envase / False si no hay.
void llenandoEnvase();           // Activa el llenado del envase.
int estadoCantidadTolva();       // Verificar el estado de la tolva.
bool revisarLLenado();           // Verfica el llenado del envase.
float stopLllenadoEnvase();      // Detiene el llenado del envase.

/********************* FUNCIONES PARA SISTEMA DE ALERTA ************************/

void initAlarma();                   // Inizialiar el sistema de Alarma.
void alarma(short type, bool state); // Activa la alarma correspondiente al evento que suceda.

/********************* FUNCIONES PARA MOTORES *********************/

void encenderMotor();          // Encender Alimentador.
void apagarMotor();            // Apagar Alimentador.
void InitPortsCNYRELE();       // Inicializar puertos para CNY, Rele Y Servomotor.
void angulo(int vel);          // Apertura de la tolva dosificadora.
void retornarCerrado(int vel); // Cerrar tolva dosificadora.

/********************** CONFIGURACIÓN SISTEMA ********************/

void ConfigSistem(); // Configuración basica del sistema:.

#endif
