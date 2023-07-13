#include <Build.h> //Desarrollado por Ricardo Pabón Serna.
#include <Memoria.h>

/************ IMPLEMETNACIÓN PARA EL CALCULO DE LA REGRESION CUADRATICA **************/

static struct // Valores Para el calculo de la matriz.
{
	float x[NFILAS]{}; // Medidas de la masa con respecto al eje X.
	float y[NFILAS]{}; // Medidas de la masa con respecto al eje Y.
} Valores_Sensores;

// Insanciación de un obecto de la clase::<Regresion_Cuadratica.h>
static Regresion_Cuadratica Matriz(NFILAS, Valores_Sensores.x, Valores_Sensores.y);

// Setup para la realización de la regresión Cuadratica:
void initRegresionCuadratica()
{
	for (int i = 0; i < NFILAS; i++)
	{
		Valores_Sensores.x[i] = 0;
		Valores_Sensores.y[i] = 0;
	}
	Matriz.reset();
}

short puntos_busquedad[] = {1, 2, 3, 4, 5}; // Posiciones buscadas en porcentaje
static int contador_num{0};					// Variable contadora.

// Variables para cualcular el promedio de la derivada:
static float derivada = 0;
static float auxAlmacenarDerivada = 0;
static unsigned short contador = 0;

#define NUM_MEDIDAS_MEDIA_DERIVADA 3

// Función para realizar la captura de datos para realizar la regresión cuadratica usnado promedio
static void captureModulate(float &x, int (*y)(void), float &z, bool get_realizar)
{
	//Colocar un ángulo en la entrada:
	angulo(puntos_busquedad[1]);
	delay(1000);
	// Iniciar toma del dato:
	initCeldad(50);
	fillTolva();
	while (1)
	{
		Captura_dato();
		if ((Medidas.resultDev != auxAlmacenarDerivada) && (contador <= NUM_MEDIDAS_MEDIA_DERIVADA))
		{
			derivada += Medidas.resultDev;
			auxAlmacenarDerivada = Medidas.resultDev;
			contador++;
		}
		else if (contador > NUM_MEDIDAS_MEDIA_DERIVADA)
		{
			contador = 0;
			break;
		}
	}
	// Detener llenado
	offTolva();
	stopLllenadoEnvase();
	// Obtener medidas:
	z = (derivada / NUM_MEDIDAS_MEDIA_DERIVADA);
	x = puntos_busquedad[contador_num];
}

// Realización de la Obtención de los datos Para la Regresión Cuadratica:
void doRegresionCuadratica()
{
	contador_num = 0;
	// Bucle principal de toma de datos:
	do
	{
		// Serial_events(Valores_Sensores.x[contador_num], Valores_Sensores.y[contador_num], Matriz.get_Realizar());
		captureModulate(Valores_Sensores.x[contador_num], nullptr, Valores_Sensores.y[contador_num], Matriz.get_Realizar());
		contador_num++;
		// Si se ha realizado la toma de valores con un patron incapaz de ser acomodados en una curva cuadratica. Lazara error.
		if (Matriz.get_Realizar())
		{
			Serial.print("Coordenada #: ");
			Serial.print(contador_num);
			Serial.println(" Tomada correctamente.");
			Serial.print("Valor del adc: ");
			Serial.println(Valores_Sensores.y[contador_num - 1]);
		}
	} while (Matriz.Update(Valores_Sensores.x[contador_num - 1], Valores_Sensores.y[contador_num - 1]));
	retornarCerrado(6);
	// Si se ha realizado la toma de valores con un patron incapaz de ser acomodados en una curva cuadratica. Lazara error.
	if (Matriz.get_Realizar())
	{
		Matriz.get_Matriz();
		Matriz.Calcular();
	}
	++(A = Matriz.Get_valor_a());
	++(X = Matriz.Get_valor_x1());
	++(X2 = Matriz.Get_valor_x2());
	escribirLcd<String>("X2: ", 0, 6, true);
	escribirLcd<float>(*X2, 0, 6);
	escribirLcd<String>("X: ", 0, 9);
	escribirLcd<float>(*X, 0, 13);
	escribirLcd<String>("A: ", 1, 0);
	escribirLcd<float>(*A, 1, 5);
	delay(4000);
}

/************ IMPLEMETNACIÓN DE LA DISPENSADORA DE GRANO **************/

// Obtener el estado de la tolva:
// 0:=Tolva requiere llenado
// 1:=Tolva llena
// 2:=Tolva no requiera
// 3:=Error
static int estadoCantidadTolva()
{
	auxConteo = 0;
	auxSuma1 = 0;
	auxSuma2 = 0;
	while (currentMillisCny - previousMillisCny < 2000)
	{
		val1 = analogRead(CNY1);
		val2 = analogRead(CNY2);
		auxSuma1 = auxSuma1 + val1;
		auxSuma2 = auxSuma2 + val2;
		auxConteo++;
		delay(300);
		currentMillisCny = millis();
	}
	lectura1 = auxSuma1 / auxConteo;
	lectura2 = auxSuma2 / auxConteo;
	previousMillisCny = currentMillisCny;
	if (lectura1 < tresshold && lectura2 < tresshold)
	{
		// Tolva requiere llenado
		return 0;
	}
	else if (lectura1 > tresshold && lectura2 > tresshold)
	{
		// Tolva llena
		return 1;
	}
	else if (lectura1 > tresshold && lectura2 < tresshold)
	{
		// Tolva no requiere llenado
		return 2;
	}
	else
	{
		// Error
		return 3;
	}
}

