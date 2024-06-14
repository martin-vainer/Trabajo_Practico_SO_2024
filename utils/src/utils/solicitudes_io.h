#ifndef SOLICITUDES_IO_H
#define SOLICITUDES_IO_H

#include "estructuras.h"

// INTERFAZ GENERICA
typedef struct {
    t_PCB* pcb;
    char* nombre_interfaz;
    t_io_generica* generica;
} t_solicitud_io_generica;

typedef struct {
    char* nombre_interfaz;
    uint32_t tiempo_sleep;
} t_io_generica;

// INTERFAZ STDIN
typedef struct {
    t_PCB* pcb;
    char* nombre_interfaz;
    t_io_stdin* stdin;
} t_solicitud_io_stdin;

typedef struct {
    uint32_t direccion_fisica;
    uint32_t tamanio;
} t_io_stdin;

// INTERFAZ STDOUT
typedef struct {
    t_PCB* pcb;
    char* nombre_interfaz;
    t_io_stdout* stdout;
} t_solicitud_io_stdout;

typedef struct {
    uint32_t direccion_fisica;
    uint32_t tamanio;
} t_io_stdout;

//===============================================
// FUNCIONES DE SERIALIZACION Y DESERIALIZACIÓN
//===============================================

// Función para serializar una solicitud de E/S genérica.
// Pre: El buffer y la solicitud deben ser válidos y no NULL.
// Post: La solicitud se serializa en el buffer.
void serializar_solicitud_io_generica(t_buffer* buffer, t_solicitud_io_generica* solicitud);

// Función para serializar una E/S genérica.
// Pre: El buffer y la E/S genérica deben ser válidos y no NULL.
// Post: La E/S genérica se serializa en el buffer.
void serializar_io_generica(t_buffer* buffer, t_io_generica* io_generica);

// Función para deserializar una solicitud de E/S genérica.
// Pre: El buffer debe ser válido y no NULL.
// Post: Retorna un puntero a una estructura t_solicitud_io_generica deserializada.
t_solicitud_io_generica* deserializar_solicitud_io_generica(t_buffer* buffer);

// Función para deserializar una E/S genérica.
// Pre: El buffer debe ser válido y no NULL.
// Post: Retorna un puntero a una estructura t_io_generica deserializada.
t_io_generica* deserializar_io_generica(t_buffer* buffer);

// Función para serializar una solicitud de E/S de STDIN.
// Pre: El buffer y la solicitud deben ser válidos y no NULL.
// Post: La solicitud se serializa en el buffer.
void serializar_solicitud_io_stdin(t_buffer* buffer, t_solicitud_io_stdin* solicitud);

// Función para serializar una E/S de STDIN.
// Pre: El buffer y la E/S de STDIN deben ser válidos y no NULL.
// Post: La E/S de STDIN se serializa en el buffer.
void serializar_io_stdin(t_buffer* buffer, t_io_stdin* io_stdin);

// Función para deserializar una solicitud de E/S de STDIN.
// Pre: El buffer debe ser válido y no NULL.
// Post: Retorna un puntero a una estructura t_solicitud_io_stdin deserializada.
t_solicitud_io_stdin* deserializar_solicitud_io_stdin(t_buffer* buffer);

// Función para deserializar una E/S de STDIN.
// Pre: El buffer debe ser válido y no NULL.
// Post: Retorna un puntero a una estructura t_io_stdin deserializada.
t_io_stdin* deserializar_io_stdin(t_buffer* buffer);

// Función para serializar una solicitud de E/S de STDOUT.
// Pre: El buffer y la solicitud deben ser válidos y no NULL.
// Post: La solicitud se serializa en el buffer.
void serializar_solicitud_io_stdout(t_buffer* buffer, t_solicitud_io_stdout* solicitud);

// Función para serializar una E/S de STDOUT.
// Pre: El buffer y la E/S de STDOUT deben ser válidos y no NULL.
// Post: La E/S de STDOUT se serializa en el buffer.
void serializar_io_stdout(t_buffer* buffer, t_io_stdout* io_stdout);

