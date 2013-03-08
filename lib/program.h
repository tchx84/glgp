/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

/*
   A program is just an array of instructions
*/

struct PROGRAM {

  struct INSTRUCTION **instructions;
  int *constants;
  int length;

};

struct EFFECTIVE_DATA {

  int *instructions;
  int instructions_length;
  int *booleans;
  int booleans_length;
  int *numerics;
  int numerics_length;

};

struct CROSSOVER_STUMPS{

  struct PROGRAM *stump1;
  struct PROGRAM *stump2;
};

struct EFFECTIVE_DATA *new_data(instructions_length, booleans_length, numerics_length){

  struct EFFECTIVE_DATA *data;

  data = malloc(sizeof(struct EFFECTIVE_DATA));
  data->instructions_length = instructions_length;
  data->booleans_length = booleans_length;
  data->numerics_length = numerics_length;

  data->instructions = malloc(sizeof(int)*instructions_length);
  data->booleans = malloc(sizeof(int)*booleans_length);
  data->numerics = malloc(sizeof(int)*numerics_length);

  return data;
}

void delete_data(struct EFFECTIVE_DATA *data){

  free(data->numerics);
  free(data->booleans);
  free(data->instructions);
  free(data);
}

struct PROGRAM *new_program(int program_size, int *constants){

  int i;
  struct PROGRAM *aux_program;

  program_size = (program_size == SIZE_NO_SPECIFIED ) ? INIT_PROGRAM_LENGTH : program_size; 

  aux_program = malloc(sizeof(struct PROGRAM));
  aux_program->instructions = malloc(sizeof(struct INSTRUCTION*)*program_size);
  aux_program->length = program_size;

  aux_program->constants = malloc(sizeof(int)*TOTAL_CONSTANTS);
  memcpy(aux_program->constants, constants, sizeof(int)*TOTAL_CONSTANTS);

  for (i=0; i<aux_program->length; i++){

    aux_program->instructions[i] = malloc(sizeof(struct INSTRUCTION));
    memset(aux_program->instructions[i], 0, sizeof(struct INSTRUCTION));
  }
 
  return aux_program;

}

void delete_program(struct PROGRAM *program){

  int i;

  if (program != NULL){

    for (i = 0; i<program->length; i++){

      free(program->instructions[i]);
    }
    free(program->instructions);
    free(program->constants);
    free(program);
  }
}

void generate_random_program(struct PROGRAM *program ){

  int i;

  for (i=0; i<program->length; i++){

    random_instruction(program->instructions[i]);
  }
}

void print_constants(int *constants){

  int i;

  for (i=0; i<TOTAL_CONSTANTS; i++){

    printf("%s = %d, ", REGISTERS_LABELS[TOTAL_INPUTS+i], constants[i]);
  }
  printf("\n");
}

void print_program(struct PROGRAM *program){

  int i;

  if (program != NULL){

    printf("BEGIN\n");
    for (i=0; i<program->length; i++){

      printf("%d ", i);
      print_instruction(program->instructions[i]);
    }

    printf("END\n");
    printf("Using Constants: \n");
    print_constants(program->constants);
  } else {

    printf("Empty Program\n");
  }
}

void print_data(struct EFFECTIVE_DATA *data){

  int i;

  printf("Instructions: ");
  for(i = 0; i < data->instructions_length; i++){

    printf("%d ", data->instructions[i]);
  }
  printf("\n");

  printf("Booleans: ");
  for(i = 0; i < data->booleans_length; i++){

    printf("%d ", data->booleans[i]);
  }
  printf("\n");

  printf("Numerics: ");
  for(i = 0; i < data->numerics_length; i++){

    printf("%d ", data->numerics[i]);
  }
  printf("\n");

}

struct EFFECTIVE_DATA *find_effective_data(struct PROGRAM *program, int target_register, int instruction_limit){

  int *is_effective_register;
  int booleans_length;
  int current_boolean_register;
  int numerics_length;
  int current_numeric_register;
  int current_register;

