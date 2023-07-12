#include <Alerta.h>
#include <Config.h>
#include <EnableInterrupt.h>
#include <Memoria.h>

// CÓDIGO DEL FUNCIONAMIENTO DEL SISTEMA DE ALARMAS:
static unsigned long time_Alerta = 0;
static unsigned long time_Visual = 0;

bool CambiarPantalla = 0;

//Variables de Salir del Menudo:
static bool SalirMenudo = true;
static unsigned short contadorSalir = 0;


// Declaración de clase:
Alerta Alertas(BUZZER);

void setupInteruptMassage()
{
    pinMode(interruptorMaestro, INPUT);
    enableInterrupt(interruptorMaestro, isr_handler, RISING);
}

void isr_handler()
{
    mostrarMensaje = !mostrarMensaje;
    if(Estado ==  5)
    {
        if(contadorSalir == 3)
        {
            SalirMenudo = false;
        }
        contadorSalir++;
    }
}

void Alerta::S0() // mensaje de estado s1, esperando accionamineto conmutador
{
    if (DELAY_TIME_SONIDO < (millis() - time_Alerta))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
    if (DELAY_TIME_VISUALIZACION < (millis() - time_Visual))
    {
        if (Cambio)
        {
            escribirLcd<String>("Esperando", 0, 0, true);
            escribirLcd<String>("Accionamiento", 1, 0);
            Cambio = false;
        }
        else
        {
            escribirLcd<String>("Conmutador", 0, 0, true);
            Cambio = true;
        }
        time_Visual = millis();
    }
}

void Alerta::S1() // mensaje de alerta de envase en la salida
{
    escribirLcd<String>("Identificando Si", 0, 0, true);
    escribirLcd<String>("Hay una lata", 1, 0);
    if (!Cambio)
    {
        short aux = 0;
        if (RECONOCIMIENTO_ENVASE)
            escribirLcd<String>("Hay un envase", 0, 0, true);
        else
            escribirLcd<String>("No Hay un envase", 0, 0, true);
        while (aux <= 6)
        {
            if (((unsigned long)DELAY_TIME_SONIDO / DIVISOR_S1) < (millis() - time_Alerta))
            {
                digitalWrite(bozzer, !digitalRead(bozzer));
                time_Alerta = millis();
                aux++;
            }
        }
    }
}

void Alerta::S2() // alerta identificacion del tipo de envase
{
    if (DELAY_TIME_VISUALIZACION < (millis() - time_Visual))
    {
        escribirLcd<String>("Retire", 0, 0, true);
        escribirLcd<String>("El recipiente", 0, 0);
        time_Visual = millis();
    }
    if ((unsigned int)DELAY_TIME_SONIDO / DIVISOR_S2_RECONOCE < (millis() - time_Alerta))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
}

void Alerta::S3() // alerta de nivel de tolva
{
    if (Cambio)
    {
        escribirLcd<String>("Comprobando ", 0, 0, true);
        escribirLcd<String>("Nivel tolva.", 1, 0);
        delay(2000);
        Cambio = false;
    }
    else
    {
        if ((*REGENTRADAS & 0x02) & ~(1 << 2)) // nivel bajo o vacio tolva
        {
            escribirLcd<String>("Nivel de tolva", 0, true);
            escribirLcd<String>("Maximo.", 1, 0);
        }
        else
        {
            escribirLcd<String>("Nivel de tolva", 0, 0, true);
            escribirLcd<String>("Bajo.", 1, 0);
        }
        Cambio = true;
        delay(2000);
    }
}

void Alerta::S4() // alerta de alimentador encendido
{
    escribirLcd<String>("Alimentador", 0, 0, true);
    escribirLcd<String>("encendido", 1, 0);
    if (((unsigned int)DELAY_TIME_SONIDO / DIVISOR_S4) < (millis() - time_Alerta))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
}

void Alerta::S5() // Alerta de llenado de recipiente
{
    escribirLcd<String>("Coloque un", 0, 0, true);
    escribirLcd<String>("Envase", 1, 0);
}

