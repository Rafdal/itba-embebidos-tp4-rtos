#include "user_table.h"

user_table_t* user_table_create(uint8_t max_users)
{
    user_table_t *table = (user_table_t*)malloc(sizeof(user_table_t));
    if (table == NULL)
        return NULL;

    table->max_users = max_users;
    table->user_table = (user_data_t*)calloc(max_users, sizeof(user_data_t));
    if (table->user_table == NULL)
    {
        free(table);
        return NULL;
    }
    for (int i = 0; i < max_users; i++)
    {
        table->user_table[i].id = -1;
    }
    return table;
}

void user_table_destroy(user_table_t *table)
{
    if (table == NULL)
        return;
    free(table->user_table);
    free(table);
}

uint8_t user_table_get_max_users(user_table_t* table)
{
    return table->max_users;
}

bool user_table_available(user_table_t* table, index_t index)    // returns true if index is valid and available
{
    if (index < 0 || index >= table->max_users)
        return false;

    return !(table->user_table[index].id >= 0);
}

index_t user_table_add(user_table_t* table, user_data_t user_data)  // returns index  or -1 if table is full
{
    for (int i = 0; i < table->max_users; i++)
    {
        if (table->user_table[i].id == -1)
        {
            table->user_table[i] = user_data;
            return i;
        }
    }
    return -1;
}

uint8_t user_table_count_users_in_floor(user_table_t* table, uint8_t floor) // returns number of users in floor
{
    uint8_t count = 0;
    for (int i = 0; i < table->max_users; i++)
    {
        if (table->user_table[i].current_floor == floor)
            count++;
    }
    return count;
}

void user_table_remove(user_table_t* table, index_t index)       // removes user at index
{
    if (index < 0 || index >= table->max_users)
        return;

    table->user_table[index].id = -1;
}

index_t user_table_find(user_table_t* table, int32_t id)           // returns index  or -1 if not found
{
    if (id < 0)
        return -1;
    for (int i = 0; i < table->max_users; i++)
    {
        if (table->user_table[i].id == id)
            return i;
    }
    return -1;
}

// returns pointer to user or NULL if not found
user_data_t *user_table_get_user_ptr(user_table_t* table, index_t index)
{
    if (index < 0 || index >= table->max_users || !(table->user_table[index].id >= 0))
        return NULL;

    return &table->user_table[index];
}

// returns user with id or id=-1 if not found
user_data_t user_table_get_user(user_table_t* table, index_t index)
{
    user_data_t user = {0};
    user.id = -1;
    if (index < 0 || index >= table->max_users || !(table->user_table[index].id >= 0))
        return user;

    return table->user_table[index];
}

void integer_to_string(int num, char *str, uint8_t len)
{
    if (str == NULL || num < 0 || len < 1)
        return;

    int i = 0;
	while ((num != 0) && (i<len))
	{
		str[len-i-1] = (char)('0' + num % 10);
		num /= 10;
		i++;
	}
	str[len] = '\0'; // Null-terminate the string
}

int string_to_integer(char *str, uint8_t len)
{
    if (str == NULL || len < 1)
        return 0;

    int num = 0;
    int i = 0;
    while (i < len && str[i] >= '0' && str[i] <= '9')
    {
        num = num * 10 + (str[i] - '0');
        i++;
    }
    return num;
}
