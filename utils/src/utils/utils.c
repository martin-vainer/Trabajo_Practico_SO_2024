#include "utils.h"


uint32_t* convert_string_array_to_int_array(char** array_of_strings){
    uint16_t strings_array_length = arrayLength(array_of_strings);
    uint32_t* array_of_numbers = malloc(sizeof(uint32_t) * (strings_array_length + 1));
    
    for(uint16_t i = 0; array_of_strings[i] != NULL; i++) {
        array_of_numbers[i] = atoi(array_of_strings[i]);
    }
    array_of_numbers[strings_array_length] = 0;

    return array_of_numbers;
}

uint16_t arrayLength(char** array){
    uint16_t i = 0;
    while(array[i] != NULL){
        i++;
    }
    return i;
}

uint16_t intArrayLength(uint32_t* array){
    uint16_t i = 0;
    while(array[i] != 0){
        i++;
    }
    return i;
}

char* uint8_to_string(uint8_t value) {
    // Un valor uint8_t puede ser de 0 a 255, así que el máximo número de dígitos es 3
    // Añadimos 1 para el terminador nulo '\0'
    int length_uint = GET_MAX_LENGTH(UINT8)+1;
    char* str = malloc(length_uint * sizeof(char));

    if (str == NULL) {
        return NULL;
    }

    // Convierte el valor uint8_t a una cadena de texto
    snprintf(str, length_uint, "%u", value);

    return str;
}


int get_max_length_uint(uint64_t max_value) {
    int length = 0;
    while (max_value > 0) {
        length++;
        max_value /= 10;
    }
    return length;
}

char* uint32_to_string(uint32_t value) {
    // Un valor uint8_t puede ser de 0 a 255, así que el máximo número de dígitos es 3
    // Añadimos 1 para el terminador nulo '\0'
    int length_uint = GET_MAX_LENGTH(UINT32)+1;
    char* str = malloc(length_uint * sizeof(char));

    if (str == NULL) {
        return NULL;
    }

    // Convierte el valor uint8_t a una cadena de texto
    snprintf(str, length_uint, "%u", value);

    return str;
}
