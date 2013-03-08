/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

struct DEME{

  struct INDIVIDUAL **individuals;
  int population_length;
  int population_limit;
};

struct SELECTION_DATA{

  struct DEME *selected;
  int *selected_indexes;
  int selection_length;
};

struct DATA_PAIR{

 struct INDIVIDUAL *contender;
 int index;
};

struct DEME *new_deme(int population_limit){

  int current;
  struct DEME *deme;

  deme = malloc(sizeof(struct DEME));
  deme->individuals = malloc(sizeof(struct PROGRAM*)*population_limit);
  deme->population_limit = population_limit;
  deme->population_length = 0;

  return deme;
}

void delete_deme(struct DEME *deme, int individuals_too){

  int current;

  if (individuals_too){

    for(current = 0; current < deme->population_length; current++){

      delete_individual(deme->individuals[current]);
    }
  }

  free(deme->individuals);
  free(deme);
}

void print_selection_data(struct SELECTION_DATA *data, int population_length){

  int current;
 
  for (current = 0; current < population_length; current++){

    printf("i = %d, s = %d \n", current, data->selected_indexes[current]);
  }
}

struct SELECTION_DATA *new_selection_data(int selection_limit, int population_length){

  struct SELECTION_DATA *data;

  data = malloc(sizeof(struct SELECTION_DATA));
  data->selected = new_deme(selection_limit);

  data->selected_indexes = malloc(sizeof(int)*population_length);
  memset(data->selected_indexes, FALSE, sizeof(int)*population_length);

  data->selection_length = selection_limit;

  return data;
}

void delete_selection_data(struct SELECTION_DATA *data){

  delete_deme(data->selected, FALSE);
  free(data->selected_indexes);
  free(data);
}

void push_individual(struct DEME *deme, struct INDIVIDUAL *individual){

  deme->population_length++;
  deme->individuals[deme->population_length-1] = individual;
}

void randomly_populate_deme(struct DEME *deme, int individual_length, int *constants, struct DATA_SET *data_set){

  int current;
  struct INDIVIDUAL *individual;

  for(current = 0; current < deme->population_limit; current++){

    individual = new_individual();

    individual->program = new_program(individual_length, constants);
    generate_random_program(individual->program);
    individual->effective_program = extract_effective_program(individual->program);
    calculate_fitness(individual, data_set);

    push_individual(deme, individual);
  }
}

void print_deme(struct DEME *deme){

  int current;

  for(current = 0; current < deme->population_length; current++){

   printf("Individual %d\n", current);
   print_individual(deme->individuals[current]);
  }
}

int compare_fitness(const void *abstract1, const void *abstract2){

  int result;
  float float_result;
  struct INDIVIDUAL *individual1;
  struct INDIVIDUAL *individual2;

  individual1 = (struct INDIVIDUAL *) *((const void **) abstract1);
  individual2 = (struct INDIVIDUAL *) *((const void **) abstract2);
 
  //printf("%f > %f ", individual1->fitness, individual2->fitness );

  float_result = (individual2->fitness - individual1->fitness);
  //printf(" (%f) ", float_result);
  result = (float_result < 0) ? -1 : (float_result > 0) ? 1 : 0;

  if (result == 0){
    result = individual1->effective_program->length - individual2->effective_program->length;
  }

  //printf("  ==== > %d \n", result);
  //getchar();
  return result;
}

void sort_deme(struct DEME *deme){

  qsort(deme->individuals, deme->population_length, sizeof(struct INDIVIDUAL*), compare_fitness);
}

struct SELECTION_DATA *tournament_selection(struct DEME *deme, int tournament_length, int elite_length){

  int current_contender;
  int currently_selected;
  int selection_limit;

  int contender_index;
  struct INDIVIDUAL *contender;
  struct DEME *contenders;
  struct DATA_PAIR *contenders_indexes;
  struct SELECTION_DATA *data;
  
  struct RANDOM_POOL *pool;

  selection_limit = (deme->population_length - elite_length) / tournament_length;
  data = new_selection_data(selection_limit, deme->population_length);
  contenders = new_deme(tournament_length);
  contenders_indexes = malloc(sizeof(struct DATA_PAIR)*tournament_length);
  pool = new_pool(deme->population_length, TRUE);

  for (currently_selected = 0; currently_selected < selection_limit; currently_selected++){

    //printf("Tournamet #%d :\n", currently_selected);
    contenders->population_length = 0;
    for (current_contender = 0; current_contender < tournament_length; current_contender++){

       //print_pool(pool);
       contender_index = random_element(pool);
       contender = deme->individuals[contender_index];

       contenders_indexes[current_contender].contender = contender;
       contenders_indexes[current_contender].index = contender_index;

       //printf("Ci = %d, Mi = %d, Fi = %f\n", contender_index, (int)contender, contender->fitness );

       push_individual(contenders, contender);
       
    }

    sort_deme(contenders);
    //print_deme(contenders);

    contender = contenders->individuals[0];
    push_individual(data->selected, contender);

    for (current_contender = 0; current_contender < tournament_length; current_contender++){

      if (contenders_indexes[current_contender].contender == contender){

        contender_index = contenders_indexes[current_contender].index;
        break;
      }
    }
 
    //printf("Winner, Ci = %d, Mi = %d, Fi = %f \n\n", contender_index, (int)contender, contender->fitness);

    data->selected_indexes[contender_index] = TRUE;
    //getchar();

  }

