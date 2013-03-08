/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

struct VIRTUAL_MACHINE {

  int program_counter;
  int *registers;

};

struct VIRTUAL_MACHINE *new_virtual_machine() {

  struct VIRTUAL_MACHINE *aux_vm;

  aux_vm = malloc(sizeof(struct VIRTUAL_MACHINE));
  aux_vm->registers = malloc(sizeof(int)*MEMORY_SPACE);

  return aux_vm;

};

void delete_virtual_machine(struct VIRTUAL_MACHINE *virtual_machine){

  free(virtual_machine->registers);
  free(virtual_machine);
}

void print_registers(struct VIRTUAL_MACHINE *virtual_machine){

  int i;
  for(i=0; i<MEMORY_SPACE; i++){

    printf("register: %d, value: %d\n", i, virtual_machine->registers[i]);

  }

}

void init_virtual_machine(struct VIRTUAL_MACHINE *virtual_machine, int *input_elements, int *constants_elements){

  virtual_machine->program_counter = 0;
  memset(virtual_machine->registers, 0, sizeof(int)*MEMORY_SPACE);

  if (TOTAL_INPUTS){
    memcpy(virtual_machine->registers, input_elements, sizeof(int )*TOTAL_INPUTS);
  }

  if (TOTAL_CONSTANTS){
    memcpy(&virtual_machine->registers[TOTAL_INPUTS], constants_elements, sizeof(int )*TOTAL_CONSTANTS);
  }

}

int run_instruction( int _operator, int _operand1, int _operand2){

  //printf("Ran: < \"%s\", %d, %d > ", OPERATORS_STR[_operator], _operand1, _operand2);
  int result;

  switch (_operator) {

    case L_AND: {
      result = (_operand1 && _operand2);
      break;
    }

    case L_OR: {
      result = (_operand1 || _operand2);
      break;
    }

    case L_NOT: {
      result = (!_operand1);
      break;
    }

    case L_EQL: {
      result = (_operand1 == _operand2);
      break;
    }

    case L_N_EQL: {
      result = (_operand1 != _operand2);
      break;
    }

    case C_GTR: {
      result = (_operand1 > _operand2);
      break;
    }

    case C_GTR_OR_EQL: {
      result = (_operand1 >= _operand2);
      break;
    }

    case C_LSS: {
      result = (_operand1 < _operand2);
      break;
    }

    case C_LSS_OR_EQL: {
      result = (_operand1 <= _operand2);
      break;
    }

    case C_EQL: {
      result = (_operand1 == _operand2);
      break;
    }

    case C_N_EQL: {
      result = (_operand1 != _operand2);
      break;
    }

    case PLS: {
      result = (_operand1 + _operand2);
      break;
    }

    case SUB: {
      result = (_operand1 - _operand2);
      break;
    }

    case MUL: {
      result = (_operand1 * _operand2);
      break;
    }

    case DIV: {
      result = (_operand2 > 0 ? (_operand1/_operand2) : 0);
      break;
    }

  }

  //printf("Result: %d\n", result);
  //getchar();
  return result;

}

void run_program(struct VIRTUAL_MACHINE *virtual_machine, struct PROGRAM *program){

  int current_counter;

  int _operator;
  int _operand1_register;
  int _operand2_register;
  int _result_register;

  int _operand1_value;
  int _operand2_value;

  while(virtual_machine->program_counter < program->length){

     current_counter = virtual_machine->program_counter;

    _operator = program->instructions[current_counter]->_operator;
    _operand1_register = program->instructions[current_counter]->_operand1;
    _operand2_register = program->instructions[current_counter]->_operand2;
    _result_register = program->instructions[current_counter]->_result;

    _operand1_value = virtual_machine->registers[_operand1_register];
    _operand2_value = virtual_machine->registers[_operand2_register];

    virtual_machine->registers[_result_register] = run_instruction(_operator, _operand1_value, _operand2_value); 

    virtual_machine->program_counter++; 

  }

}

int extract_result(struct VIRTUAL_MACHINE *virtual_machine){

  return virtual_machine->registers[RESULT_REGISTER_INDEX];

}

void extract_constants(struct VIRTUAL_MACHINE *virtual_machine, int *constants){

  memcpy(constants, virtual_machine->registers+TOTAL_INPUTS, sizeof(int)*TOTAL_CONSTANTS);
}

int run_virtual_machine(struct VIRTUAL_MACHINE *virtual_machine, struct PROGRAM *program, int *inputs){

  int result;

  init_virtual_machine(virtual_machine, inputs, program->constants);
  run_program(virtual_machine, program);

  result = extract_result(virtual_machine);
  //extract_constants(virtual_machine, program->constants);
  
  return result;
}

