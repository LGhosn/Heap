#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

#define VOLUMEN_INICIAL 10
#define VOLUMEN_MINIMO 5

/* *****************************************************************
 *                          ESTRUCTURAS
 * *****************************************************************/
struct heap{
    void **datos;
    size_t tam;
    size_t cant;
    cmp_func_t comparar;
};

/* *****************************************************************
 *                          AUXILIARES
 * *****************************************************************/
void swap(void **x, void **y) {
    void *aux = *x;
    *x = *y;
    *y = aux;
}

void upheap(heap_t *heap, size_t pos_elemento, void *elemento){
    if (pos_elemento == 0) return;
    
    size_t pos_padre = (pos_elemento - 1) / 2;
    
    if (heap->comparar(heap->datos[pos_padre], elemento) < 0){
        swap(&heap->datos[pos_padre], &heap->datos[pos_elemento]);
        upheap(heap, pos_padre, elemento);
    }
}

void downheap(void *elementos[],  size_t pos_elemento, size_t cant, cmp_func_t comparar){
    if (pos_elemento == cant - 1) return;

    size_t hijo_izq = 2 * pos_elemento + 1;
    size_t hijo_der = 2 * pos_elemento + 2;
    size_t max = pos_elemento;

    if (hijo_der < cant && comparar(elementos[hijo_der], elementos[max]) > 0){
        max = hijo_der;
    }
    if (hijo_izq < cant && comparar(elementos[hijo_izq], elementos[max]) > 0){
        max = hijo_izq;
    }
    if (max != pos_elemento){
        swap(&elementos[pos_elemento], &elementos[max]);
        downheap(elementos, max, cant, comparar);
    }
}

bool heap_redimensionar(heap_t *heap, size_t tam_nuevo){
    if (tam_nuevo < VOLUMEN_MINIMO){
        tam_nuevo = VOLUMEN_MINIMO;
    }

    void **datos_nuevo = realloc(heap->datos, tam_nuevo * sizeof(void*));
    if (!datos_nuevo) return false;
    
    heap->datos = datos_nuevo;
    heap->tam = tam_nuevo;

    return true;
}

void heapify(void *elementos[],size_t cant, cmp_func_t cmp){
    for (size_t i = cant; i > 0; i--){
        downheap(elementos, i - 1, cant, cmp);
    }
}

/* *****************************************************************
 *                          PRIMITIVAS
 * *****************************************************************/

heap_t *heap_crear(cmp_func_t cmp){
    heap_t *heap = malloc(sizeof(heap_t));
    if (!heap) return NULL;
    
    void **datos = calloc(VOLUMEN_INICIAL, sizeof(void*));
    if (!datos) {
        free(heap);
        return NULL;  
    } 
    
    heap->tam = VOLUMEN_INICIAL;
    heap->cant = 0;
    heap->comparar = cmp;
    heap->datos = datos;
    return heap;
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)) {
    if (destruir_elemento) {
        for (int i = 0; i < heap->cant;i++) {
            destruir_elemento(heap->datos[i]);
        }
    }
    free(heap->datos);
    free(heap);
}


size_t heap_cantidad(const heap_t *heap) {
    return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap) {
    return heap->cant == 0;
}

void *heap_ver_max(const heap_t *heap) {
    return heap_cantidad(heap) == 0 ? NULL : heap->datos[0];
}

bool heap_encolar(heap_t *heap, void *elem) {
    if (heap_cantidad(heap) == heap->tam){
        if (!heap_redimensionar(heap, heap->tam * 2)){
            return false;
        }
    }

    heap->datos[heap->cant] = elem;
    upheap(heap, heap->cant, elem);
    
    heap->cant++;
    return true;
}

void *heap_desencolar(heap_t *heap){
    if (heap_cantidad(heap) == 0) return NULL;

    if (heap->cant > VOLUMEN_MINIMO && heap_cantidad(heap) == heap->tam / 4){
        if (!heap_redimensionar(heap, heap->tam / 2)){
            return NULL;
        }
    }

    void *maximo = heap->datos[0];
    
    if (heap_cantidad(heap) > 1) {
        swap(&heap->datos[0], &heap->datos[heap->cant - 1]);
    }
    heap->cant--;
    downheap(heap->datos, 0, heap->cant, heap->comparar);

    return maximo;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
    heap_t *heap = malloc(sizeof(heap_t));
    if (!heap) return NULL;
    
    void **datos = calloc(n, sizeof(void*));
    if (!datos){
        free(heap);
        return NULL;
    }

    for (size_t i = 0; i < n; i++){
        datos[i] = arreglo[i]; 
    }

    heapify(datos, n, cmp);

    heap->tam = n;
    heap->cant = n;
    heap->comparar = cmp;
    heap->datos = datos;

    return heap;
}

/* *****************************************************************
 *                    		HEAP SORT
 * *****************************************************************/

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
    heapify(elementos, cant, cmp);

    while (cant > 0){
        swap(&elementos[0], &elementos[cant - 1]);
        cant--;
        downheap(elementos, 0, cant, cmp);
    }
}

