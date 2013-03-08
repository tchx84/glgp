/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

void recalculate_distribution(){

  int numerics_length = 0;
  int booleans_length = 0;
  int n_input_dist = 0;
  int n_constant_dist = 0;
  int n_register_dist = 0;
  int b_input_dist = 0;
  int b_constant_dist = 0;
  int b_register_dist = 0;

  int operators_length = 0;
  int arithmetic_balance = 0;
  int logic_balance = 0;
  int comparison_balance = 0;
  int arithmetic_abs_dist = 0;
  int comparison_abs_dist = 0;
  int logic_abs_dist = 0;
  int arithmetic_dist = 0;
  int comparison_dist = 0;
  int logic_dist = 0;

  //printf("here!\n");

  // Registers distribution calc
  numerics_length = (REGISTERS_DEFINITION[NUMERIC][INPUT][CARDINALITY][0]+REGISTERS_DEFINITION[NUMERIC][CONSTANT][CARDINALITY][0]+REGISTERS_DEFINITION[NUMERIC][REGISTER][CARDINALITY][0]);

  //printf("NL: %d\n", numerics_length);

  if (numerics_length > 0){

    n_input_dist = (REGISTERS_DEFINITION[NUMERIC][INPUT][CARDINALITY][0]*100/numerics_length);
    n_constant_dist = ((REGISTERS_DEFINITION[NUMERIC][CONSTANT][CARDINALITY][0]*100/numerics_length));
    n_register_dist = ((REGISTERS_DEFINITION[NUMERIC][REGISTER][CARDINALITY][0]*100/numerics_length));

    REGISTERS_DEFINITION[NUMERIC][INPUT][DISTRIBUTION][0] = n_input_dist;
    REGISTERS_DEFINITION[NUMERIC][CONSTANT][DISTRIBUTION][0] = n_constant_dist == 0 ? 0 : n_input_dist + n_constant_dist;
    REGISTERS_DEFINITION[NUMERIC][REGISTER][DISTRIBUTION][0] = n_register_dist == 0 ? 0 : n_input_dist + n_constant_dist + n_register_dist;
  }

  //printf("nID: %d, nCD: %d, nRD: %d\n", n_input_dist, n_constant_dist, n_register_dist);

  booleans_length = (REGISTERS_DEFINITION[BOOLEAN][INPUT][CARDINALITY][0]+REGISTERS_DEFINITION[BOOLEAN][CONSTANT][CARDINALITY][0]+REGISTERS_DEFINITION[BOOLEAN][REGISTER][CARDINALITY][0]);

  //printf("BL: %d\n", booleans_length);

  if (booleans_length > 0){

    b_input_dist = (REGISTERS_DEFINITION[BOOLEAN][INPUT][CARDINALITY][0]*100/booleans_length);
    b_constant_dist = ((REGISTERS_DEFINITION[BOOLEAN][CONSTANT][CARDINALITY][0]*100/booleans_length));
    b_register_dist = ((REGISTERS_DEFINITION[BOOLEAN][REGISTER][CARDINALITY][0]*100/booleans_length));

    REGISTERS_DEFINITION[BOOLEAN][INPUT][DISTRIBUTION][0] = b_input_dist;
    REGISTERS_DEFINITION[BOOLEAN][CONSTANT][DISTRIBUTION][0] = b_constant_dist == 0 ? 0 : b_input_dist + b_constant_dist;
    REGISTERS_DEFINITION[BOOLEAN][REGISTER][DISTRIBUTION][0] = b_register_dist == 0 ? 0 : b_input_dist + b_constant_dist + b_register_dist;

  }

  //printf("bID: %d, bCD: %d, bRD: %d\n", b_input_dist, b_constant_dist, b_register_dist);
  //Now depending on register dist, recalc instructions dist

  arithmetic_abs_dist = 1 * ARITHMERIC_AVAILABLE;//OPERATORS_DEFINITION[ARITHMETIC][CARDINALITY][0]*100/TOTAL_OPERATORS;
  logic_abs_dist = 1;//OPERATORS_DEFINITION[LOGIC][CARDINALITY][0]*100/TOTAL_OPERATORS;
  comparison_abs_dist = 1;//OPERATORS_DEFINITION[COMPARISON][CARDINALITY][0]*100/TOTAL_OPERATORS;

  //printf("AAD: %d, LAD: %d, CAD %d:\n", arithmetic_abs_dist, logic_abs_dist, comparison_abs_dist);

  arithmetic_balance = (arithmetic_abs_dist * ((numerics_length)*n_register_dist));
  logic_balance = (logic_abs_dist * ((booleans_length)*b_register_dist));
  comparison_balance = (comparison_abs_dist * ((numerics_length)*b_register_dist));

  operators_length = arithmetic_balance + logic_balance + comparison_balance;

  arithmetic_dist = (arithmetic_balance*100)/operators_length;
  logic_dist = (logic_balance*100)/operators_length;
  comparison_dist = (comparison_balance*100)/operators_length;

  //printf("OL: %d, AD: %d, LD: %d, CD: %d\n",operators_length, arithmetic_dist, logic_dist, comparison_dist);

  OPERATORS_DEFINITION[ARITHMETIC][DISTRIBUTION][0] = arithmetic_dist;
  OPERATORS_DEFINITION[LOGIC][DISTRIBUTION][0] = logic_dist == 0 ? 0 : arithmetic_dist+logic_dist;
  OPERATORS_DEFINITION[COMPARISON][DISTRIBUTION][0] = comparison_dist == 0 ? 0 : arithmetic_dist+logic_dist+comparison_dist;

}

