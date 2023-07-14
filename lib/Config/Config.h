/**
 * @file Config.h
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Contiene las configuraciones para el funcionamiento del sistema.
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright (c) 2023
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "C:\Users\ricardo\Desktop\Taller_Ing_II\include\Lista_puertos.h"

/***************** MANEJO DE BITS SOBRE LOS ESTADOS DE ENTRADA *********************/

#define PORTCONMUT 32 // Pin reservado exclusivamente para el conmutador en Arduino.
#define PIN_AVR 6     // Pin reservado exclusivamente para el conmutador.
#define CONMUTADOR !digitalRead(PORTCONMUT)//(0X01 & ~((*((volatile uint8_t *)_SFR_MEM_ADDR(PINB)) & (1 << PIN_AVR)) >> PIN_AVR))

/**
 * @note La conexión del conmutador está hecha en pull_down
 */

/*************************** REGISTRO DE ENTRADA ***********************************/
typedef struct
{
    unsigned char bit0 : 1; // Conmutador
    unsigned char bit1 : 1; // Sensor de nivel de la tolva.
    unsigned char bit2 : 1; // Reconocimiento Si hay un envase o No.
    unsigned char bit3 : 1; // El envase corresponde a uno conocido.
    unsigned char bit4 : 1; // Tipo de envase.
    unsigned char bit5 : 1; // Tipo de envase.
    unsigned char bit6 : 1; // Ciclo de llenado completado
    unsigned char bit7 : 1; // N/A
} regEntrada;

extern volatile unsigned char *REGENTRADAS;

// Lectura de los valores del reg entrada:
#define SENSAR_TOLVA (((volatile regEntrada *)REGENTRADAS)->bit1)
#define RECONOCIMIENTO_ENVASE (((volatile regEntrada *)REGENTRADAS)->bit2)
#define ENVASE_CORRECTO (((volatile regEntrada *)REGENTRADAS)->bit3)
#define CICLO_LLENADO (((volatile regEntrada *)REGENTRADAS)->bit6)
#define ENVASE_1 (((volatile regEntrada *)REGENTRADAS)->bit4)
#define ENVASE_2 (((volatile regEntrada *)REGENTRADAS)->bit5)

// Escritura de los valores del reg entrada:
#define SETTERBIT(bit) (*REGENTRADAS |= (1 << (bit)))
#define LIMPIARBIT(bit) (*REGENTRADAS &= ~(1 << (bit)))

// Modificar el bit de Sensor tolva:
#define MSENSORTOLVA(value) \
    if ((value) == 0)       \
    {                       \
        LIMPIARBIT(1);      \
    }                       \
    else if ((value) == 1)  \
    {                       \
        SETTERBIT(1);       \
    }

// Modificar el bit del Reconocimiento del envase:
#define MRECONOCIMIENTO_ENVASE(value) \
    if ((value) == 0)                 \
    {                                 \
        LIMPIARBIT(2);                \
    }                                 \
    else if ((value) == 1)            \
    {                                 \
        SETTERBIT(2);                 \
    }

// Modificar el bit de Envase correcto:
#define MENVASE_CORRECTO(value) \
    if ((value) == 0)           \
    {                           \
        LIMPIARBIT(3);          \
    }                           \
    else if ((value) == 1)      \
    {                           \
        SETTERBIT(3);           \
    }

// Modificicar el bit de ciclo de llenado:
#define MCICLO_LLENADO(value) \
    if ((value) == 0)         \
    {                         \
        LIMPIARBIT(6);        \
    }                         \
    else if ((value) == 1)    \
    {                         \
        SETTERBIT(6);         \
    }

/*********** Modificar los bits de llenado **********/
// Modificar el bit msb:
#define MODIFICAR_1(value) \
    if ((value) == 0)      \
    {                      \
        LIMPIARBIT(4);     \
    }                      \
    else if ((value) == 1) \
    {                      \
        SETTERBIT(4);      \
    }

// Modificar el bit lsb:
#define MODIFICAR_0(value) \
    if ((value) == 0)      \
    {                      \
        LIMPIARBIT(5);     \
    }                      \
    else if ((value) == 1) \
    {                      \
        SETTERBIT(5);      \
    }

