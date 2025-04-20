#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "film.h"
#include "watchstack.h"

// Initializes the stack
tApiError filmstack_init(tFilmstack* stack) {
    if (stack == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    stack->top = NULL;
    stack->count = 0;

    return E_SUCCESS;
}

// Returns true if the stack is empty
bool filmstack_isEmpty(tFilmstack stack) {
    /////////////////////////////////
    // PR2_2d
    /////////////////////////////////

    return (stack.top == NULL);
}


// Adds a new film to the stack, unless it already exists
tApiError filmstack_push(tFilmstack* stack, tFilm film) {
    if (stack == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    tFilmstackNode* current = stack->top;
    while (current != NULL) {
        if (strcmp(current->elem.name, film.name) == 0) {
            return E_FILM_ALREADY_EXISTS;
        }
        current = current->next;
    }

    tFilmstackNode* newNode = (tFilmstackNode*)malloc(sizeof(tFilmstackNode));
    if (newNode == NULL) {
        return E_MEMORY_ERROR;
    }

    // Copiar la película al nuevo nodo
    film_cpy(&newNode->elem, film);

    // Insertar el nodo en la cima de la pila
    newNode->next = stack->top;
    stack->top = newNode;

    stack->count++;
    return E_SUCCESS;
}

// Returns the top film (without removing it)
tFilm* filmstack_top(tFilmstack stack) {
    if (stack.top == NULL) {
        return NULL;
    }
    return &stack.top->elem;
}

// Removes the top film from the stack
tApiError filmstack_pop(tFilmstack* stack) {
    if (stack == NULL || stack->top == NULL) {
        return E_STRUCTURE_EMPTY;
    }

    tFilmstackNode* temp = stack->top;
    stack->top = stack->top->next;

    film_free(&temp->elem); // Liberar la película
    free(temp);             // Liberar el nodo

    stack->count--;
    return E_SUCCESS;
}

// Frees the whole stack
void filmstack_free(tFilmstack* stack) {
    if (stack == NULL) {
        return;
    }

    tFilmstackNode* current = stack->top;
    while (current != NULL) {
        tFilmstackNode* temp = current;
        current = current->next;
        film_free(&temp->elem);
        free(temp);
    }

    stack->top = NULL;
    stack->count = 0;
}

// Example usage of the stack
void example_usage() {
    tFilmstack* stack = (tFilmstack*)malloc(sizeof(tFilmstack));
    if (stack == NULL) {
        printf("Error: No se pudo asignar memoria para el stack.\n");
        return;
    }

    tApiError error = filmstack_init(stack);
    if (error != E_SUCCESS) {
        printf("Error al inicializar el stack.\n");
        free(stack);
        return;
    }

    // Crear películas de ejemplo
    tFilm film1, film2, film3;
    tTime duration1 = {2, 28}; // Duración en horas y minutos
    tTime duration2 = {2, 49};
    tTime duration3 = {2, 32};
    tDate release1 = {2010, 7, 16}; // Fecha de lanzamiento (año, mes, día)
    tDate release2 = {2014, 11, 7};
    tDate release3 = {2008, 7, 18};

    // Usar los géneros correctos
    film_init(&film1, "Inception", duration1, GENRE_ACTION, release1, 8.8, true);
    film_init(&film2, "Interstellar", duration2, GENRE_SCIENCE_FICTION, release2, 8.6, false);
    film_init(&film3, "The Dark Knight", duration3, GENRE_ACTION, release3, 9.0, true);

    // Agregar películas al stack
    filmstack_push(stack, film1);
    filmstack_push(stack, film2);
    filmstack_push(stack, film3);

    // Mostrar el elemento superior del stack
    tFilm* topFilm = filmstack_top(*stack);
    if (topFilm != NULL) {
        printf("Película en la cima del stack: %s (%d)\n", topFilm->name, topFilm->release.year);
    }

    // Eliminar el elemento superior del stack
    filmstack_pop(stack);

    // Mostrar el nuevo elemento superior
    topFilm = filmstack_top(*stack);
    if (topFilm != NULL) {
        printf("Nueva película en la cima del stack: %s (%d)\n", topFilm->name, topFilm->release.year);
    }

    // Liberar el stack
    filmstack_free(stack);
    free(stack);
}