void Alerta::S6() // Alerta de llenado terminado
{
    escribirLcd<String>("Error", 0, 0, true);
    if (((unsigned int)DELAY_TIME_SONIDO / DIVISOR_S6) < (millis() - time_Alerta))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
}

void Alerta::Desactivar()
{
    digitalWrite(bozzer, LOW);
}

//// Variables auxiliares ////
static short aux = 0;
static short reset = 0;
static short mostrando = 0;        /* Variable utilizada para determinar si esta siendo mostrada la opción seleccionada en el menú
                             Se utiliza en la función titilarSeleccion*/
static const long interval = 1500; // Intervalo de tiempo para el titilar de la selección.
static char auxCadena[16];

static int del = 200; // Delay para evitar el ruido.

//// Variables del sistema ////

unsigned short velocidadDeLlenado = 4;

static int PESOSMAXIMOS[3] = {179, 282, 330}; // Pesos de los tres tipos de recipientes cuando están llenos.
static int PESOSMINIMOS[3] = {31, 40, 50};    // Pesos de los tres tipos de recipientes cuando está con la cantidad mínima.

//Pesos de los envases:
int pesos[3] = {179, 282, 330}; // Pesos del envase 1, 2 y 3 respectivamente (incluyendo el peso de la lata vacía).
                                       // NOTA: el peso 3 no se ha definido aún, por lo que por ahora se define (arbitrariamente) como 330.

/* Variables de Menú */
static char MENU[3][17] = {"1-Vel.Llenado(5)", "2-CambiarPesos", "3-Estadísticas"}; // Menú inicial
static int seccionMenu = 0;                                                         /* Cual sección del menú está siendo mostrada (0=: sección 1 MENU[0:3]
                                                                                                                            1=: sección 2 MENU[3])*/
static int opcion = 0;                                                              // Opción del menú seleccionada al momento (esta titila).

/// Variables para el uso de millis()
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

static void titilarSeleccion(int seccion, int opcion, int reset)
{
    // Se utiliza millis() para mostrar y ocultar la selección actual con el fin de que esta titile.

    if (reset == 0)
    {
        currentMillis = millis();
    }
    if (mostrando == 0 && currentMillis - previousMillis >= interval / 2)
    {
        switch (seccion)
        {
        case 0:
            Lcd.clear();
            Lcd.setCursor(0, 0);
            Lcd.print(MENU[0]);
            Lcd.setCursor(0, 1);
            Lcd.print(MENU[1]);
            mostrando = 1;
            break;
        case 1:
            Lcd.clear();
            Lcd.setCursor(0, 0);
            Lcd.print(MENU[2]);
            mostrando = 1;
            break;
        }
        previousMillis = currentMillis;
    }
    else if (mostrando == 1 && currentMillis - previousMillis >= interval)
    {
        switch (seccion)
        {
        case 0:
            Lcd.clear();
            Lcd.setCursor(0, !opcion);
            Lcd.print(MENU[!opcion]);

            mostrando = 0;
            break;

        case 1:
            Lcd.clear();
            mostrando = 0;
            break;
        }
        previousMillis = currentMillis;
    }
}

static void SeleccionDeVelocidad()
{
    // El usuario selecciona entre las 5 velocidades de llenado.
    Lcd.clear();
    Lcd.setCursor(0, 0);
    Lcd.print("Velocidad de");
    int vel_actual = velocidadDeLlenado;
    delay(del);

    while (1)
    {
        // Se cambia la velocidad.
        if (digitalRead(botonArriba) == LOW && velocidadDeLlenado < 5)
        {
            velocidadDeLlenado++;
        }
        else if (digitalRead(botonAbajo) == LOW && velocidadDeLlenado > 1)
        {
            velocidadDeLlenado--;
        }
        Lcd.setCursor(0, 1);
        sprintf(auxCadena, "llenado: %d", velocidadDeLlenado);
        Lcd.print(auxCadena);
        delay(del);

        // Se confirma ó cancela el cambio.
        if (digitalRead(botonCancelar) == LOW)
        {
            velocidadDeLlenado = vel_actual;
            delay(del);
            break;
        }
        else if (digitalRead(botonSeleccionar) == LOW)
        {
            sprintf(auxCadena, "1-Vel.Llenado(%d)", velocidadDeLlenado);
            // Se actualiza la primera sección del menú para que muestre la velocidad de llenado actual.
            strcpy(MENU[0], auxCadena);
            strcpy(MENU[1], "2-CambiarPesos");
            delay(del);
            break;
        }
    }
}