  int *is_effective_instruction;
  int instructions_length;
  int current_instruction;
  int current_effective_instruction;

  int current_operator;
  int current_operand1;
  int current_operand2;
  int current_result;

  struct EFFECTIVE_DATA *data;

  //printf("eff in..\n");

  is_effective_register = malloc(sizeof(int)*MEMORY_SPACE);
  memset(is_effective_register, FALSE, sizeof(int)*MEMORY_SPACE);

  is_effective_instruction = malloc(sizeof(int)*program->length);
  memset(is_effective_instruction, FALSE, sizeof(int)*program->length);

  is_effective_register[target_register] = TRUE;
  current_instruction = program->length-1;
  instructions_length = 0;

  //printf("before while\n");
  while (current_instruction > instruction_limit){

    if (is_effective_register[program->instructions[current_instruction]->_result]){
      
      is_effective_instruction[current_instruction] = TRUE;

      current_operator = program->instructions[current_instruction]->_operator;
      current_operand1 = program->instructions[current_instruction]->_operand1;
      current_operand2 = program->instructions[current_instruction]->_operand2;
      current_result = program->instructions[current_instruction]->_result;

      is_effective_register[current_result] = FALSE;
      is_effective_register[current_operand1] = TRUE;

      if (current_operator != L_NOT){

        is_effective_register[current_operand2] = TRUE;
      }

      instructions_length++;
    }

    current_instruction--;
  }

  //printf("before booleans..\n");
  booleans_length = 0;
  numerics_length = 0;
  for(current_register = RESULT_REGISTER_INDEX; current_register < MEMORY_SPACE; current_register++){

    if (is_effective_register[current_register]){
  
      if(is_boolean(current_register)){

        booleans_length++;
      } else {
 
        numerics_length++;
      }
    }
  }

  data = new_data(instructions_length, booleans_length, numerics_length);

  //printf("before this current\n");
  current_effective_instruction = 0;
  for (current_instruction = 0; current_instruction < program->length; current_instruction++){

    if (is_effective_instruction[current_instruction]){

      data->instructions[current_effective_instruction] = current_instruction;
      current_effective_instruction++;
    }
  }

  //printf("before algo...\n");
  current_boolean_register = 0;
  current_numeric_register = 0;
  for (current_register = RESULT_REGISTER_INDEX; current_register < MEMORY_SPACE; current_register++){

    if (is_effective_register[current_register]){

      if (is_boolean(current_register)){

        data->booleans[current_boolean_register] = current_register;
        current_boolean_register++;
      } else {

        data->numerics[current_numeric_register] = current_register;
        current_numeric_register++;
      }
    }
  }
  
  //printf("before freedd\n");
  free(is_effective_register);
  free(is_effective_instruction);
  //printf("rff out\n");
  return data;
}

struct PROGRAM *extract_effective_program(struct PROGRAM *program) {

  int current_instruction;
  int effective_instruction;
  struct PROGRAM *effective_program;
  struct EFFECTIVE_DATA *data;

  data = find_effective_data(program, RESULT_REGISTER_INDEX, NONE);
  effective_program = new_program(data->instructions_length, program->constants);

  for (current_instruction = 0; current_instruction < data->instructions_length; current_instruction++){

    effective_instruction = data->instructions[current_instruction];
    memcpy( effective_program->instructions[current_instruction], program->instructions[effective_instruction], sizeof(struct INSTRUCTION) );
  }

  delete_data(data);
  return effective_program;
}

struct PROGRAM *add_instruction(struct PROGRAM *program, int is_effective_mutation){

  int current_instruction;
  int selected_instruction;
  int new_length;
  struct INSTRUCTION instruction;
  struct PROGRAM *mutated_program;
  struct EFFECTIVE_DATA *data;

  new_length = program->length+1;
  mutated_program = NULL;

  //printf("IN ADD\n");

  //print_program(program);

