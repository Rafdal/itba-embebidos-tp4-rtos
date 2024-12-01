#ifndef SIMPLE_TABLE_H
#define SIMPLE_TABLE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int32_t id;
    int16_t pin;
    uint8_t current_floor;
} user_data_t;

typedef int index_t;    // -1 for invalid index

typedef struct USER_TABLE {
    user_data_t *user_table;
    uint8_t max_users;
} user_table_t;

user_table_t *user_table_create(uint8_t max_users);
void        user_table_destroy(user_table_t *table);


bool        user_table_available(user_table_t* table, index_t index);    // returns true if index is valid and available
uint8_t     user_table_get_max_users(user_table_t* table);             // returns max_users
uint8_t     user_table_count_users_in_floor(user_table_t* table, uint8_t floor); // returns number of users in floor
index_t     user_table_add(user_table_t* table, user_data_t user_data);  // returns index  or -1 if table is full
void        user_table_remove(user_table_t* table, index_t index);       // removes user at index
index_t     user_table_find(user_table_t* table, int32_t id);           // returns index  or -1 if not found
user_data_t *user_table_get_user_ptr(user_table_t* table, index_t index);    // returns pointer to user or NULL if not found
user_data_t user_table_get_user(user_table_t* table, index_t index);    // returns user or NULL if not found

void integer_to_string(int num, char *str, uint8_t len);    // converts integer to string
int string_to_integer(char *str, uint8_t len);              // converts string to integer


#endif // SIMPLE_TABLE_H