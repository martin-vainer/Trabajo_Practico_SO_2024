#include <protocolo_kernel.h>

// void *serializar_pcb(t_PCB *pcb, int *size) {
//     int total_size = sizeof(pcb->pid) + sizeof(pcb->program_counter) + sizeof(pcb->estado) + sizeof(pcb->registros);
//     *size = total_size;  // Set the size of the serialized data

//     void *buffer = malloc(total_size);
//     int offset = 0;

//     memcpy(buffer + offset, &(pcb->pid), sizeof(pcb->pid));
//     offset += sizeof(pcb->pid);

//     memcpy(buffer + offset, &(pcb->program_counter), sizeof(pcb->program_counter));
//     offset += sizeof(pcb->program_counter);

//     memcpy(buffer + offset, &(pcb->estado), sizeof(pcb->estado));
//     offset += sizeof(pcb->estado);

//     memcpy(buffer + offset, pcb->registros, sizeof(pcb->registros));
//     offset += sizeof(pcb->registros);

//     return buffer;
// }

int send_example_cpu()
{
    t_package *package_example = package_create(EXAMPLE);
    t_message_example *example = malloc(sizeof(t_message_example));
    char *cadena = "KERNEL ENVIO MENSAJE A CPU";
    example->cadena = malloc(strlen(cadena) + 1);
    strcpy(example->cadena, cadena);
    example->entero = 5;

    example_serialize_msg(package_example->buffer, example);

    package_send(package_example, fd_cpu_dispatch);

    free(example->cadena);
    free(example);
    package_destroy(package_example);
    return 0;
}

int send_pcb_cpu()
{
    t_package *package = package_create(MSG_KERNEL_CPU_DISPATCH);
    t_PCB*  pcb = pcb_create(0, 1);

    serialize_pcb(package->buffer, pcb);
    package_send(package, fd_cpu_dispatch);

    //pcb_destroy(pcb); no eliminar hasta que termine de ejecutar 
    package_destroy(package);

    return 0;
}

int send_example_memoria()
{
    t_package *package_example = package_create(EXAMPLE);
    t_message_example *example = malloc(sizeof(t_message_example));
    char *cadena = "KERNEL ENVIO MENSAJE A MEMORIA";
    example->cadena = malloc(strlen(cadena) + 1);
    strcpy(example->cadena, cadena);
    example->entero = 5;

    example_serialize_msg(package_example->buffer, example);

    package_send(package_example, fd_kernel_memoria);

    free(example->cadena);
    free(example);
    package_destroy(package_example);
    return 0;
}

int recv_example_msg_entradasalida()
{
    log_info(logger_kernel, "<<<<< EXAMPLE RECIVE MESSAGE FROM ENTRADASALIDA >>>>");
    t_message_example *new_msg = malloc(sizeof(t_message_example));
    t_buffer *new_buffer = recive_full_buffer(fd_kernel_IO);

    example_deserialize_msg(new_buffer, new_msg);

    log_info(logger_kernel, "%s", new_msg->cadena);
    log_info(logger_kernel, "%d", new_msg->entero);
    free(new_msg->cadena);
    free(new_msg);
    buffer_destroy(new_buffer);

    return 0;
}

void atender_kernel_memoria()
{
    bool control_key = 1;
    while (control_key)
    {
        int cod_op = recibir_operacion(fd_kernel_memoria);

        switch (cod_op)
        {

        case MSG_MEMORIA_KERNEL:

            log_info(logger_kernel, "Se recibio un mje del memoria");
            break;

        case -1:
            log_error(logger_kernel, "la memoria se desconecto. Terminando servidor");
            control_key = 0;
            break;
        default:
            log_warning(logger_kernel, "Operacion desconocida en memoria. No quieras meter la pata");
            break;
        }
    }
}