  if ( new_length < MAX_PROGRAM_LENGTH ){
   
    do {

      if (is_effective_mutation){

        selected_instruction = (rand()%program->length+1)-1;
        //printf("ADD WILL FIND DATA\n");
        data = find_effective_data(program, RESULT_REGISTER_INDEX, selected_instruction);
        //print_data(data);
        //printf("ADD FOUND DATA\n");

        if (data->booleans_length*(LOGIC_DISTRIBUTION+COMPARISON_DISTRIBUTION) > 0 || data->numerics_length*ARITHMETIC_DISTRIBUTION > 0){

          //printf("N: %d B: %d\n", data->numerics_length, data->booleans_length);
          //printf("ADD WILL CREATE eff INST\n");
          result_inducted_instruction(&instruction, data->booleans, data->booleans_length, data->numerics, data->numerics_length);
          //printf("ADD CREATED eff INST\n");
          //printf("Selected Instruction = %d\n", selected_instruction);
          //print_data(data);
          free(data);
          break;
        }
   
        free(data);
      } else {

        random_instruction(&instruction); 
        break;
      }

    }while(TRUE);

    //printf("i = %d, generated = ", selected_instruction);
    //print_instruction(&instruction);

    mutated_program = new_program(new_length, program->constants);

    for(current_instruction = 0; current_instruction <= selected_instruction; current_instruction++){

      memcpy(mutated_program->instructions[current_instruction], program->instructions[current_instruction], sizeof(struct INSTRUCTION));
    }

    memcpy(mutated_program->instructions[selected_instruction+1], &instruction, sizeof(struct INSTRUCTION));

    for(current_instruction = selected_instruction + 2; current_instruction < mutated_program->length; current_instruction++){

      memcpy(mutated_program->instructions[current_instruction], program->instructions[current_instruction-1], sizeof(struct INSTRUCTION));
    }

  }

  return mutated_program;
}

struct PROGRAM *remove_instruction(struct PROGRAM *program, int is_effective_mutation){

  int selected_instruction;
  int current_instruction;
  int mutation_current_instruction;
  int new_length;
  int inst_length;

  struct PROGRAM *mutated_program;
  struct EFFECTIVE_DATA *data;

  new_length = program->length-1;
  mutated_program = NULL;
  selected_instruction = rand()%program->length;

  if (new_length > MIN_PROGRAM_LENGTH){

    if (is_effective_mutation){

      data = find_effective_data(program, RESULT_REGISTER_INDEX, NONE);
      if (data->instructions_length > 0){

        selected_instruction = data->instructions[rand()%data->instructions_length];
      }
      delete_data(data);
    }

    //printf("Eliminated i = %d ", selected_instruction);
    //print_instruction(program->instructions[selected_instruction]);

    mutated_program = new_program(new_length, program->constants);

    current_instruction = 0;
    mutation_current_instruction = 0;
    while (current_instruction < program->length){

      if (current_instruction != selected_instruction){
  
        memcpy(mutated_program->instructions[mutation_current_instruction], program->instructions[current_instruction], sizeof(struct INSTRUCTION));
        mutation_current_instruction++;
      }
    
      current_instruction++;
    }
  }

  return mutated_program;
}

