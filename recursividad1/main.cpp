/*
OPERATOR - OPERAND
NOT - OPERAND


*/
#include <iostream>
#include <iostream>
#include <stdio.h>
#include <stdint.h>

#define eof 127


using namespace std;

enum _cycle
{
    CYCLE_NONE,//close
    CYCLE_NOT,//CYCLE_1OP
    CYCLE_2OP,
};
enum
{
    STATE_1OP_NOT_NONE,
    STATE_1OP_NOT_OPERATOR,
    STATE_1OP_NOT_OPERAND,
};
enum
{
    STATE_2OP_NONE,
    STATE_2OP_LVAL,
    STATE_2OP_OPERATOR,
    STATE_2OP_RVAL,
};
enum
{
    RCSV_TYPE_NONE,
    RCSV_TYPE_1OP_NOT,
    RCSV_TYPE_PARENTHESIS,
};

uint8_t  logical(uint8_t lval, uint8_t op, uint8_t rval)
{
    uint8_t r=0;

    if (op == '&')
        r = lval && rval;
    else if (op == '|')
        r = lval || rval;

    return r;
}

//char exp[20]= {'n','n','n','n','n','n','n','n',1, eof};
//char exp[20]= {'n',1, eof};
//char exp[20]= {'n','n','n','n','n', 0, eof};
//char exp[20]= {'n','(','n', 1, ')', eof};
//char exp[20]= {'n','(','(', 'n', 1,')',')', eof};
//char exp[20]= {'n','(','(', 'n','n',1,')',')', eof};
//char exp[20]= {'(','n','(','(','(', 'n','(', 'n',0,')',')',')',')',')', eof};
//char exp[20]= {'(','n',1,')', eof};
//char exp[20]= {'(','n','(', 0,')',')', eof};
//char exp[20]= {'(','n','n','(','n',0,')',')', eof};
//char exp[20]= {'(','n','n','(','n',0,')',')', '&', 1, eof};
//char exp[20]= {'(','n','n','(','n',0,')',')', '&', 'n', 0, eof};
//char exp[20]= {1,'&',0, eof};
//char exp[20]= {1,'&',0, '|', 1, '&', 0, eof};
//char exp[20]= {'n',0,'|','n',1, eof};
//char exp[20]= {1,'&','n',0, eof};
//char exp[20]= {'n',0,'&','(', 1 ,')', eof};
char exp[20]= {'n','(', 0,')','&','(', 'n', '(',0, ')', '&', 1,')', eof};

