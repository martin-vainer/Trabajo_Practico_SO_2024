#include <memoria.h>

int main(void){
    init();

    // ############## Socket ###################

    char* server_port = string_itoa(memoria_config->PUERTO_ESCUCHA);
    fd_server = iniciar_servidor(logger_memoria, NULL, NULL, server_port);

    if (fd_server != -1) {
        log_info(logger_memoria, "%s server listo escuchando en puerto %s", SERVERNAME, server_port);
    } else {
        log_error(logger_memoria, "Error al iniciar %s server en puerto %s", SERVERNAME, server_port);
    }

<<<<<<< HEAD
    // escucha para entradaSalida:


    esperar_cliente(logger_memoria, "MEMORIA", fd_server);
    
    log_info(logger_memoria,"Conexion establecida con io");

    while (1) {}
=======
    // ############## Espera de Clientes ###################
    int fd_cliente = esperar_cliente(logger_memoria, NULL, fd_server);
>>>>>>> b3bc59453ff58b836c94c1601d39138511077092

    liberar_conexion(fd_cliente);

    free(server_port);
    log_info(logger_memoria,"server liberado");

    return EXIT_SUCCESS;
}