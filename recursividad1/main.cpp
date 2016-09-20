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
    NOT_END,
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
//unsigned
//char exp[20]= {'n',1, eof};
//unsigned
//char exp[20]= {'n','n','n','n','n', 1, eof};

//char exp[20]= {'n','(','n', 1, ')', eof};
//char exp[20]= {'n','(','(', 'n', 1,')',')', eof};
//char exp[20]= {'n','(','(', 'n','n',1,')',')', eof};
//char exp[20]= {'(','n','(','(','(', 'n','(', 'n',1,')',')',')',')',')', eof};
//char exp[20]= {'(','n',1,')', eof};

char exp[20]= {1,'&',0, eof};
uint8_t fr(char *p, uint8_t *counter, uint8_t rec)
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

        if (el == 'n')
        {
            if (cycle == CYCLE_NONE)
            {
                cycle = CYCLE_NOT;
                state_NOT = NOT_BEGIN;
            }
            else
            {
                rcsvd = 1;
                lval = fr(p, &counter_rec, rec_not);
                state_NOT = NOT_TO_END;
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
                lval = _lval;
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
                    rval = lval;
                    state_2OP = STATE_2OP_RVAL;
                }
            }
        }
        else if (el == ')')
        {
            (*counter)++;
            break;//retorna del (
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
        //////////////////////////////////////
        if (rcsvd == 0)
            (*counter)++;

        ///////////////////////////////////////
        if (cycle == CYCLE_2OP)
        {
            if (state_2OP == STATE_2OP_RVAL)
            {
                //ready to operate
                lval = logical(lval, op, rval);
            }
        }
        ///////////////////////////////////////

        //////////////////////////////////////
        if (cycle == CYCLE_NOT)
        {
            if (state_NOT == NOT_TO_END)
            {
                lval = !lval;

                state_NOT = NOT_END;
            }
        }

        //////////////////////////////////////

        if (rcsvd == 0)//
            p++;//incrementa normalmente
        else if (rcsvd == 1)
            p = p + counter_rec;
        //////////////////////////////////////

        *counter = *counter + counter_rec;

        ///////////////////////////////

        if (rec > rec_none)
        {
            if ((cycle == CYCLE_NOT) && (state_NOT == NOT_END))
            {
                state_NOT = 0;
                cycle = CYCLE_NONE;

                if (rec == rec_not)
                {
                    rec = rec_none;
                    break;
                }
            }
            //else if (rec == rec_parenth)
        }

    }//end while
    printf("lval es: %u\n", lval);
    return lval;
}

int main()
{
    uint8_t counter_el=0;

    uint8_t lval = fr (exp, &counter_el, 0);

    printf("\n\n valor final es %u, counter is: %u \n", lval, counter_el);

    return 0;
}
