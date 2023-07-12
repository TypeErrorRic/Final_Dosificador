/**
 * @file Memoria.h.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Manejo de la memoria EEPROM.
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright (c) 2023
 */

#include <Memoria.h>

//Inicialización de la longitud de la EEPROM
template<typename T, int (*func_ptr)(T&, classMemoria&, const unsigned short&, int)>
const uint16_t Memoria<T, func_ptr>::length = E2END + 1;

//Inicialización de la EEPROM
template<typename T, int (*func_ptr)(T&, classMemoria&, const unsigned short&, int)>
classMemoria Memoria<T, func_ptr>::EEPROM;

//Instancias de la plantilla Memoria creadas:
Memoria<unsigned long, Sumar<unsigned long>> FACTOR_CELDADCARGA;
Memoria<float, Sumar<float>> X2;
Memoria<float, Sumar<float>> X;
Memoria<float, Sumar<float>> A;
Memoria<unsigned int, Sumar<unsigned int>> NUM_CICLO_FINAL;

void Escribir_dato(uint8_t dato, unsigned int dirrecion)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address and Data Registers */
	EEAR = dirrecion;
	EEDR = dato;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}


uint8_t Leer_dato(unsigned int adress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address register */
	EEAR = adress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	return EEDR;
}
//Almacenar envases de diferente tipo:

//Función de Asignación:
Memoria<unsigned int, Array<unsigned int>>& Envases::operator[](int index)
{
	switch (index % NUM_ELEMENTOS_ARREGLO(0))
	{
	case 0:
		return NUM_ENVASES1;
		break;
	case 1:
		return NUM_ENVASES2;
		break;
	case 2:
		return NUM_ENVASES3;
		break;
	case 3:
		return NUM_ENVASES4;
		break;
	case 4:
		return NUM_ENVASES5;
		break;
	default:
		return NUM_ENVASES1;
		break;
	}
}

//Función de limpieza:
void Envases::Limpiar()
{
	for (unsigned int i = 0; i < NUM_ELEMENTOS_ARREGLO(1); i++) ++(NUM_ENVASES1[i] = 0);
	for (unsigned int i = 0; i < NUM_ELEMENTOS_ARREGLO(1); i++) ++(NUM_ENVASES2[i] = 0);
	for (unsigned int i = 0; i < NUM_ELEMENTOS_ARREGLO(1); i++) ++(NUM_ENVASES3[i] = 0);
	for (unsigned int i = 0; i < NUM_ELEMENTOS_ARREGLO(1); i++) ++(NUM_ENVASES4[i] = 0);
	for (unsigned int i = 0; i < NUM_ELEMENTOS_ARREGLO(1); i++) ++(NUM_ENVASES5[i] = 0);
}

//Función para obtener la dirrección de cada uno de los envases.
void Envases::Dirrecciones()
{
	Serial.println("Envases:");
	Serial.println(NUM_ENVASES1.Dirr());
	Serial.println(NUM_ENVASES2.Dirr());
	Serial.println(NUM_ENVASES3.Dirr());
	Serial.println(NUM_ENVASES4.Dirr());
	Serial.println(NUM_ENVASES5.Dirr());
}

void Envases::Begin(short index)
{
	 for(short i = 0; i < NUM_ELEMENTOS_ARREGLO(1); i++)
	 {
		((operator[](index))[i]).begin();
	 }
}

//Inicialización de clase:
Envases NUM_ENVASES;
//Almacena el peso del ultimo ciclo:
Memoria<float, Array<float>> PESO(true);
//Almacena la entrada al modo de configuracion:
Memoria<unsigned short, Sumar<unsigned short>> VARIABLE_REST;
//Almacena el modo de operacion:
Memoria<unsigned short, Sumar<unsigned short>> MODO_OPERACION;
//Alamacena el facto de escala de la celdad:
Memoria<float, Sumar<float>> SCALE;

void LIMPIAR()
{
	NUM_ENVASES.Limpiar();
	++(X2 = 0);
	++(X = 0);
	++(A = 0);
	for (unsigned int i = 0; i < NUM_ELEMENTOS_ARREGLO(0); i++)
	{
		++(PESO[i] = 0);
	}
	++(NUM_CICLO_FINAL = 5);
}

void LIMPIAR_DEFAULT()
{
	for(unsigned short i = 0; i < PESO.size(); i++) {(PESO.Ep()).write(i, 0xff); }
}

void limpirSitioEnvasado()
{
	++(FACTOR_CELDADCARGA = 17456);
	++(SCALE = -448.59);
}

void Dirreciones()
{
	NUM_ENVASES.Dirrecciones();
	Serial.print("Celdad Carga: "); Serial.println(FACTOR_CELDADCARGA.Dirr());
	Serial.println("X2, X, A: "); Serial.println(X2.Dirr()); Serial.println(X.Dirr()); Serial.println(A.Dirr());
	Serial.println("Pesos:");
	for (unsigned int i = 0; i < NUM_ELEMENTOS_ARREGLO(0); i++)
	{
		Serial.println(PESO[i].Dirr());
	}
	Serial.print("Ciclos: "); Serial.println(NUM_CICLO_FINAL.Dirr());
	Serial.print("Escala: "); Serial.println(SCALE.Dirr());
}


//Funciones para el control del uso de la Memoria:

static unsigned short conservar = 0;

//Permite incrementar la dirrección de memoria:
void size_Escrito(unsigned short& dato, unsigned short aumento)
{
    dato = aumento + conservar;
    conservar = dato;
}

//Permite devolver la sumatoria de las dirrecciones:
unsigned short getConserva()
{
    return conservar;
}