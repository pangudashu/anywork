.text
.globl make_context
.type make_context,@function
.align 16
make_context:

    // save the stack top to rax
    movq %rdi, %rax

    // 16-align for the stack top address
    movabs $-16, %r8
    andq %r8, %rax

    // reserve space for context-data on context-stack
    leaq -64(%rax), %rax

    // context.rip = func
    movq %rsi, 48(%rax)

    // context.end = the address of label __end
    leaq __exit(%rip), %rcx
    movq %rcx, 56(%rax)

    // return the context pointer
    ret

__exit:
    // exit(0)
    xorq %rdi, %rdi
    call _exit@PLT
    hlt

.size make_context,.-make_context


.globl jump_context
.type jump_context,@function
.align 16
jump_context:

    // save registers and construct the current context
    pushq %rbp
    pushq %rbx
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12

    // save the old context(rsp) to rax
    movq %rsp, (%rdi)

    // switch to the new context(rsp) and stack
    movq %rsi, %rsp

    // restore registers of the new context
    popq %r12
    popq %r13
    popq %r14
    popq %r15
    popq %rbx
    popq %rbp

    // restore the return or function address(rip)
    popq %r8

    // return from-context(context: rax, priv: rdx) from jump 
    movq %rdx, %rdi

    jmp *%r8

.size jump_context,.-jump_context

/* Mark that we don't need executable stack. */
.section .note.GNU-stack,"",%progbits
