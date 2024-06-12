#ifndef CPU_SERVER_H_
#define CPU_SERVER_H_

#include "variables_globales.h"
#include "protocolo_cpu.h"
#include "cpu_dispatch.h"
#include "utils/sockets.h"
#include "utils/buffer.h"
#include "utils/estructuras.h"

void atender_cpu_kernel_dispatch();
void atender_cpu_memoria();
void levantar_servidor();
void inicializar_sockets();
void cerrar_servidor();
void _cerrar_conexiones();
void _cerrar_puertos();


#endif //CPU_SERVER_H_