void atender_kernel_IO()
{
    bool control_key = 1;
    while (control_key)
    {
        int cod_op = recibir_operacion(fd_kernel_IO);

        switch (cod_op)
        {
        case EXAMPLE:
            // Se procesa el request
            recv_example_msg_entradasalida();
            control_key = false; // Cortamos la espera de solicitudes
            break;
        // TODO:
        /*
            Agregar operaciones a las que dara servicio el modulo
        */
        case MSG_IO_KERNEL:

            log_info(logger_kernel, "Se recibio un mje de IO");
            recibir_confirmacion_io();
            break;

        case -1:
            log_error(logger_kernel, "la IO se desconecto. Terminando servidor");
            control_key = 0;
            break;
        default:
            log_warning(logger_kernel, "Operacion desconocida en IO. No quieras meter la pata");
            break;
        }
    }
}

void atender_kernel_cpu_dispatch()
{
    bool control_key = 1;
    while (control_key)
    {
        int cod_op = recibir_operacion(fd_cpu_dispatch);

        switch (cod_op)
        {

        case MSG_CPU_DISPATCH_KERNEL:

            log_info(logger_kernel, "Se recibio un mje de CPU DISPATCH");
            break;

        case -1:
            log_error(logger_kernel, "CPU DISPATCH se desconecto. Terminando servidor");
            control_key = 0;
            break;
        default:
            log_warning(logger_kernel, "Operacion desconocida en dispatch. No quieras meter la pata");
            break;
        }
    }
}

// Agrego la función que envía la instrucción IO_GEN_SLEEP al módulo de E/S
int enviar_io_gen_sleep(int fd, int pid, int unidades_trabajo) {
    t_package *paquete = package_create(MSG_IO_KERNEL_GEN_SLEEP);

    // Crear la instrucción
    t_instruction *instruccion = malloc(sizeof(t_instruction));
    instruccion->name = IO_GEN_SLEEP;
    instruccion->params = list_create();

    // Agregar los parámetros a la lista
    int *param_pid = malloc(sizeof(int));
    *param_pid = pid;
    list_add(instruccion->params, param_pid);

    int *param_unidades_trabajo = malloc(sizeof(int));
    *param_unidades_trabajo = unidades_trabajo;
    list_add(instruccion->params, param_unidades_trabajo);

    // Serializar la instrucción
    t_buffer *buffer = paquete->buffer;
    buffer->size = sizeof(t_name_instruct) + sizeof(uint32_t) * 2 + sizeof(int) * 2;
    buffer->stream = malloc(buffer->size);

    void *stream = buffer->stream;
    memcpy(stream, &(instruccion->name), sizeof(t_name_instruct));
    stream += sizeof(t_name_instruct);

    uint32_t cant_params = list_size(instruccion->params);
    memcpy(stream, &cant_params, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    for (int i = 0; i < cant_params; i++) {
        int *param = list_get(instruccion->params, i);
        memcpy(stream, param, sizeof(int));
        stream += sizeof(int);
    }

    if (package_send(paquete, fd) != EXIT_SUCCESS) {
        log_error(logger_kernel, "Error al enviar la instrucción IO_GEN_SLEEP");
        list_destroy_and_destroy_elements(instruccion->params, free);
        free(instruccion);
        package_destroy(paquete);
        return -1;
    }

    log_info(logger_kernel, "Instrucción IO_GEN_SLEEP enviada con pid %d y %d unidades de trabajo", pid, unidades_trabajo);
    list_destroy_and_destroy_elements(instruccion->params, free);
    free(instruccion);
    package_destroy(paquete);
    return 0;
}

// Agrego la función que recibe la confirmación de que la instrucción IO_GEN_SLEEP fue recibida y procesada
int recibir_confirmacion_io() {
    t_package *package = package_create(NULL_HEADER);
    if (package_recv(package, fd_kernel_IO) != EXIT_SUCCESS) {
        log_error(logger_kernel, "Error al recibir confirmación IO_GEN_SLEEP");
        package_destroy(package);
        return -1;
    }

    t_buffer *buffer = package->buffer;
    void *stream = buffer->stream;

    uint32_t mensaje_id;
    memcpy(&mensaje_id, stream, sizeof(uint32_t));
    
    if (mensaje_id == MSG_KERNEL_IO) {
        log_info(logger_kernel, "Confirmación de IO_GEN_SLEEP recibida");
    } else {
        log_warning(logger_kernel, "Se recibió un mensaje desconocido en confirmación IO_GEN_SLEEP");
    }

    package_destroy(package);
    return 0;
}
