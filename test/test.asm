main:
    addi $t0, $zero, 0
    main_loop_begin:
        sw $t0, variable($zero)
        addi $t0, $t0, 1
        j main_loop_begin

variable:
    dd 0
