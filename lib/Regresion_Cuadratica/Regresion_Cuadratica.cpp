/**
 * @file Regresion_Cuadratica.cpp
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Implementación de <Regresion_Cuadratica.h>
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Regresion_Cuadratica.h>

//Numero de pruebas realizadas:
int Regresion_Cuadratica::Num_test = 0;
//Variable de realización de Matrices.
bool Regresion_Cuadratica::Realizar = true;

/**
 * @brief Construct a new Regresion_Cuadratica::Regresion_Cuadratica object
 * 
 * @param size_x tamaño de la cantidad da datos a leer.
 * @param x vector de X.
 * @param y vector de y.
 */
Regresion_Cuadratica::Regresion_Cuadratica(const int size_x, float x[], float y[]) : Matrices(size_x,x,y)
{
    this -> a = 0;
    this -> x1 = 0;
    this -> x2 = 0;
}

/**
 * @brief Se encarga de multiplicar un Número por cada uno de los elementos de la fila.
 * 
 * @param num Número por el que se multplica.
 * @param fila Fila en la que se multiplica num. 
 */
void Regresion_Cuadratica::Multiplicacion_Const(const float num, const int fila)
{
    for(int j=0;j<4;j++) Matriz[fila][j] *= num;
}

/**
 * @brief Se encarga de intercambiar las filas de la Matriz de Regresión Cuadratica
 *
 * @param fila
 * @param fila2
 */
void Regresion_Cuadratica::Intercambiar(const int fila, const int fila2)
{
    float aux[4];
    for(int i=0;i<4;i++)
    {
        aux[i] = Matriz[fila][i];
        Matriz[fila][i] = Matriz[fila2][i];
        Matriz[fila2][i] = aux[i];
    }
}

/**
 * @brief Interfaz que se encarga de realiza la segunda operación de redución por renglones.
 * 
 * @param num Parametro del pivote.
 * @param fila Parametro del argumento de la fila que se va a modificar.
 */
bool Regresion_Cuadratica::Segunda_Operacion(const int num, const int fila)
{
    float aux = Matriz[fila][num];
    if(aux != 0) for (int j = 0; j < 4; j++) Matriz[fila][j] -= (Matriz[num][j] * aux);
    else return false;
    return true;
}


/**
 * @brief Metodo que se encarga de reducir la Martriz de regresión cuadratica por medio de operaciones
 *        por renglones en Gauss-Jordan.
 * 
 * @attention Solo funciona con matrices 3*3.
 */
bool Regresion_Cuadratica::Gauss_Jordan()
{
    if(Realizar)
    {
        //Determinar el pivote:
        for (int i = 0; i < 3; i++)
        {
            //Se encarga de verificar si hay un uno en la primera columna de cada fila:
            if (Matriz[i][0] == 1)
            {
                Intercambiar(0, i);
                break;
            }
            //Se encarga de verificar si hay un cero en la primera columna y establecelo en la segunda:
            else if(Matriz[i][0] == 0)
            {
                if(Matriz[1][0] != 0) 
                {
                    Intercambiar(1, i);
                    break;
                }
            }
            else if(Matriz[i][0] == 0 and Matriz[i][1] == 0)
            {
                if (Matriz[2][0] != 0  and Matriz[2][1] != 0)
                {
                    Intercambiar(2, i);
                    break;
                }
            }
        }
        if (Matriz[0][0] != 1) Multiplicacion_Const((1 / Matriz[0][0]), 0);
        //Test(); /* Habilitar en caso de comprobar resulstado */
        for (int i = 1; i < 3; i++) Segunda_Operacion(0, i); // Primer argumento es la fila del pivote. El segundo es donde se aplica
        //Test(); /* Habilitar en caso de comprobar resulstado */
        if (Matriz[1][1] != 1 and Matriz[1][1] != 0) Multiplicacion_Const((1 / Matriz[1][1]), 1);
        else if (Matriz[1][1] == 0 and Matriz[2][1] != 0) Intercambiar(1, 2);
        if(not(Segunda_Operacion(1, 2))) return false;
        if (Matriz[2][2] != 1 and Matriz[2][2] != 0) Multiplicacion_Const((1 / Matriz[2][2]), 2);
        else if (Matriz[2][2] == 0) return false;
        // Test(); /* Habilitar en caso de comprobar resulstado */
        if(not(Segunda_Operacion(1, 0)) and (Matriz[0][2] == 0) and (Matriz[1][2] == 0)) return true;
        Segunda_Operacion(2, 0);
        Segunda_Operacion(2, 1);
        // Test(); /* Habilitar en caso de comprobar resultado */
        if(Matriz[0][0] == 1 && Matriz[1][1] == 1 && Matriz[2][2]== 1) return true; //Verificación de la Matriz identidad.
        return false;
    }
    else return false;
}


/**
 * @brief Regresa el valor independiente.
 * 
 * @return float 
 */
float Regresion_Cuadratica::Get_valor_a()
{
    a = Matriz[0][3];
    return a;
}


/**
 * @brief Metodo que regresa el valor que acompaña a X.
 * 
 * @return float 
 */
float Regresion_Cuadratica::Get_valor_x1()
{
    x1 = Matriz[1][3];
    return x1;
}


/**
 * @brief Metodo que regresa el valor que acompaña a X^2.
 * 
 * @return float 
 */
float Regresion_Cuadratica::Get_valor_x2()
{
    x2 = Matriz[2][3];
    return x2;
}

/**
 * @brief Se encarga de realizar la operación de reducción por renglones en Gauss-jordan y de
 *        impirir cada uno de los terminos la columan independiente
 *
 * @attention Metodo virtual.
 */
void Regresion_Cuadratica::Calcular()
{
    Matrices::Calcular();
    //Lo ejecuta si puede reducir la matriz a la matriz identidad.
    if (Gauss_Jordan())
    {
        Num_test = 0;
        Serial.print("[");
        Serial.print(Get_valor_a());
        Serial.print(", ");
        Serial.print(Get_valor_x1());
        Serial.print(", ");
        Serial.print(Get_valor_x2());
        Serial.println("] tal que: [C,X,X^2]");
        Realizar = true;
    }
    else
    {
        Serial.println("Error: Reiniciar.");
        Num_test = 0;
        Realizar = false;
    }
}


/**
 * @brief Metodo que encarga de imprir la matriz como pureba de test.
 */
void Regresion_Cuadratica::Test()
{
    Serial.print("Teste #: ");
    Serial.println(Num_test);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Serial.print(Matriz[i][j]);
            Serial.print("  ");
        }
        Serial.println(" ");
    }
    Serial.println("#################");
    Num_test++;
}


/**
 * @brief Permite inicalizar la variable::Realizar.
 */
void Regresion_Cuadratica::reset()
{
    Realizar = true;
}


/**
 * @brief Permite determinar si se sigue realizando la toma de valores o no.
 * 
 * @return true 
 * @return false 
 */
bool & Regresion_Cuadratica::get_Realizar()
{
    return Realizar;
}