#ifndef MATRICES_H
#define MATRICES_H

/**
 * @file Matrices.h
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief La clase Permitira crear Matrices de Regresion cuadratica por medio de lectura de 2 valores.
 * @version 0.1
 * @date 2022-10-17
 * 
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h> //Libreria que incluye las funcionalidades de Arduino.

class Matrices
{
    //Miembros de la clase Matrices:
    private:
        //Matriz de terminos independientes x:
        float *x; //vector de valores X.
        float *y; //vector de valores Y.
        //Variables de ejecución:
        const int size_x;
        int contador; //Contador de número elementos agregados.

    //Miembros protegidos de la clase:
    protected:
        float Matriz[3][4]; // Matriz de minimos caudrados;

    //Metodos de la clase Matrices:
    public:
        Matrices(const int size_x, float x[], float y[]);
        ~Matrices();
        bool Update(const float x, const float y);
        virtual void Calcular();
        void get_Matriz();
};

#else
    class Matrices;

#endif