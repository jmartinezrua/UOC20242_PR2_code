#include <stdio.h>
#include <assert.h>
#include "csv.h"
#include "api.h"
#include "error.h"
#include <string.h>
#include "subscription.h"

// Get the API version information
const char* api_version()
{
    return "UOC PP 20242";
}

// Load data from a CSV file. If reset is true, remove previous data
tApiError api_loadData(tApiData* data, const char* filename, bool reset) {
    assert(data != NULL);
    assert(filename != NULL);

    FILE* fin = fopen(filename, "r");
    if (fin == NULL) {
        return E_FILE_NOT_FOUND;
    }

    if (reset) {
        api_freeData(data);
        api_initData(data);
    }

    char buffer[FILE_READ_BUFFER_SIZE];
    tCSVEntry entry;
    tApiError error = E_SUCCESS;

    while (fgets(buffer, FILE_READ_BUFFER_SIZE, fin) != NULL) {
        csv_initEntry(&entry);
        csv_parseEntry(&entry, buffer, NULL);

        printf("Procesando entrada: %s\n", buffer);

        error = api_addDataEntry(data, entry);
        csv_freeEntry(&entry);

        if (error != E_SUCCESS) {
            fclose(fin);
            return error;
        }
    }

    fclose(fin);
    return E_SUCCESS;
}

// Initialize the data structure
tApiError api_initData(tApiData* data) {
    assert(data != NULL);

    people_init(&(data->people));
    subscriptions_init(&(data->subscriptions)); // Inicializar suscripciones
    film_catalog_init(&(data->films));
    showList_init(&(data->shows));

    return E_SUCCESS;
}

// Add a person into the data if it does not exist
tApiError api_addPerson(tApiData* data, tCSVEntry entry) {
	/////////////////////////////////
	// PR1_3c
	/////////////////////////////////
	tPerson person;
	tApiError error;
	
	// Check preconditions
	assert(data != NULL);
	
	// Check the entry type
    if (strcmp(csv_getType(&entry), "PERSON") != 0)
        return E_INVALID_ENTRY_TYPE;
    
    // Check the number of fields
    if(csv_numFields(entry) != NUM_FIELDS_PERSON)
        return E_INVALID_ENTRY_FORMAT;
	
	// Parse a person
	person_parse(&person, entry);
	
	// Try to add the person or return an error if the person already exists
	error = people_add(&(data->people), person);
	
	// Release the allocated memory
	person_free(&person);
	
	return error;
	/////////////////////////////////
    // return E_NOT_IMPLEMENTED;
}

// Add a subscription if it does not exist
tApiError api_addSubscription(tApiData* data, tCSVEntry entry) {
    tSubscription subscription;

    assert(data != NULL);

    if (strcmp(csv_getType(&entry), "SUBSCRIPTION") != 0) {
        return E_INVALID_ENTRY_TYPE;
    }

    if (csv_numFields(entry) != NUM_FIELDS_SUBSCRIPTION) {
        return E_INVALID_ENTRY_FORMAT;
    }

    subscription_parse(&subscription, entry);

    printf("Añadiendo suscripción con ID: %d y documento: %s\n", subscription.id, subscription.document);

    return subscriptions_add(&(data->subscriptions), subscription);
}

// Add a film if it does not exist
tApiError api_addFilm(tApiData* data, tCSVEntry entry) {
	/////////////////////////////////
	// PR1_3e
	/////////////////////////////////
	tFilm film;
	tApiError error;
	
	// Check preconditions
	assert(data != NULL);
	
	// Check the entry type
    if (strcmp(csv_getType(&entry), "FILM") != 0)
        return E_INVALID_ENTRY_TYPE;
    
    // Check the number of fields
    if(csv_numFields(entry) != NUM_FIELDS_FILM)
        return E_INVALID_ENTRY_FORMAT;
	
	// Parse a film
	film_parse(&film, entry);
	
	// Add the film into the list of films and into the list of free films if the film is free
	error = film_catalog_add(&(data->films), film);
	
	// Release the allocated memory
	film_free(&film);
	
	return error;
	/////////////////////////////////
    // return E_NOT_IMPLEMENTED;
}