void micro_mutation(struct PROGRAM *program, int is_effective_mutation){

  int mutated;
  int selected_instruction;
  int dice_roll;
  int operand1;
  int operand2;
  int operand;
  struct EFFECTIVE_DATA *data;

  mutated = FALSE;

  //printf("Micro Got IN\n");
  //printf("micro program len%d \n", program->length);

  do {

    selected_instruction = rand()%program->length;
    if (is_effective_mutation){

      //printf("Micro find data IN\n");    
      data = find_effective_data(program, RESULT_REGISTER_INDEX, NONE);
      //printf("Micro find data OUT\n");
      if (data->instructions_length > 0){

        selected_instruction = data->instructions[rand()%data->instructions_length];
      } else {

        is_effective_mutation = FALSE;
      }
      
      //printf("Micro Deleting data\n");
      delete_data(data);
    }
  
    //printf("Will try i = %d\n", selected_instruction);
    //print_instruction(program->instructions[selected_instruction]);

    dice_roll = rand()%100;
    if (dice_roll < REGISTER_DISTRIBUTION){

      dice_roll = rand()%100;
      if (dice_roll < DESTINATION_DISTRIBUTION){
    
        if (is_effective_mutation){

          //printf("micro will find\n");
          data = find_effective_data(program, RESULT_REGISTER_INDEX, selected_instruction);
          if (data->booleans_length > 0 || data->numerics_length > 0) {

            //printf("Micro DEST IN\n");
            mutated = mutate_destination(program->instructions[selected_instruction], data->booleans, data->booleans_length, data->numerics, data->numerics_length);
            //printf("Micro DEST OUT\n");
          }
          delete_data(data);
        } else {
        
          //printf("MICRO DEST NOT EFF IN\n");
          mutated = mutate_destination(program->instructions[selected_instruction], all_booleans, all_booleans_length, all_numerics, all_numerics_length);
          //printf("MICRO DEST NOT EFF OUT\n");
        }
      } else {

        //printf("Micro OPRND IN\n");
        mutated = mutate_operand(program->instructions[selected_instruction]);
        //printf("Micro OPRND OUT\n");
      }
    } else {

      if ( dice_roll < OPERATOR_DISTRIBUTION){

        //printf("Micro OPTR IN\n");
        mutated = mutate_operator(program->instructions[selected_instruction]);
        //printf("Micro OPTR OUT\n");
      } else {
 
        //printf("Micro CONST IN\n");
        mutated = did_mutate_constant(program->instructions[selected_instruction], program->constants);
        //printf("Micro CONST OUT\n");
      }
    }
  } while(!mutated);

  //printf("Mutated, i = %d ", selected_instruction);
  //print_instruction(program->instructions[selected_instruction]);
  //printf("Micro Got OUT\n");
}

struct PROGRAM *clon_program(struct PROGRAM *program){

  int current_instruction;
  struct PROGRAM *program_clon;

  program_clon = new_program(program->length, program->constants);

  for (current_instruction = 0; current_instruction < program->length; current_instruction++){

    memcpy(program_clon->instructions[current_instruction], program->instructions[current_instruction], sizeof(struct PROGRAM));
  }

  return program_clon;
}

struct PROGRAM *mutate_program(struct PROGRAM *program){

  int dice_roll;
  struct PROGRAM *mutated_program;
  
  mutated_program = NULL;
  do {

    dice_roll = rand()%100;
    if (dice_roll < MICRO_MUTATION_DISTRIBUTION){

      //printf("MUT CLON IN ..., %d \n", mutated_program);
      mutated_program = clon_program(program);
      //printf("MUT MICRO IN\n");
      micro_mutation(mutated_program, EFFECTIVE_MUTATION);
      //printf("MUT MICRO OUT %d \n", mutated_program);
    } else {

      dice_roll = rand()%100;
      if (dice_roll < REMOVE_MUTATION_DISTRIBUTION){

        //printf("MUT REMOVE IN, %d \n", mutated_program);
        mutated_program = remove_instruction(program, EFFECTIVE_MUTATION);
        //printf("MUT REMOVE OUT, %d \n", mutated_program);
      } else {

        //printf("MUT ADD IN, %d \n", mutated_program);
        mutated_program = add_instruction(program, EFFECTIVE_MUTATION);
        //printf("MUT ADD OUT, %d \n", mutated_program);
      }
    }
  } while(!mutated_program);

  return mutated_program;
}

struct CROSSOVER_STUMPS onepoint_crossover(struct PROGRAM *program1, struct PROGRAM *program2){

  int minimum;

  int smaller_point;
  int bigger_point;
  int distance;

  int smaller_descendant_length;
  int bigger_descendant_length;

