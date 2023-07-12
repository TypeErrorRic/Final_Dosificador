/**
 * @file Config.h
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Contiene el funcionamiento de la celdad de carga.
 * @version 0.1
 * @date 2023-06-11
 *
 * @copyright Copyright (c) 2023
 */

#ifndef CELDAD_CARGA_H
#define CELDAD_CARGA_H

#include <Arduino.h>
#include "C:\Users\ricardo\Desktop\Taller_Ing_II\include\Lista_puertos.h"

// Librerias para la celdad de carga.
#include <HX711.h>

#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#define SIZE_ARREGLO 5 //Media de datos rapida.

#define APROX_PARA_VALOR_CELDAD_CARGA 3 // Número de datos tomados por la celdad de carga.

#define CRITERIO_ENVASE 10 // Criterio minimo para identificar un envase.

#define PESO_COMPROBACION 500.0 // Peso de confirmación

// Lista de pesos de de medición:
#define IMPORTANCIA_1 0.20
#define IMPORTANCIA_2 0.30
#define IMPORTANCIA_3 0.50

// Hay un envase metalico en el sitio de envasado.
bool isEnvaseIn();

// Clase para acceder a las funciones de HX711 por medio de Asociación.
class Celdad_Carga
{
private:
    unsigned int time;       // Tiempo entre toma de datos.
    static bool configHx711; // Config HX711;

public:
    static bool medicion;                                       // Permite definir si se está realizando la medición
    static HX711 celdadCarga;                                   // Clase con las operaciones de obtención de pesos.
    static volatile unsigned int counter;                       // Contador de iteraciones
    static volatile double data[APROX_PARA_VALOR_CELDAD_CARGA]; // Valores de la regresión cuadratica.
    Celdad_Carga()
    {
        celdadCarga.begin(DOUT, CLOCK);
        pinMode(INDUCTIVO, INPUT);
    }                                  // Inicializar HX711.
    void stop();                       // Detiene la toma de datos.
    void begin();                      // Inicia la toma de datos.
    void configTime(unsigned int num); // Configura el tiempo entre toma de datos
};

#define TIME_COMPROBACION 100 // Tiempo entre comprobaciones para permitir saber si hay un envase

// Funciones para cargar los valores de la celdad de carga en la heap.
void initCeldad(unsigned int num);
// Detiene las mediciones.
void stopMediciones();
// Obtener el valor de la celdad de carga.
float getCeldadcargaValue();
// Confirma si hay un envase o no en el sitio de envasado.
bool confirmarEnvase();
// Determinar el envase que hay en el sitio de envasado.
int tipoEnvase();

// Banderas de control del flujo de valores.
typedef struct
{
    volatile bool flagCaptureMedicion;
    volatile bool doDerivada;
} pvMecionParams;

// Captura del dato:
void Captura_dato();

// Estrcutura de datos que se obtienen de la zona de envasado.
typedef struct
{
    float medicionHx;
    float resultDev;
    float angle;
    bool changeMedicion;
} pvResultsMedicion;

extern pvResultsMedicion Medidas;

// Medidas de reconocimiento de los pesos de los envases:
#define ENVASE_1_PESO 31
#define ENVASE_2_PESO 40
#define ENVASE_3_PESO 60

// Rango de redondeo de los valores:
#define VALOR_DESFASE 2
// Porcentaje de error admitido entre 2 medidas
#define VALOR_ERROR 5
// Porcentaje de error admitido entre 2 medidas cuando la última medida es mayor al umbral de que hay un envase.
#define VALOR_ERROR_PASAR 5

bool reconocerEnvaseEnSitioEnvasado(short &tipo);

#include <Alerta.h>

// Reconocimiento del envase

#define ENVASE_1_PESO_COMPLETO pesos[0]
#define ENVASE_2_PESO_COMPLETO pesos[1]
#define ENVASE_3_PESO_COMPLETO pesos[2]

#define LLENADO_DEL_ENVASE_DESFASE 5

// Función para verificar si hay u envase en el sitio de envasado:
bool verficarLlenadoCompleto();

#endif