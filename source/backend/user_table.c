#include "user_table.h"

static user_data_t* user_table = NULL;

static uint8_t max_users = 0;


void user_table_init(uint8_t max_users_)
{
    if (user_table != NULL)
        free(user_table);

    max_users = max_users_;
    user_table = (user_data_t*)calloc(max_users, sizeof(user_data_t));
    for (int i = 0; i < max_users_; i++)
    {
        user_table[i].id = -1;
    }
}

uint8_t user_table_get_max_users()
{
    return max_users;
}

bool user_table_available(index_t index)    // returns true if index is valid and available
{
    if (index < 0 || index >= max_users)
        return false;

    return !(user_table[index].id >= 0);
}

index_t user_table_add(user_data_t user_data)  // returns index  or -1 if table is full
{
    for (int i = 0; i < max_users; i++)
    {
        if (!user_table[i].id >= 0)
        {
            user_table[i] = user_data;
            return i;
        }
    }
    return -1;
}

uint8_t user_table_count_users_in_floor(uint8_t floor) // returns number of users in floor
{
    uint8_t count = 0;
    for (int i = 0; i < max_users; i++)
    {
        if (user_table[i].current_floor == floor)
            count++;
    }
    return count;
}

void user_table_remove(index_t index)       // removes user at index
{
    if (index < 0 || index >= max_users)
        return;

    user_table[index].id = -1;
}

index_t user_table_find(int32_t id)           // returns index  or -1 if not found
{
    if (id < 0)
        return -1;
    for (int i = 0; i < max_users; i++)
    {
        if (user_table[i].id == id)
            return i;
    }
    return -1;
}

// returns pointer to user or NULL if not found
user_data_t *user_table_get_user(index_t index)
{
    if (index < 0 || index >= max_users || !(user_table[index].id >= 0))
        return NULL;

    return &user_table[index];
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
