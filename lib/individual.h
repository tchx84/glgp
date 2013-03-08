/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

struct INDIVIDUAL{

  struct PROGRAM *program;
  struct PROGRAM *effective_program;
  float fitness;
  float accuracy;
  float fmeasure;
  float specificity;
  float sensitivity;
  float precision;
};

void calculate_fitness(struct INDIVIDUAL *, struct DATA_SET*);

struct INDIVIDUAL *new_individual(){

  struct INDIVIDUAL *individual;

  individual = malloc(sizeof(struct INDIVIDUAL));
  individual->program = NULL;
  individual->effective_program = NULL;
  individual->fitness = 0;
  individual->accuracy = 0;
  individual->fmeasure = 0;
  individual->specificity = 0;
  individual->sensitivity = 0;
  individual->precision = 0;

  return individual;
}

struct INDIVIDUAL *new_individual_from(struct PROGRAM *program, struct DATA_SET *data_set){

  struct INDIVIDUAL *individual;

  individual = malloc(sizeof(struct INDIVIDUAL));
  individual->program = program;
  individual->effective_program = extract_effective_program(program);
  calculate_fitness(individual, data_set);

  return individual;
}

void delete_individual(struct INDIVIDUAL *individual){

  delete_program(individual->program);
  delete_program(individual->effective_program);
  free(individual);
}

void print_individual(struct INDIVIDUAL *individual){

  //printf("Program:\n");
  //print_program(individual->program);
  
  printf("Effective Program:\n");
  print_program(individual->effective_program);

  printf("Accuracy: %f \n", individual->accuracy);
  printf("Specificity: %f \n", individual->specificity);
  printf("Sensitivity: %f \n", individual->sensitivity);
  printf("Precision: %f \n", individual->precision);
  printf("F-measure: %f \n", individual->fmeasure);
  printf("ROC axes: (%f, %f) \n", (1.0 - individual->specificity), individual->sensitivity);
  printf("Fitness: %f (%s) \n", individual->fitness, FITNESS_METHOD_STR[fitness_method]);
  //printf("____________\n");
}

void calculate_fitness(struct INDIVIDUAL *individual, struct DATA_SET *data_set){

  int row;
  int output;
  int result;
  float true_positives = 0;
  float false_positives = 0;
  float true_negatives = 0;
  float false_negatives = 0;
  float specificity = 0;
  float sensitivity = 0;
  float accuracy = 0;
  float precision = 0;
  float fmeasure = 0;
  float aux = 0;
  struct VIRTUAL_MACHINE *virtual_machine;

  virtual_machine = new_virtual_machine();
  for(row = 0; row < data_set->rows; row++){

    output = run_virtual_machine(virtual_machine, individual->effective_program, data_set->data[row]);
    result = data_set->data[row][data_set->columns-1];

    if (output == TRUE){

      if (result == TRUE){
        true_positives++;
      }else {
        false_positives++;
      }
    }else {

      if (result == FALSE){
        true_negatives++;
      }else {
        false_negatives++;
      }
    }
  }
  
  aux = (true_negatives + false_positives);
  specificity = (aux > 0) ? (true_negatives / aux) : 0;
  individual->specificity = specificity;

  accuracy = (true_positives + true_negatives) / (true_negatives + false_negatives + true_positives + false_positives);
  individual->accuracy = accuracy;

  aux = (true_positives + false_negatives);
  sensitivity = (aux > 0) ? (true_positives / aux) : 0;
  individual->sensitivity = sensitivity;

  aux = ( true_positives + false_positives);
  precision = (aux > 0) ? (true_positives / aux) : 0;
  individual->precision = precision;

  aux = (precision + sensitivity);
  fmeasure = (aux > 0) ? (2.0 * ((precision * sensitivity) / aux)) : 0;
  individual->fmeasure = fmeasure;

  switch(fitness_method){
  
    case ACCURACY: {
      individual->fitness = accuracy;
      break;
    }

    case FMEASURE:{
      individual->fitness = fmeasure;
      break;
    }
  }

  //printf("TP: %f \n", true_positives);
  //printf("FP: %f \n", false_positives);
  //printf("TN: %f \n", true_negatives);
  //printf("FN: %f \n", false_negatives);
  //printf("SPEC: %f \n", specificity);
  //printf("SENS: %f \n", sensitivity);
  //printf("ACCU: %f \n", accuracy);
  //printf("PREC: %f \n", precision);
  //printf("FMEA: %f \n", fmeasure);
  delete_virtual_machine(virtual_machine);
}

void apply_genetic_operand(struct INDIVIDUAL *individual, struct DATA_SET *training_data_set){

  struct PROGRAM *mutated_program;

  //printf("Orifinal Program:\n");
  //print_program(individual->program);
  //getchar();

  mutated_program = mutate_program(individual->program);
  //printf("mutated program...\n");

  //printf("O = %d, C = %d\n", individual->program, mutated_program);
  //print_program(individual->program);
  //print_program(mutated_program);
  //getchar();

  delete_program(individual->program);
  //printf("Deleted Program...\n");

  delete_program(individual->effective_program);
  //printf("Deleted efftive Program...\n");

  //printf("About to recalculate...\n");
  individual->program = mutated_program;
  individual->effective_program = extract_effective_program(mutated_program);
  calculate_fitness(individual, training_data_set);
  //printf("got out!\n");
  //getchar();
}

struct INDIVIDUAL *clone_individual(struct INDIVIDUAL *individual){

  struct INDIVIDUAL *clone;
  
  clone = new_individual();
  clone->program = clon_program(individual->program);
  clone->effective_program = clon_program(individual->effective_program);
  clone->accuracy = individual->accuracy;
  clone->fmeasure = individual->fmeasure;
  clone->fitness = individual->fitness;
  clone->specificity = individual->specificity;
  clone->sensitivity = individual->sensitivity;
  clone->precision = individual->precision;

  return clone;
}

