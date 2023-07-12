#ifndef ALERTAS_H
#define ALERTAS_H

//Pines de funcionamiento de alarma:
#define DELAY_TIME_VISUALIZACION  1000UL
#define DELAY_TIME_SONIDO         1000UL

// PARAMETROS PARA LA CONFIGURACIÓN DEL SONIDO:
// Divisores de sonido:
#define DIVISOR_S1            2
#define DIVISOR_S2_RECONOCE   5
#define DIVISOR_S2_FALLA      4
#define DIVISOR_S3            4
#define DIVISOR_S4            4
#define DIVISOR_S6            2

#include <Arduino.h>

class Alerta
{
private:
  const short bozzer;

public:
  Alerta(short pin)
      : bozzer(pin)                         { pinMode(bozzer, 0x01); }
  void S0();                                // mensaje de estado s1, esperando accionamineto conmutador.
  void S1();                                // mensaje de alerta de envase en la salida.
  void S2();                                // alerta identificacion del tipo de envase.
  void S3();                                // alerta de nivel de tolva.
  void S4();                                // alerta de alimentador encendido.
  void S5();                                // Alerta de llenado de recipiente.
  void S6();                                // Alerta de llenado terminado.
  void Desactivar();                        //Desactivar Alarmas.
};

extern Alerta Alertas;

//Mostrar mensajes sobre el ciclo de envasado:

void setupPantalla();
void MostrarPantalla(bool isEnvasadoEnable, bool Envase);
void setupInteruptMassage();
void isr_handler();

extern int pesos[3];
extern unsigned short velocidadDeLlenado;

#endif