// Función para deserializar una solicitud de E/S de STDOUT.
// Pre: El buffer debe ser válido y no NULL.
// Post: Retorna un puntero a una estructura t_solicitud_io_stdout deserializada.
t_solicitud_io_stdout* deserializar_solicitud_io_stdout(t_buffer* buffer);

// Función para deserializar una E/S de STDOUT.
// Pre: El buffer debe ser válido y no NULL.
// Post: Retorna un puntero a una estructura t_io_stdout deserializada.
t_io_stdout* deserializar_io_stdout(t_buffer* buffer);

//===============================================
// FUNCIONES DE CREACIÓN Y DESTRUCCIÓN
//===============================================

// Función para crear una solicitud de E/S genérica.
// Pre: El PCB, el nombre de interfaz y la E/S genérica deben ser válidos y no NULL.
// Post: Retorna un puntero a una estructura t_solicitud_io_generica creada.
t_solicitud_io_generica* crear_solicitud_io_generica(t_PCB* pcb, char* nombre_interfaz, t_io_generica* generica);

// Función para crear una E/S genérica.
// Pre: El nombre de interfaz y el tiempo de sleep deben ser válidos.
// Post: Retorna un puntero a una estructura t_io_generica creada.
t_io_generica* crear_io_generica(char* nombre_interfaz, uint32_t tiempo_sleep);

// Función para destruir una solicitud de E/S genérica.
// Pre: La solicitud debe ser válida y no NULL.
// Post: La memoria asociada a la solicitud se libera.
void destruir_solicitud_io_generica(t_solicitud_io_generica* solicitud);

// Función para destruir una E/S genérica.
// Pre: La E/S genérica debe ser válida y no NULL.
// Post: La memoria asociada a la E/S genérica se libera.
void destruir_io_generica(t_io_generica* io_generica);

// Función para crear una solicitud de E/S de STDIN.
// Pre: El PCB, el nombre de interfaz y la E/S de STDIN deben ser válidos y no NULL.
// Post: Retorna un puntero a una estructura t_solicitud_io_stdin creada.
t_solicitud_io_stdin* crear_solicitud_io_stdin(t_PCB* pcb, char* nombre_interfaz, t_io_stdin* stdin);

// Función para crear una E/S de STDIN.
// Pre: La dirección física y el tamaño deben ser válidos.
// Post: Retorna un puntero a una estructura t_io_stdin creada.
t_io_stdin* crear_io_stdin(uint32_t direccion_fisica, uint32_t tamanio);

// Función para destruir una solicitud de E/S de STDIN.
// Pre: La solicitud debe ser válida y no NULL.
// Post: La memoria asociada a la solicitud se libera.
void destruir_solicitud_io_stdin(t_solicitud_io_stdin* solicitud);

// Función para destruir una E/S de STDIN.
// Pre: La E/S de STDIN debe ser válida y no NULL.
// Post: La memoria asociada a la E/S de STDIN se libera.
void destruir_io_stdin(t_io_stdin* io_stdin);

// Función para crear una solicitud de E/S de STDOUT.
// Pre: El PCB, el nombre de interfaz y la E/S de STDOUT deben ser válidos y no NULL.
// Post: Retorna un puntero a una estructura t_solicitud_io_stdout creada.
t_solicitud_io_stdout* crear_solicitud_io_stdout(t_PCB* pcb, char* nombre_interfaz, t_io_stdout* stdout);

// Función para crear una E/S de STDOUT.
// Pre: La dirección física y el tamaño deben ser válidos.
// Post: Retorna un puntero a una estructura t_io_stdout creada.
t_io_stdout* crear_io_stdout(uint32_t direccion_fisica, uint32_t tamanio);

// Función para destruir una solicitud de E/S de STDOUT.
// Pre: La solicitud debe ser válida y no NULL.
// Post: La memoria asociada a la solicitud se libera.
void destruir_solicitud_io_stdout(t_solicitud_io_stdout* solicitud);

// Función para destruir una E/S de STDOUT.
// Pre: La E/S de STDOUT debe ser válida y no NULL.
// Post: La memoria asociada a la E/S de STDOUT se libera.
void destruir_io_stdout(t_io_stdout* io_stdout);

#endif