#include <math.h>       /*pow*/
#include <stdlib.h>     /*free*/
#include <string.h>     /*strlen*/
#include <assert.h>     /*assert*/

#include "calc.h"
#include "stack.h"

#define NUM_OF_STATES (2)
#define NUM_OF_ASCII_VALUES (128)
#define NUM_OF_ERRORS (5)

#define VALID (0)
#define MALLOC_ERR (-1)
#define INPUT_ERR (-2)
#define BRAC_ERR (-3)
#define ARITHMETIC_ERR (-4)

typedef enum {GET_NUMBER_STATE = 0, GET_OPERATOR_STATE} calc_state_t;

typedef double (*action_func_t)(double num1, double num2);
typedef calc_state_t (*calc_func_t)(char **value, stack_t *number_stack, stack_t
                            *operator_stack, calc_state_t curr_state, int *err);

/****************************************************************************
                            LUT DECLARATION
****************************************************************************/
static calc_func_t calc_func[NUM_OF_STATES][NUM_OF_ASCII_VALUES];
static action_func_t action_func[NUM_OF_ASCII_VALUES];
static int presedence_lut[NUM_OF_ASCII_VALUES] = {0};
static char opposite_parenthesis_lut[NUM_OF_ASCII_VALUES] = { 0 };

/****************************************************************************
                        STATIC FUNCTION DECLARATION
****************************************************************************/
static double Add(double num1,  double num2);

static double Substract(double num1, double num2);

static double Multiply(double num1, double num2);

static double Divide(double divider, double dividee);

static double Power(double num, double power);


static calc_state_t TransformToNumber(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err);

static calc_state_t ReturnResult(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err);

static calc_state_t ErrorFuncUndefinedChar(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err);

static calc_state_t AdvanceAtGetNumState(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err);

static calc_state_t AdvanceAtGetOperatorState(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err);

static calc_state_t AcceptOpeningParenthesis(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err);

static void InitAllLuts();

static void InitPresedenceLut(int *presedence_lut);

static void InitActionFunc(action_func_t *action_func);

static void InitOppositeParenthesisLut(char *parenthesis_lut);

static void InitCalcFunc(
                    calc_func_t calc_func[NUM_OF_STATES][NUM_OF_ASCII_VALUES]);


static int IsOppositeParenthesis(char operator, char last_operator);

static calc_state_t IsOpenParenthesis(char operator);

static int IsClosingParenthesis(char bracket);

static int IsParenthesis(char bracket);

/****************************************************************************
                        FUNCTION IMPLEMENTATION
****************************************************************************/
double Calculate(char *exp, int *err)
{
    calc_state_t curr_state = GET_NUMBER_STATE;

    char *runner = exp;

    char *end = NULL;

    stack_t *numbers = StackCreate(strlen(exp), sizeof(double));
    stack_t *operators = StackCreate(strlen(exp), sizeof(char));

    char dummy_op ='#';

    double return_value = 0;

    assert(NULL != exp);
    assert(NULL != err);

    end = exp + strlen(exp);

    *err = ((NULL == numbers) || (NULL == operators)) * (MALLOC_ERR);

    StackPush(operators, &dummy_op);

    *err = 0;

    curr_state = IsOpenParenthesis(*runner);

    InitAllLuts();

    while (((end > runner) || (StackSize(operators) > 1)) && (*err == VALID))
    {
        curr_state = calc_func[curr_state][(int)*runner](&runner, numbers, operators, curr_state, err);
    }

    if (NULL != StackPeek(numbers))
    {
        return_value = (*(double *)StackPeek(numbers));
    }

    StackDestroy(numbers);
    StackDestroy(operators);

    return(((*err) == 0) * return_value);

}


/****************************************************************************
                        STATIC FUNCTION DEFINITION
****************************************************************************/
static double Add(double num1, double num2)
{
    return (num1 + num2);
}

static double Substract(double num1, double num2)
{
    return (num1 - num2);
}

static double Multiply(double num1, double num2)
{
    return (num1 * num2);
}

static double Divide(double divider, double dividee)
{
    return (divider / dividee);
}

static double Power(double num, double power)
{
    return (pow(num, power));
}

static void InitAllLuts()
{
    InitCalcFunc(calc_func);
    InitActionFunc(action_func);
    InitPresedenceLut(presedence_lut);
		InitOppositeParenthesisLut(opposite_parenthesis_lut);
}

static void InitPresedenceLut(int *presedence_lut)
{
    presedence_lut[0] = 0;
    presedence_lut['+'] = 5;
    presedence_lut['-'] = 5;
    presedence_lut['/'] = 6;
    presedence_lut['*'] = 6;
    presedence_lut['^'] = 7;
    presedence_lut['#'] = -1;

    presedence_lut[' '] = 10;
}

static void InitActionFunc(action_func_t *action_func)
{
    action_func['+'] = Add;
    action_func['-'] = Substract;
    action_func['*'] = Multiply;
    action_func['/'] = Divide;
    action_func['^'] = Power;
}

