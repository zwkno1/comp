#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

enum OpClass
{
    opclRR,     /* reg operands r,s,t */
    opclRM,     /* reg r, mem d+s */
    opclRA      /* reg r, int d+s */
};

enum OpCode
{
    /* RR instructions */
    opHALT,    /* RR     halt, operands are ignored */
    opIN,      /* RR     read into reg(r); s and t are ignored */
    opOUT,     /* RR     write from reg(r), s and t are ignored */
    opADD,    /* RR     reg(r) = reg(s)+reg(t) */
    opSUB,    /* RR     reg(r) = reg(s)-reg(t) */
    opMUL,    /* RR     reg(r) = reg(s)*reg(t) */
    opDIV,    /* RR     reg(r) = reg(s)/reg(t) */
    opRRLim,   /* limit of RR opcodes */

    /* RM instructions */
    opLD,      /* RM     reg(r) = mem(d+reg(s)) */
    opST,      /* RM     mem(d+reg(s)) = reg(r) */
    opRMLim,   /* Limit of RM opcodes */

    /* RA instructions */
    opLDA,     /* RA     reg(r) = d+reg(s) */
    opLDC,     /* RA     reg(r) = d ; reg(s) is ignored */
    opJLT,     /* RA     if reg(r)<0 then reg(7) = d+reg(s) */
    opJLE,     /* RA     if reg(r)<=0 then reg(7) = d+reg(s) */
    opJGT,     /* RA     if reg(r)>0 then reg(7) = d+reg(s) */
    opJGE,     /* RA     if reg(r)>=0 then reg(7) = d+reg(s) */
    opJEQ,     /* RA     if reg(r)==0 then reg(7) = d+reg(s) */
    opJNE,     /* RA     if reg(r)!=0 then reg(7) = d+reg(s) */
    opRALim    /* Limit of RA opcodes */
};

enum Result
{
    srOKAY,
    srHALT,
    srIMEM_ERR,
    srDMEM_ERR,
    srZERODIVIDE
};

struct Instrcution
{
    Instrcution()
        : op(0)
        , arg1(0)
        , arg2(0)
        , arg3(0)
    {
    }

    int op;
    int arg1;
    int arg2;
    int arg3;
};


const int IADDR_SIZE = 1024; /* increase for large programs */
const int DADDR_SIZE = 1024; /* increase for large programs */
const int NO_REGS = 8;
const int PC_REG = 7;

INSTRUCTION iMem [IADDR_SIZE];
int dMem [DADDR_SIZE];
int reg [NO_REGS];

const char * opCodeTab[] =
{
    "HALT","IN","OUT","ADD","SUB","MUL","DIV","????",
    /* RR opcodes */
    "LD","ST","????", /* RM opcodes */
    "LDA","LDC","JLT","JLE","JGT","JGE","JEQ","JNE","????"
    /* RA opcodes */
};

const char * stepResultTab[] =
{
    "OK","Halted","Instruction Memory Fault",
    "Data Memory Fault","Division by 0"
};

int getOpClass(int op)
{
    if(op <= opRRLim)
        return opclRR;
    else if(op <= opRMLim)
        return opclRM;
    else
        return opclRA;
}

void printRegs()
{
    for(int i = 0; i < NO_REGS; ++i)
    {
        printf("%8d,", reg[i]);
    }
    printf("\n");
}

void printMem()
{
    for(int i = 0; i < DADDR_SIZE; ++i)
    {
        printf("%d,", dMem[i]);
    }
    printf("\n");
}

int readInstructions()
{
}

Result step()
{
    INSTRUCTION currentinstruction  ;
    int pc  ;
    int r,s,t,m  ;
    int ok ;

    pc = reg[PC_REG] ;
    if ( (pc < 0) || (pc > IADDR_SIZE)  )
        return srIMEM_ERR ;
    reg[PC_REG] = pc + 1 ;
    currentinstruction = iMem[ pc ] ;
    switch (opClass(currentinstruction.iop) )
    { case opclRR :
        /***********************************/
        r = currentinstruction.iarg1 ;
        s = currentinstruction.iarg2 ;
        t = currentinstruction.iarg3 ;
        break;

    case opclRM :
        /***********************************/
        r = currentinstruction.iarg1 ;
        s = currentinstruction.iarg3 ;
        m = currentinstruction.iarg2 + reg[s] ;
        if ( (m < 0) || (m > DADDR_SIZE))
            return srDMEM_ERR ;
        break;

    case opclRA :
        /***********************************/
        r = currentinstruction.iarg1 ;
        s = currentinstruction.iarg3 ;
        m = currentinstruction.iarg2 + reg[s] ;
        break;
    } /* case */

    switch ( currentinstruction.iop)
    { /* RR instructions */
    case opHALT :
        /***********************************/
        printf("HALT: %1d,%1d,%1d\n",r,s,t);
        return srHALT ;
        /* break; */

    case opIN :
        /***********************************/
        do
        { printf("Enter value for IN instruction: ") ;
            fflush (stdin);
            fflush (stdout);
            gets(in_Line);
            lineLen = strlen(in_Line) ;
            inCol = 0;
            ok = getNum();
            if ( ! ok ) printf ("Illegal value\n");
            else reg[r] = num;
        }
        while (! ok);
        break;

    case opOUT :
        printf ("OUT instruction prints: %d\n", reg[r] ) ;
        break;
    case opADD :  reg[r] = reg[s] + reg[t] ;  break;
    case opSUB :  reg[r] = reg[s] - reg[t] ;  break;
    case opMUL :  reg[r] = reg[s] * reg[t] ;  break;

    case opDIV :
        /***********************************/
        if ( reg[t] != 0 ) reg[r] = reg[s] / reg[t];
        else return srZERODIVIDE ;
        break;

        /*************** RM instructions ********************/
    case opLD :    reg[r] = dMem[m] ;  break;
    case opST :    dMem[m] = reg[r] ;  break;

        /*************** RA instructions ********************/
    case opLDA :    reg[r] = m ; break;
    case opLDC :    reg[r] = currentinstruction.iarg2 ;   break;
    case opJLT :    if ( reg[r] <  0 ) reg[PC_REG] = m ; break;
    case opJLE :    if ( reg[r] <=  0 ) reg[PC_REG] = m ; break;
    case opJGT :    if ( reg[r] >  0 ) reg[PC_REG] = m ; break;
    case opJGE :    if ( reg[r] >=  0 ) reg[PC_REG] = m ; break;
    case opJEQ :    if ( reg[r] == 0 ) reg[PC_REG] = m ; break;
    case opJNE :    if ( reg[r] != 0 ) reg[PC_REG] = m ; break;

        /* end of legal instructions */
    } /* case */
    return srOKAY ;
} /* stepTM */

int main( int argc, char * argv[] )
{
    if (argc != 2)
    {
        printf("usage: %s <filename>\n",argv[0]);
        return -1;
    }
    std::
    if (pgm == NULL)
    { printf("file '%s' not found\n",pgmName);
        exit(1);
    }

    /* read the program */
    if ( ! readInstructions ())
        exit(1) ;
    /* switch input file to terminal */
    /* reset( input ); */
    /* read-eval-print */
    printf("TM  simulation (enter h for help)...\n");
    do
        done = ! doCommand ();
    while (! done );
    printf("Simulation done.\n");
    return 0;
}
