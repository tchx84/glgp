/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

int is_register(int register_index){

  if (register_index >= RESULT_REGISTER_INDEX){
    return TRUE;
  }

  return FALSE;
}

int is_constant(int register_index){

  if (register_index >= TOTAL_INPUTS && register_index < RESULT_REGISTER_INDEX){
     return TRUE;
  }
 
  return FALSE;
}

int is_operand(int register_index){

  if (register_index >= 0 && register_index < TOTAL_INPUTS ){
    return TRUE;
  }
 
  return FALSE;
}

int is_boolean(int register_index){

  if (REGISTERS_MAPPING[register_index] == BOOLEAN){
    return TRUE;
  }

  return FALSE;
}

int is_numeric(int register_index){

  if (REGISTERS_MAPPING[register_index] == NUMERIC){
    return TRUE;
  }

  return FALSE;
}