// Devuelve el estado de la Tolva dispensadora.
bool stateTolva()
{
	switch (estadoCantidadTolva())
	{
	case 0:
		Serial.println("Tolva requiere llenado");
		return false;
	case 1:
		Serial.println("Tolva llena");
		return true;
	case 2:
		Serial.println("Tolva no requiere llenado");
		return true;
	default:
		// Esperar un tiempo antes de realizar de nuevo la verificación:
		delay(500);
		// Evaluar de nuevo la condición:
		stateTolva();
		break;
	}
	return false;
}

// Activa la función de llenado de la Tolva dispensadora.
void fillTolva()
{
	encenderMotor();
	// Aquiva el programa:
	Serial.println("Llenado...");
}

// Dectiene la Función de llenado de la Tolva dispensadora.
void offTolva()
{
	apagarMotor();
	// Aquiva el programa:
	Serial.println("Se ha llenado la tolva Dosificadora.");
}

/************ IMPLEMETACIÓN DEl SISTEMA DE ENVASADO **************/

// Función que permite conocer el tipo de envase.
bool revisarEnvase(short &Tipo)
{
	if (confirmarEnvase())
	{
		MRECONOCIMIENTO_ENVASE(1);
	}
	else
	{
		MRECONOCIMIENTO_ENVASE(0);
	}
	// Programa de reconocimiento de envase:
	if (RECONOCIMIENTO_ENVASE)
	{
		// Reconoce el tipo de envase:
		if (getEstado() == 5)
		{
			if (reconocerEnvaseEnSitioEnvasado(Tipo))
			{
				Serial.print("Tipo de envase seleccionado: ");
				Serial.println(Tipo);
				return true;
			}
			else
			{
				Tipo = 10;
				Serial.println("Error de reconocimiento.");
				return true;
			}
		}
		else
		{
			Serial.println("Hay un envase.");
			return true;
		}
	}
	else
	{
		if ((getEstado() == 2) || (getEstado() == 8) || (getEstado() == 6))
		{
			Serial.print("Se ha retirado");
			MRECONOCIMIENTO_ENVASE(0);
		}
		else
			Serial.println("No hay envase.");
		return false;
	}
}

// Activa el llenado del envase.
void llenandoEnvase()
{
	initCeldad(100);
	angulo(velocidadDeLlenado);
}

// Verfica el llenado del envase.
bool revisarLLenado()
{
	// Programa para revisar se lleno el envase.
	return verficarLlenadoCompleto();
}

// Función para detención del llenado del envase.
float stopLllenadoEnvase()
{
	stopLllenadoEnvase();
	retornarCerrado(velocidadDeLlenado);
	return 10;
}

/********************* IMPLEMENTACIÓN PARA SISTEMA DE ALERTA ************************/

// Activa la alarma correspondiente al evento que suceda.
void alarma(short type, bool state)
{
	if (state)
	{
		switch (type)
		{
		case 0:
			Alertas.S0();
			break;
		case 1:
			Alertas.S1();
			break;
		case 2:
			Alertas.S2();
			break;
		case 3:
			Alertas.S3();
			break;
		case 4:
			Alertas.S4();
			break;
		case 5:
			Alertas.S5();
			break;
		case 6:
			Alertas.S6();
		case 7:
			getCeldadcargaValue();
			escribirLcd<String>("Peso: ", 0, 0, true);
			escribirLcd<float>(Medidas.medicionHx, 0, 8);
			escribirLcd<String>("E", 0, 14);
			escribirLcd<int>(TIPO_ENVASE, 0, 15);
			escribirLcd<String>("Derivada: ", 1, 0);
			escribirLcd<float>(Medidas.resultDev, 1, 12);
			break;
		case 8:
			Alertas.S6();
		default:
			mostrarMensaje = false;
			break;
		}
	}
	else
	{
		Alertas.Desactivar();
	}
}

// Inizialiar el sistema de Alarma
void initAlarma()
{
	// Inicializar pantalla y mensajes:
	setupPantalla();
	setupInteruptMassage();

	// Inicializar el sistema de alarmas:
	pinMode(BUZZER, OUTPUT);
}

/********************* IMPLEMENTACIÓN PARA SISTEMA ALIMENTADOR ************************/

// Función para prender motor:
void encenderMotor()
{
	digitalWrite(RELAY_MOTOR, HIGH);
}

// Función para apagar motor:
void apagarMotor()
{
	digitalWrite(RELAY_MOTOR, LOW);
}

/************************ CONFIGURACIÓN DEL SISTEMA	**********************/
static short counterChangeMessage = 0;
static unsigned long timer = 0;

#define DELAY_MESSAGE 3000UL

