#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "film.h"
#include "watchstack.h"

// Initializes the stack
tApiError filmstack_init(tFilmstack* stack) {
    // Validar precondiciones
    if (stack == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    // Inicializar la pila
    stack->top = NULL;  // La pila comienza vacía
    stack->count = 0;   // El contador de elementos es 0

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

    // Liberar la película
    film_free(&temp->elem);

    // Liberar el nodo
    free(temp);

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

        // Liberar la película (asegúrate de que film_free no cause problemas)
        film_free(&temp->elem);

        // Liberar el nodo
        free(temp);
    }

    // Reinicializar la pila
    stack->top = NULL;
    stack->count = 0;
}