/**
 * @file Memoria.h.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Manejo de la memoria EEPROM.
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright (c) 2023
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <avr/io.h>
#include <inttypes.h>
#include <Arduino.h>

void Escribir_dato(uint8_t dato, unsigned int dirrecion);
uint8_t Leer_dato(unsigned int adress);

// Está estructura hace referencia a una celda de la EEPROM:
struct M_Ref
{
    // Constructor:
    M_Ref(const unsigned int index)
        : index(index) {}

    // Access/read members.
    uint8_t operator*() const                   { return Leer_dato(index); }
    operator uint8_t() const                    { return **this; }

    // ESCRIBIR DATO:
    M_Ref &operator=(uint8_t in)                { return Escribir_dato(in, index), *this; }
    // ACTUALIZAR DATO:
    M_Ref &update(uint8_t in)                   { return in != *this ? *this = in : *this; }
    unsigned int index;
};

// Está estrucutra es un puntero bidirrecional a las celdad representadas por M_Ref:
struct pointer
{
    // Constructor
    pointer(const int index)
        : index(index)                          {}

    operator int() const                        { return index; }
    pointer &operator=(int in)                  { return index = in, *this; }

    // Iteración:
    bool operator!=(const pointer &ptr)         { return index != ptr.index; }
    M_Ref operator*()                           { return index; }
    // Incremento:
    pointer &operator++()                       { return ++index, *this; }
    pointer operator++(int)                     { return index++; }

    int index; // dirreción de la celdad actual.
};

// Esta estructura permite manipular datos desde la EEPROOM.
struct classMemoria
{
    M_Ref operator[](const int idx)             { return idx; }
    unsigned char read(int idx)                 { return M_Ref(idx); }
    void write(int idx, unsigned char val)      { (M_Ref(idx)) = val; }
    void update(int idx, uint8_t val)           { M_Ref(idx).update(val); }

    // Funciones para conseguir y leer datos de la EPPROM:
    template <typename T>
    T &get(int idx, T &t)
    {
        pointer e = idx;
        uint8_t *ptr = (uint8_t *)&t;
        for (int count = sizeof(T); count; --count, ++e)
            *ptr++ = *e;
        return t;
    }

    template <typename T>
    const T &put(int idx, const T &t)
    {
        pointer e = idx;
        const uint8_t *ptr = (const uint8_t *)&t;
        for (int count = sizeof(T); count; --count, ++e)
            (*e).update(*ptr++);
        return t;
    }
};

#define NUM_ELEMENTOS_ARREGLO(value) ((value) == 0 ? 5 : ((value) == 1 ? 3 : 0)) //NO MODIFICABLE. Devuelve el valor de filas y columnas de la matriz.
//El parametro 5 es fijo.

void size_Escrito(unsigned short &dato, unsigned short aumento);
unsigned short getConserva();

// Clase para la Manipulación de los datos creados desde classMemoria:
template <typename Tipo_dato, int (*func_ptr)(Tipo_dato &, classMemoria &, const unsigned short &, int)>
class Memoria
{
    private:
        Tipo_dato dato;
        unsigned short dirrecion;
        const static uint16_t length;
        static classMemoria EEPROM;
        const unsigned short size_dato;
        const unsigned short dirrecion_estable;
        unsigned short sizeEscrito;

    public:
        // Constructor:
        Memoria(bool array = false) : size_dato(sizeof(Tipo_dato)), dirrecion_estable(getConserva())
        {
            this->dirrecion = getConserva();
            EEPROM.get(dirrecion, dato);
            if (array)
            {
                if (sizeof(Tipo_dato) == sizeof(int)) {size_Escrito(sizeEscrito, size_dato * NUM_ELEMENTOS_ARREGLO(1));}
                else if (sizeof(Tipo_dato) == sizeof(float)) {size_Escrito(sizeEscrito, size_dato * NUM_ELEMENTOS_ARREGLO(0));}
            }
            else size_Escrito(sizeEscrito, size_dato);
        }
        ~Memoria()                                  {} // Destructor.
        // Operaciones Aplicables:
        Tipo_dato operator*() const                 { return dato; }
        Memoria &operator=(Tipo_dato cambio)        { return dato = cambio, *this; }
        void operator=(const Memoria &dato)         {*this = dato; }
        // Gaurdar Dato:
        void operator++() const                     { EEPROM.put(dirrecion, dato); }
        // Regresar al Dato original:
        void operator--()                           { EEPROM.get(dirrecion, dato); }
        // Capacidad iterativa:
        void begin()                                { EEPROM.put(dirrecion, 0); this->dato = 0;} //Reiniciar valor.
        pointer &end() const                        {return length; }
        // Operaciones basicas de funcionamiento:
        Memoria &operator+=(float suma)             {if (sizeof(Tipo_dato) != sizeof(char)) {dato += suma; if(suma == 1){this->operator++();}} return *this;}
        void operator-=(float suma)                 {if (sizeof(Tipo_dato) != sizeof(char)) dato -= suma;}
        // Registros Escritos:
        const unsigned short &size() const          { return sizeEscrito; }
        static uint8_t M_DDR(int indx)              { return EEPROM[indx]; } // Se utiliza para obtener los bits del registro.
        static classMemoria &Ep()                   { return EEPROM; }
        const unsigned short &Dirr() const          { return dirrecion; }
        // Funcionalidad para arreglos:
        Memoria &operator[](int indx)               { return dirrecion = func_ptr(dato, EEPROM, dirrecion_estable, indx), *this; }
        //Funciones de comparación:
        bool operator==(Tipo_dato compara) const    { return dato == compara ? true : false; }    
        bool operator!=(Tipo_dato compara) const    { return dato != compara ? true : false; }  
};

// Template para manejo de datos normales.
template <typename T>
int Sumar(T &valor, classMemoria &param, const unsigned short &dirr, int indx)
{
    if (sizeof(T) != sizeof(short))
        valor = valor + indx;
    else if (valor == 0)
    {
        valor = 1;
    }
    else
    {
        valor = 0;
    }
    return dirr;
}

// Valores de las variables:
extern Memoria<unsigned long, Sumar<unsigned long>> FACTOR_CELDADCARGA; // Celdad de Carga.
// Regresión Cuadratica:
extern Memoria<float, Sumar<float>> X2; //Termino de segundo grado.
extern Memoria<float, Sumar<float>> X; //Termino de primer grado.
extern Memoria<float, Sumar<float>> A; //Termino independiente.

// Template para manejo de arrays.
template <typename T>
int Array(T &valor, classMemoria &celdas, const unsigned short &dirr, int indx)
{
    unsigned short aux;
    if (sizeof(T) == sizeof(unsigned int))
    {
        aux = dirr + (sizeof(T) * (indx % NUM_ELEMENTOS_ARREGLO(1)));
    }
    else if (sizeof(T) == sizeof(float))
    {
        aux = dirr + (sizeof(T) * (indx % NUM_ELEMENTOS_ARREGLO(0)));
    }
    celdas.get(aux, valor);
    return aux;
}

// Array de Envases:
// Clase Envases
class Envases {
    private:
        Memoria<unsigned int, Array<unsigned int>> NUM_ENVASES1;
        Memoria<unsigned int, Array<unsigned int>> NUM_ENVASES2;
        Memoria<unsigned int, Array<unsigned int>> NUM_ENVASES3;
        Memoria<unsigned int, Array<unsigned int>> NUM_ENVASES4;
        Memoria<unsigned int, Array<unsigned int>> NUM_ENVASES5;
    public: 
        Envases()   :
             NUM_ENVASES1(true), NUM_ENVASES2(true), NUM_ENVASES3(true), 
             NUM_ENVASES4(true), NUM_ENVASES5(true) {}
        // Acceder a cada uno de los envases
        Memoria<unsigned int, Array<unsigned int>>& operator[](int index);
        // Limpiar todos los envases
        void Limpiar();
        void Dirrecciones();
        void Begin(short index);
};

// Objeto estático de la clase Envases
extern Envases NUM_ENVASES;

// Número de ciclos finalizados:
extern Memoria<unsigned int, Sumar<unsigned int>> NUM_CICLO_FINAL;

// Array de valores para peso:
extern Memoria<float, Array<float>> PESO;

// Conmutación de valor:
extern Memoria<unsigned short, Sumar<unsigned short>> VARIABLE_REST;
// Guardar el Modo de Operacion:
extern Memoria<unsigned short, Sumar<unsigned short>> MODO_OPERACION;
// Guardar la escala de la celdad de cargsa:
extern Memoria<float, Sumar<float>> SCALE;

// funciones de Limpieza:
void LIMPIAR();
void LIMPIAR_DEFAULT();
void limpirSitioEnvasado();
void Dirreciones();

#endif