/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

void translate_instruction(FILE *output_file, struct INSTRUCTION *instruction){

  int _operator;
  int _operand1;
  int _operand2;
  int _result;

  _operator = instruction->_operator;
  _operand1 = instruction->_operand1;
  _operand2 = instruction->_operand2;
  _result = instruction->_result;


  fprintf(output_file, "\tprint_state();\n");
  if (_operator == DIV){

    fprintf(output_file, "\t%s = safe_div(%s, %s);\n", REGISTERS_LABELS[_result], REGISTERS_LABELS[_operand1], REGISTERS_LABELS[_operand2]);
    fprintf(output_file, "\tprintf(\"%s = safe_div(%s, %s)\\n\");\n\n", REGISTERS_LABELS[_result], REGISTERS_LABELS[_operand1], REGISTERS_LABELS[_operand2]);
  }else{

    if (_operator == L_NOT){

      fprintf(output_file, "\t%s = %s%s;\n", REGISTERS_LABELS[_result], OPERATORS_STR[_operator], REGISTERS_LABELS[_operand1]);
      fprintf(output_file, "\tprintf(\"%s = %s%s\\n\");\n\n", REGISTERS_LABELS[_result], OPERATORS_STR[_operator], REGISTERS_LABELS[_operand1]);
      }else{

        fprintf(output_file, "\t%s = %s %s %s;\n", REGISTERS_LABELS[_result], REGISTERS_LABELS[_operand1], OPERATORS_STR[_operator], REGISTERS_LABELS[_operand2]);
        fprintf(output_file, "\tprintf(\"%s = %s %s %s\\n\");\n\n", REGISTERS_LABELS[_result], REGISTERS_LABELS[_operand1], OPERATORS_STR[_operator], REGISTERS_LABELS[_operand2]);
    }
  }
}

int translate_program(struct PROGRAM *program){

  int counter;
  FILE *output_file;
  char de[] = {'%', 'd', '\0'};
  char str[] = {'%', 's', '\0'};

  output_file = fopen(OUTPUT, "w");
  if (output_file == NULL){
  
    printf("Output file could not be saved\n");
    return -1;
  }

  fprintf(output_file, "#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\n\n");

  for (counter = 0; counter < TOTAL_INPUTS; counter++){
    fprintf(output_file, "int %s = 0;\n", REGISTERS_LABELS[counter]);
  }
  for (counter = 0; counter < TOTAL_CONSTANTS; counter++){
    fprintf(output_file, "int %s = %d;\n", REGISTERS_LABELS[TOTAL_INPUTS+counter], program->constants[counter]);
  }
  for (counter = TOTAL_INPUTS+TOTAL_CONSTANTS; counter < MEMORY_SPACE; counter++){
    fprintf(output_file, "int %s = 0;\n", REGISTERS_LABELS[counter]);
  }

  fprintf(output_file, "\nint safe_div(int a, int b){\n\n\treturn (b > 0 ? a/b : 0);\n}\n");

  fprintf(output_file, "\nvoid print_state(){\n\n");
  for (counter = 0; counter < MEMORY_SPACE; counter++){
    fprintf(output_file, "\tprintf(\"%s = %s\\n\", %s);\n", REGISTERS_LABELS[counter], de, REGISTERS_LABELS[counter]);
  }
  fprintf(output_file,"\tprintf(\"\\n\");\n");
  fprintf(output_file, "}\n\n");

  fprintf(output_file, "int main(int argc, char *argv[]){\n\n");

  fprintf(output_file, "\n\tif (argc != %d){\n\n\t\tprintf(\"Wrong arguments number\\n.\");\n\t\texit(-1);\n\t}\n\n", (TOTAL_INPUTS+1));
  
  for (counter = 0; counter < TOTAL_INPUTS; counter++){
    fprintf(output_file, "\t%s = atoi(argv[%d]);\n", REGISTERS_LABELS[counter], counter+1);    
  }
  
  fprintf(output_file, "\n");

  for (counter = 0; counter < program->length; counter++){
    translate_instruction(output_file, program->instructions[counter]);
  }

  fprintf(output_file, "\n\tprintf(\"Result: %s\\n\", %s);\n", de, REGISTERS_LABELS[RESULT_REGISTER_INDEX]);
  fprintf(output_file, "\treturn %s;\n}\n\n", REGISTERS_LABELS[RESULT_REGISTER_INDEX]);

  fclose(output_file);
  printf("\nTranslation saved at %s\n", OUTPUT);
  return 0;
}
