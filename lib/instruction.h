/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

/* 
   Three address code instruction:
   2^5 = 32 possible operators
   2^9 = 512 possible registers 
*/

struct INSTRUCTION {

  int _operator : 5;
  int _operand1 : 9;
  int _operand2 : 9;
  int _result : 9; 

};

int random_register(int definition[3][MAX_REGISTERS]);
int random_operand(int definition[3][3][MAX_REGISTERS]);
int random_operator(int definition[3][6]);
void random_instruction(struct INSTRUCTION *instruction);

void print_instruction(struct INSTRUCTION *instruction){

    printf("< \"%s\", %s, %s, %s >\n", OPERATORS_STR[instruction->_operator], REGISTERS_LABELS[instruction->_operand1], REGISTERS_LABELS[instruction->_operand2], REGISTERS_LABELS[instruction->_result]);
}

void set_random_instruction(struct INSTRUCTION *instruction, int operator_type, int operand_type, int result_type){

  instruction->_operator = random_operator(OPERATORS_DEFINITION[operator_type]);
  instruction->_operand1 = random_operand(REGISTERS_DEFINITION[operand_type]);
  do
    instruction->_operand2 = random_operand(REGISTERS_DEFINITION[operand_type]);
  while ( (instruction->_operand1 == instruction->_operand2));
  instruction->_result = random_register(REGISTERS_DEFINITION[result_type][REGISTER]);

}

void random_instruction(struct INSTRUCTION *instruction){

  int dice_roll;

  dice_roll = rand()%100;

  //printf("DR: %d\n", dice_roll);
  if ( dice_roll < ARITHMETIC_DISTRIBUTION ){

    set_random_instruction(instruction, ARITHMETIC, NUMERIC, NUMERIC);
  } else {

    //if ( dice_roll > ARITHMETIC_DISTRIBUTION && dice_roll < LOGIC_DISTRIBUTION ) {
    if ( dice_roll < LOGIC_DISTRIBUTION ) {

      set_random_instruction(instruction, LOGIC, BOOLEAN, BOOLEAN);
    } else {
 
      set_random_instruction(instruction, COMPARISON, NUMERIC, BOOLEAN);
    }

  }
  //printf("Out of DR...\n");
  //getchar();

}

int random_operator(int definition[3][6]){

  int operator_index;
  int _operator;

  operator_index = rand() % *definition[CARDINALITY];
  _operator = definition[SET][operator_index];

  //printf("\"%s\", ", OPERATORS_STR[_operator]);
  return _operator;
}

int random_operand(int definition[2][3][MAX_REGISTERS]){

  int operand_index;
  int register_type;
  int dice_roll;

  dice_roll = rand()%100;
  //printf("RO IN: (%d)\n", dice_roll);

  if ( dice_roll  < *definition[INPUT][DISTRIBUTION] ){

    //printf("INPUT\n");
    register_type = INPUT;

  } else {

    //if ( ( dice_roll >= *definition[INPUT][DISTRIBUTION] ) && ( dice_roll < *definition[CONSTANT][DISTRIBUTION] ) ){
    if ( dice_roll < *definition[CONSTANT][DISTRIBUTION] ){

      //printf("CONSTANT\n");
      register_type = CONSTANT;

    } else {

      //printf("REGISTER\n");
      register_type = REGISTER;

    }

  }

  operand_index = random_register(definition[register_type]);

  //printf("RO OUT: %d\n", operand_index);
  //getchar();
  return operand_index;
}

int random_register(int definition[3][MAX_REGISTERS]){

  int relative_index;
  int index;

  //printf("RR IN:\n");
  relative_index = rand()% *definition[CARDINALITY];
  index = definition[INDEXES][relative_index];

  //printf("RR OUT:\n");
  return index;
}

/*void result_inducted_instruction(struct INSTRUCTION *instruction, int *boolean_registers, int booleans_length, int *numeric_registers, int numerics_length){

  int dice_roll;

  dice_roll = rand()%(booleans_length+numerics_length);

  if (dice_roll < booleans_length){

    dice_roll = rand()%(100-ARITHMETIC_DISTRIBUTION);
    
    if (dice_roll+ARITHMETIC_DISTRIBUTION < LOGIC_DISTRIBUTION){

      set_random_instruction(instruction, LOGIC, BOOLEAN, BOOLEAN);
    } else {

      set_random_instruction(instruction, COMPARISON, NUMERIC, BOOLEAN);
    }

    instruction->_result = boolean_registers[rand()%booleans_length];
  } else {

    set_random_instruction(instruction, ARITHMETIC, NUMERIC, NUMERIC);
    instruction->_result = numeric_registers[rand()%numerics_length];
  }

}*/