uint8_t fr(const char *p, uint8_t *counter_el, uint8_t rcsv_type)
{
    char el;
    uint8_t cycle = CYCLE_NONE;
    uint8_t rcsv = 0;
    uint8_t counter_el_rcsv=0;

    uint8_t state_1OP = 0;
    uint8_t state_2OP = 0;
    uint8_t lval, _lval;
    uint8_t rval;
    uint8_t op;

    while (*p != eof)
    {
        el = *p;

        if (el == 'n')
        {
            if (cycle == CYCLE_NONE)
            {
                cycle = CYCLE_NOT;

                if (state_1OP == STATE_1OP_NOT_NONE)
                    state_1OP = STATE_1OP_NOT_OPERATOR;
            }
            else if (cycle == CYCLE_NOT)
            {
                rcsv = 1;
                lval = fr(p, &counter_el_rcsv, RCSV_TYPE_1OP_NOT);

                if (state_1OP == STATE_1OP_NOT_OPERATOR)
                    state_1OP = STATE_1OP_NOT_OPERAND;
            }
            else if (cycle == CYCLE_2OP)
            {
                rcsv = 1;

                if (state_2OP == STATE_2OP_OPERATOR)
                {
                    rval = fr(p, &counter_el_rcsv, RCSV_TYPE_1OP_NOT);
                    state_2OP = STATE_2OP_RVAL;
                }
            }
        }
        else if (el == '(')
        {
            rcsv = 1;

            p++;
            (*counter_el)++;

            _lval = fr(p, &counter_el_rcsv, RCSV_TYPE_PARENTHESIS);//regresa despues de haber )

            if (cycle == CYCLE_NONE)
            {
                lval = _lval;//simplemente un seguidor del ultimo lval
            }
            else if (cycle == CYCLE_NOT)
            {
                lval = _lval;

                if (state_1OP == STATE_1OP_NOT_OPERATOR)
                    state_1OP = STATE_1OP_NOT_OPERAND;
            }
            else if (cycle == CYCLE_2OP)
            {
                if (state_2OP == STATE_2OP_OPERATOR)
                {
                    rval = _lval;
                    state_2OP = STATE_2OP_RVAL;
                }
            }
        }
        else if (el == ')')
        {
            (*counter_el)++;   //por su naturaleza, siempre regresa de una llamada recursiva
            break;          //originada por '(', osea, este *counter_el es el &counter_el_rcsv
            //pasado por la llamada anterior.
        }
        else if ((el=='&') || (el=='|'))
        {
            if (cycle == CYCLE_2OP)
            {
                if (state_2OP == STATE_2OP_LVAL)
                {
                    op = el;
                    state_2OP = STATE_2OP_OPERATOR;
                }
            }
            else if (cycle == CYCLE_NONE)
            {
                op = el;    //x defecto sera tomado como una operacion de 2 op
                //por ejmp, puede provenir de una expr. iniciando con NOT,
                //en donde el LVAL es el resultado del NOT, a continuacion, pasa
                //a ser el LVAL & y se transforma en un ciclo de 2 op.
                cycle = CYCLE_2OP;
                state_2OP = STATE_2OP_OPERATOR;
            }
            //ojo: NOT es un cyclo que no incluye en su parsing "Operadores logicos"
        }
        else //operandos
        {
            if (cycle == CYCLE_NONE)
            {
                cycle = CYCLE_2OP;

                if (state_2OP == STATE_2OP_NONE)
                {
                    lval = el;
                    state_2OP = STATE_2OP_LVAL;
                }
            }
            else if (cycle == CYCLE_NOT)
            {
                lval = el;

                if (state_1OP == STATE_1OP_NOT_OPERATOR)
                    state_1OP = STATE_1OP_NOT_OPERAND;
            }
            else if (cycle == CYCLE_2OP)
            {
                if (state_2OP == STATE_2OP_OPERATOR)
                {
                    rval = el;
                    state_2OP = STATE_2OP_RVAL;
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        if (rcsv == 0)
        {
            p++;            //incrementa normalmente
            (*counter_el)++;   //se suma el elemento actual(salvo '(' y ')' que tambien incrementan a *counter_el)
                            //"en linea", puesto que no ha regresado en esta pasada de una recursion,
        }
        else//==1
        {
            p = p + counter_el_rcsv;
            *counter_el = *counter_el + counter_el_rcsv;
            //fix:
            counter_el_rcsv = 0;//x cada pasada debe de limpiarse estas 2 variables
            rcsv = 0;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        //Ejecucion de las funciones de 1 op:NOT y de 2 op: AND, OR, etc ///////
        //no se usa nada de punteros, ya fueron deferenciados a su correspondientes lval / op / rval
        if (cycle == CYCLE_NOT)
        {
            if (state_1OP == STATE_1OP_NOT_OPERAND)//NOT_TO_END)
            {
                lval = !lval;

                cycle = CYCLE_NONE;
                state_1OP = STATE_1OP_NOT_NONE;
                if (rcsv_type == RCSV_TYPE_1OP_NOT)
                {
                    rcsv_type = RCSV_TYPE_NONE;
                    break;
                }
            }
        }
        else if (cycle == CYCLE_2OP)
        {
            if (state_2OP == STATE_2OP_RVAL)
            {
                lval = logical(lval, op, rval);

                cycle = CYCLE_NONE;
                state_2OP = STATE_2OP_NONE;
            }
        }

    }//end while

    //printf("lval es: %u\n", lval);
    return lval;
}

int main()
{
    //
    uint8_t counter_el=0;
    uint8_t lval = fr (exp, &counter_el, RCSV_TYPE_NONE);
    //
    printf("Valor final es %u, num.de elementos es: %u \n", lval, counter_el);
    return 0;
}
