.text

.global __td_entry
__td_entry:
    movq %r12, %rdi
    jmp *%r13

.global __td_switch
__td_switch:
    movq %rsp, 8(%rdi)
    movq %rbp, 16(%rdi)
    movq %rbx, 24(%rdi)
    movq %r12, 32(%rdi)
    movq %r13, 40(%rdi)
    movq %r14, 48(%rdi)
    movq %r15, 56(%rdi)
    movq 8(%rsi), %rsp
    movq 16(%rsi), %rbp
    movq 24(%rsi), %rbx
    movq 32(%rsi), %r12
    movq 40(%rsi), %r13
    movq 48(%rsi), %r14
    movq 56(%rsi), %r15
    leaq switch(%rip), %rax
    movq %rax, (%rdi)
    jmp *(%rsi)

switch:
    ret
