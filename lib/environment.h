/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

struct ENVIRONMENT {

  int demes_length;
  int data_sets_length;
  struct DEME **demes;
  struct DATA_SET **data_sets;
};

struct DATA_SET *random_data_set(struct ENVIRONMENT *environment){

  return environment->data_sets[rand() %  environment->data_sets_length];
}

struct ENVIRONMENT *new_environment(int demes_length, int population_limit, int individual_length, int data_sets_length, char sources[][BUFFER_SIZE], int *constants){

  int current;
  struct ENVIRONMENT *environment;

  environment = malloc(sizeof(struct ENVIRONMENT));

  environment->data_sets_length = data_sets_length;
  environment->data_sets = malloc(sizeof(struct DATA_SET*)*data_sets_length);
  for (current = 0; current < data_sets_length; current++){

    environment->data_sets[current] = load_data_set(sources[current]);
  }

  environment->demes_length = demes_length;
  environment->demes = malloc(sizeof(struct DEME*)*demes_length);
  for (current = 0; current < demes_length; current++){

    environment->demes[current] = new_deme(population_limit);
    randomly_populate_deme(environment->demes[current], individual_length, constants, environment->data_sets[TRAINING_SET]);
  }

  return environment;
}

void delete_environment(struct ENVIRONMENT *environment){

  int counter;

  for (counter = 0; counter < environment->demes_length; counter++){

    delete_deme(environment->demes[counter], TRUE);
  }
  free(environment->demes);

  for (counter = 0; counter < environment->data_sets_length; counter++){

    delete_data_set(environment->data_sets[counter]);
  }
  free(environment->data_sets);
  free(environment);
}

void print_environment(struct ENVIRONMENT *environment){

  int current;

  for (current = 0; current < environment->demes_length; current++){

    print_deme(environment->demes[current]);
  }  
  
  /*for (current = 0; current < environment->data_sets_length; current++){

    print_data_set(environment->data_sets[current]);
  }*/

}

void evolve_environment(struct ENVIRONMENT *environment, int generations){

  int current_generation;
  int current_deme;
  struct DATA_SET *training_data_set;
  struct DATA_SET *validation_data_set;
  struct DEME *bests_deme;
  struct INDIVIDUAL *best_individual;

  training_data_set = environment->data_sets[TRAINING_SET];
  validation_data_set = environment->data_sets[VALIDATION_SET];

  for (current_generation = 0; current_generation < generations;  current_generation++){

      printf("Generation %d: \n", current_generation);
      for (current_deme = 0; current_deme < environment->demes_length;  current_deme++){

        evolve_one_step_deme(environment->demes[current_deme], training_data_set, validation_data_set);
        printf("Deme %d, best fitness: %f (%s) ", current_deme, environment->demes[current_deme]->individuals[0]->fitness, FITNESS_METHOD_STR[fitness_method]);
        printf("Length: %d(eff)|%d \n", environment->demes[current_deme]->individuals[0]->effective_program->length, environment->demes[current_deme]->individuals[0]->program->length);
    }

      if (environment->demes_length > 1){

        printf("Migrating... \n");
        for (current_deme = 0; current_deme < environment->demes_length;  current_deme++){

          migrate_deme(environment->demes[current_deme], environment->demes[(current_deme+1)%environment->demes_length]);
        }
      }
  }

  bests_deme = new_deme(environment->demes_length);
  for (current_deme = 0; current_deme < environment->demes_length;  current_deme++){

    sort_deme(environment->demes[current_deme]);
    push_individual(bests_deme, environment->demes[current_deme]->individuals[0]);
  }

  sort_deme(bests_deme);
  best_individual = bests_deme->individuals[0];
  printf("\nTrainning results: \n"); 
  print_individual(best_individual);

  printf("\nValidation: \n");
  calculate_fitness(best_individual, validation_data_set);
  print_individual(best_individual);
  delete_deme(bests_deme, FALSE);

  translate_program(best_individual->effective_program);
}

