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
    NOT_BEGIN,
    NOT_TO_END,
    //NOT_END,
};

enum
{
    rec_none,
    rec_not,
    rec_parant,
};
enum
{
    STATE_2OP_NONE,
    STATE_2OP_LVAL,
    STATE_2OP_OPERATOR,
    STATE_2OP_RVAL,
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
//char exp[20]= {'(','n','n','(','n',1,')',')', '|', 'n', 1, eof};
//char exp[20]= {1,'&',0, eof};
//char exp[20]= {1,'&',0, '|', 1, '&', 0, eof};
//char exp[20]= {'n',0,'|','n',1, eof};
//char exp[20]= {1,'&','n',0, eof};
//char exp[20]= {'n',0,'&','(', 1 ,')', eof};
char exp[20]= {'n','(', 0,')','&','(', 'n', '(',0, ')', '&', 1,')', eof};

uint8_t fr(const char *p, uint8_t *counter, uint8_t rec)
{
    char el;
    uint8_t cycle = CYCLE_NONE;
    uint8_t lval, _lval;
    uint8_t counter_rec=0;
    uint8_t rcsvd = 0;

    uint8_t state_NOT = 0;
    uint8_t state_2OP = 0;
    uint8_t rval;
    uint8_t op;

    while (*p != eof)
    {
        el = *p;

        //rcsvd = 0;//fix:

        if (el == 'n')
        {
            if (cycle == CYCLE_NONE)
            {
                cycle = CYCLE_NOT;
                state_NOT = NOT_BEGIN;
            }
            else if (cycle == CYCLE_NOT)
            {
                rcsvd = 1;
                lval = fr(p, &counter_rec, rec_not);
                state_NOT = NOT_TO_END;
            }
            else if (cycle == CYCLE_2OP)
            {
                rcsvd = 1;

                if (state_2OP == STATE_2OP_OPERATOR)
                {
                    rval = fr(p, &counter_rec, rec_not);
                    state_2OP = STATE_2OP_RVAL;
                }
            }
        }
        else if (el == '(')
        {
            rcsvd = 1;

            p++;
            (*counter)++;

            _lval = fr(p, &counter_rec, rec_parant);//regresa despues de haber )

            if (cycle == CYCLE_NONE)
            {
                lval = _lval;//simplemente un seguidor del ultimo lval
            }
            else if (cycle == CYCLE_NOT)
            {
                lval = _lval;
                state_NOT = NOT_TO_END;
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
            (*counter)++;   //por su naturaleza, siempre regresa de una llamada recursiva
            break;          //originada por '(', osea, este *counter es el &counter_rec
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
                state_NOT = NOT_TO_END;
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
        //        //////////////////////////////////////
        //        if (rcsvd == 0)     //se suma el elemento actual(salvo '(' y ')' que tambien incrementan a *counter)
        //            (*counter)++;   //"en linea", puesto que no ha regresado en esta pasada de una recursion,
        //        //////////////////////////////////////
        //        if (rcsvd == 0) //
        //            p++;        //incrementa normalmente
        //        else if (rcsvd == 1)
        //            p = p + counter_rec;
        //        //////////////////////////////////////
        //        *counter = *counter + counter_rec;
        //        counter_rec = 0;//fix:
        //        //////////////////////////////////////
        if (rcsvd == 0)
        {
            p++;
            (*counter)++;
        }
        else//==1
        {
            p = p + counter_rec;
            *counter = *counter + counter_rec;
            //fix:
            counter_rec = 0;//x cada pasada debe de limpiarse estas 2 variables
            rcsvd = 0;
        }

        //Ejecucion de las funciones de 1 op:NOT y de 2 op: AND, OR, etc ///////
        //no se usa nada de punteros, ya fueron deferenciados a su correspondientes lval / op / rval
        if (cycle == CYCLE_NOT)
        {
            if (state_NOT == NOT_TO_END)
            {
                lval = !lval;

                //---------------------
                cycle = CYCLE_NONE;
                state_NOT = 0;
                if (rec == rec_not)
                {
                    rec = rec_none;
                    break;
                }
                //---------------------
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
    uint8_t counter_el=0;

    uint8_t lval = fr (exp, &counter_el, 0);

    printf("\n\n valor final es %u, counter is: %u \n", lval, counter_el);

    return 0;
}
