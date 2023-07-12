/**
 * @file Matrices.cpp
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Implementación de <Matrices.h>
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Matrices.h>


/**
 * @brief Construct a new Matrices:: Matrices object
 *
 * @param size_x tamaño de la cantidad da datos a leer.
 * @param x vector de X.
 * @param y vector de y.
 */
Matrices::Matrices(const int size_x, float x[], float y[]):size_x(size_x),contador(size_x-1)
{
    this -> x = &x[0];
    this -> y = &y[0];
}


/**
 * @brief Destroy the Matrices:: Matrices object
 */
Matrices::~Matrices()
{
    Serial.println("Finalizacion.");
}


/**
 * @brief Metodo encargado de la reoleción de datos.
 *
 * @param x Parametro que recive el valor de X.
 * @param y Parametro que recive el valor de y.
 * @return true
 * @return false
 */
bool Matrices::Update(const float x, const float y)
{
    contador++;
    contador = contador % (size_x);
    this->x[contador] = x;
    this->y[contador] = y;
    if (not(contador == (size_x-1))) return true;
    else return false;
}


/**
 * @brief Interfaz que se encarga del calculo de Cada uno de los elementos de la Matriz de Regresión.
 */
void Matrices::Calcular()
{
    float sum_x=0, sum_x2=0, sum_x3=0, sum_x4=0, sum_y=0,sum_yx=0,sum_yx2=0;
    Matriz[0][0] = size_x;
    for(int i=0;i<size_x;i++)
    {
        sum_x += x[i];
        sum_x2 += x[i] * x[i];
        sum_x3 += x[i] * x[i] * x[i];
        sum_x4 += x[i] * x[i] * x[i] *  x[i];
        sum_y += y[i];
        sum_yx += x[i] * y[i];
        sum_yx2 += x[i] * x[i] * y[i];
    }
    Matriz[0][1] = sum_x;
    Matriz[0][2] = sum_x2;
    Matriz[0][3] = sum_y;
    Matriz[1][0] = sum_x;
    Matriz[1][1] = sum_x2;
    Matriz[1][2] = sum_x3;
    Matriz[1][3] = sum_yx;
    Matriz[2][0] = sum_x2;
    Matriz[2][1] = sum_x3;
    Matriz[2][2] = sum_x4;
    Matriz[2][3] = sum_yx2;
    //Habilitar en caso de querer confirmar el resultado calculador.
    /*
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Serial.print(Matriz[i][j]);
            Serial.print("  ");
        }
        Serial.println(" ");
    }
    Serial.println("Matriz De Mejoramiento.");
    */
}


/**
 * @brief Metodo que se encarga de imprimir la Matriz de Regresión.
 */
void Matrices::get_Matriz()
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if(j==0) Serial.print(x[i]);
            else if(j==1) Serial.print(y[i]);
            Serial.print("  ");
        }
        Serial.println(" ");
    }
    Serial.println("Matriz generada.");
}