// Función para identificar el tipo de envase:
inline int tipoEnvase()
{
    int tipo = 0;
    if (ENVASE_1)
    {
        if (ENVASE_2)
            tipo = 3;
        else
            tipo = 2;
    }
    else
    {
        if (ENVASE_2)
            tipo = 1;
        else
            tipo = 0;
    }
    return tipo;
}

#define TIPO_ENVASE tipoEnvase() // Tipo de envase:

// Permite obtener una copia del registro.
inline volatile unsigned char getRegEntrada() { return *REGENTRADAS; }

#define MTIPO_ENVASE(type)       \
    do                           \
    {                            \
        MENVASE_CORRECTO(1);     \
        switch (type)            \
        {                        \
        case 0:                  \
            MODIFICAR_1(0);      \
            MODIFICAR_0(0);      \
            break;               \
        case 1:                  \
            MODIFICAR_1(0);      \
            MODIFICAR_0(1);      \
            break;               \
        case 2:                  \
            MODIFICAR_1(1);      \
            MODIFICAR_0(0);      \
            break;               \
        case 3:                  \
            MODIFICAR_1(1);      \
            MODIFICAR_0(1);      \
            break;               \
        default:                 \
            MODIFICAR_1(0);      \
            MODIFICAR_0(0);      \
            MENVASE_CORRECTO(0); \
            break;               \
        }                        \
    } while (0)

/////////////////////////LIBRERIA CON ARDUINO//////////////////////////////
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// CONFIGURACIÓN DEL ARDUINO:
#define VELOCIDA_TX 9600 // Velocidad Baudrate de trasmición.
#define NFILAS 5         // Número de datos tomados.

extern volatile unsigned short mostrarMensaje;

extern short Estado; // Número de estados.

/*********************FUNCIONES DE IMPRECIÓN EN LCD **************************/
// Inicializar LCD
void setLCD();

extern LiquidCrystal_I2C Lcd;

// Obtener LCD.
LiquidCrystal_I2C &getLcd();

// Escribir en la LCD:
template <typename T>
void escribirLcd(const T estado, short fila, short columna, bool limpiar = false)
{
    LiquidCrystal_I2C lcd = getLcd();
    if (limpiar)
        lcd.clear();
    lcd.setCursor(columna, fila);
    lcd.print(estado);
}

/*************FUNCIONES PARA LA SELECCIÓN DEL MODO DE FUNCIONAMIENTO*********/
short Modo_Configuracion();

/**FUNCIONES PARA EL MANEJO DEl FLUJO DEL PROGRAMA Y EJECUCIÓN DEL SISTEMA**/
#define TIME_CONFIRMACION_CELDAD 100

// Función de flujo de control:
void flujo_ejecucion_programa(bool (*revisarTolva)(void), void (*llenarTolva)(void), void (*ApagarTolva)(void),
                              bool (*revisarEnvase)(short &), bool (*llenado)(void), void (*doLlenado)(void),
                              float (*stopLlenado)(void), void (*alerta)(short type, bool state), void (*MenuPrincipal)(bool, bool(*)(void)),
                              void (*actualizar)(void), void (*initCeldad)(unsigned int num), bool (*confirmarEnvase)(void));

// Función de Inicialización de registro y Memoria:
void RevisionSensoresInit();

// Función con las funciones de verificación de sensores:
void Revision_variables(bool (*revisarTolva)(void), void (*llenarTolva)(void),
                        bool (*revisarEnvase)(short &), bool (*llenado)(void), void (*alerta)(short, bool),
                        void (*MenuPrincipal)(bool, bool(*)(void)), bool (*confirmarEnvase)(void));

// Getter: retorna Estado:
const short &getEstado();
const short getModoOperacion();

// Función del manejo de Memoria:
void Imprimir_dato();
void EjecucionMemoria();

////////// VARIABLES BANDERAS PARA EL FUNCIONAMIENTO DEL SISTEMA //////////

extern bool Cambio;

/**
 * @note Las funciones para el control del flujo de programa está desarrolladas en el
 *       Paradigma de programación orientada a registros.
 */

#endif