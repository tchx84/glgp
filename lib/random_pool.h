/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

struct RANDOM_POOL{

 int *elements;
 int length;

};

struct RANDOM_POOL *new_pool(int length, int fill_with_values){

  int i;
  struct RANDOM_POOL *pool;

  pool = malloc(sizeof(struct RANDOM_POOL));
  pool->elements = malloc(sizeof(int)*length);
  pool->length = length;

  if (fill_with_values){

    for(i=0; i<length; i++){

      pool->elements[i] = i;
    }
  }
  
  return pool;
}

void delete_pool(struct RANDOM_POOL *pool){

  free(pool->elements);
  free(pool);
}

void print_pool(struct RANDOM_POOL *pool){

  int i;

  for (i=0; i<pool->length; i++){

    printf("%d, ", pool->elements[i]);
  }
  printf("\n");
}

int random_element(struct RANDOM_POOL *pool){

  int index;
  int element;
  int copied_length;
  int *next_elements;

  index = rand()%pool->length;
  element = pool->elements[index];

  //printf("i = %d, e = %d\n", index, element);

  next_elements = malloc(sizeof(int)*(pool->length-1));
  if (next_elements == NULL){

    printf("No memory available.\n");
    exit(-1);
  }

  copied_length = index;
  if (copied_length > 0){

    memcpy(next_elements, pool->elements, sizeof(int)*copied_length);
  }

  copied_length = (pool->length - index)-1;
  if (copied_length > 0){
  
    memcpy(next_elements+index, pool->elements+(index+1), sizeof(int)*copied_length);
  }
  //printf("done.\n");

  free(pool->elements);
  pool->elements = next_elements;
  pool->length--;

  //printf("out!\n");
  return element;
}

