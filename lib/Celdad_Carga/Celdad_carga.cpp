#include <Celdad_Carga.h>
#include <Memoria.h>
#include <Config.h>
// Interrupciones del sistema:
#include <avr/interrupt.h>

///////////////////////////DECLARACIÓN DE VARAIBLES FUNCIONAMIENTO CELDAD DE CARGA///////////////////////////

// Parametros:
volatile double Celdad_Carga::data[APROX_PARA_VALOR_CELDAD_CARGA] = {};
volatile unsigned int Celdad_Carga::counter = 0;
bool Celdad_Carga::medicion = false;
bool Celdad_Carga::configHx711 = true;

// Incializar la celdad de carga:
HX711 Celdad_Carga::celdadCarga = HX711();

// Inicializar estructura de operación de datos:
static pvMecionParams Mediciones;

// Valores de configuración del registro.
#define MAX_VALUE 65525 // En realidad es 65530, pero así da por debajo de este Valor.
#define PRESCALER 1024
#define MIN_VALUE 157 // Valor minimo del contador OCR1A

// Valor maximo en milisegundo ingresable:
#define TIME_MAX ((unsigned int)((((float)PRESCALER / F_CPU) * MAX_VALUE) * 1000))

// Valores de calculos anteriores del sistema de celdad de carga:
static unsigned long past_timer;
static float auxmed;

// Realizar estructura:
pvResultsMedicion Medidas;

///////////////////////////FUNCIONES DE LA CLASE HX711///////////////////////////

// Inicializar las mediciones:
void Celdad_Carga::begin()
{
    if (!medicion)
    {
        // Deshabilitar las interupciones por un momento.
        cli();
        // Reiniciar registros.
        TCCR1A = 0x00;
        TCCR1B = 0x00;
        // Configurar registros.
        TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); // PRESCALER 1024.
        TIMSK1 |= (1 << OCIE1A);
        // Configurando el valor asignado a OCR1A
        time = time < TIME_MAX ? time : TIME_MAX;
        unsigned int aux = ((time * (F_CPU / PRESCALER)) / 1000);
        OCR1A = aux <= MIN_VALUE ? MIN_VALUE : aux;
        // ACTIVANDO MODO DE INTERRUPCIONES:
        sei();
        if (configHx711)
        {
            // Esperar a que el
            celdadCarga.read();
            // Funciones para incializar el HX711:
            celdadCarga.set_offset(*FACTOR_CELDADCARGA); // Establece la tara.
            celdadCarga.set_scale(*SCALE);
            configHx711 = false;
        }
        // Medición activada:
        medicion = true;
        // Permite determinar si la escala va a hacer en kilogramos o gramos.
        Mediciones.flagCaptureMedicion = true;
        // Refrezcar medidas de los datos:
        for (short i = 0; i < APROX_PARA_VALOR_CELDAD_CARGA; i++)
        {
            Mediciones.flagCaptureMedicion = true;
            Captura_dato();
        }
        // Inicializar sistema:
        auxmed = getCeldadcargaValue();
        past_timer = millis();
        counter = 3;
        Medidas.changeMedicion = true;
        Medidas.medicionHx = 0;
    }
}

// Función de captura y realización de derivada del sistema:
void Captura_dato()
{
    if (Mediciones.flagCaptureMedicion)
    {
        if (Mediciones.doDerivada)
        {
            // Dervidad:
            Medidas.resultDev = (auxmed - getCeldadcargaValue()) / (millis() - past_timer);
            past_timer = millis();
            auxmed = getCeldadcargaValue();
            Mediciones.doDerivada = false;
            Celdad_Carga::data[Celdad_Carga::counter % APROX_PARA_VALOR_CELDAD_CARGA] = Celdad_Carga::celdadCarga.get_units(5);
            Celdad_Carga::counter++;
        }
        else
        {
            // Tomar el valor del HX711:
            Celdad_Carga::data[Celdad_Carga::counter % APROX_PARA_VALOR_CELDAD_CARGA] = Celdad_Carga::celdadCarga.get_units(5);
            Celdad_Carga::counter++;
            // Restalecer banderas:
            Mediciones.flagCaptureMedicion = false;
            Medidas.changeMedicion = true;
        }
    }
}