  free(contenders_indexes);
  delete_deme(contenders, FALSE);
  delete_pool(pool);
  return data;
}

/* The deme individuals must be already sorted in DESC order */
struct DEME *elite_selection(struct DEME *deme, int elite_length){

  int current;
  struct DEME *elite;

  elite = new_deme(elite_length);
  elite->population_length = elite_length;

  for (current=0; current < elite_length; current++){

    elite->individuals[current] = clone_individual(deme->individuals[current]);
  }

  return elite;
}

struct INDIVIDUAL *apply_crossover(struct PROGRAM *program1, struct PROGRAM *program2, struct DATA_SET *training_data_set){

  struct INDIVIDUAL *individual1;
  struct INDIVIDUAL *individual2;
  struct DEME *deme;
  struct INDIVIDUAL *result;
  struct CROSSOVER_STUMPS stumps;

  //printf("Entered...\n");

  stumps = onepoint_crossover(program1, program2);
  //printf("crossed...\n");

  individual1 = new_individual_from(stumps.stump1, training_data_set);
  individual2 = new_individual_from(stumps.stump2, training_data_set);
  
  deme = new_deme(2);
  push_individual(deme, individual1);
  push_individual(deme, individual2);
  sort_deme(deme);
  //printf("Sorted...\n");

  result = deme->individuals[0];
  delete_individual(deme->individuals[1]);
  delete_deme(deme, FALSE);
  return result;
}

// Check for repeated individuals!
void check_deme(struct DEME *deme){

  int i,j;
  
  for (i = 0; i < deme->population_length; i++){

    for (j=0; j< deme->population_length; j++){

      if (i != j && deme->individuals[i] == deme->individuals[j]){

          //printf("ERROR: i = %d, j = %d, Mem = %d\n", i, j, (int)(deme->individuals[i]));
          exit(-1);
      } 
    }    
  }

}

void evolve_one_step_deme(struct DEME *deme, struct DATA_SET *training_data_set, struct DATA_SET *validation_data_set){

  int current;
  int ind_mem_size;
  struct SELECTION_DATA *data;
  struct DEME *next_deme;
  struct DEME *elite;

  //print_deme(deme);
  //getchar();
  //printf("Got in...\n");

  next_deme = new_deme(deme->population_length);

  sort_deme(deme);
  //printf("Sorted...\n");

  elite = elite_selection(deme, ELITE_LENGTH);
  //check_deme(elite);
  //printf("Elite selected...\n");

  data = tournament_selection(deme, TOURNAMENT_LENGTH, ELITE_LENGTH);
  //check_deme(data->selected);
  //printf("Tournamente selected...\n");

  ind_mem_size = sizeof(struct INDIVIDUAL*);
  memcpy(next_deme->individuals, elite->individuals, ind_mem_size*ELITE_LENGTH);
  memcpy(next_deme->individuals+ELITE_LENGTH, data->selected->individuals, ind_mem_size*data->selection_length);
  //printf("Copied...\n");

  next_deme->population_length = ELITE_LENGTH+data->selection_length;
  for (current = next_deme->population_length; current < next_deme->population_limit; current++){

    next_deme->individuals[current] = apply_crossover(next_deme->individuals[rand()%next_deme->population_length]->program, next_deme->individuals[rand()%next_deme->population_length]->program, training_data_set);
    //print_individual(next_deme->individuals[current]);
  }
  next_deme->population_length = next_deme->population_limit;
  //printf("Crossed...\n");
 
  for (current = ELITE_LENGTH; current < ELITE_LENGTH+data->selection_length; current++){

    apply_genetic_operand(next_deme->individuals[current], training_data_set);
    //printf("C = %d\n", current);
    //getchar();
  } 
  //printf("Mutated...\n");

  //print_deme(next_deme);
  
  for (current = ELITE_LENGTH; current < deme->population_length; current++){

    if (!data->selected_indexes[current]){
   
      //printf("Going to delete c = %d, Mi = %d\n", current, deme->individuals[current]);
      delete_individual(deme->individuals[current]);
    }
  }  
  delete_selection_data(data);
  //printf("Deleted...\n");

  memcpy(deme->individuals, next_deme->individuals, ind_mem_size*deme->population_length);
  delete_deme(next_deme, FALSE);
  delete_deme(elite, FALSE);
}

/* WANING destination_deme must be sorted! */
void migrate_deme(struct DEME *origin_deme, struct DEME *destination_deme){

  int individual_index;
  int counter;
  int replaced_index;
  struct INDIVIDUAL *replaced_individual;

  sort_deme(destination_deme);
  for (counter = 0; counter < MIGRATION_LENGTH; counter++){

    individual_index = rand()%origin_deme->population_length;
    replaced_index = (destination_deme->population_length - 1) - counter;
    replaced_individual = destination_deme->individuals[replaced_index];
    destination_deme->individuals[replaced_index] = clone_individual(origin_deme->individuals[individual_index]);
    delete_individual(replaced_individual);
  }
}

