#ifndef ENTRADASALIDA_SERVER_GENERICA_H
#define ENTRADASALIDA_SERVER_GENERICA_H

#include "variables_globales.h"
#include "protocolo_entrada.h"
#include <commons/string.h>
#include <utils/sockets.h>
#include <utils/estructuras.h>
#include <utils/buffer.h>
#include <utils/sockets.h>
#include <commons/log.h>
#include <entradasalida_config.h>
#include <pthread.h>

void crear_hilos_conexiones_generica();
void *atender_solicitudes_generica(void *args);
void inicializar_sockets_generica();

#endif