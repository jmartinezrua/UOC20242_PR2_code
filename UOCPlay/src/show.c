#include "show.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


// Parse input from CSVEntry
// Parse input from CSVEntry and initialize a show with its season and episode
void show_parse(tShow* show, tCSVEntry entry) {
    // Variable declarations
    int pos;
    int read;

    const char* showName;
    int seasonNumber;
    tDate seasonDate;

    int episodeNumber;
    const char* episodeTitle;
    tTime duration;
    float rating;

    tEpisode episode;
    tSeason season;

    // Check input
    assert(show != NULL);
    assert(csv_numFields(entry) == NUM_FIELDS_SHOW);

    // Initialize field index
    pos = 0;

    // Extract show name and season number
    showName = entry.fields[pos++];
    seasonNumber = csv_getAsInteger(entry, pos++);

    // Parse season release date
    assert(strlen(entry.fields[pos]) == DATE_LENGTH);
    read = sscanf(entry.fields[pos++], "%d/%d/%d", &seasonDate.day, &seasonDate.month, &seasonDate.year);
    assert(read == 3);

    // Parse episode number, title and duration
    episodeNumber = csv_getAsInteger(entry, pos++);
    episodeTitle = entry.fields[pos++];
    assert(strlen(entry.fields[pos]) == TIME_LENGTH);
    read = sscanf(entry.fields[pos++], "%d:%d", &duration.hour, &duration.minutes);
    assert(read == 2);

    // Parse rating
    rating = csv_getAsReal(entry, pos++);

    // Initialize episode
    episode_init(&episode, episodeNumber, episodeTitle, duration, rating);

    // Initialize season
    season_init(&season, seasonNumber, seasonDate);

    // Add episode to season
    episodeQueue_enqueue(&season.episodes, episode);
    episode_free(&episode);

    // Initialize show
    show_init(show, showName);

    // Add season to show
    seasonList_add(&show->seasons, season);
    season_free(&season);
}