static void CambiarPeso(int tipoDeEnvase)
{
    // Se configura el peso del tipo de envase seleccionado.

    aux = pesos[tipoDeEnvase - 1];
    Lcd.clear();
    Lcd.setCursor(0, 0);
    Lcd.print("Seleccione el");
    Lcd.setCursor(0, 1);
    Lcd.print("nuevo peso: ");
    Lcd.setCursor(12, 1);
    Lcd.print(aux);

    while (1)
    {

        // El usuario puede aumentar/disminuir de 10 en 10 el peso siempre y cuando no sobrepase los límites establecidos.
        if (digitalRead(botonArriba) == LOW && aux < PESOSMAXIMOS[tipoDeEnvase - 1])
        {

            if (aux + 10 > PESOSMAXIMOS[tipoDeEnvase - 1])
            {
                aux = PESOSMAXIMOS[tipoDeEnvase] - 1;
            }
            else
            {
                aux = aux + 10;
            }
            Lcd.setCursor(12, 1);
            Lcd.print("    ");
            Lcd.setCursor(12, 1);
            Lcd.print(aux);
            Lcd.setCursor(0, 0);
            delay(del);
        }
        else if (digitalRead(botonAbajo) == LOW && aux > PESOSMINIMOS[tipoDeEnvase - 1])
        {

            if (aux - 10 < PESOSMINIMOS[tipoDeEnvase - 1])
            {
                aux = PESOSMINIMOS[tipoDeEnvase - 1];
            }
            else
            {
                aux = aux - 10;
            }
            Lcd.setCursor(12, 1);
            Lcd.print("    ");
            Lcd.setCursor(12, 1);
            Lcd.print(aux);
            Lcd.setCursor(0, 0);
            delay(del);
        }

        // Se confirma ó cancela la modificación.

        if (digitalRead(botonCancelar) == LOW)
        {
            aux = 0;
            delay(del);
            break;
        }
        else if (digitalRead(botonSeleccionar) == LOW)
        {
            pesos[tipoDeEnvase - 1] = aux;
            Lcd.clear();
            Lcd.setCursor(0, 0);
            Lcd.print("Se ha cambiado");
            Lcd.setCursor(0, 1);
            sprintf(auxCadena, "el peso %d (%d)", tipoDeEnvase, pesos[tipoDeEnvase - 1]);
            Lcd.print(auxCadena);
            delay(3000);
            Lcd.clear();
            aux = 0;
            Lcd.setCursor(0, 0);
            break;
        }
    }
}

static void configPesos()
{
    // Se seleccionar el tipo de envase al cual cambiar su peso objetivo (aux = tipo de envase).

    aux = 1;

    Lcd.clear();
    Lcd.setCursor(0, 0);
    Lcd.print("Tipo de envase:");

    while (1)
    {
        delay(del);
        Lcd.setCursor(0, 1);
        Lcd.print(aux);

        if (digitalRead(botonArriba) == LOW && aux != 3)
        {
            aux++;
        }
        else if (digitalRead(botonAbajo) == LOW && aux != 1)
        {
            aux--;
        }

        if (digitalRead(botonSeleccionar) == LOW)
        {
            delay(del);
            CambiarPeso(aux);

            break;
        }
        else if (digitalRead(botonCancelar) == LOW)
        {
            break;
        }
    }
}

