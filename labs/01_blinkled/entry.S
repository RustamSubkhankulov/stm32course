.syntax unified # instruction set choosing (ARM + Thumb) такто Thumb

.section .text  # code section

.thumb_func     
.global __reset_handler 
__reset_handler:
    blx main # jmp main
__halt:
	b __halt # branch -> jmp 

.section .vector_table
.word __stack_start    # Initial SP    - 0x00
.word __reset_handler  # Reset Handler - 0x04
