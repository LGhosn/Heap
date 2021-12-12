#define _POSIX_C_SOURCE 200809L

#include "heap.h"
#include "testing.h"
#include "pila.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* *****************************************************************
 *                          AUXLIARES
 * *****************************************************************/
int comparar_numeros(const void *x, const void *y){
    if (*(int *)x < *(int*)y) return -1;
    else if (*(int *)x > *(int*)y) return 1;
    return 0; 
}

int comparar_char(const void *x, const void *y){
    if (strcmp((char*)x, (char*)y) < 0) return -1;
    else if (strcmp((char*)x, (char*)y) > 0) return 1;
    return 0;
}

void destruir_wrapper(void *elem){
    pila_destruir(elem);
}

/* *****************************************************************
 *                          PRUEBAS
 * *****************************************************************/
static void prueba_crear_heap_vacio()
{
    printf("\n----------------PRUEBA CREAR HEAP VACIO----------------\n");
    heap_t *heap = heap_crear(NULL);

    print_test("Prueba heap crear heap vacio", heap);
    print_test("Prueba heap la cantidad de elementos es 0", heap_cantidad(heap) == 0);
    print_test("Prueba heap está vacío", heap_esta_vacio(heap));
    print_test("Prueba heap ver máximo en heap vacío es NULL", heap_ver_max(heap) == NULL);
    print_test("Prueba heap desencolar clave A, es NULL, no existe", !heap_desencolar(heap));

    heap_destruir(heap, NULL);
}

static void prueba_heap_insertar()
{
    printf("\n----------------PRUEBA HEAP INSERTAR----------------\n");
    heap_t *heap = heap_crear(comparar_numeros);

    int clave[] = {1, 2, 3};

    
    print_test("Prueba heap insertar clave1", heap_encolar(heap, &clave[0]));
    print_test("Prueba heap la cantidad de elementos es 1", heap_cantidad(heap) == 1);
    print_test("Prueba heap ver máximo es la clave 1", heap_ver_max(heap) == &clave[0]);

    print_test("Prueba heap insertar clave2", heap_encolar(heap, &clave[1]));
    print_test("Prueba heap la cantidad de elementos es 2", heap_cantidad(heap) == 2);
    print_test("Prueba heap ver máximo es la clave 2", heap_ver_max(heap) == &clave[1]);

    print_test("Prueba heap insertar clave3", heap_encolar(heap, &clave[2]));
    print_test("Prueba heap la cantidad de elementos es 3", heap_cantidad(heap) == 3);
    print_test("Prueba heap ver máximo es la clave 3", heap_ver_max(heap) == &clave[2]);

    heap_destruir(heap, NULL);
}

static void prueba_heap_valor_null() {
    printf("\n----------------PRUEBA HEAP INSERTAR NULL----------------\n");

    heap_t *heap = heap_crear(comparar_numeros);

    char *clave = NULL;

    /* Inserta 1 valor y luego lo borra */
    print_test("Prueba heap insertar NULL", heap_encolar(heap, clave));
    print_test("Prueba heap la cantidad de elementos es 1", heap_cantidad(heap) == 1);
    print_test("Prueba heap desencola NULL", heap_desencolar(heap) == clave);
    print_test("Prueba heap la cantidad de elementos es 0", heap_cantidad(heap) == 0);

    heap_destruir(heap, NULL);
}

static void prueba_heap_volumen(size_t largo, bool debug) {
    printf("\n----------------PRUEBA HEAP VOLUMEN----------------\n");

    heap_t* heap = heap_crear(comparar_char);

    const size_t largo_clave = 10;
    char (*claves)[largo_clave] = malloc(largo * largo_clave);

    /* Inserta 'largo' parejas en el heap */
    bool ok = true;
    for (unsigned i = 0; i < largo; i++) {
        sprintf(claves[i], "%08d", i);
        ok = heap_encolar(heap, claves[i]);
        if (!ok) break;
    }

    if (debug) print_test("Prueba heap almacenar muchos elementos", ok);
    if (debug) print_test("Prueba heap la cantidad de elementos es correcta", heap_cantidad(heap) == largo);

    /* Verifica que borre y devuelva los valores correctos */
    for (size_t i = 0; i < largo; i++) {
        heap_desencolar(heap);
    }

    if (debug) print_test("Prueba heap la cantidad de elementos es 0", heap_cantidad(heap) == 0);
    if (debug) print_test("Prueba heap desencolar muchos elementos", true);

    /* Destruye el heap y crea uno nuevo que sí libera */
    heap_destruir(heap, NULL);
    heap = heap_crear(comparar_numeros);

    /* Inserta 'largo' parejas en el heap */
    ok = true;
    for (size_t i = 0; i < largo; i++) {
        ok = heap_encolar(heap, claves[i]);
        if (!ok) break;
    }

    /* Destruye el heap - debería liberar los enteros */
    heap_destruir(heap, NULL);
    free(claves);
}

