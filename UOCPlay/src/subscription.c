#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "subscription.h"
#include "date.h"

// Parse input from CSVEntry
void subscription_parse(tSubscription* data, tCSVEntry entry) {
    // Check input data
    assert(data != NULL);

    // Check entry fields
    assert(csv_numFields(entry) == NUM_FIELDS_SUBSCRIPTION);

    int pos = 0; // Allow to easy incremental position of the income data

    // Copy subscription's id data
    data->id = csv_getAsInteger(entry, pos);

    // Copy identity document data
    assert(strlen(entry.fields[++pos]) == MAX_DOCUMENT);
    csv_getAsString(entry, pos, data->document, MAX_DOCUMENT + 1);

    // Parse start date
    assert(strlen(entry.fields[++pos]) == DATE_LENGTH);
    date_parse(&(data->start_date), entry.fields[pos]);

    // Parse end date
    assert(strlen(entry.fields[++pos]) == DATE_LENGTH);
    date_parse(&(data->end_date), entry.fields[pos]);

    // Copy plan data
    csv_getAsString(entry, ++pos, data->plan, MAX_PLAN + 1);

    // Copy price data
    data->price = csv_getAsReal(entry, ++pos);

    // Copy number of devices data
    data->numDevices = csv_getAsInteger(entry, ++pos);

    // Check preconditions that needs the readed values
    assert(data->price >= 0);
    assert(data->numDevices >= 1);
}

// Copy the data from the source to destination (individual data)
void subscription_cpy(tSubscription* destination, tSubscription source) {
    // Copy subscription's id data
    destination->id = source.id;

    // Copy identity document data
    strncpy(destination->document, source.document, MAX_DOCUMENT + 1);

    // Copy start date
    date_cpy(&(destination->start_date), source.start_date);

    // Copy end date
    date_cpy(&(destination->end_date), source.end_date);

    // Copy plan data
    strncpy(destination->plan, source.plan, MAX_PLAN + 1);

    // Copy price data
    destination->price = source.price;

    // Copy number of devices data
    destination->numDevices = source.numDevices;
}

// Get subscription data using a string
void subscription_get(tSubscription data, char* buffer) {
    // Print all data at same time
    sprintf(buffer, "%d;%s;%02d/%02d/%04d;%02d/%02d/%04d;%s;%.2f;%d",
        data.id,
        data.document,
        data.start_date.day, data.start_date.month, data.start_date.year,
        data.end_date.day, data.end_date.month, data.end_date.year,
        data.plan,
        data.price,
        data.numDevices);
}

// Initialize subscriptions data
tApiError subscriptions_init(tSubscriptions* subscriptions) {
    if (subscriptions == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    subscriptions->count = 0;
    subscriptions->elems = NULL;

    return E_SUCCESS;
}

// Return the number of subscriptions
int subscriptions_len(tSubscriptions data) {
	return data.count;
}

// Add a new subscription
tApiError subscriptions_add(tSubscriptions* subscriptions, tSubscription sub) {
    if (subscriptions == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    subscriptions->count++;
    subscriptions->elems = (tSubscription*)realloc(subscriptions->elems, subscriptions->count * sizeof(tSubscription));
    if (subscriptions->elems == NULL) {
        return E_MEMORY_ERROR;
    }

    subscription_cpy(&subscriptions->elems[subscriptions->count - 1], sub);
    return E_SUCCESS;
}

// Remove a subscription
tApiError subscriptions_del(tSubscriptions* data, int id) {
    int idx;
    int i;
    
    // Check if an entry with this data already exists
    printf("Buscando suscripción con ID: %d\n", id);
    idx = subscriptions_find(*data, id);
	
	// If the subscription does not exist, return an error
	if (idx < 0)
		return E_SUBSCRIPTION_NOT_FOUND;
    
    // Shift elements to remove selected
	for(i = idx; i < data->count-1; i++) {
			// Copy element on position i+1 to position i
			subscription_cpy(&(data->elems[i]), data->elems[i+1]);
	}
	// Update the number of elements
	data->count--;  

	if (data->count > 0) {
		data->elems = (tSubscription*) realloc(data->elems, data->count * sizeof(tSubscription));
		assert(data->elems != NULL);
	} else {
		subscriptions_free(data);
	}
	
	return E_SUCCESS;
}

// Get subscription data of position index using a string
tApiError subscriptions_get(tSubscriptions subscriptions, int index, char* buffer) {
    if (index < 0 || index >= subscriptions.count) {
        return E_INVALID_ENTRY_FORMAT;
    }

    tSubscription* sub = &subscriptions.elems[index];
    if (sub == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    sprintf(buffer, "%d;%s;%02d/%02d/%04d;%02d/%02d/%04d;%s;%.2f;%d",
            sub->id,
            sub->document,
            sub->start_date.day, sub->start_date.month, sub->start_date.year,
            sub->end_date.day, sub->end_date.month, sub->end_date.year,
            sub->plan,
            sub->price,
            sub->numDevices);

    return E_SUCCESS;
}

// Returns the position of a subscription looking for id's subscription. -1 if it does not exist
int subscriptions_find(tSubscriptions subscriptions, int id) {
    for (int i = 0; i < subscriptions.count; i++) {
        if (subscriptions.elems[i].id == id) {
            return i;
        }
    }
    return -1; // No encontrado
}

// Print subscriptions data
void subscriptions_print(tSubscriptions data) {
    char buffer[1024];
    int i;
    for (i = 0; i < data.count; i++) {
        subscriptions_get(data, i, buffer);
        printf("%s\n", buffer);
    }
}

// Remove all elements
tApiError subscriptions_free(tSubscriptions* data) {
    if (data->elems != NULL) {
        for (int i = 0; i < data->count; i++) {
            filmstack_free(&(data->elems[i].watchlist));
        }
        free(data->elems);
    }
    subscriptions_init(data);
	
	return E_SUCCESS;
}

// Initialize subscription data
tApiError subscription_init(tSubscription* sub) {
    if (sub == NULL) {
        return E_INVALID_ENTRY_FORMAT;
    }

    sub->id = 0;
    strcpy(sub->document, "");
    strcpy(sub->plan, "");
    sub->price = 0.0;
    sub->numDevices = 0;

    date_init(&(sub->start_date));
    date_init(&(sub->end_date));

    // Inicializar el stack
    return filmstack_init(&sub->watchlist);
}