.extern exit
.global main, intr
.section text
main:
  
  ldr r0, $2
  ldr r1, $1
  cmp r0, r1
  ldr r5, $4  #
  int r5      # skok ali sacuvaj psw
  jgt save_psw
  jmp exit
intr:
  ldr r1, $2
  cmp r0, r0
  iret
save_psw:
  ldr r2, $3
  ret