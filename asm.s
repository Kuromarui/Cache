section .text
    global _main
_main:
 
    mov rax, 10      

    mov rbx, rax     

    mov rax, 60      
    xor rdi, rdi   
    syscall           