// Find a show by its name
tShow* api_findShow(tApiData data, const char* name) {
	/////////////////////////////////
	// PR2_3d
	/////////////////////////////////
    return NULL;
    
}

// Add a show with one season and one episode (from CSV entry)
tApiError api_addShow(tApiData* data, tCSVEntry entry) {
    if (data == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    const char* showName = entry.fields[1];
    if (showName == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    tShow* show = showList_find(data->shows, showName);
    if (show == NULL) {
        return E_FILM_NOT_FOUND;
    }

    char buffer[256];
    tSeasonNode* seasonNode = show->seasons.first;
    while (seasonNode != NULL) {
        tSeason* season = &seasonNode->season;
        tEpisodeNode* episodeNode = season->episodes.first;
        while (episodeNode != NULL) {
            tEpisode episode = episodeNode->episode;
            snprintf(buffer, sizeof(buffer), 
                     "SHOW;%s;%d;%02d/%02d/%04d;%d;%s;%02d:%02d;%.1f",
                     showName,
                     season->number,
                     season->releaseDate.day,
                     season->releaseDate.month,
                     season->releaseDate.year,
                     episode.number,
                     episode.title,
                     episode.duration.hour,
                     episode.duration.minutes,
                     episode.rating);

            csv_parseEntry(&entry, buffer, "SHOW");
            episodeNode = episodeNode->next;
        }
        seasonNode = seasonNode->next;
    }

    return E_SUCCESS;
}

// Add a film to a subscription's watchlist from a CSV entry, avoiding duplicates
tApiError api_addToWatchlist(tApiData* data, int subscriptionId, tCSVEntry entry) {
    if (data == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    int subscriptionIndex = subscriptions_find(data->subscriptions, subscriptionId);
    if (subscriptionIndex < 0) {
        return E_SUBSCRIPTION_NOT_FOUND;
    }

    tFilm film;
    film_parse(&film, entry);

    tSubscription* subscription = &data->subscriptions.elems[subscriptionIndex];
    tApiError error = filmstack_push(&(subscription->watchlist), film);

    film_free(&film);
    return error;
}

// Get the number of people registered on the application
int api_peopleCount(tApiData data) {
	/////////////////////////////////
	// PR1_3f
	/////////////////////////////////
	return people_count(data.people);
	/////////////////////////////////
    // return -1;
}

// Get the number of subscriptions registered on the application
int api_subscriptionsCount(tApiData data) {
	/////////////////////////////////
	// PR1_3f
	/////////////////////////////////
	return subscriptions_len(data.subscriptions);
	/////////////////////////////////
    // return -1;
}

// Get the number of films registered on the application
int api_filmsCount(tApiData data) {
	/////////////////////////////////
	// PR1_3f
	/////////////////////////////////
	return film_catalog_len(data.films);
	/////////////////////////////////
    // return -1;
}

// Get the number of free films registered on the application
int api_freeFilmsCount(tApiData data) {
	/////////////////////////////////
	// PR1_3f
	/////////////////////////////////
	return film_catalog_freeLen(data.films);
	/////////////////////////////////
    // return -1;
}

// Get the number of shows registered on the application
int api_showsCount(tApiData data) {
    return showsList_len(data.shows);
}

// Free all used memory
tApiError api_freeData(tApiData* data) {
    assert(data != NULL);

    people_free(&(data->people));
    subscriptions_free(&(data->subscriptions));
    film_catalog_free(&(data->films));
    showList_free(&(data->shows)); // PR2_3h

    return E_SUCCESS;
}

// Add a new entry
tApiError api_addDataEntry(tApiData* data, tCSVEntry entry) {
    const char* type = csv_getType(&entry);

    if (strcmp(type, "SUBSCRIPTION") == 0) {
        printf("Procesando suscripción con ID: %d\n", csv_getAsInteger(entry, 0));
        return api_addSubscription(data, entry);
    }

    if (strcmp(type, "PERSON") == 0) {
        char buffer[256]; // Crear un buffer para almacenar el resultado
        csv_getAsString(entry, 0, buffer, sizeof(buffer)); // Llamar correctamente a la función
        printf("Añadiendo persona con documento: %s\n", buffer);
        return api_addPerson(data, entry);
    }

    // Manejo de otros tipos (FILM, SHOW, etc.)
    return E_INVALID_ENTRY_TYPE;
}

// Get subscription data
tApiError api_getSubscription(tApiData data, int id, tCSVEntry *entry) {
	/////////////////////////////////
	// Ex1 PR1 4a
	/////////////////////////////////
	char buffer[FILE_READ_BUFFER_SIZE];
	tSubscription *subscription = NULL;
	int idx;
	
	// Check preconditions
    assert(entry != NULL);
	
	// Search the subscription
	idx = subscriptions_find(data.subscriptions, id);
	
	if (idx < 0)
		return E_SUBSCRIPTION_NOT_FOUND;
	
	subscription = &(data.subscriptions.elems[idx]);
	
	// Print data in the buffer
    subscription_get(*subscription, buffer);
	
	// Initialize the ouput structure
	csv_initEntry(entry);
	csv_parseEntry(entry, buffer, "SUBSCRIPTION");
	
	return E_SUCCESS;
	/////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}

// Get film data
tApiError api_getFilm(tApiData data, const char* name, tCSVEntry *entry) {
	/////////////////////////////////
	// Ex1 PR1 4b
	/////////////////////////////////
	char buffer[FILE_READ_BUFFER_SIZE];
	tFilm *film;
	
	// Check preconditions
	assert(name != NULL);
    assert(entry != NULL);
	
	// Search the film
	film = filmList_find(data.films.filmList, name);
	
	if (film == NULL)
		return E_FILM_NOT_FOUND;
	
	film_get(*film, buffer);
	
	// Initialize the output structure
	csv_initEntry(entry);
	csv_parseEntry(entry, buffer, "FILM");
	
	return E_SUCCESS;
	/////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}

// Get free films data
tApiError api_getFreeFilms(tApiData data, tCSVData *freeFilms) {
	/////////////////////////////////
	// Ex1 PR1 4c
	/////////////////////////////////
	char buffer[FILE_READ_BUFFER_SIZE];
	tFreeFilmListNode *pNode = NULL;
	
	// Check preconditions
	assert(freeFilms != NULL);
	
	// Initialize the output structure
	csv_init(freeFilms);
	
	// Iterate all free films
	pNode = data.films.freeFilmList.first;
	while (pNode != NULL) {
		film_get(*(pNode->elem), buffer);
		csv_addStrEntry(freeFilms, buffer, "FILM");
		pNode = pNode->next;
	}
	
	return E_SUCCESS;
	/////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}

// Get films data by genre
tApiError api_getFilmsByGenre(tApiData data, tCSVData *films, int genre) {
	/////////////////////////////////
	// Ex1 PR1 4d
	/////////////////////////////////
	char buffer[FILE_READ_BUFFER_SIZE];
	tFilmListNode *pNode = NULL;
	
	// Check preconditions
	assert(films != NULL);
	
	// Initialize the output structure
	csv_init(films);
	
	// Iterate all free films
	pNode = data.films.filmList.first;
	while (pNode != NULL) {
		if (pNode->elem.genre == genre) {
			film_get(pNode->elem, buffer);
			csv_addStrEntry(films, buffer, "FILM");
		}
		pNode = pNode->next;
	}
	
	return E_SUCCESS;
	/////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}