static void pruebasModulares()
{
	escribirLcd<String>("Verificando", 0, 0, true);
	escribirLcd<String>("Alimentador", 1, 0);
	// Probar Rele.
	fillTolva();
	delay(2000);
	offTolva();
	delay(2000);
	escribirLcd<String>("Verificando", 0, 0, true);
	escribirLcd<String>("Servomotor.", 1, 0);
	// Probar Servomotor:
	for (int i = 1; i <= 6; i++)
	{
		angulo(i);
		delay(500);
	}
	retornarCerrado(6);
	delay(2000);
	// Probar Sensor de Inducción:
	escribirLcd<String>("Verificando", 0, 0, true);
	escribirLcd<String>("Sensor Inducion", 1, 0);
	while (isEnvaseIn());
	//Probar Celdad de Carga:
	escribirLcd<String>("Verificando", 0, 0, true);
	escribirLcd<String>("Celdad Carga", 1, 0);
	initCeldad(TIME_COMPROBACION);
	short aux = 0;
	while(1)
	{
		if (reconocerEnvaseEnSitioEnvasado(aux))
		{
			if(aux == 1)
				break;
		}
		if((millis() - timer) > DELAY_MESSAGE)
		{
			escribirLcd<String>("Peso:", 0, 0, true);
			escribirLcd<float>(Medidas.medicionHx, 1, 0);
		}
	}
	stopMediciones();
}

static bool crash = false; // Bandera para salir del menu.

void ConfigSistem()
{
	crash = false;
	while (!crash)
	{
		switch (counterChangeMessage)
		{
		case 0:
			if ((millis() - timer) > DELAY_MESSAGE)
			{
				escribirLcd<String>("Configuracion", 0, 0, true);
				escribirLcd<String>("Sistema.", 1, 0);
				timer = millis();
				counterChangeMessage++;
			}
			break;
		case 1:
			if ((millis() - timer) > DELAY_MESSAGE)
			{
				escribirLcd<String>("1. Regresion.", 0, 0, true);
				escribirLcd<String>("2. Conf. Celda.", 1, 0);
				timer = millis();
				counterChangeMessage++;
			}
			break;
		case 2:
			if ((millis() - timer) > DELAY_MESSAGE)
			{
				escribirLcd<String>("3. P.Modular.", 0, 0, true);
				escribirLcd<String>("4. Salir.", 1, 0, true);
				timer = millis();
				counterChangeMessage++;
			}
			break;
		default:
			counterChangeMessage = 0;
			break;
		}
		if ((Serial.available() > 0) && !crash)
		{
			switch ((char)Serial.read())
			{
			case '1':
				doRegresionCuadratica();
				crash = true;
				break;
			case '2':
				FACTOR_CELDADCARGA = 0;
				SCALE = 0;
				initCeldad(100);
				stopMediciones();
				Serial.print("Factor de Celdad: ");
				Serial.println(*FACTOR_CELDADCARGA);
				Serial.print("Scala: ");
				Serial.println(*SCALE);
				crash = true;
				break;
			case '3':
				pruebasModulares();
				crash = true;
			case '4':
				crash = true;
			default:
				break;
			}
		}
	}
}

/********************* SISTEMA DE SERVOMOTOR Y CNY **********************/

static int val1 = 0;
static int val2 = 0;

static int velActual = 0;

static int lectura1 = 0; // Lectura Cny70 1
static int lectura2 = 0; // Lecutra Cny70 2

static unsigned int auxSuma1 = 0;
static unsigned int auxSuma2 = 0;
static unsigned int auxConteo = 0;
static unsigned int tresshold = 15;

static unsigned long currentMillisCny = 0;
static unsigned long previousMillisCny = 0;

void InitPortsCNYRELE()
{
	// Inicializar puertos de entrada:
	pinMode(CNY1, INPUT);
	pinMode(CNY2, INPUT);
	pinMode(RELAY_MOTOR, OUTPUT);
	pinMode(PWM, OUTPUT);
	// Inicializar en Bajo:
	digitalWrite(PWM, LOW);
}

// Función para hacer girar el servomotor en ángulo de apertura.
// Entradas: 1,2,3,4,5, 6.
void angulo(int vel)
{
	for (int _ = 0; _ < vel; _++)
	{
		for (int i = 0; i < 1; i++)
		{
			digitalWrite(PWM, HIGH);
			delay(1);
			digitalWrite(PWM, LOW);
			delay(19);
		}
		delay(250);
	}
}

// Función para hacer girar el servomotor en ángulo para cerrar.
// Entradas: 1,2,3,4,5, 6.
void retornarCerrado(int vel)
{
	if (vel == 1)
	{
		for (int i = 0; i < 1; i++)
		{
			digitalWrite(PWM, HIGH);
			delay(2);
			digitalWrite(PWM, LOW);
			delay(18);
		}
		delay(250);
	}
	else
	{
		for (int _ = 0; _ < vel; _++)
		{
			for (int i = 0; i < 1; i++)
			{
				digitalWrite(PWM, HIGH);
				delay(2);
				digitalWrite(PWM, LOW);
				delay(18);
			}
			delay(250);
		}
	}
}