// Interrupcción:
ISR(TIMER1_COMPA_vect)
{
    // Determinar si se toma medición de un dato:
    Mediciones.flagCaptureMedicion = true;
    // Determinar si se hace la derivada:
    if ((Celdad_Carga::counter % 10) == 0)
        Mediciones.doDerivada = true;
    // Reiniciar el contador.
    TCNT1 = 0;
    // FUNCIONAMINETO DE LA INTERRUPCIÓN CORRECTO:
    digitalWrite(13, !digitalRead(13));
}

// Deteiene el funcionamiento de las mediciones.
void Celdad_Carga::stop()
{
    if (medicion)
    {
        // Deshabilitar el timer 1.
        cli();
        TIMSK1 &= ~(1 << OCIE1A);
        TCCR1A = 0x00;
        TCCR1B = 0x00;
        TCNT1 = 0;
        sei();

        // Volver contador a 0.
        Celdad_Carga::counter = 0;
        medicion = false;

        // Reiniciar arreglo de valores:
        for (short i = 0; i < APROX_PARA_VALOR_CELDAD_CARGA; i++)
        {
            Celdad_Carga::data[i] = 0;
        }
    }
}

void Celdad_Carga::configTime(unsigned int num)
{
    this->time = num;
}

///////////////////////////FUNCIONES MANEJAR EL FLUJO DEL FUNCIONAMIENTO CELDAD DE CARGA///////////////////////////

// Instancia de celdad de carga para realizar mediciones.
static Celdad_Carga HX_711;

/**
 * @brief Inicializar las mediciones.
 * @param num Tiempo en milisegundos.
 */
void initCeldad(unsigned int num)
{
    if (*FACTOR_CELDADCARGA == 0)
    {
        // Calibración de la Celdad de Carga:
        escribirLcd<String>("Calibrando", 0, 0);
        escribirLcd<String>("Celdad...", 1, 0);
        // Tiempo de espera para retirar peso.
        delay(4000);
        // Funciones para determinar la calibración:
        Celdad_Carga::celdadCarga.set_scale(1000);
        Celdad_Carga::celdadCarga.begin(DOUT, CLOCK);
        Celdad_Carga::celdadCarga.tare(20);
        escribirLcd<String>("Coloque un peso", 0, 0, true);
        escribirLcd<String>("500 g", 1, 0);
        // Tiempo de espera para retirar peso.
        delay(10000);
        escribirLcd<String>("Realizando ", 0, 0, true);
        escribirLcd<String>("Medicion.", 1, 0);
        delay(2000);
        // Toma de valores para la calibración.
        cli();
        for (int i = 0; i < 100; i++)
        {
            // Tomar valor de la Escala:
            SCALE += Celdad_Carga::celdadCarga.get_value();
        }
        // Cargar valores de la memoria.
        ++(FACTOR_CELDADCARGA = Celdad_Carga::celdadCarga.get_offset());
        SCALE = (*SCALE / 100);
        ++(SCALE = *SCALE / PESO_COMPROBACION);
        sei();
        // Finalización de calibración.
        escribirLcd<String>("Calibrado.", 0, 0, true);
        delay(2000);
        // Inicializar captura de datos.
        HX_711.configTime(num);
        HX_711.begin();
        // Led de verificación de datos.
        pinMode(13, OUTPUT);
    }
    if (!Celdad_Carga::medicion)
    {
        // Inicializar captura de datos.
        HX_711.configTime(num);
        HX_711.begin();
        // Led de verificación de datos.
        pinMode(13, OUTPUT);
    }
}

// Detener las mediciones.
void stopMediciones()
{
    HX_711.stop();
}