static void prueba_heap_desencolar(){
    printf("\n----------------PRUEBA HEAP DESENCOLAR----------------\n");

    heap_t *heap = heap_crear(comparar_numeros);

    int clave[] = {3, 2, 1};

    /* Inserta 3 valores y luego los borra */
    print_test("Prueba heap insertar clave1", heap_encolar(heap, &clave[0]));
    print_test("Prueba heap insertar clave2", heap_encolar(heap, &clave[1]));
    print_test("Prueba heap insertar clave3", heap_encolar(heap, &clave[2]));
    print_test("Ver el maximo es 3", heap_ver_max(heap) == &clave[0]);

    /* Al desencolar cada elemento comprueba que ya no está pero los otros sí. */
    print_test("Prueba heap desencolar devuelve clave1", heap_desencolar(heap) == &clave[0]);
    print_test("Prueba heap la cantidad de elementos es 2", heap_cantidad(heap) == 2);
    print_test("Ver el maximo es 2 ", heap_ver_max(heap) == &clave[1]);

    print_test("Prueba heap desencolar devuelve clave2", heap_desencolar(heap) == &clave[1]);
    print_test("Prueba heap la cantidad de elementos es 1", heap_cantidad(heap) == 1);
    print_test("Ver el maximo es 1 ", heap_ver_max(heap) == &clave[2]);
    
    print_test("Prueba heap desencolar devuelve clave3", heap_desencolar(heap) == &clave[2]);
    print_test("Prueba heap la cantidad de elementos es 0", heap_cantidad(heap) == 0);
    print_test("Ver el maximo es NULL", !heap_ver_max(heap));

    heap_destruir(heap, NULL);
}

static void prueba_heap_destruir_con_funcion_de_destruccion() {
    printf("\n----------------PRUEBA HEAP DESTRUIR CON FUNCION DE DESTRUCCION----------------\n");

    heap_t* heap = heap_crear(comparar_char);
    pila_t* p1 = pila_crear();
    pila_t* p2 = pila_crear();
    pila_t* p3 = pila_crear();

    print_test("Prueba heap insertar pila 1", heap_encolar(heap, p1));
    print_test("Prueba heap insertar pila 2", heap_encolar(heap, p2));
    print_test("Prueba heap insertar pila 3", heap_encolar(heap, p3));


    heap_destruir(heap, destruir_wrapper);
}

static void prueba_heap_crear_arreglo(){
    int clave[] = {1,3,2,11};
    void *datos[4];
    for (size_t i = 0; i < 4; i++){
        datos[i] = &clave[i];
    }

    heap_t *heap = heap_crear_arr(datos, 4, comparar_numeros);

    print_test("Se crea un heap con arreglo", heap);
    
    print_test("Ver el maximo es 11", heap_ver_max(heap) == &clave[3]);

    /* Al desencolar cada elemento comprueba que ya no está pero los otros sí. */
    print_test("Prueba heap desencolar devuelve clave3", heap_desencolar(heap) == &clave[3]);
    print_test("Prueba heap la cantidad de elementos es 3", heap_cantidad(heap) == 3);
    print_test("Ver el maximo es 3 ", heap_ver_max(heap) == &clave[1]);

    print_test("Prueba heap desencolar devuelve clave1", heap_desencolar(heap) == &clave[1]);
    print_test("Prueba heap la cantidad de elementos es 2", heap_cantidad(heap) == 2);
    print_test("Ver el maximo es 2 ", heap_ver_max(heap) == &clave[2]);
    
    print_test("Prueba heap desencolar devuelve clave2", heap_desencolar(heap) == &clave[2]);
    print_test("Prueba heap la cantidad de elementos es 2", heap_cantidad(heap) == 1);
    print_test("Ver el maximo es 1 ", heap_ver_max(heap) == &clave[0]);

    print_test("Prueba heap desencolar devuelve clave0", heap_desencolar(heap) == &clave[0]);
    print_test("Prueba heap la cantidad de elementos es 0", heap_cantidad(heap) == 0);
    print_test("Ver el maximo es NULL", !heap_ver_max(heap));

    heap_destruir(heap, NULL);
}

static void prueba_heap_sort() {
    printf("\n----------------PRUEBA HEAP SORT----------------\n");
    int clave[] = {4, 7, 8, 14, 10, 9, 16, 2, 3, 1};
    void* datos[10];
    for (size_t i = 0; i < 10; i++){
        datos[i] = &clave[i];
    }

    heap_sort(datos, 10, comparar_numeros);

    print_test("El primer elemento del arreglo es 1", *(int*)datos[0] == 1);
    print_test("El primer elemento del arreglo es 2", *(int*)datos[1] == 2);
    print_test("El primer elemento del arreglo es 3", *(int*)datos[2] == 3);
    print_test("El primer elemento del arreglo es 4", *(int*)datos[3] == 4);
    print_test("El primer elemento del arreglo es 7", *(int*)datos[4] == 7);
    print_test("El primer elemento del arreglo es 8", *(int*)datos[5] == 8);
    print_test("El primer elemento del arreglo es 9", *(int*)datos[6] == 9);
    print_test("El primer elemento del arreglo es 10", *(int*)datos[7] == 10);
    print_test("El primer elemento del arreglo es 14", *(int*)datos[8] == 14);
    print_test("El primer elemento del arreglo es 16", *(int*)datos[9] == 16);
}

void pruebas_heap_estudiante()
{
    /* Ejecuta todas las pruebas unitarias. */
    prueba_crear_heap_vacio();
    prueba_heap_insertar();
    prueba_heap_valor_null();
    prueba_heap_volumen(5000, true);
    prueba_heap_desencolar();
    prueba_heap_destruir_con_funcion_de_destruccion();
    prueba_heap_crear_arreglo();
    prueba_heap_sort();
}

#ifndef CORRECTOR // Para que no dé conflicto con el main() del corrector.

int main(void){
    pruebas_heap_estudiante();
    return failure_count() > 0; // Indica si falló alguna prueba.
}

#endif