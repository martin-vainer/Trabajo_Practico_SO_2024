#include "protocolo.h"

/*
 *      Package create      t_package* package_create(t_msg_header msg_header)
 *      Package destroy     void package_destroy(t_package* package)
 *      Package send        int package_send(t_package* package, int fd)
 *      Package receive     int package_recv(t_package* package, int fd)
 */

/*
 *      Package
 */

/*########################################## T_PACKAGE FUNCTIONS ##########################################*/
// Package create
t_package *package_create(t_msg_header msg_header, u_int32_t buffer_size)
{

    t_package *package = malloc(sizeof(t_package));

    package->msg_header = msg_header;
    package->buffer = buffer_create(buffer_size);

    return package;
}

// Package destroy
void package_destroy(t_package *package)
{
    if (package->buffer != 0)
    {
        buffer_destroy(package->buffer);
    }

    free(package);
}

// Package send
int package_send(t_package *package, int fd)
{
    size_t size = sizeof(t_msg_header)     // Message header
                  + sizeof(uint32_t)       // Buffer size
                  + package->buffer->size; // Buffer

    void *stream = malloc(size);
    int offset = 0;

    memcpy(stream + offset, &(package->msg_header), sizeof(t_msg_header));
    offset += sizeof(t_msg_header);
    memcpy(stream + offset, &(package->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, package->buffer->stream, package->buffer->size);

    send(fd, stream, size, 0);

    free(stream);

    return EXIT_SUCCESS;
}

// Abstraction of sockets recv method. It should check if the connection has ended
void _recv_and_check_response(int fd, void *buf, size_t size, int flags)
{
    ssize_t recv_response = recv(fd, buf, size, flags);
    if (recv_response == 0)
    {
        printf("SE HA DESCONECTADO EL SOCKET. ABORTANDO PROCESO\n");
        exit(1); // TODO para resolver memoryleaks se debería hacer un graceful-exit
    }
}

// Package receive
// El package debe ser creado con package_create
int package_recv(t_package *package, int fd)
{

    _recv_and_check_response(fd, &(package->msg_header), sizeof(t_msg_header), 0);

    uint32_t size;
    _recv_and_check_response(fd, &size, sizeof(uint32_t), 0);

    if (size == 0)
    {
        return EXIT_SUCCESS;
    }

    void *stream = malloc(size);
    _recv_and_check_response(fd, stream, size, 0);

    package->buffer->size = size;
    package->buffer->stream = stream;

    return EXIT_SUCCESS;
}

t_buffer* get_buffer(t_package* package) {
    return package->buffer;
}

t_msg_header get_message_header(t_package* package) {
    return package->msg_header;
}

/*########################################## T_MESSAGE_EXAMPLE FUNCTIONS ##########################################*/
t_message_example* message_example_create(char* cadena, uint8_t entero)
{
    t_message_example* example = malloc(sizeof(t_message_example));

    if(example == NULL) return NULL;

    // Guardo la cadena
    example->cadena = my_strdup(cadena);
    
    // Guardo el entero
    example->entero = entero;

    return example;
}

char* get_cadena(t_message_example* example) 
{
    return example->cadena;
}

uint8_t get_entero(t_message_example* example)
{
    return example->entero;
}

uint32_t get_message_example_size(t_message_example* example) 
{
    return sizeof(uint32_t) + (strlen(example->cadena) + 1) + sizeof(example->entero);
}

void message_example_destroy(t_message_example* example)
{
    if (example->cadena != NULL)
    {
        free(example->cadena);
    }

    free(example);
}
/*##########################################  SEND AND RECIVE FUNCTIONS ##########################################*/
int send_pcb(t_msg_header msg_header, int fd, t_PCB* pcb) 
{
    uint32_t buffer_size = get_pcb_size(pcb);
    t_package* package = package_create(msg_header, buffer_size);

    serialize_pcb(get_buffer(package), pcb);
    package_send(package, fd);

    package_destroy(package);

    return 0;
}

t_PCB* recv_pcb(int fd)
{
    t_buffer *new_buffer = recive_full_buffer(fd);
    t_PCB* pcb = deserialize_pcb(new_buffer);

    buffer_destroy(new_buffer);

    return pcb; 
}

int send_example(char* cadena, uint8_t entero, int fd) 
{
    t_message_example* example = message_example_create(cadena, entero);

    if(example == NULL) return -1;

    //Tamaño para el buffer
    uint32_t buffer_size = get_message_example_size(example);

    //Creo el paquete
    t_package* package_example = package_create(EXAMPLE, buffer_size);

    // Serializo el t_message_example en el buffer
    example_serialize_msg(get_buffer(package_example), example);

    // Envio el paquete a fd
    package_send(package_example, fd);

    message_example_destroy(example);
    package_destroy(package_example);
    return 0;
}

t_message_example* recv_example(int fd) 
{
    t_buffer *new_buffer = recive_full_buffer(fd);
    t_message_example* msg_example = example_deserialize_msg(new_buffer);

    buffer_destroy(new_buffer);

    return msg_example;
}

int send_interruption(t_interruption* interruption, int fd)
{
    t_package* package = package_create(MSG_QUANTUM, get_interruption_size(interruption));

    serialize_interruption(get_buffer(package), interruption);

    package_send(package, fd);

    package_destroy(package);

    return 0;
}

t_interruption* recv_interruption(int fd) 
{
    t_buffer *new_buffer = recive_full_buffer(fd);
    t_interruption* interruption = deserialize_interruption(new_buffer);

    buffer_destroy(new_buffer);

    return interruption;
}

void send_instrution(int fd, t_instruction* instruction)
{
    // Creo el paquete que se va a enviar
    t_package* package = package_create(MSG_INSTRUCTION_MEMORIA, obtener_instruction_size(instruction));

    // Serializo en el buffer el t_instruction
    serialize_instruction(get_buffer(package), instruction);

    // Envio el paquete
    package_send(package, fd);

    // Elimino t_instruction
    eliminar_instruccion(instruction);

    //Elimino el paquete usado
    package_destroy(package);
}

t_instruction* recv_instruction(int fd)
{
    t_buffer* buffer = recive_full_buffer(fd);
    t_instruction* instruction = deserialize_instruction(buffer);
    
    buffer_destroy(buffer);

    return instruction;
}

void send_get_next_instruction(int fd, uint32_t pid, uint32_t program_counter)
{
    t_next_instruction* next_instruction = crear_siguiente_instruccion(pid, program_counter);

    // Creo el paquete que se va a enviar
    t_package* package = package_create(MSG_NEXT_INSTRUCTION_CPU, obtener_next_instruction_size(next_instruction));

    // Serializo en el buffer el t_next_instruction
    serialize_next_instruction(get_buffer(package), next_instruction);

    // Envio el paquete
    package_send(package, fd);

    // Elimino t_next_instruction
    eliminar_next_instruction(next_instruction);

    //Elimino el paquete usado
    package_destroy(package);
}

t_next_instruction* recv_next_instruction(int fd) 
{
    t_buffer* buffer = recive_full_buffer(fd);

    if(buffer == NULL) return NULL;

    t_next_instruction* next_instruction= deserialize_next_instruction(buffer);

    buffer_destroy(buffer);

    return next_instruction;
}

void send_new_process(int fd, uint32_t pid, char* path)
{
    t_new_process* new_process = create_new_process(pid, path);

    // Creo el paquete que se va a enviar
    t_package* package = package_create(MSG_KERNEL_CREATE_PROCESS, get_size_new_process(new_process));

    // Serializo en el buffer el t_new_process
    serialize_nuevo_proceso(get_buffer(package), new_process);

    // Envio el paquete
    package_send(package, fd);

    // Elimino t_new_process
    destroy_new_process(new_process);

    //Elimino el paquete usado
    package_destroy(package);
}

t_new_process* recv_new_process(int fd)
{
    t_buffer* buffer = recive_full_buffer(fd);

    if(buffer == NULL) return NULL;

    t_new_process* new_process= deserialize_nuevo_proceso(buffer);

    buffer_destroy(buffer);

    return new_process;
}

void send_solicitud_io_generica(int fd, t_PCB* pcb, char* nombre_interfaz, t_io_generica* generica)
{
    t_solicitud_io_generica* solicitud = crear_solicitud_io_generica(pcb, nombre_interfaz, generica);

    // Creo el paquete que se va a enviar
    t_package* package = package_create(MSG_CPU_IO_GEN_SLEEP, obtener_tamanio_solicitud_generica(solicitud));

    // Serializo en el buffer el t_solicitud_io_generica
    serializar_solicitud_io_generica(get_buffer(package), solicitud);

    // Envio el paquete
    package_send(package, fd);

    // Elimino t_new_process
    destruir_solicitud_io_generica(solicitud);

    //Elimino el paquete usado
    package_destroy(package);
} 

t_solicitud_io_generica* recv_solicitud_io_generica(int fd)
{
    t_buffer* buffer = recive_full_buffer(fd);

    if(buffer == NULL) return NULL;

    t_solicitud_io_generica* solicitud= deserializar_solicitud_io_generica(buffer);

    buffer_destroy(buffer);

    return solicitud;
}

void send_solicitud_io_stdin(int fd, t_PCB* pcb, char* nombre_interfaz, t_io_stdin* io_stdin)
{
    t_solicitud_io_stdin* solicitud = crear_solicitud_io_stdin(pcb, nombre_interfaz, io_stdin);

    // Creo el paquete que se va a enviar
    t_package* package = package_create(MSG_CPU_IO_GEN_SLEEP, obtener_tamanio_solicitud_stdin(solicitud));

    // Serializo en el buffer el t_solicitud_io_generica
    serializar_solicitud_io_stdin(get_buffer(package), solicitud);

    // Envio el paquete
    package_send(package, fd);

    // Elimino t_new_process
    destruir_solicitud_io_stdin(solicitud);

    //Elimino el paquete usado
    package_destroy(package);
}

t_solicitud_io_stdin* recv_solicitud_io_stdin(int fd)
{
    t_buffer* buffer = recive_full_buffer(fd);

    if(buffer == NULL) return NULL;

    t_solicitud_io_stdin* solicitud= deserializar_solicitud_io_stdin(buffer);

    buffer_destroy(buffer);

    return solicitud;
}

void send_solicitud_io_stdout(int fd, t_PCB* pcb, char* nombre_interfaz, t_io_stdout* io_stdout)
{
    t_solicitud_io_stdout* solicitud = crear_solicitud_io_stdout(pcb, nombre_interfaz, io_stdout);

    // Creo el paquete que se va a enviar
    t_package* package = package_create(MSG_CPU_IO_GEN_SLEEP, obtener_tamanio_solicitud_stdout(solicitud));

    // Serializo en el buffer el t_solicitud_io_generica
    serializar_solicitud_io_stdout(get_buffer(package), solicitud);

    // Envio el paquete
    package_send(package, fd);

    // Elimino t_new_process
    destruir_solicitud_io_stdout(solicitud);

    //Elimino el paquete usado
    package_destroy(package);
} 

t_solicitud_io_stdout* recv_solicitud_io_stdout(int fd)
{
    t_buffer* buffer = recive_full_buffer(fd);

    if(buffer == NULL) return NULL;

    t_solicitud_io_stdout* solicitud= deserializar_solicitud_io_stdout(buffer);

    buffer_destroy(buffer);

    return solicitud;
}

void send_IO_interface(int fd, char* nombre_interfaz, char* tipo)
{
    tipo_interfaz_t tipo_interfaz = string_to_tipo_interfaz(tipo);

    t_IO_interface* interface = crear_IO_interface(nombre_interfaz, tipo_interfaz);

    // Creo el paquete que se va a enviar
    t_package* package = package_create(MSG_IO_KERNEL, obtener_size_IO_interface(interface));

    // Serializo en el buffer el t_solicitud_io_generica
    serializar_IO_interface(get_buffer(package), interface);

    // Envio el paquete
    package_send(package, fd);

    // Elimino t_new_process
    liberar_IO_interface(interface);

    //Elimino el paquete usado
    package_destroy(package);
} 

t_IO_interface* recv_IO_interface(int fd)
{
    t_buffer* buffer = recive_full_buffer(fd);

    if(buffer == NULL) return NULL;

    t_IO_interface* interface= deserializar_IO_interface(buffer);

    buffer_destroy(buffer);

    return interface;
}

/*########################################## SERIALIZE AND DESERIALIZE FUNCTIONS ##########################################*/
// serializado generico TP0
void *serializar_paquete(t_package *paquete, int bytes)
{
    void *stream = malloc(bytes);
    int desplazamiento = 0;

    memcpy(stream + desplazamiento, &(paquete->msg_header), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(stream + desplazamiento, &(paquete->buffer->size), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(stream + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    desplazamiento += paquete->buffer->size;

    return stream;
}

void example_serialize_msg(t_buffer *buffer, t_message_example *msg)
{
    // Agrego la cadena
    buffer_add_string(buffer, get_cadena(msg));

    // Agrego el numero entero
    buffer_add_uint8(buffer, get_entero(msg));
}

t_message_example* example_deserialize_msg(t_buffer* buffer)
{
    // Copiar el tamaño de la cadena
    uint32_t size_cadena = buffer_read_uint32(buffer);

    // Obtengo la cadena
    char* cadena = buffer_read_string(buffer, size_cadena);

    // Obtengo el numero entero
    uint8_t entero = buffer_read_uint8(buffer);

    //Creo la estrucutura t_message_example
    return message_example_create(cadena, entero);
}

void serialize_cpu_registers(t_buffer *buffer, t_cpu_registers *cpu_registers)
{
    //Agrego pc
    buffer_add_uint32(buffer, cpu_registers->pc);
    //Agrego ax
    buffer_add_uint8(buffer, cpu_registers->ax);
    //Agrego bx
    buffer_add_uint8(buffer, cpu_registers->bx);
    //Agrego cx
    buffer_add_uint8(buffer, cpu_registers->cx);
    //Agrego dx
    buffer_add_uint8(buffer, cpu_registers->dx);
    //Agrego eax
    buffer_add_uint32(buffer, cpu_registers->eax);
    //Agrego ebx
    buffer_add_uint32(buffer, cpu_registers->ebx);
    //Agrego ecx
    buffer_add_uint32(buffer, cpu_registers->ecx);
    //Agrego edx
    buffer_add_uint32(buffer, cpu_registers->edx);
    //Agrego si
    buffer_add_uint32(buffer, cpu_registers->si);
    //Agrego di
    buffer_add_uint32(buffer, cpu_registers->di);
}

// PCB
void serialize_pcb(t_buffer *buffer, t_PCB *pcb)
{
    //Agrego el pid
    buffer_add_uint32(buffer, pcb->pid);

    //Agrego quantum
    buffer_add_uint32(buffer, pcb->quantum);

    //Agrego el pc
    buffer_add_uint32(buffer, pcb->program_counter);

    // CPU registers
    uint32_t buffer_cpu_registers_size = get_cpu_registers_size(get_cpu_registers(pcb));
    t_buffer *buffer_cpu_registers = buffer_create(buffer_cpu_registers_size);

    serialize_cpu_registers(buffer_cpu_registers, get_cpu_registers(pcb));

    //Agrego size del buffer_cpu_registers
    buffer_add_uint32(buffer, buffer_cpu_registers_size);

    //Agrego el contenido de buffer_cpu_registers en buffer 
    buffer_add_buffer(buffer, buffer_cpu_registers);

    buffer_destroy(buffer_cpu_registers);
}

t_PCB* deserialize_pcb(t_buffer *buffer)
{
    // Obtengo pid
    uint32_t pid = buffer_read_uint32(buffer);

    // Obtengo quantum 
    uint32_t quantum = buffer_read_uint32(buffer);

    // Creo pcb
    t_PCB* pcb = pcb_create(pid, quantum);

    // Obtengo pc
    pcb->program_counter = buffer_read_uint32(buffer);

    // Obtengo el tamaño del sub-buffer de t_cpu_registers
    uint32_t buffer_cpu_registers_size = buffer_read_uint32(buffer);

    // Creo sub-buffer de t_cpu_registers
    t_buffer* buffer_cpu_registers = buffer_create(buffer_cpu_registers_size);

    // Agrego parte del contenido de buffer en buffer_cpu_registers
    buffer_add_partial_buffer(buffer_cpu_registers, buffer, buffer_cpu_registers_size);

    // Obtengo los registros de la cpu
    deserialize_cpu_registers(buffer_cpu_registers, pcb->cpu_registers);
    
    buffer_destroy(buffer_cpu_registers);

    return pcb;
}

void deserialize_cpu_registers(t_buffer *buffer, t_cpu_registers *cpu_registers)
{
    cpu_registers->pc= buffer_read_uint32(buffer);
    cpu_registers->ax= buffer_read_uint8(buffer);
    cpu_registers->bx= buffer_read_uint8(buffer);
    cpu_registers->cx= buffer_read_uint8(buffer);
    cpu_registers->dx= buffer_read_uint8(buffer);
    cpu_registers->eax= buffer_read_uint32(buffer);
    cpu_registers->ebx= buffer_read_uint32(buffer);
    cpu_registers->ecx= buffer_read_uint32(buffer);
    cpu_registers->edx= buffer_read_uint32(buffer);
    cpu_registers->si= buffer_read_uint32(buffer);
    cpu_registers->di= buffer_read_uint32(buffer);
}

void serialize_nuevo_proceso(t_buffer *buffer, t_new_process *nuevo_proceso) {

    //Agrego el pid del nuevo proceso
    buffer_add_uint32(buffer, nuevo_proceso->pid);

    //Agrego el largo de la cadena y el path
    buffer_add_string(buffer, nuevo_proceso->path);
}

t_new_process* deserialize_nuevo_proceso(t_buffer* buffer) {
    if(buffer == NULL) return NULL;

    //Obtenemos el pid del nuevo proceso
    uint32_t pid_new_process = buffer_read_uint32(buffer);

    //Obtenemos el size del path relativo
    uint32_t path_length= buffer_read_uint32(buffer);

    //Obtenemos el path relativo del codigo
    char* path_relative = buffer_read_string(buffer, path_length);

    return create_new_process(pid_new_process, path_relative);
}

void serialize_instruction(t_buffer *buffer, t_instruction *instruccion) {
    // Serializar el nombre de la instrucción
    buffer_add_uint32(buffer, (uint32_t)instruccion->name);

    // Serializar el número de parámetros
    uint32_t num_params = list_size(instruccion->params);
    buffer_add_uint32(buffer, num_params);

    // Serializar cada parámetro
    for (int i = 0; i < num_params; i++) {
        char* param = list_get(instruccion->params, i);
        buffer_add_string(buffer, param);
    }
}

t_instruction* deserialize_instruction(t_buffer *buffer) {

    // Obtengo el nombre de la instruccion
    t_name_instruction name = buffer_read_uint32(buffer);

    // Obtengo la cantidad de parametros
    uint32_t count_params = buffer_read_uint32(buffer);

    // Creo la lista donde se guardaran los parametros
    t_list* params = list_create();

    for(size_t i = 0; i < count_params; i++) {
        uint32_t size_param = buffer_read_uint32(buffer);

        // Leo el parametro
        char* param = buffer_read_string(buffer, size_param);

        // Agrego el parametro a la lista de parametros
        list_add(params, param);
    }

    return crear_instruccion_con_parametros(name, params);
}

void serialize_next_instruction(t_buffer* buffer, t_next_instruction* next_instruction) {
    // Serializar el pid del proceso
    buffer_add_uint32(buffer, obtener_pid_process(next_instruction));

    // Serializar el program counter que apunta a la siguiente instruccion
    buffer_add_uint32(buffer, obtener_pc_process(next_instruction));
}

t_next_instruction* deserialize_next_instruction(t_buffer* buffer) {
    // Obtengo el pid del proceso
    uint32_t pid = buffer_read_uint32(buffer);

    // Obtengo el program counter que apunta a la siguiente instruccion
    uint32_t pc = buffer_read_uint32(buffer);

    return crear_siguiente_instruccion(pid, pc);
}

t_interruption* deserialize_interruption(t_buffer* buffer)
{
    // Obtengo el nombre de la interrupcion
    t_name_interruption name = buffer_read_uint32(buffer);

    // Obtengo el pid
    uint32_t pid = buffer_read_uint32(buffer);

    return create_interruption(name, pid);
}

void serialize_interruption(t_buffer* buffer, t_interruption* interruption)
{
    // Agrego el nombre de la interruption
    buffer_add_uint32(buffer, (uint32_t)get_name(interruption));

    // Agrego el pid de la interruption
    buffer_add_uint32(buffer, get_pid_interruption(interruption));
}

void serializar_solicitud_io_generica(t_buffer* buffer, t_solicitud_io_generica* solicitud) {
    serialize_pcb(buffer, solicitud->pcb);
    buffer_add_string(buffer, solicitud->nombre_interfaz);
    serializar_io_generica(buffer, solicitud->generica);
}

t_solicitud_io_generica* deserializar_solicitud_io_generica(t_buffer* buffer) {
    t_PCB* pcb = deserialize_pcb(buffer);

    uint32_t length_string = buffer_read_uint32(buffer);
    char* nombre_interfaz = buffer_read_string(buffer,length_string);

    t_io_generica* generica = deserializar_io_generica(buffer);

    return crear_solicitud_io_generica(pcb, nombre_interfaz, generica);
}

void serializar_io_generica(t_buffer* buffer, t_io_generica* io_generica) {
    buffer_add_string(buffer, io_generica->nombre_interfaz);
    buffer_add_uint32(buffer, io_generica->tiempo_sleep);
    buffer_add_uint32(buffer, io_generica->pid);
}

t_io_generica* deserializar_io_generica(t_buffer* buffer) {
    uint32_t length_string = buffer_read_uint32(buffer);
    char* nombre_interfaz = buffer_read_string(buffer,length_string);

    uint32_t tiempo_sleep = buffer_read_uint32(buffer);

    uint32_t pid = buffer_read_uint32(buffer);

    return crear_io_generica(nombre_interfaz, tiempo_sleep, pid);
}

void serializar_solicitud_io_stdin(t_buffer* buffer, t_solicitud_io_stdin* solicitud) {
    serialize_pcb(buffer, solicitud->pcb);
    buffer_add_string(buffer, solicitud->nombre_interfaz);
    serializar_io_stdin(buffer, solicitud->io_stdin);
}

t_solicitud_io_stdin* deserializar_solicitud_io_stdin(t_buffer* buffer) {
    t_PCB* pcb = deserialize_pcb(buffer);

    uint32_t length_string = buffer_read_uint32(buffer);
    char* nombre_interfaz = buffer_read_string(buffer,length_string);

    t_io_stdin* io_stdin = deserializar_io_stdin(buffer);

    return crear_solicitud_io_stdin(pcb, nombre_interfaz, io_stdin);
}

void serializar_io_stdin(t_buffer* buffer, t_io_stdin* io_stdin) {
    buffer_add_uint32(buffer, io_stdin->direccion_fisica);
    buffer_add_uint32(buffer, io_stdin->tamanio);
    buffer_add_uint32(buffer, io_stdin->pid);
}

t_io_stdin* deserializar_io_stdin(t_buffer* buffer) {
    uint32_t direccion_fisica = buffer_read_uint32(buffer);
    uint32_t tamanio = buffer_read_uint32(buffer);
    uint32_t pid = buffer_read_uint32(buffer);
    return crear_io_stdin(direccion_fisica, tamanio, pid);
}

void serializar_solicitud_io_stdout(t_buffer* buffer, t_solicitud_io_stdout* solicitud) {
    serialize_pcb(buffer, solicitud->pcb);
    buffer_add_string(buffer, solicitud->nombre_interfaz);
    serializar_io_stdout(buffer, solicitud->io_stdout);
}

t_solicitud_io_stdout* deserializar_solicitud_io_stdout(t_buffer* buffer) {
    t_PCB* pcb = deserialize_pcb(buffer);

    uint32_t length_string = buffer_read_uint32(buffer);
    char* nombre_interfaz = buffer_read_string(buffer,length_string);

    t_io_stdout* io_stdout = deserializar_io_stdout(buffer);

    return crear_solicitud_io_stdout(pcb, nombre_interfaz, io_stdout);
}

void serializar_io_stdout(t_buffer* buffer, t_io_stdout* io_stdout) {
    buffer_add_uint32(buffer, io_stdout->direccion_fisica);
    buffer_add_uint32(buffer, io_stdout->tamanio);
    buffer_add_uint32(buffer, io_stdout->pid);
}

t_io_stdout* deserializar_io_stdout(t_buffer* buffer) {
    uint32_t direccion_fisica = buffer_read_uint32(buffer);
    uint32_t tamanio = buffer_read_uint32(buffer);
    uint32_t pid = buffer_read_uint32(buffer);
    return crear_io_stdout(direccion_fisica, tamanio, pid);
}

void serializar_IO_interface(t_buffer* buffer, t_IO_interface* interface)
{
    buffer_add_string(buffer, obtener_nombre_IO_interface(interface));
    buffer_add_uint32(buffer, obtener_tipo_IO_interface(interface));
}

t_IO_interface* deserializar_IO_interface(t_buffer* buffer)
{
    uint32_t lenght_string = buffer_read_uint32(buffer);
    char* nombre_interfaz = buffer_read_string(buffer, lenght_string);
    tipo_interfaz_t tipo = buffer_read_uint32(buffer);

    t_IO_interface* interfaz = crear_IO_interface(nombre_interfaz, tipo);

    free(nombre_interfaz);

    return interfaz;
}