static void InitCalcFunc(calc_func_t calc_func[NUM_OF_STATES][NUM_OF_ASCII_VALUES])
{
    size_t i = 0;

    for (i = 0; i < NUM_OF_ASCII_VALUES; ++i)
    {
        calc_func[GET_NUMBER_STATE][i] = ErrorFuncUndefinedChar;
        calc_func[GET_OPERATOR_STATE][i] = ErrorFuncUndefinedChar;
    }

    for (i = 0; i < 10; ++i)
    {
        calc_func[GET_NUMBER_STATE]['0' + i] = TransformToNumber;
    }

    calc_func[GET_NUMBER_STATE]['-'] = TransformToNumber;
    calc_func[GET_NUMBER_STATE]['+'] = TransformToNumber;
    calc_func[GET_NUMBER_STATE][' '] = AdvanceAtGetNumState;

    calc_func[GET_OPERATOR_STATE]['+'] = ReturnResult;
    calc_func[GET_OPERATOR_STATE]['-'] = ReturnResult;
    calc_func[GET_OPERATOR_STATE]['*'] = ReturnResult;
    calc_func[GET_OPERATOR_STATE]['/'] = ReturnResult;
    calc_func[GET_OPERATOR_STATE]['^'] = ReturnResult;
    calc_func[GET_OPERATOR_STATE][' '] = AdvanceAtGetOperatorState;
    calc_func[GET_OPERATOR_STATE][0] = ReturnResult;

    calc_func[GET_OPERATOR_STATE]['('] = AcceptOpeningParenthesis;
    calc_func[GET_OPERATOR_STATE][')'] = ReturnResult;
    calc_func[GET_OPERATOR_STATE]['['] =  AcceptOpeningParenthesis;
    calc_func[GET_OPERATOR_STATE][']'] = ReturnResult;
    calc_func[GET_OPERATOR_STATE]['{'] =  AcceptOpeningParenthesis;
    calc_func[GET_OPERATOR_STATE]['}'] = ReturnResult;

}

static void InitOppositeParenthesisLut(char *parenthesis_lut)
{
	parenthesis_lut['('] = ')';
	parenthesis_lut[')'] = '(';

	parenthesis_lut['['] = ']';
	parenthesis_lut[']'] = '[';

	parenthesis_lut['{'] = '}';
	parenthesis_lut['}'] = '{';
}



static calc_state_t IsOpenParenthesis(char operator)
{
    return ((operator == '(') || (operator == '[') || (operator == '{'));
}

static int IsClosingParenthesis(char bracket)
{
    return ((bracket == ')') || (bracket == ']') || (bracket == '}'));
}

static calc_state_t TransformToNumber(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err)
{
    double result = strtod(*value, value);

		(void)operator_stack;
		(void)err;

    StackPush(number_stack, &result);
    curr_state = GET_OPERATOR_STATE;

    return (curr_state);
}

static calc_state_t ReturnResult(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err)
{
    double data1 = 0;
    double data2 = 0;
    double result = 0;

    char operator = **value;
    char last_operator = *(char *)StackPeek(operator_stack);

    while ((presedence_lut[(int)operator] <= presedence_lut[(int)last_operator])
    && (operator != '^') && (!IsParenthesis(last_operator)) && (*err == VALID))
    {

        data1 = *(double *)StackPeek(number_stack);
        StackPop(number_stack);

        data2 = *(double *)StackPeek(number_stack);
        StackPop(number_stack);

        StackPop(operator_stack);

        *err = ((last_operator == '/') && (data1 == 0)) * (ARITHMETIC_ERR);
        result = action_func[(int)last_operator](data2, data1);
        StackPush(number_stack, &result);

        last_operator = *(char *)StackPeek(operator_stack);
    }

    if (*err != ARITHMETIC_ERR)
    {
        *err = ((IsOpenParenthesis(last_operator)) && ('\0' == operator)) * (BRAC_ERR);
    }

    if (('\0' != **value) && (!IsOppositeParenthesis(operator, last_operator)))
    {
        StackPush(operator_stack, &operator);
				curr_state = IsOpenParenthesis(*(*value + 1));
    }
		else if (IsOppositeParenthesis(operator, last_operator))
		{
			StackPop(operator_stack);
			curr_state = GET_OPERATOR_STATE;
		}

		++(*value);

    return (curr_state);
}

static calc_state_t ErrorFuncUndefinedChar(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err)
{
    (void)value;
    (void)number_stack;
    (void)operator_stack;

    *err = INPUT_ERR;
    return (curr_state);
}

static calc_state_t AdvanceAtGetNumState(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err)
{
    ++(*value);

	(void)number_stack;
	(void)operator_stack;
	(void)err;
    (void)curr_state;

    return ((IsOpenParenthesis(**value)));
}

static calc_state_t AdvanceAtGetOperatorState(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err)
{
    ++(*value);

	(void)number_stack;
	(void)operator_stack;
	(void)err;
    (void)curr_state;

    return (GET_OPERATOR_STATE);
}

static calc_state_t AcceptOpeningParenthesis(char **value, stack_t *number_stack,
                    stack_t *operator_stack, calc_state_t curr_state, int *err)
{
    char bracket = **value;

	(void)number_stack;
	(void)err;

    StackPush(operator_stack, &bracket);
    curr_state = GET_NUMBER_STATE;
	++(*value);

	curr_state = IsOpenParenthesis(**value);

    return(curr_state);
}

int IsOppositeParenthesis(char operator, char last_operator)
{
	return (opposite_parenthesis_lut[(int)operator] == last_operator);
}

int IsParenthesis(char bracket)
{
    return (IsOpenParenthesis(bracket) || IsClosingParenthesis(bracket));
}