void load_definition(char *filepath){

  FILE *file;
  char category_str[BUFFER_SIZE];
  char type_str[BUFFER_SIZE];
  char register_label[BUFFER_SIZE];
  int current_register;
  int type;
  int category;
  int length;
  int value;
  int current_constant;

  file = fopen(filepath, "r");
  if (!file){

    printf("%s register definition not found.\n", filepath);
    exit(-1);
  }

  current_constant = 0;
  current_register = 0;
  while(!feof(file)){

    //printf("R: %d\n", current_register);
    fscanf(file, "%s %s %d %s\n", category_str, type_str, &value, register_label);
    //printf("%s %s %d\n", category_str, type_str, value);

    strcpy(REGISTERS_LABELS[current_register], register_label);
    if (strcmp(type_str, "NUMERIC")==0){

      //printf("type = 0\n");
      REGISTERS_MAPPING[current_register] = NUMERIC;
      type = 0; 
    } else {

      //printf("type = 1\n");
      REGISTERS_MAPPING[current_register] = BOOLEAN;
      type = 1;
    }

    if (strcmp(category_str, "INPUT")==0){

      //printf("category = 0\n");
      category = 0;
    } else {

      if (strcmp(category_str, "CONSTANT")==0){

        //printf("category = 1\n");
        category = 1;

        INITIAL_CONSTANTS[current_constant++] = value;

      } else {

        //printf("category = 2\n");
        category = 2;
      }
    }

    REGISTERS_DEFINITION[type][category][INDEXES][REGISTERS_DEFINITION[type][category][CARDINALITY][0]++] = current_register;
    current_register++;
  }

  //printf("Got here...\n");
  fclose(file);

  recalculate_distribution();

}

void print_index(int set[MAX_REGISTERS], int length, int distribution){

  int i;

  printf("{");
  for (i=0; i<length; i++){

    printf("%d,", set[i]);
  }
  printf("}");

  printf(" {%d}", length);

  printf(" {%d} \n", distribution);

}

void print_definition(){

 int i;

 printf("<<Using constants>>\n");
 for (i=0; i<TOTAL_CONSTANTS; i++){

   printf("%d (%s) : %s = %d\n", i, REGISTERS_LABELS[TOTAL_INPUTS+i], REGISTERS_MAPPING[TOTAL_INPUTS+i] == NUMERIC ? "NUMERIC" : "BOOLEAN", INITIAL_CONSTANTS[i]);
 }

 printf("\n<<Using registers mapping>>\n");
 for (i=0; i<MEMORY_SPACE; i++){

   printf("%d (%s) : %s\n", i, REGISTERS_LABELS[i], REGISTERS_MAPPING[i] == NUMERIC ? "NUMERIC" : "BOOLEAN");
 }

 printf("\n<<Registers distribution>>\n");
 //printf("NUMERIC:\n");
 //printf("INPUTS: ");
 print_index(REGISTERS_DEFINITION[NUMERIC][INPUT][INDEXES], REGISTERS_DEFINITION[NUMERIC][INPUT][CARDINALITY][0], REGISTERS_DEFINITION[NUMERIC][INPUT][DISTRIBUTION][0]);
 //printf("CONSTANTS:\n");
 print_index(REGISTERS_DEFINITION[NUMERIC][CONSTANT][INDEXES], REGISTERS_DEFINITION[NUMERIC][CONSTANT][CARDINALITY][0], REGISTERS_DEFINITION[NUMERIC][CONSTANT][DISTRIBUTION][0]);
 //printf("REGISTERS:\n");
 print_index(REGISTERS_DEFINITION[NUMERIC][REGISTER][INDEXES], REGISTERS_DEFINITION[NUMERIC][REGISTER][CARDINALITY][0], REGISTERS_DEFINITION[NUMERIC][REGISTER][DISTRIBUTION][0]);

 //printf("BOOLEAN:\n");
 //printf("INPUTS: ");
 print_index(REGISTERS_DEFINITION[BOOLEAN][INPUT][INDEXES], REGISTERS_DEFINITION[BOOLEAN][INPUT][CARDINALITY][0], REGISTERS_DEFINITION[BOOLEAN][INPUT][DISTRIBUTION][0]);
 //printf("CONSTANTS:\n");
 print_index(REGISTERS_DEFINITION[BOOLEAN][CONSTANT][INDEXES], REGISTERS_DEFINITION[BOOLEAN][CONSTANT][CARDINALITY][0], REGISTERS_DEFINITION[BOOLEAN][CONSTANT][DISTRIBUTION][0]);
 //printf("REGISTERS:\n");
 print_index(REGISTERS_DEFINITION[BOOLEAN][REGISTER][INDEXES], REGISTERS_DEFINITION[BOOLEAN][REGISTER][CARDINALITY][0], REGISTERS_DEFINITION[BOOLEAN][REGISTER][DISTRIBUTION][0]);


 printf("\n<<Operators_distribution>>\n");
 print_index(OPERATORS_DEFINITION[ARITHMETIC][INDEXES], OPERATORS_DEFINITION[ARITHMETIC][CARDINALITY][0], OPERATORS_DEFINITION[ARITHMETIC][DISTRIBUTION][0]);
 print_index(OPERATORS_DEFINITION[LOGIC][INDEXES], OPERATORS_DEFINITION[LOGIC][CARDINALITY][0], OPERATORS_DEFINITION[LOGIC][DISTRIBUTION][0]);
 print_index(OPERATORS_DEFINITION[COMPARISON][INDEXES], OPERATORS_DEFINITION[COMPARISON][CARDINALITY][0], OPERATORS_DEFINITION[COMPARISON][DISTRIBUTION][0]);

}

