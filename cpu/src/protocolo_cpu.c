#include "protocolo_cpu.h"

int send_example_memoria()
{
    char* cadena = "CPU ENVIO MENSAJE A MEMORIA";
    uint8_t entero = 10;

    return send_example(cadena, entero, fd_memoria);
}

int recv_example_msg_kernel()
{
    log_info(logger_cpu, "<<<<< EXAMPLE RECIVE MESSAGE FROM KERNEL >>>>");
    t_message_example* new_msg = recv_example(fd_kernel_dispatch);

    log_info(logger_cpu, "MENSAJE => %s", get_cadena(new_msg));
    log_info(logger_cpu, "ENTERO => %d", get_entero(new_msg));
    
    message_example_destroy(new_msg);

    return 0;
}

t_PCB* recv_pcb_kernel()
{
    t_PCB* pcb = recv_pcb(fd_kernel_dispatch);

    log_info(logger_cpu, "Se recibio un PCB del Kernel, PID => %d", pcb->pid);
    // log_info(logger_cpu, "PCB pc => %d", pcb->program_counter);
    // log_info(logger_cpu, "PCB Quantum => %d", pcb->quantum);
    // log_info(logger_cpu, "PCB cpu_registers AX => %d", pcb->cpu_registers->ax);

    //pcb_destroy(pcb);

    return pcb;
}

// void atender_instruccion_memoria()
// {
//     //DECODE:

//     t_buffer *new_buffer = recive_full_buffer(fd_memoria);
//     t_instruction *instruccion = deserialize_instruction(new_buffer); //TODO
//     log_info(logger_cpu, "INSTRUCCION => %d", instruccion->name);
//     //TODO corregir para recorrer la lista y loggear los parametros
//     // log_info(logger_cpu, "PARAMETROS => %s", instruccion->params);

//     // EXECUTE:
//     //TODO crear variable global de los registros de la cpu 
//     t_cpu_registers* registros = NULL;
//     ejecutar_instruccion(instruccion,registros);//TODO: antes implementar la logica de registros globales


//     buffer_destroy(new_buffer);
//     instruccion_destroy(instruccion); //TODO
    
// }

t_instruction* recv_instruction_memoria()
{
    t_instruction* instruction = recv_instruction(fd_memoria);
    
    log_info(logger_cpu, "Se recibio una instruccion de memoria");

    return instruction;
}

void send_get_next_instruction_memoria(uint32_t pid, uint32_t program_counter)
{
    send_get_next_instruction(fd_memoria, pid, program_counter);
}

void send_pcb_kernel()
{
    send_pcb(MSG_PCB_KERNEL_EXIT, fd_kernel_dispatch, pcb_execute);
}

void send_solicitud_io_generica_kernel(t_PCB* pcb, t_instruction* instruccion) 
{
    t_list* parametros = obtener_parametros(instruccion);

    char* nombre = (char*)list_get(parametros, 0);
    uint32_t tiempo_sleep = atoi((char*)list_get(instruccion->params, 1));

    t_io_generica* generica = crear_io_generica(nombre, tiempo_sleep);

    send_solicitud_io_generica(fd_kernel_dispatch, pcb, nombre, generica);
}

void send_pcb_kernel_interruption(t_name_interruption tipo_de_interrupcion)
{
    switch (tipo_de_interrupcion)
    {
    case QUANTUM_INTERRUPT:
            send_pcb(MSG_PCB_KERNEL_INTERRUPTION_QUANTUM, fd_kernel_dispatch, pcb_execute);
        break;

    //TODO : mandarle el RECURSO.
    // camino de la interrupcion a la negativa, ya sea  que no haya recursos disponibles, o el signal termino bien.
    // case RECURSO_INTERRUPT:
    //         send_pcb(MSG_PCB_KERNEL_INTERRUPTION_RECURSO, fd_kernel_dispatch, pcb_execute);
    //     break;

    // case MSG_IO:
    //         send_pcb(MSG_PCB_KERNEL_INTERRUPTION_IO, fd_kernel_dispatch, pcb_execute);
    //     break;
    default:
        break;
    }
}
void send_msg_cpu_kernel_recurso(t_name_instruction tipo_de_interrupcion, const char *resource_name) {
    if(tipo_de_interrupcion = WAIT)
    {
        send_message_with_string(fd_kernel_dispatch, MSG_CPU_KERNEL_WAIT, resource_name);
    } 
    if(tipo_de_interrupcion = SIGNAL) 
    {
        send_message_with_string(fd_kernel_dispatch, MSG_CPU_KERNEL_SIGNAL, resource_name);
    } 
    else 
    {
        log_error(logger_cpu, "Tipo de interrupcion invalido para recurso.");
    }
}
