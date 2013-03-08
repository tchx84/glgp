/*
* Grammar-based Linear Genetic Programming
*
* (C) 2009, 2010 Martin Abente Lahaye <martin.abente.lahaye@gmail.com>
*
* This code is released under the GPL version 2.
*/

/* Numeric Operators */
#define PLS 0
#define SUB 1
#define MUL 2
#define DIV 3

/* Logic Operators */
#define L_AND 4
#define L_OR 5
#define L_NOT 6
#define L_EQL 7
#define L_N_EQL 8

/* Comparison Operators */
#define C_GTR 9
#define C_GTR_OR_EQL 10
#define C_LSS 11
#define C_LSS_OR_EQL 12
#define C_EQL 13
#define C_N_EQL 14

/* Boolean */
#define TRUE 1
#define FALSE 0

/* Special words */
#define SIZE_NO_SPECIFIED 0

/* etc */
#define BUFFER_SIZE 128

/***************************
  PROGRAM DEFINITIONS
***************************/

#define NONE -1

int INIT_PROGRAM_LENGTH = 10;
int MAX_PROGRAM_LENGTH = 100;
int MIN_PROGRAM_LENGTH = 10;

/*************************
  OPERATORS DEFINITIONS
*************************/

#define ARITHMETIC 0
#define LOGIC 1
#define COMPARISON 2

#define SET 0
#define CARDINALITY 1
#define DISTRIBUTION 2

int OPERATORS_MAPPING[] = {
                                     ARITHMETIC, ARITHMETIC, ARITHMETIC, ARITHMETIC,
                                     LOGIC, LOGIC, LOGIC, LOGIC, LOGIC,
                                     COMPARISON, COMPARISON, COMPARISON, COMPARISON, COMPARISON, COMPARISON
                                   };

int OPERATORS_DEFINITION[3][3][6] = {
                                           { {PLS, SUB, MUL, DIV}, {4}, {26} },
                                           { {L_AND, L_OR, L_NOT, L_EQL, L_N_EQL}, {5}, {59} },
                                           { {C_GTR, C_GTR_OR_EQL, C_LSS, C_LSS_OR_EQL, C_EQL, C_N_EQL}, {6}, {99} }
                                        };

#define ARITHMETIC_DISTRIBUTION (OPERATORS_DEFINITION[ARITHMETIC][DISTRIBUTION][0])
#define LOGIC_DISTRIBUTION (OPERATORS_DEFINITION[LOGIC][DISTRIBUTION][0])
#define COMPARISON_DISTRIBUTION (OPERATORS_DEFINITION[COMPARISON][DISTRIBUTION][0])

#define TOTAL_OPERATORS (OPERATORS_DEFINITION[ARITHMETIC][CARDINALITY][0]+OPERATORS_DEFINITION[LOGIC][CARDINALITY][0]+OPERATORS_DEFINITION[COMPARISON][CARDINALITY][0])

unsigned char OPERATORS_STR[][5] = {"+", "-", "*", "/", "&&","||","!","==","!=",">",">=","<","<=","==","!="};

int ARITHMERIC_AVAILABLE = TRUE;

/*************************
  REGISTERS DEFINITION
*************************/

#define MAX_REGISTERS 5

#define NUMERIC 0
#define BOOLEAN 1

#define INPUT 0
#define CONSTANT 1
#define REGISTER 2

#define INDEXES 0

int REGISTERS_MAPPING[MAX_REGISTERS*6];
char REGISTERS_LABELS[30][128];

int REGISTERS_DEFINITION[2][3][3][MAX_REGISTERS] = {
                                           {
                                             { { 0 }, { 0 }, { 0 } },
                                             { { 0 }, { 0 }, { 0 } },
                                             { { 0 }, { 0 }, { 0 } }
                                           },
                                           {
                                             { { 0 }, { 0 }, { 0 } },
                                             { { 0 }, { 0 }, { 0 } },
                                             { { 0 }, { 0 }, { 0 } }
                                           }
                                         };

#define TOTAL_INPUTS (REGISTERS_DEFINITION[NUMERIC][INPUT][CARDINALITY][0]+REGISTERS_DEFINITION[BOOLEAN][INPUT][CARDINALITY][0])
#define TOTAL_CONSTANTS (REGISTERS_DEFINITION[NUMERIC][CONSTANT][CARDINALITY][0]+REGISTERS_DEFINITION[BOOLEAN][CONSTANT][CARDINALITY][0])
#define TOTAL_REGISTERS (REGISTERS_DEFINITION[NUMERIC][REGISTER][CARDINALITY][0]+REGISTERS_DEFINITION[BOOLEAN][REGISTER][CARDINALITY][0])

#define RESULT_REGISTER_INDEX (TOTAL_INPUTS+TOTAL_CONSTANTS)
#define MEMORY_SPACE (TOTAL_INPUTS+TOTAL_CONSTANTS+TOTAL_REGISTERS)

#define all_booleans REGISTERS_DEFINITION[BOOLEAN][REGISTER][INDEXES]
#define all_booleans_length (*REGISTERS_DEFINITION[BOOLEAN][REGISTER][CARDINALITY])

#define all_numerics REGISTERS_DEFINITION[NUMERIC][REGISTER][INDEXES]
#define all_numerics_length (*REGISTERS_DEFINITION[NUMERIC][REGISTER][CARDINALITY])

#define STANDARD_DEVIATION 5

int INITIAL_CONSTANTS[MAX_REGISTERS];

/******************
  MANDATORY RULES
*******************/

/* 
  1. There has to be at least 1 register type per each operator type available
  2. There has to be at least 1 differente type of registry per each type (available)
  3. The deme population limit mod tournament size = 0
*/


/******************************
  GENETIC OPERATORS RELATED DEFINITIONS
******************************/

/* MUTATION RELATION */
int EFFECTIVE_MUTATION = TRUE;

int REGISTER_DISTRIBUTION = 33;
int OPERATOR_DISTRIBUTION = 66;
int CONSTANTS_DISTRIBUTION = 99;

int DESTINATION_DISTRIBUTION = 49;
int OPERAND_DISTRIBUTION = 99;

int MICRO_MUTATION_DISTRIBUTION = 49;
int MACRO_MUTATION_DISTRIBUTION = 99;

int REMOVE_MUTATION_DISTRIBUTION = 24;
int ADD_MUTATION_DISTRIBUTION = 99;

/**********************************
  CROSSOVER RELATED 
***********************************/

int MAX_CROSSOVER_DISTANCE = 100;

/**********************************
  LGP RELATED DEFINITIONS
**********************************/

int DEMES_LENGTH = 10;
int POPULATION_LIMIT = 5000;
int DATA_SETS_LENGTH = 2;
int MIGRATION_LENGTH = 250;

/*************************************
  HYBRID TOURNAMENT SELECTION DEFINITIONS
**************************************/

int ELITE_LENGTH = 10;
int TOURNAMENT_LENGTH = 2;

/*****************************************
  FITNESS CALCULATION
******************************************/

#define ACCURACY 0
#define FMEASURE 1

int fitness_method = FMEASURE;

char FITNESS_METHOD_STR[][20] = {"Accuracy", "F-measure"};

/*******************************************
  DATA SETS STUFF
*******************************************/

#define TRAINING_SET 0
#define VALIDATION_SET 1
#define OTHER_SET 2


/*******************************************
  TRANSLATION RELATED STUFF
*******************************************/
char OUTPUT[BUFFER_SIZE];