///////////////////////////FUNCIONES PARA LA OBTENCIÓN DE VALORES DE LA CELDAD DE CARGA///////////////////////////

// Devolver el valor del HX711 de las 3 ultimas mediciones:
float getCeldadcargaValue()
{
    double aux = 0;
    // Obtener el valor medio de las mediciones de la celdad de carga:
    if (Celdad_Carga::medicion && Medidas.changeMedicion)
    {
        // Deshabilitar interupciones:
        cli();
        // Obtener el peso del dato en relación con su importancia:
        aux += Celdad_Carga::data[((Celdad_Carga::counter - 2) % APROX_PARA_VALOR_CELDAD_CARGA)] * 0.20;
        aux += Celdad_Carga::data[((Celdad_Carga::counter - 1) % APROX_PARA_VALOR_CELDAD_CARGA)] * 0.30;
        aux += Celdad_Carga::data[((Celdad_Carga::counter) % APROX_PARA_VALOR_CELDAD_CARGA)] * 0.50;
        // Habilitar interupciones:
        sei();
        if (aux < 0)
            Medidas.medicionHx = 0;
        else
            Medidas.medicionHx = aux;
        Medidas.changeMedicion = false;
    }
    return Medidas.medicionHx;
}

#define TIME_COMPROBACION_GLOBAL ((unsigned int)TIME_COMPROBACION * SIZE_ARREGLO)

// Función para devolver el valor medio de 10 datos:
static float getaValueFull()
{
    float result = 0;
    // Obtener media con 10 datos:
    for (short i = 0; i < SIZE_ARREGLO; i++)
    {
        result += Celdad_Carga::celdadCarga.get_units(5);
    }
    result = (result / SIZE_ARREGLO);
    if (result < 0)
        return 0.01;
    else
        return result;
}

// Función para obtener el peso exacto:
static void detecionEnvase(float &tipo)
{
    initCeldad(100);
    // Tiempo de toma de datos:
    while (Celdad_Carga::counter < (APROX_PARA_VALOR_CELDAD_CARGA + 3))
    {
        // Capturando datos:
        Captura_dato();
    }
    float resultConfirm2 = getCeldadcargaValue();
    // Detener toma de datos
    stopMediciones();
    // Tiempo de confirmación
    delay(TIME_COMPROBACION);
    // Comprobación del dato Tomado:
    float resultConfirm1 = getaValueFull();
    // Calculo del error de funcionamiento:
    float porcentajeError = abs(((resultConfirm1 - resultConfirm2) / resultConfirm1) * 100);
    Serial.print("Porcentaje error medidas: ");
    Serial.println(porcentajeError);
    // Obteniendo dato final:
    if (porcentajeError <= VALOR_ERROR)
        tipo = (resultConfirm1 + resultConfirm2) / 2;
    else
    {
        // Colocar el envase a último momento
        if ((resultConfirm1 > CRITERIO_ENVASE) && (porcentajeError <= VALOR_ERROR_PASAR))
            tipo = resultConfirm1;
        else
            tipo = 0;
    }
    Serial.print("Peso Envase: ");
    Serial.println(tipo);
}

// Permirte comporbar que tipo de envase hay:
bool confirmarEnvase()
{
    float resultfinal = 0;
    if (!Celdad_Carga::medicion)
    {
        // Reconocer cuanto peso hay en la balanza.
        detecionEnvase(resultfinal);
    }
    else
    {
        // Obteneción del reconocimiento de envase rapido:
        resultfinal = getCeldadcargaValue();
    }
    Serial.println(resultfinal);
    // Verificar si hay un envase o no:
    if ((resultfinal >= CRITERIO_ENVASE))
        return true;
    return false;
}