// Initialize a show with the given name and an empty season list
tApiError show_init(tShow* data, const char* name) {
    if (data == NULL || name == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    // Reservar memoria para el nombre del show
    data->name = (char*)malloc(strlen(name) + 1);
    if (data->name == NULL) {
        return E_MEMORY_ERROR;
    }
    strcpy(data->name, name);

    // Inicializar la lista de temporadas
    if (seasonList_init(&data->seasons) != E_SUCCESS) {
        free(data->name);
        return E_MEMORY_ERROR;
    }
    
    return E_SUCCESS;
}

// Initialize a season with the given number and release date
tApiError season_init(tSeason* season, int number, tDate releaseDate) {

    assert(season != NULL); 
    season->number = number; 
    date_cpy(&season->releaseDate, releaseDate); 
    season->numEpisodes = 0; 
    // Initialize the episode queue for the season
    if (episodeQueue_init(&season->episodes) != E_SUCCESS) {
        return E_MEMORY_ERROR; 
    }
    return E_SUCCESS; // Return success
}

// Initialize an episode with the given title, duration, and rating
tApiError episode_init(tEpisode* ep, int episodeNumber, const char* title, tTime duration, float rating) {

    assert(ep != NULL);
    assert(title != NULL);

    // Allocate and copy the title string
    ep->title = (char*)malloc((strlen(title) + 1) * sizeof(char));
    if (ep->title == NULL) return E_MEMORY_ERROR;
    strcpy(ep->title, title);

    // Copy duration and rating
    time_cpy(&ep->duration, duration);
    ep->rating = rating;
    ep->number = episodeNumber;

    return E_SUCCESS;
}

// Initialize a show list
tApiError showList_init(tShowCatalog* list) {
    /////////////////////////////////
	// PR2_1a
	/////////////////////////////////
    if (list != NULL) {
        list->first = NULL;  // Inicializa el puntero al primer nodo como NULL
        list->last = NULL;   // Inicializa el puntero al último nodo como NULL
        list->count = 0;     // Inicializa el contador de series a 0
        return E_SUCCESS;    // Devuelve éxito
    }
    return E_INVALID_ENTRY_TYPE; // Devuelve error si el argumento es inválido
}

// Initialize a season list
tApiError seasonList_init(tSeasonList* list) {
    list->first = NULL;
    list->count = 0;
    return E_SUCCESS;
}

// Initialize an episode queue
tApiError episodeQueue_init(tEpisodeQueue* queue) {
    queue->first = NULL;
    queue->last = NULL;
    queue->count = 0;
    return E_SUCCESS;
}

// Add a new show, or if it exists, add season and episode if they don't exist
tApiError showList_add(tShowCatalog* list, tShow show) {
    // Validar precondiciones
    if (list == NULL) {
        return E_INVALID_ENTRY_FORMAT; // Handle null pointer
    }

    // Allocate memory for the new node
    tShowNode* newNode = (tShowNode*)malloc(sizeof(tShowNode));
    if (newNode == NULL) {
        return E_MEMORY_ERROR; // Handle memory allocation failure
    }

    // Copy the show into the new node
    tApiError err = show_cpy(&newNode->show, &show);
    if (err != E_SUCCESS) {
        free(newNode); // Free allocated memory if copying fails
        return err;
    }

    // Initialize the new node
    newNode->next = NULL;
    newNode->prev = list->last;

    // Update the list
    if (list->last != NULL) {
        list->last->next = newNode;
    } else {
        list->first = newNode; // If the list was empty, set the first node
    }
    list->last = newNode;
    list->count++;

    return E_SUCCESS;
}

// Add a new season at the beginning of the season list
tApiError seasonList_add(tSeasonList* list, tSeason season) {
   /////////////////////////////////
	// PR2_3a
	/////////////////////////////////
   
    return E_NOT_IMPLEMENTED;
}


tApiError episodeQueue_enqueue(tEpisodeQueue* queue, tEpisode episode) {
    // Validar parámetro de entrada
    if(queue == NULL || episode.title == NULL) {
        return E_INVALID_ENTRY_FORMAT; // o algún otro error adecuado
    }
    
    // Reservar memoria para el nuevo nodo de episodio
    tEpisodeNode* newNode = (tEpisodeNode*)malloc(sizeof(tEpisodeNode));
    if(newNode == NULL) {
        return E_MEMORY_ERROR;
    }
    
    // Copiar los datos del episodio en el nuevo nodo
    newNode->episode.number = episode.number;
    newNode->episode.rating = episode.rating;
    newNode->episode.duration = episode.duration;  // Se asume que tTime es copiable por asignación
    
    // Reserva y copia profunda del título
    newNode->episode.title = (char*)malloc(strlen(episode.title) + 1);
    if(newNode->episode.title == NULL) {
        free(newNode);
        return E_MEMORY_ERROR;
    }
    strcpy(newNode->episode.title, episode.title);
    
    // Inicializar el puntero siguiente del nodo
    newNode->next = NULL;
    
    // Inserción en la cola:
    // Si la cola está vacía, se establece como primer y último nodo; de lo contrario se enlaza al final
    if(queue->first == NULL) {
        queue->first = newNode;
        queue->last = newNode;
    } else {
        queue->last->next = newNode;
        queue->last = newNode;
    }
    
    // Incrementar el contador de nodos en la cola
    queue->count++;
    
    return E_SUCCESS;
}


// Copy a show from src to dst
tApiError show_cpy(tShow* dst, const tShow* src) {
    assert(dst != NULL);
    assert(src != NULL);
    assert(src->name != NULL);

    // Initialize the destination show using show_init
    tApiError error = show_init(dst, src->name);
    if (error != E_SUCCESS) {
        return error;
    }

    // Copy each season using season_cpy and add it to the destination
    tSeasonNode* seasonNode = src->seasons.first;
    while (seasonNode != NULL) {        
        error = seasonList_add(&dst->seasons, seasonNode->season);
        if (error != E_SUCCESS) {
            show_free(dst);
            return error;
        }

        seasonNode = seasonNode->next;
    }

    return E_SUCCESS;
}


// Copy a season from src to dst
tApiError season_cpy(tSeason* dst, const tSeason* src) {
    assert(dst != NULL);
    assert(src != NULL);

    // Initialize destination season
    tApiError error = season_init(dst, src->number, src->releaseDate);
    if (error != E_SUCCESS) {
        return error;
    }

    // Copy each episode using episode_cpy
    tEpisodeNode* epNode = src->episodes.first;
    while (epNode != NULL) {    
    
        error = episodeQueue_enqueue(&dst->episodes, epNode->episode);
       
        if (error != E_SUCCESS) {
            episodeQueue_free(&dst->episodes);
            return error;
        }

        dst->numEpisodes++;
        epNode = epNode->next;
    }

    return E_SUCCESS;
}


// Copy an episode from src to dst
tApiError episode_cpy(tEpisode* dst, const tEpisode* src) {
    // Check preconditions
    assert(dst != NULL);
    assert(src != NULL);    

    // Use episode_init to perform a deep copy of the episode
    return episode_init(dst, src->number, src->title, src->duration, src->rating);
}


// Find a show by its name
tShow* showList_find(tShowCatalog list, const char* name) {
    tShowNode* node = list.first;
    while (node != NULL) {
        if (strcmp(node->show.name, name) == 0)
            return &(node->show);
        node = node->next;
    }
    return NULL;
}

// Find a season by its number
tSeason* seasonList_find(tSeasonList list, int number) {
    tSeasonNode* node = list.first;
    while (node != NULL) {
        if (node->season.number == number)
            return &(node->season);
        node = node->next;
    }
    return NULL;
}

// Add an episode to a specific season of a specific show
tApiError show_addEpisode(tShowCatalog* shows, const char* showName, int seasonNumber, tEpisode episode) {
    /////////////////////////////////
    // PR2_2b 
    /////////////////////////////////
    
    return E_NOT_IMPLEMENTED;
}


// Calculate total duration of a season
tTime show_seasonTotalDuration(tShowCatalog shows, const char* showName, int seasonNumber) {
    /////////////////////////////////
	// PR2_1g
	/////////////////////////////////   
    tTime time;
    time_parse(&time,"00:00");
  
    return time;
}

// Calculate average rating of episodes in a season
float show_seasonAverageRating(tShowCatalog shows, const char* showName, int seasonNumber) {
    /////////////////////////////////
	// Ex1 PR2 1h
	/////////////////////////////////
  
    return  0.0f;
}

// Return the number of total shows
int showsList_len(tShowCatalog shows) {
	/////////////////////////////////
	// PR2_1j
	/////////////////////////////////
	
	/////////////////////////////////
    return -1;
}

// Free the memory allocated for show list
tApiError showList_free(tShowCatalog* list) {
	/////////////////////////////////
	// PR2_4a
	/////////////////////////////////       
   
    
    return E_NOT_IMPLEMENTED;
}

// Free memory allocated for a single tShow (not the show list)
tApiError show_free(tShow* show) {
    // Check preconditions
	assert(show != NULL);   

    // Free the season list (which also frees all episodes)
    seasonList_free(&show->seasons);

    // Free the show name
    if (show->name != NULL) {
        free(show->name);
        show->name = NULL;
    }

    return E_SUCCESS;
}

// Free the memory allocated for season list
tApiError seasonList_free(tSeasonList* list) {  
   
    tSeasonNode* current = list->first;
    while (current) {           
        season_free(&current->season);

        tSeasonNode* temp = current;
        current = current->next;
        free(temp);       
    }
    

    list->first = NULL;
    list->count = 0;
    return E_SUCCESS;
}

// Free memory allocated for a single season
tApiError season_free(tSeason* season) {
    assert(season != NULL);   
   
    episodeQueue_free(&season->episodes);

    season->number = 0;
    season->releaseDate.year = 0;
    season->releaseDate.month = 0;
    season->releaseDate.day = 0;
    season->numEpisodes = 0;

    return E_SUCCESS;
}

// Free the memory allocated for episode queue
tApiError episodeQueue_free(tEpisodeQueue* queue) {
    tEpisodeNode* current = queue->first;
    while (current) {
        // Free dynamic title string
        if (current->episode.title != NULL) {
            free(current->episode.title);
            current->episode.title = NULL;
        }

        tEpisodeNode* temp = current;
        current = current->next;
        free(temp);        
    }
    queue->first = queue->last = NULL;
    queue->count = 0;
    return E_SUCCESS;
}

// Free memory allocated for a single episode
tApiError episode_free(tEpisode* episode) {
    assert(episode != NULL);

    // Free the dynamically allocated title
    if (episode->title != NULL) {
        free(episode->title);
        episode->title = NULL;
    }

    // Optionally reset other fields (not strictly necessary)
    episode->number = 0;
    episode->duration.hour = 0;
    episode->duration.minutes = 0;
    episode->rating = 0.0f;
    
    return E_SUCCESS;
}

// Print a single show, including its seasons and episodes
void show_print(const tShow* show) {
    if (show == NULL) {
        printf("NULL show\n");
        return;
    }

    printf("Show: %s\n", show->name);
    printf("Number of Seasons: %d\n", show->seasons.count);

    tSeasonNode* seasonNode = show->seasons.first;
    while (seasonNode != NULL) {
        tSeason* season = &seasonNode->season;
        printf("    Season %d (Release: %02d/%02d/%04d)\n",
               season->number,
               season->releaseDate.day,
               season->releaseDate.month,
               season->releaseDate.year);

        tEpisodeNode* episodeNode = season->episodes.first;
        while (episodeNode != NULL) {
            tEpisode* ep = &episodeNode->episode;
            printf("      Episode %d: %s [%02d:%02d] (Rating: %.1f)\n",
                   ep->number,
                   ep->title,
                   ep->duration.hour,
                   ep->duration.minutes,
                   ep->rating);
            episodeNode = episodeNode->next;
        }

        seasonNode = seasonNode->next;
    }
} 


// Print all shows in the list using show_print
void showList_print(tShowCatalog list) {
    tShowNode* showNode = list.first;

    while (showNode != NULL) {
        show_print(&showNode->show);
        showNode = showNode->next;
    }
}