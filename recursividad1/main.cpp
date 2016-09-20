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
//char exp[20]= {'n','n','n','n','n','n','n','n',1, eof};
//unsigned
//char exp[20]= {'n',1, eof};
//unsigned
//char exp[20]= {'n','n','n','n','n', 1, eof};

//char exp[20]= {'n','(','n', 1, ')', eof};
//char exp[20]= {'n','(','(', 'n', 1,')',')', eof};
//char exp[20]= {'n','(','(', 'n','n',1,')',')', eof};
//char exp[20]= {'(','n','(','(','(', 'n','(', 'n',1,')',')',')',')',')', eof};
char exp[20]= {'(','n',1,')', eof};
uint8_t fr(char *p, uint8_t *counter, uint8_t rec)
{
    uint8_t state = 0;
    uint8_t cycle = CYCLE_NONE;
    uint8_t lval, rval;
    uint8_t counter_rec=0;
    uint8_t rcsvd = 0;
    char el;

    while (*p != eof)
    {
        el = *p;

        if (el == 'n')
        {
            if (cycle == CYCLE_NONE)
            {
                cycle = CYCLE_NOT;
                 state = NOT_BEGIN;
            }
            else
            {
                rcsvd = 1;
                lval = fr(p, &counter_rec, rec_not);
                state = NOT_TO_END;
            }
        }
        else if (el == '(')
        {
            rcsvd = 1;

            p++;
            (*counter)++;

            lval = fr(p, &counter_rec, rec_parant);//regresa despues de haber )

            if (cycle == CYCLE_NOT)
            {
                //lval = _lval;
                state = NOT_TO_END;
            }
            else if (cycle == CYCLE_2OP)
            {

            }
        }
        else if (el == ')')
        {
            (*counter)++;
            break;//retorna del (
        }
        else if ((el=='&') || (el=='|'))
        {

        }
        else //operandos
        {
            if (cycle == CYCLE_NONE)
            {
                cycle = CYCLE_2OP;

            }
            else if (cycle == CYCLE_NOT)
            {
                lval = el;
                state = NOT_TO_END;
            }
        }
        //////////////////////////////////////
        if (rcsvd == 0)
            (*counter)++;

        //////////////////////////////////////
        if (cycle == CYCLE_NOT)
        {
            if (state == NOT_TO_END)
            {
                lval = !lval;

                state = NOT_END;
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
            if ((cycle == CYCLE_NOT) && (state == NOT_END))
            {
                state = 0;
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