// Reconocer el tipo de envase:
static short definirEnvase(float valor)
{
    // Comprobar si es Primer Envase:
    if ((valor > (ENVASE_1_PESO - VALOR_DESFASE)) && (valor < (ENVASE_1_PESO + VALOR_DESFASE)))
        return 1;
    // Comprobar si es sengundo Envase
    else if ((valor > (ENVASE_2_PESO - VALOR_DESFASE)) && (valor < (ENVASE_2_PESO + VALOR_DESFASE)))
        return 2;
    // Comprobar si es Tercer Envase
    else if ((valor > (ENVASE_3_PESO - VALOR_DESFASE)) && (valor < (ENVASE_3_PESO + VALOR_DESFASE)))
        return 3;
    // Envase no reconocido:
    else
        return 0;
}

// Reconocer envase en sitio de envasado:
bool reconocerEnvaseEnSitioEnvasado(short &tipo)
{
    float resultfinal = 0;
    // Inicializar la celdad de carga.
    if (!Celdad_Carga::medicion)
    {
        // detección de envase:
        detecionEnvase(resultfinal);
    }
    else
    {
        resultfinal = getCeldadcargaValue();
    }
    if (isEnvaseIn())
    {
        // Determinar el envase:
        switch (definirEnvase(resultfinal))
        {
        case 0:
            // No es ningun envase retorna false:
            tipo = 0;
            return false;
            break;
        case 1:
            tipo = 1;
            break;
        case 2:
            tipo = 2;
            break;
        case 3:
            tipo = 3;
            break;
        default:
            // Verificar que tipo de envase es:
            tipo = 0;
            break;
        }
        Serial.print("Envase: ");
        Serial.println(tipo);
        return true;
    }
    return false;
}

// Verifica si ya se lleno los recipientes:
bool verficarLlenadoCompleto(void (*retornarCerrado)(int))
{
    bool salir1 = false;
    if (Celdad_Carga::medicion)
    {
        // Finalizar verificación del llenado:
        if (!digitalRead(botonCancelar))
        {
            Estado = 1;
            stopMediciones();
            retornarCerrado(6);
        }
        Serial.println("Identificando si esta lleno.");
        // Comprobar el tipo de envase:
        switch (TIPO_ENVASE)
        {
        case 1:
            if (Medidas.medicionHx >= (ENVASE_1_PESO_COMPLETO - LLENADO_DEL_ENVASE_DESFASE))
            {
                Serial.println("LLeno.");
                salir1 = true;
            }
            else
            {
                Serial.println("Vacio.");
            }
            break;
        case 2:
            if (Medidas.medicionHx >= (ENVASE_2_PESO_COMPLETO - LLENADO_DEL_ENVASE_DESFASE))
            {
                Serial.println("LLeno.");
                salir1 = true;
            }
            else
            {
                Serial.println("Vacio.");
            }
            break;
        case 3:
            if (Medidas.medicionHx >= (ENVASE_3_PESO_COMPLETO - LLENADO_DEL_ENVASE_DESFASE))
            {
                Serial.println("LLeno.");
                salir1 = true;
            }
            else
            {
                Serial.println("Vacio.");
            }
            break;
        default:
            break;
        }
    }
    // Comprobar el tipo de envase cuando la celdad de carga no está inicializada:
    else
    {
        switch (TIPO_ENVASE)
        {
        case 1:
            if (getaValueFull() >= (ENVASE_1_PESO_COMPLETO - LLENADO_DEL_ENVASE_DESFASE))
                salir1 = true;
            break;
        case 2:
            if (getaValueFull() >= (ENVASE_2_PESO_COMPLETO - LLENADO_DEL_ENVASE_DESFASE))
                salir1 = true;
            break;
        case 3:
            if (getaValueFull() >= (ENVASE_3_PESO_COMPLETO - LLENADO_DEL_ENVASE_DESFASE))
                salir1 = true;
            break;
        default:
            break;
        }
    }
    return salir1;
}

// Funcionamiento del sensor de inducción:
bool isEnvaseIn()
{
    if (digitalRead(INDUCTIVO))
        return 0;
    else
        return 1;
}