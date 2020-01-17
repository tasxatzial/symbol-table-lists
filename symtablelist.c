/* Library for creating and using Symbol tables.

List based implementation */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "symtable.h"


/* Struct that represents a binding in the symbol table. Each binding
has a pointer to a character key, a pointer to any value and a pointer to
the next binding.

Note: A binding owns its key. A binding does not own its value. */
struct abind {
    char *key;
    void *value;
    struct abind *next;
};


/* Struct that represents a symbol table as a list of bindings. Only the
number of bindings and a pointer to the first binding are required. */
struct SymTable {
    unsigned int uiSize;
    struct abind *first;
};


/* SymTable_new

Creates a SymTable struct with no bindings.

Checks: if memory was allocated succesfully for oSymTable at runtime.

Parameters: void

Returns: a SymTable_T type */
SymTable_T SymTable_new(void) {
    struct SymTable *symtable;

    symtable = malloc(sizeof(struct SymTable));
    assert(symtable);
    symtable->uiSize = 0U;
    symtable->first = NULL;

    return (SymTable_T) symtable;
}


/* SymTable_free

Frees all memory used by oSymTable.

Parameters:
oSymTable: a SymTable_T type

Returns: void */
void SymTable_free(SymTable_T oSymTable) {
    struct abind *ptr, *ptr_next;
    struct SymTable *symtable;

    symtable = oSymTable;
    if (!symtable) {
        return;
    }
    ptr = symtable->first;
    while(ptr) {
        /* remember pointer to next binding before deleting current */
        ptr_next = ptr->next;
        free(ptr->key); 
        free(ptr);
        ptr = ptr_next;
    }
    free(symtable);

    return;
}


/* SymTable_getLength

Returns the number of bindings in oSymTable.

Checks: if oSymTable is not NULL at runtime.

Parameters:
oSymTable: a SymTable_T type */
unsigned int SymTable_getLength(SymTable_T oSymTable) {
    struct SymTable *symtable;

    symtable = oSymTable;
    assert(symtable);

    return (symtable->uiSize);
}


/* SymTable_contains

Checks whether a binding with key equal to pcKey is present in oSymTable.

Checks: if oSymTable and pcKey are not NULL at runtime.

Parameters: 
oSymTable: a SymTable_T type.
pcKey: character array (key). Must be null terminated.

Returns: 1 if pcKey is found, 0 otherwise */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct abind *ptr;
    struct SymTable *symtable;

    symtable = oSymTable;
    assert(symtable);
    assert(pcKey);

    ptr = symtable->first;
    while(ptr) {
        if (!strcmp(ptr->key, pcKey)) {
            return 1;
        }
        ptr = ptr->next;
    }

    return 0;
}


/* SymTable_put

Creates a new binding for oSymTable from a given pcKey and pvValue.

Checks: 
1) if oSymTable and pcKey are not NULL at runtime.
2) if necessary memory was allocated succesfully at runtime.

Parameters:
oSymTable: a SymTable_T type
pcKey: a character array (key). Must be null terminated.
pvValue: pointer to any value

Returns: 1 if binding was created succesfully, 0 if there is already
a binding with key equal to pcKey.

NOTE: A binding owns its key. A binding does not own its value. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    struct abind *new_bind;
    struct SymTable *symtable;
    char *new_key;

    symtable = oSymTable;
    assert(symtable);
    assert(pcKey);

    /* do nothing if pcKey already exists in the table */
    if (SymTable_contains(symtable, pcKey)) {
        return 0;
    }

    /* pcKey not found -> allocate memory for a new binding + key */
    new_bind = malloc(sizeof(struct abind));
    assert(new_bind);
    new_key = malloc((strlen(pcKey) + 1) * sizeof(char));
    assert(new_key);

    /* copy pcKey into new_key */
    strcpy(new_key, pcKey);

    /* initialize binding */
    new_bind->key = new_key;        
    new_bind->value = (void *) pvValue;

    /* binding is inserted first */
    new_bind->next = symtable->first;
    symtable->first = new_bind;

    symtable->uiSize += 1;

    return 1;
}


/* SymTable_map

Applies function pfApply to every binding in oSymTable.

Checks: if oSymTable and pfApply are not NULL at runtime

Parameters:
oSymTable: a SymTable_T type
pfApply: function to apply
pvExtra: a pointer to any value. Used by pfApply.

Returns: void */
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
    struct abind *ptr;
    struct SymTable *symtable;

    symtable = oSymTable;
    assert(symtable);
    assert(pfApply);

    ptr = symtable->first;
    while(ptr) {
        pfApply(ptr->key, ptr->value, (void *) pvExtra);
        ptr = ptr->next;
    }
}


/* SymTable_get

Finds in oSymTable a binding with key equal to pcKey.

Checks: if oSymTable and pcKey are not NULL at runtime.

Parameters:
oSymTable: a SymTable_T type
pcKey: a character array (key). Must be null terminated.

Returns: a pointer to the value or NULL if such binding was not found. */
void* SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct abind *ptr;
    struct SymTable *symtable;

    symtable = oSymTable;
    assert(symtable);
    assert(pcKey);

    ptr = symtable->first;
    while(ptr) {
        if (!strcmp(ptr->key, pcKey)) {
            return ptr->value;
        }
        ptr = ptr->next;
    }

    return NULL;
}


/* SymTable_remove

Removes a binding with key equal to pcKey.

Checks: if oSymTable and pcKey are not NULL at runtime.

Parameters:
oSymTable: a SymTable_T type
pcKey: a character array (key). Must be null terminated.

Returns: 1 if removal was successful, 0 if such binding was not found */
int SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct abind *ptr, *ptr_prev;
    struct SymTable *symtable;

    symtable = oSymTable;
    assert(symtable);
    assert(pcKey);

    ptr = symtable->first;
    while(ptr) {
        /* compare pcKey with the key of each binding*/
        if (strcmp(ptr->key, pcKey)) {  
            ptr_prev = ptr;
            ptr = ptr->next;
            continue;       
        }

        /* pcKey was found. When found in first position, update the first
        key to point to the 2nd one. When not found in first position, update
        the previous key to point to the next one. */
        if (ptr == symtable->first) {
            symtable->first = ptr->next;
        }
        else {
            ptr_prev->next = ptr->next;
        }

        symtable->uiSize -= 1;
        free(ptr->key);     
        free(ptr);
        return 1;
    }

    return 0;
}