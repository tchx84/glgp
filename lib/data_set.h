/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

struct DATA_SET{

  int **data;
  int columns;
  int rows;
};

struct DATA_SET *new_data_set(int rows, int columns){

  int i;
  struct DATA_SET *data_set;

  data_set = malloc(sizeof(struct DATA_SET));
  data_set->rows = rows;
  data_set->columns = columns;

  data_set->data = malloc(sizeof(int*)*rows);
  for(i = 0; i<rows; i++){

    data_set->data[i] = malloc(sizeof(int)*columns);
  }

  return data_set;
};

void delete_data_set(struct DATA_SET *data_set){

  int i;
 
  for(i=0; i<data_set->rows; i++){

    free(data_set->data[i]);
  }
  free(data_set->data);
  free(data_set);
}

struct DATA_SET *load_data_set(char *filepath){

  int rows;
  int columns;
  char c;
  int chars;
  char buffer[BUFFER_SIZE];
  char format[BUFFER_SIZE];
  FILE *file;
  struct DATA_SET *data_set;

  file = fopen(filepath, "r");
  if (!file){

    printf("File %s was not found.\n", filepath);
    exit(-1);
  }

  fgets(buffer,BUFFER_SIZE, file);
  sscanf(buffer,"%d", &rows);
  //printf("%s\n", buffer);

  fgets(buffer,BUFFER_SIZE, file);
  sscanf(buffer,"%d", &columns);
  //printf("%s\n", buffer);

  data_set = new_data_set(rows, columns);
  //printf("created!\n");

  rows = columns = chars = 0;
  strcpy(buffer,"");
  while(rows < data_set->rows){

    while (TRUE){

      c = fgetc(file);
      //printf("%c", c);
      if (c == ' ' || c == '\n'){

        //printf("entro!\n");
        break;
      }

      buffer[chars] = c;
      chars++;
    }
    buffer[chars] = '\0';
    //printf("%s\n", buffer);

    data_set->data[rows][columns] = atoi(buffer);

    if (c == '\n'){

      columns = 0;
      rows++;
    } else {

      columns++;
    }
    
    chars = 0;
    strcpy(buffer,"");
  }

  fclose(file);
  return data_set;
}

void print_data_set(struct DATA_SET *data_set){

  int row;
  int column;

  for(row = 0; row < data_set->rows; row++){

    for(column = 0; column < data_set->columns; column++){

      printf("%d ", data_set->data[row][column]);
    }
    printf("\n");
  }
}

