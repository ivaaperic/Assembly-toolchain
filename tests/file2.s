.extern start
.global prog, exit, null
.section text
prog:
  jmp start
exit:
  halt
null:
  iret