/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lib/defines.h"
#include "lib/definitions.h"
#include "lib/data_set.h"
#include "lib/common.h"
#include "lib/random_pool.h"
#include "lib/instruction.h"
#include "lib/program.h"
#include "lib/virtual_machine.h"
#include "lib/individual.h"
#include "lib/deme.h"
#include "lib/environment.h"
#include "lib/translate.h"

int main(int argc, char *argv[]){

  int i;
  int generations;
  char definition[BUFFER_SIZE];
  char sources[2][BUFFER_SIZE];
  struct ENVIRONMENT *environment;

  system("clear");

  if (argc != 6){

    printf("Invalid argument number, %d\n USAGE: ./bin #generations definitions.df trainning_set.ds validation_set.ds output_file\n", argc);
    exit(-1);
  }

  generations = atoi(argv[1]);
  strcpy(definition, argv[2]);

  DATA_SETS_LENGTH = 2;
  for (i=3; i<5; i++){

    strcpy(sources[i-3], argv[i]);
  }
  strcpy(OUTPUT, argv[5]);

  srand(time (0));
  load_definition(definition);
  print_definition();

  environment = new_environment(DEMES_LENGTH, POPULATION_LIMIT, INIT_PROGRAM_LENGTH, DATA_SETS_LENGTH, sources, INITIAL_CONSTANTS);
  //print_data_set(environment->data_sets[TRAINING_SET]);
  printf("\nFor training using %d rows and %d columns\n", environment->data_sets[TRAINING_SET]->rows, environment->data_sets[TRAINING_SET]->columns);
  //print_data_set(environment->data_sets[VALIDATION_SET]);
  printf("For validation using %d rows and %d columns\n", environment->data_sets[VALIDATION_SET]->rows, environment->data_sets[VALIDATION_SET]->columns);

  //printf("Press any key to start....");
  //getchar();
  evolve_environment(environment, generations);
  
  delete_environment(environment);
  return 0;
}