/////////////////////////////////////Estadistica//////////////////////////////////////
static void Estadisticas()
{
    // Muestra las estadisticas (cantidad de gramos envasados y cantidad de envases llenados).
    // Los muestra con un intervalo de 3 segundos entre sí.
    int aux = 0;
    delay(del);
    Lcd.clear();
    Lcd.setCursor(0, 0);
    Lcd.print("Gramos");
    Lcd.setCursor(0, 1);
    sprintf(auxCadena, "envasados: %d", (int)*PESO[*NUM_CICLO_FINAL]);
    Lcd.print(auxCadena);

    while (1)
    {
        currentMillis = millis();
        if (currentMillis - previousMillis >= 3000)
        {
            if (aux == 0)
            {
                Lcd.clear();
                Lcd.setCursor(0, 0);
                Lcd.print("Gramos");
                Lcd.setCursor(0, 1);
                sprintf(auxCadena, "envasados: %d", (int)*PESO[*NUM_CICLO_FINAL]);
                Lcd.print(auxCadena);

                aux = 1;
                previousMillis = currentMillis;
            }
            else
            {
                Lcd.clear();
                Lcd.setCursor(0, 0);
                Lcd.print("Envases");
                Lcd.setCursor(0, 1);
                sprintf(auxCadena, "llenados: %d", *NUM_ENVASES[*NUM_CICLO_FINAL]);
                Lcd.print(auxCadena);

                aux = 0;
                previousMillis = currentMillis;
            }
        }
        // Se vuelve al menú inicial con los botones cancelar ó seleccionar.
        if (digitalRead(botonCancelar) == LOW || digitalRead(botonSeleccionar) == LOW)
        {
            Lcd.clear();
            delay(del);
            break;
        }
    }
}

//////////////////////////MENU PRINCIPAL DE FUNCIONAMIENTO DEL SISTEMA///////////////////////////

static void MenuPrincipal()
{

    titilarSeleccion(seccionMenu, opcion, reset); // Se titila la selección en el menú

    // Se verifican los botones para el cambio de selección/desplzamiento por el menú.
    if (digitalRead(botonArriba) == LOW && opcion != 0)
    {
        opcion--;

        //
        currentMillis = previousMillis + interval;
        reset = 1;
        titilarSeleccion(seccionMenu, opcion, reset);
        reset = 0;
    }
    else if (digitalRead(botonAbajo) == LOW && opcion != 2)
    {
        opcion++;

        currentMillis = previousMillis + interval;
        reset = 1;
        titilarSeleccion(seccionMenu, opcion, reset);
        reset = 0;
    }
    delay(del); // Delay para evitar relectura.

    // Se define que sección del menú (primera parte ó segunda parte) mostrar con base al desplazamiento en este.
    if (opcion <= 1)
    {
        seccionMenu = 0;
    }
    else if (opcion == 2)
    {
        seccionMenu = 1;
    }

    // Se verifica si se realizó selección de la opción seleccionada
    if (digitalRead(botonSeleccionar) == LOW)
    {
        switch (opcion)
        {
        case 0:
            SeleccionDeVelocidad();
            break;
        case 1:
            configPesos();
            break;
        case 2:
            Estadisticas();
            break;
        }
    }
}

void setupPantalla()
{
    // Setup pinMode

    // NOTA: Tener en cuenta que están configuradas en pull_up.
    pinMode(botonCancelar, INPUT_PULLUP);
    pinMode(botonArriba, INPUT_PULLUP);
    pinMode(botonSeleccionar, INPUT_PULLUP);
    pinMode(botonAbajo, INPUT_PULLUP);
}

//Bucle principal de ejecución del Menu:

#include <Celdad_Carga.h>

void MostrarPantalla(bool isEnvasadoEnable)
{
    while (isEnvasadoEnable && SalirMenudo && CONMUTADOR && confirmarEnvase())
    {
        MenuPrincipal();
    }
    SalirMenudo = true;
    // ... ejecución del ciclo de envasado...
}