void result_inducted_instruction(struct INSTRUCTION *instruction, int *boolean_registers, int booleans_length, int *numeric_registers, int numerics_length){

  int dice_roll;
  int numeric_balance = numerics_length*ARITHMETIC_DISTRIBUTION;
  int boolean_balance = booleans_length*(LOGIC_DISTRIBUTION+COMPARISON_DISTRIBUTION);

  dice_roll = rand()%(numeric_balance+boolean_balance);

  //printf("%d %d %d\n", dice_roll, numeric_balance, boolean_balance);

  if (dice_roll < numeric_balance){

    set_random_instruction(instruction, ARITHMETIC, NUMERIC, NUMERIC);
    instruction->_result = numeric_registers[rand()%numerics_length];
  } else {

    dice_roll = (rand()%(100-ARITHMETIC_DISTRIBUTION))+ARITHMETIC_DISTRIBUTION;
    
    if (dice_roll < LOGIC_DISTRIBUTION){

      set_random_instruction(instruction, LOGIC, BOOLEAN, BOOLEAN);
    } else {

      set_random_instruction(instruction, COMPARISON, NUMERIC, BOOLEAN);
    }

    instruction->_result = boolean_registers[rand()%booleans_length];
  }
  //printf("Out of here...\n");
}

int mutate_destination(struct INSTRUCTION *instruction, int *booleans, int booleans_length, int *numerics, int numerics_length){

  if (is_boolean(REGISTERS_MAPPING[instruction->_result])){

    if (booleans_length > 0) {

      instruction->_result = booleans[rand()%booleans_length];
      //printf("Boolean Destinarion Mutation!\n");
      return TRUE;
    }
  } else {

    if (numerics_length > 0){

      instruction->_result = numerics[rand()%numerics_length];
      //printf("Numeric Destinarion Mutation!\n");
      return TRUE;
    }
  }

  return FALSE;
}

int mutate_operand(struct INSTRUCTION *instruction){

  int operand_type;

  operand_type = REGISTERS_MAPPING[instruction->_operand1];

  do {

    if ( rand()%2 && instruction->_operator != L_NOT ){

      instruction->_operand2 = random_operand(REGISTERS_DEFINITION[operand_type]);
    } else {

      instruction->_operand1 = random_operand(REGISTERS_DEFINITION[operand_type]);    
    }
  } while(instruction->_operand1 == instruction->_operand2);

  //printf("Operand mutation!\n");
  return TRUE;
}

int mutate_operator(struct INSTRUCTION *instruction){

  int operator_type;
  
  operator_type = OPERATORS_MAPPING[instruction->_operator];
  instruction->_operator = random_operator(OPERATORS_DEFINITION[operator_type]);

  //printf("Operator mutation!\n");
  return TRUE;
}

void mutate_constant(int constant_index, int *constants){

  //int before;
  
  //printf("entro viste\n");
  constant_index = constant_index - TOTAL_INPUTS;
  //before = constants[constant_index];   
  switch (REGISTERS_MAPPING[constant_index]){
  
    case BOOLEAN:{

      constants[constant_index] = !constants[constant_index];
      //printf("\nBoolean Constant mutation! #%d, before = %d and now = %d \n", constant_index, before, constants[constant_index]);
      break;
    }

    case NUMERIC:{

      //printf("CONST MUT!\n");
      int value = abs(constants[constant_index]);
      constants[constant_index] += ((rand()%(value != 0 ? value : 2))*(rand()%2 ? 1 : -1));
      //constants[constant_index] += ((rand()%STANDARD_DEVIATION)*(rand()%2 ? 1 : -1));
      //printf("\nNumeric Constant mutation! #%d, before = %d and now = %d \n", constant_index, before, constants[constant_index]);
      break;
    }
  }
  //getchar();
}

int did_mutate_constant(struct INSTRUCTION *instruction, int *constants){

  int operand1;
  int operand2;

  operand1 = instruction->_operand1;
  operand2 = instruction->_operand2;

  if (is_constant(operand1) && is_constant(operand2)){

    mutate_constant(rand()%2 ? operand1 : operand2, constants);
    return TRUE;
  }

  if (is_constant(operand1)){

    mutate_constant(operand1, constants);
    return TRUE;
  }

  if (is_constant(operand2)){

    mutate_constant(operand2, constants);
    return TRUE;
  }

  return FALSE;
}

