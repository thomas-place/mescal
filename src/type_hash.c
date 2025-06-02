#include "type_hash.h"
#include "limits.h"

hash_table* create_hash_table(uchar init, uint(*hash_func)(uint, uint), bool (*comp)(uint, uint)) {
    hash_table* ht;
    MALLOC(ht, 1);
    ht->size = 1 << init;  // Size is a power of two.
    ht->num_elements = 0;
    MALLOC(ht->table, ht->size);
    for (uint i = 0; i < ht->size; i++) {
        ht->table[i] = UINT_MAX;
    }
    ht->hash_func = hash_func;
    ht->comp = comp;
    ht->inserts = 0;
    ht->collisions = 0;
    return ht;
}

void delete_hash_table(hash_table* ht) {
    if (ht == NULL) {
        return;
    }
#ifdef DEBUG_HASH
    printf("Hash table deleted. Inserts: %u, Collisions: %u, ratio: %u\n", ht->inserts, ht->collisions, (ht->inserts == 0 ? 0 : ht->collisions / ht->inserts));
#endif

    free(ht->table);
    free(ht);
}

static void rehash_table(hash_table* ht) {
#ifdef DEBUG_HASH
    printf("\n\nRehashing table from size %u to size %u\n", ht->size, 2 * ht->size);
    printf("So far. Inserts: %u, Collisions: %u, ratio: %u\n", ht->inserts, ht->collisions, (ht->inserts == 0 ? 0 : ht->collisions / ht->inserts));
#endif

    // Saving the pointer to the old table and its size.
    uint* old_table = ht->table;
    uint old_size = ht->size;


    // Doubling the size of the hash table.
    ht->size *= 2;
    MALLOC(ht->table, ht->size);
    for (uint i = 0; i < ht->size; i++) {
        ht->table[i] = UINT_MAX;
    }

    // Reashing the elements from the old table to the new one.
    for (uint i = 0; i < old_size; i++) {
        if (old_table[i] == UINT_MAX) {
            continue; // Skip empty slots
        }
        uint index = ht->hash_func(old_table[i], ht->size);

        // linear probing to find the next available slot
        uint j = 0;
        while (true) {
            uint new_index = (index + j) % ht->size; // Linear probing
            if (ht->table[new_index] == UINT_MAX) {
                //m++;
                ht->table[new_index] = old_table[i];
                break; // Successfully inserted
            }
            else {
                j++;
            }
        }
    }
    free(old_table);

#ifdef DEBUG_HASH
    printf("Rehashing finished.\n\n");
#endif
}




uint hash_table_insert(hash_table* ht, uint value) {
    ht->inserts++;

    if (ht->num_elements >= (ht->size >> 1)) {
        rehash_table(ht);
    }

    uint index = ht->hash_func(value, ht->size);
    uint s = 0;
    while (true)
    {
        uint i = (index + s) % ht->size; // Linear probing
        if (ht->table[i] == UINT_MAX) {
            ht->table[i] = value;
            ht->num_elements++;
            return value;
        }
        else if (ht->comp(ht->table[i], value)) {
            // If the value is already present, we do not insert it.
            return ht->table[i];
        }
        else {
            ht->collisions++;
            // If the slot is occupied by a different value, we try the next slot.
            s++;
        }
    }
    return 0;
}
