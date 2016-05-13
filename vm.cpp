#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>

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
    opLDC,     /* RA     reg(r) = d; reg(s) ss ignored */
    opJLT,     /* RA     ifreg(r)<0 then reg(7) = d+reg(s) */
    opJLE,     /* RA     ifreg(r)<=0 then reg(7) = d+reg(s) */
    opJGT,     /* RA     ifreg(r)>0 then reg(7) = d+reg(s) */
    opJGE,     /* RA     ifreg(r)>=0 then reg(7) = d+reg(s) */
    opJEQ,     /* RA     ifreg(r)==0 then reg(7) = d+reg(s) */
    opJNE,     /* RA     ifreg(r)!=0 then reg(7) = d+reg(s) */
    opRALim    /* Limit of RA opcodes */
};

enum Result
{
    srOKAY,
    srHALT,
    srIMEM_ERR,
    srDMEM_ERR,
    srZERODIVIDE,
    srINVALIDINPUT,
    srINVALIDOPCLASS
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

Instrcution iMem [IADDR_SIZE];
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

const char * resultTab[] =
{
    "OK",
    "Halted",
    "Instruction Memory Fault",
    "Data Memory Fault",
    "Division by 0",
    "Invalid Input",
    "Invalid Instruction",
};

void init()
{
    std::memset(reg, 0, NO_REGS);
    std::memset(dMem, 0, DADDR_SIZE);
    dMem[0] = DADDR_SIZE - 1;
}

int getOpClass(int op)
{
    if(op <= opRRLim)
        return opclRR;
    else if(op <= opRMLim)
        return opclRM;
    else
        return opclRA;
}

int readInstructions(std::istream & is)
{
    std::string str;
    while(std::getline(is, str))
    {
        std::stringstream ss(str);
        Instrcution inst;
        int loc = 0;
        ss >> loc;
        if(ss.fail())
            return -1;

        char ch;
        ss >> ch;
        if(ss.fail() || ch != ':')
            return -1;

        std::string opStr;
        ss >> opStr;
        if(ss.fail())
            return -1;
        while (inst.op < opRALim && opStr != opCodeTab[inst.op])
            ++inst.op;
        if(inst.op == opRALim || inst.op == opRMLim || inst.op == opRRLim)
            return -1;

        ss >> inst.arg1;
        if(ss.fail())
            return -1;
        ss >> ch;
        if(ss.fail() || ch != ',')
            return -1;

        ss >> inst.arg2;
        if(ss.fail())
            return -1;
        switch (getOpClass(inst.op))
        {
        case opclRR:
        {
            ss >> ch;
            if(ss.fail() || ch != ',')
                return -1;
            ss >> inst.arg3;
            if(ss.fail())
                return -1;
        }
            break;
        case opclRA:
        case opclRM:
        {
            ss >> ch;
            if(ss.fail() || ch != '(')
                return -1;
            ss >> inst.arg3;
            if(ss.fail())
                return -1;
            ss >> ch;
            if(ss.fail() || ch != ')')
                return -1;
        }
            break;
        default:
            return -1;
            break;
        }
        if(loc >= IADDR_SIZE)
            return -1;
        iMem[loc] = inst;
    }
    return 1;
}

Result step()
{
    int pc = reg[PC_REG];
    int r, s, t, m;
    if((pc < 0) || (pc > IADDR_SIZE)  )
        return srIMEM_ERR;
    reg[PC_REG] = pc + 1;

    Instrcution & currentinstruction = iMem[pc];
    switch (getOpClass(currentinstruction.op) )
    {
    case opclRR:
        r = currentinstruction.arg1;
        s = currentinstruction.arg2;
        t = currentinstruction.arg3;
        break;
    case opclRM:
        r = currentinstruction.arg1;
        s = currentinstruction.arg3;
        m = currentinstruction.arg2 + reg[s];
        if((m < 0) || (m > DADDR_SIZE))
            return srDMEM_ERR;
        break;
    case opclRA:
        r = currentinstruction.arg1;
        s = currentinstruction.arg3;
        m = currentinstruction.arg2 + reg[s];
        break;
    }

    switch(currentinstruction.op)
    {
    /* RR instructions */
    case opHALT:
        return srHALT;
    case opIN:
    {
        int num;
		std::cout << "IN:";
		std::cout.flush();
        std::cin >> num;
        if(std::cin.fail())
            return srINVALIDINPUT;
        reg[r] = num;
    }
        break;
    case opOUT:
        std::cout << "OUT:" << reg[r] << std::endl;
        break;
    case opADD: reg[r] = reg[s] + reg[t]; break;
    case opSUB: reg[r] = reg[s] - reg[t]; break;
    case opMUL: reg[r] = reg[s] * reg[t]; break;
    case opDIV:
        if(reg[t] == 0 )
            return srZERODIVIDE;
        reg[r] = reg[s] / reg[t];
        break;

        /*************** RM instructions ********************/
    case opLD: reg[r] = dMem[m]; break;
    case opST: dMem[m] = reg[r]; break;

        /*************** RA instructions ********************/
    case opLDA: reg[r] = m; break;
    case opLDC: reg[r] = currentinstruction.arg2; break;
    case opJLT: if(reg[r] <  0 ) reg[PC_REG] = m; break;
    case opJLE: if(reg[r] <= 0 ) reg[PC_REG] = m; break;
    case opJGT: if(reg[r] >  0 ) reg[PC_REG] = m; break;
    case opJGE: if(reg[r] >= 0 ) reg[PC_REG] = m; break;
    case opJEQ: if(reg[r] == 0 ) reg[PC_REG] = m; break;
    case opJNE: if(reg[r] != 0 ) reg[PC_REG] = m; break;
    default:
        return srINVALIDOPCLASS;
    }
    return srOKAY;
}

void printRegs()
{
    std::cout << "-----------------------------------------------------------REG-----------------------------------------------------------" << std::endl;
    for(int i = 0; i < NO_REGS; ++i)
    {
        std::cout << std::setw(8) << reg[i] << ",";
    }
    std::cout << std::endl;
}

void printMem()
{
    std::cout << "-----------------------------------------------------------MEM-----------------------------------------------------------" << std::endl;
    for(int i = 0; i < DADDR_SIZE; ++i)
    {
        std::cout <<  dMem[i] << ",";
    }
    std::cout << std::endl;
}

void printInstructions()
{
    for(int i = 0; i < IADDR_SIZE; ++i)
    {
        std::cout << std::setw(8) << i << ":"
                  << std::setw(4) << opCodeTab[iMem[i].op]
                  << iMem[i].arg1 << "," << iMem[i].arg2;
        switch (getOpClass(iMem[i].op))
        {
        case opclRR:
        {
            std::cout << "," << iMem[i].arg3;
        }
            break;
        case opclRA:
        case opclRM:
            std::cout << "(" << iMem[i].arg3 << ")";
            break;
        default:
            break;
        }
        std::cout << std::endl;
        if(iMem[i].op == opHALT)
            break;
    }
}

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <filename>" << std::endl;
        return -1;
    }

    std::ifstream ifs(argv[1]);

    if(!ifs.is_open())
    {
        std::cout << "file '"<< argv[1] << "' not found" << std::endl;
        return -1;
    }

    init();

    if(readInstructions(ifs) != 1)
        return -1;
#ifdef DEBUG
    printInstructions();
#endif
    Result ret;
    do
    {
#ifdef DEBUG
        printRegs();
        printMem();
#endif
        ret = step();
    }while(ret == srOKAY);

    std::cout << resultTab[ret] << std::endl;

    return 0;
}
