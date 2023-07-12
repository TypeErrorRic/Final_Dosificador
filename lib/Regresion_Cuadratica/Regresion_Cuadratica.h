#ifndef REGRESION_CUADRATICA_H
#define REGRESION_CUADRATICA_H

/**
 * @file Regresion_Cuadratica.h
 * @author Ricardo Pabón Serna (ricardo.pabon@correounivalle.edu.co)
 * @brief Clase que se encarga de realizar la redución de la matriz de regresión cuadratica.
 * @version 0.1
 * @date 2022-10-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Matrices.h> //Libreria privada que incluye la matriz de regresión cuadratica.

class Regresion_Cuadratica: public Matrices
{
     //Miembros de la clase::Regresión_Cuadractica:
    private:
        float a; //Valor del termino independiente.
        float x1; //Valor del termino que acompaña a x1.
        float x2; //Valor del termino que acompañañ a x2.
        static int Num_test; //Valor del Número de test que se ejecuta.
        static bool Realizar; //Determina si lleva a cabo la realización del calculo de la matriz.

    // Metodos de la clase::Regresión_Cuadratica:
    protected:
        // Metodos para la redución por renglones:
        void Multiplicacion_Const(const float num, const int fila);
        bool Segunda_Operacion(const int num, const int fila);
        void Intercambiar(const int fila, const int fila2);
        bool Gauss_Jordan();

    public:
        //Constructor:
        Regresion_Cuadratica(const int size_x, float x[], float y[]);
        //Reducir matriz por Gauss_Jordan:
        void Calcular() override;
        //Retorna las variables de la función Cuadráctica:
        float Get_valor_a();
        float Get_valor_x1();
        float Get_valor_x2();
        //---------------
        void Test();
        void reset();
        //Permite determinar si se coninua ejecutando o no:
        bool& get_Realizar();
};

#else
    class Regresion_Cuadratica;
    
#endif