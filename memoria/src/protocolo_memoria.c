#include "protocolo_memoria.h"

void requests_cpu() {
    bool esperar = true;

    while(esperar) {
        int cod_operacion = recibir_operacion(fd_cpu);

        switch(cod_operacion){
            case EXAMPLE:
                // Se procesa el request
                recv_example_msg_cpu();
                //esperar = false; //Cortamos la espera de solicitudes
            break;

            //TODO:
            /*
                Agregar operaciones a las que dara servicio el modulo
            */             
            case MSG_CPU_MEMORIA:

                log_info(logger_memoria, "Se recibio un mje del cpu");
            break;
            case -1:
                log_error(logger_memoria, "ERROR: Ha surgido un problema inesperado, se desconecto el modulo de memoria.");
                esperar = false; //Cortamos la espera de solicitudes
            break;
            default:
                log_warning(logger_memoria, "WARNING: El modulo de memoria ha recibido una solicitud con una operacion desconocida");
            break;
        }
    }
}

void requests_kernel() {
    bool esperar = true;

    while(esperar) {
        int cod_operacion = recibir_operacion(fd_kernel);

        switch(cod_operacion){
            case EXAMPLE:
                // Se procesa el request
                recv_example_msg_kernel();
                //esperar = false; //Cortamos la espera de solicitudes
            break;

            //TODO:
            /*
                Agregar operaciones a las que dara servicio el modulo
            */             
            case MSG_KERNEL_MEMORIA:

            case MSG_KERNEL_CREATE_PROCESS:

                t_new_process* new_process= recv_process_kernel();

                if(new_process == NULL) {
                    log_error(logger_memoria, "ERROR: Ha surgido un problema al recibir el nuevo proceso.");
                }
                
                if(add_process_to_memory(new_process)) {
                    log_info(logger_memoria, "Se ha creado correctamente en memoria el proceso con pid %d", new_process->pid);
                }else {
                    log_warning(logger_memoria, "WARNING CREATE PROCESS: No se ha podido crear el proceso en memoria");
                }

                //TODO: Hacer funcion para eliminar un t_new_process
            break;
            case -1:
                log_error(logger_memoria, "ERROR: Ha surgido un problema inesperado, se desconecto el modulo de memoria.");
                esperar = false; //Cortamos la espera de solicitudes
            break;
            default:
                log_warning(logger_memoria, "WARNING: El modulo de memoria ha recibido una solicitud con una operacion desconocida");
            break;
        }
    }
}

void requests_entradasalida() {
    bool esperar = true;

    while(esperar) {
        int cod_operacion = recibir_operacion(fd_entradasalida);

        switch(cod_operacion){
            case EXAMPLE:
                // Se procesa el request
                recv_example_msg_entradasalida();
                //esperar = false; //Cortamos la espera de solicitudes
            break;
            //TODO:
            /*
                Agregar operaciones a las que dara servicio el modulo
            */
            case MSG_IO_MEMORIA:

                log_info(logger_memoria, "Se recibio un mje del IO");
            break;
 
            case -1:
                log_error(logger_memoria, "ERROR: Ha surgido un problema inesperado, se desconecto el modulo de memoria.");
                esperar = false; //Cortamos la espera de solicitudes
            break;
            default:
                log_warning(logger_memoria, "WARNING: El modulo de memoria ha recibido una solicitud con una operacion desconocida");
            break;
        }
    }
}

int recv_example_msg_cpu(){
    log_info(logger_memoria, "<<<<< EXAMPLE RECIVE MESSAGE FROM CPU>>>>");
    t_message_example * new_msg = malloc(sizeof(t_message_example));
    t_buffer* new_buffer = recive_full_buffer(fd_cpu);

    example_deserialize_msg(new_buffer, new_msg);
            
    log_info(logger_memoria, "%s", new_msg->cadena);
    log_info(logger_memoria, "%d", new_msg->entero);
    free(new_msg->cadena);
    free(new_msg);
    buffer_destroy(new_buffer);

    return 0;
}

int recv_example_msg_kernel(){
    log_info(logger_memoria, "<<<<< EXAMPLE RECIVE MESSAGE FROM KERNEL>>>>");
    t_message_example * new_msg = malloc(sizeof(t_message_example));
    t_buffer* new_buffer = recive_full_buffer(fd_kernel);

    example_deserialize_msg(new_buffer, new_msg);
            
    log_info(logger_memoria, "%s", new_msg->cadena);
    log_info(logger_memoria, "%d", new_msg->entero);
    free(new_msg->cadena);
    free(new_msg);
    buffer_destroy(new_buffer);

    return 0;
}

int recv_example_msg_entradasalida(){
    log_info(logger_memoria, "<<<<< EXAMPLE RECIVE MESSAGE FROM ENTRADASALIDA>>>>");
    t_message_example * new_msg = malloc(sizeof(t_message_example));
    t_buffer* new_buffer = recive_full_buffer(fd_entradasalida);

    example_deserialize_msg(new_buffer, new_msg);
            
    log_info(logger_memoria, "%s", new_msg->cadena);
    log_info(logger_memoria, "%d", new_msg->entero);
    free(new_msg->cadena);
    free(new_msg);
    buffer_destroy(new_buffer);

    return 0;
}

t_new_process* recv_process_kernel() {
    
    log_info(logger_memoria, "Se recibio la solicitud de KERNEL para crear un proceso en memoria");
    
    t_buffer* buffer = recive_full_buffer(fd_kernel);
    //t_new_process* nuevo_proceso= malloc(sizeof(t_new_process)); NO hace falta uso la funcion que crea

    if(buffer == NULL) return NULL;

    t_new_process* new_process= deserialize_nuevo_proceso(buffer);

    // Crear estructuras administrativas necesarias
    //t_proceso* proceso = malloc(sizeof(t_proceso));
    //proceso->pid = nuevo_proceso->pid;
    //proceso->path = strdup(nuevo_proceso->path);

    log_info(logger_memoria,"\nNuevo proceso:\npid:%d\npath relativo: %s",new_process->pid, new_process->path);

    // // Agregar proceso a la lista de procesos
    // pthread_mutex_lock(&mutex_lista_procesos);
    // list_add(lista_procesos, proceso);
    // pthread_mutex_unlock(&mutex_lista_procesos);

    //free(proceso->path);
    //free(proceso);
    //free(nuevo_proceso->path);
    //free(nuevo_proceso);
    buffer_destroy(buffer);

    return new_process;
}