  int condition0;
  int condition1;
  int condition2;
  int condition3;
  int condition4;

  int smaller_instruction;
  int bigger_instruction;

  struct PROGRAM *smaller;
  struct PROGRAM *bigger;
  struct CROSSOVER_STUMPS stumps;

  smaller = program1;
  bigger = program2;
  if (program2->length < program1->length){

    smaller = program2;
    bigger = program1;
  }

  //printf("CROSS s->l = %d, b->l = %d\n", smaller->length, bigger->length);

  minimum = smaller->length < MAX_CROSSOVER_DISTANCE ? smaller->length : MAX_CROSSOVER_DISTANCE;

  do {

    smaller_point = rand()%smaller->length;
    bigger_point = rand()%bigger->length;

    distance = smaller_point < bigger_point ? (bigger_point - smaller_point) : (smaller_point - bigger_point);
    condition1 = (distance <= minimum);

    smaller_descendant_length = smaller->length - smaller_point;
    bigger_descendant_length = bigger->length - bigger_point;
    condition2 = (smaller_descendant_length <= bigger_descendant_length);

    condition3 = (bigger->length - (bigger_descendant_length - smaller_descendant_length)) < MIN_PROGRAM_LENGTH;
    condition4 = (smaller->length + (bigger_descendant_length - smaller_descendant_length)) > MAX_PROGRAM_LENGTH;

  } while(!(condition1 && condition2 && !(condition3 || condition4)));

  //printf("D = %d, MAX = %d \n", distance, minimum);
  //printf("L(s) = %d, L(b) = %d \n", smaller->length, bigger->length);
  //printf("I(s) = %d, I(b) = %d \n", smaller_point, bigger_point);
  //printf("S(s) = %d, S(b) = %d \n", smaller_descendant_length, bigger_descendant_length);
  //printf("C1 = %d, C2 = %d, C3 = %d, C4 = %d\n", condition1, condition2, condition3, condition4);


  stumps.stump1 = new_program(smaller->length + (bigger_descendant_length - smaller_descendant_length), smaller->constants);
  stumps.stump2 = new_program(bigger->length - (bigger_descendant_length - smaller_descendant_length), bigger->constants);

  for (smaller_instruction = 0; smaller_instruction < smaller_point; smaller_instruction++ ){

    memcpy(stumps.stump1->instructions[smaller_instruction], smaller->instructions[smaller_instruction], sizeof(struct INSTRUCTION));
  }

  bigger_instruction = bigger_point;
  for (smaller_instruction = smaller_point; smaller_instruction < stumps.stump1->length; smaller_instruction++ ){

    memcpy(stumps.stump1->instructions[smaller_instruction], bigger->instructions[bigger_instruction], sizeof(struct INSTRUCTION));
    bigger_instruction++;
  }

  for (bigger_instruction = 0; bigger_instruction < bigger_point; bigger_instruction++){
  
    memcpy(stumps.stump2->instructions[bigger_instruction], bigger->instructions[bigger_instruction], sizeof(struct INSTRUCTION));
  }
  
  smaller_instruction = smaller_point;
  for (bigger_instruction = bigger_point; bigger_instruction < stumps.stump2->length; bigger_instruction++){

    memcpy(stumps.stump2->instructions[bigger_instruction], smaller->instructions[smaller_instruction], sizeof(struct INSTRUCTION));
    smaller_instruction++;   
  }
  //printf("CROSS cpy\n");

  //printf("CROSS done...\n");
  return stumps;
}

void check_programs(struct PROGRAM *program1, struct PROGRAM *program2){

  int i,j;

  for (i=0; i<program1->length; i++){

    for (j=0; j<program2->length; j++){
    
      if (i != j && program1->instructions[i] == program2->instructions[j]){

        //printf("ERROR: %d == %d\n",program1->instructions[i], program2->instructions[j]);
        exit(-1);
      }
    }  
  }
  printf("programs OK.\n");
}

