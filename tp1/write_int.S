/* Funcion para escribir un entero a un archivo
// PARAMETROS
// a0: entero a escribir
// a1: file descriptor
// Valor de retorno: numeros de bytes escritos
*/

#include <mips/regdef.h>
#include <sys/syscall.h>
#define STDERR 2
#define FRAME_SIZE 40
#define GP_POS 24
#define TEMP_CHAR 16
#define ASCII_OFFSET 48
#define ERROR_MSG_LENGTH 19
.text
.align 2
.ent write_int
.global write_int

write_int:
### Inicializo el frame y guardo los registros correspondientes ###
.frame $fp, FRAME_SIZE, ra
.set noreorder
.cpload t9
.set reorder
subu sp, sp, FRAME_SIZE
.cprestore GP_POS
sw $fp, 28(sp)
sw ra, 32(sp)
move $fp, sp
sw a0, FRAME_SIZE($fp)
sw a1, FRAME_SIZE+4($fp)

### Comienzo de la funcion ###

### Analizo si el entero a escribir es zero ###
lw t0, FRAME_SIZE($fp)
bnez t0, no_zero
sw t0, FRAME_SIZE($fp)
addu t0, t0, zero 
sw t0, TEMP_CHAR($fp)
j write

/* Hago a0 / 10 para obtener el ultimo digito del numero a escribir
// Este digito va a estar en el resto de la division
// Si el cociente es distinto de cero, debo llamar recursivamente a la funcion
// ya que el primer numero a escribir es el de mas significativo
*/
no_zero:
lw t0, FRAME_SIZE($fp)
li t1, 10
div t0, t1
sw t0, FRAME_SIZE($fp)
mflo t0	# cociente
mfhi t1 # resto
sw t1, TEMP_CHAR($fp)
beqz t0, write

### Preparo llamada recursiva ###
move a0, t0
lw a1, FRAME_SIZE+4($fp)
la t9, write_int
jal write_int

### Escribo el caracter al archivo indicado ###
write:
lw t0, TEMP_CHAR($fp)
addu t0, t0, ASCII_OFFSET
sw t0, TEMP_CHAR($fp)
lw a0, FRAME_SIZE+4($fp)
la a1, TEMP_CHAR($fp)
li a2, 1
li v0, SYS_write
syscall

### Chequeo error ###
bgez v0, end
li a0, STDERR 
la a1, error_msg
li a2, ERROR_MSG_LENGTH 
li v0, SYS_write
syscall

### Rearmo el stack ###
end:
lw ra, 32($fp)
lw gp, 24($fp)
lw $fp, 28($fp)
addu sp, sp, FRAME_SIZE
jr ra

.end write_int

.data
error_msg: .asciiz "Error de escritura\n"
