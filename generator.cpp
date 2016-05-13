#include "generator.h"

#include <iomanip>

namespace
{

/* pc = program counter  */
const int pc = 7;

/* mp = "memory pointer" points
 * to top of memory (for temp storage)
 */
const int mp = 6;

/* gp = "global pointer" points
 * to bottom of memory for (global)
 * variable storage
 */
const int gp = 5;

/* accumulator */
const int ac = 0;

/* 2nd accumulator */
const int  ac1 = 1;

} // namespace

Generator::Generator(Analyzer & analyzer, std::ostream & os)
    : analyzer_(analyzer)
    , os_(os)
    , current_location_(0)
    , highest_location_(0)
    , tmp_offset_(0)
{
}


void Generator::genCode()
{
    genRM("LD", mp, 0, ac);
    genRM("ST", ac, 0, ac);
    genCode(analyzer_.root());
    genRO("HALT", 0, 0, 0);
}

void Generator::genCode(TreeNode * t)
{
    if(t)
    {
        switch (t->kind)
        {
        case StmtSeqK:
            genStmtSeq(t);
            break;
        case ExprK:
            genExpr(t);
            break;
        case StmtK:
            genStmt(t);
            break;
        default:
            break;
        }

    }
}

void Generator::genStmtSeq(TreeNode * t)
{
    TreeNode *child = t->child;
    while(child)
    {
        genCode(child);
        child = child->sibling;
    }
}


void Generator::genStmt(TreeNode * t)
{
    switch (t->stmt_kind)
    {
    case IfK :
    {
        /*
         * -------------------
         *        expr
         * -------------------
         *  IF FALSE JMP ELSE
         * -------------------
         *        ...
         * -------------------
         *      JMP END
         * -------------------
         * ELSE:
         * --------------------
         *        ...
         * --------------------
         * END:
         * --------------------
         */

        /* generate code for test expression */
        genCode(t->child);

        /* save location for if */
        size_t loc1 = skipLocation(1);

        /* recurse on then part */
        genCode(t->child->sibling);

        /* save location for esle */
        size_t loc2 = skipLocation(1);

        /* generate code for if jmp */
        size_t current_loc = skipLocation(0);
        backupLocation(loc1);
        genRMAbs("JEQ", ac, current_loc);
        restoreLocation();

        /* recurse on else part */
        genCode(t->child->sibling->sibling);

        /* generate code for else jmp */
        current_loc = skipLocation(0);
        backupLocation(loc2);
        genRMAbs("LDA", ac, current_loc);
        restoreLocation();
    }
        break;
    case RepeatK :
    {
        /* save location for repeat */
        size_t loc = skipLocation(0);
        genCode(t->child);
        genCode(t->child->sibling);
        genRMAbs("JEQ", ac, loc);
    }
        break;
    case AssignK :
    {
        genCode(t->child);
        size_t loc = analyzer_.findSymbol(t->name);
        genRM("ST", ac, loc, gp);
    }
        break;
    case ReadK :
    {
        genRO("IN", ac, 0, 0);
        size_t loc = analyzer_.findSymbol(t->name);
        genRM("ST", ac, loc, gp);
    }
        break;
    case WriteK :
    {
        genCode(t->child);
        genRO("OUT", ac, 0, 0);
    }
        break;
    default:
        break;
    }
}

void Generator::genExpr(TreeNode * t)
{
    switch (t->expr_kind)
    {
    case ConstK:
    {
        genRM("LDC", ac, t->value, 0);
    }
        break;
    case IdK:
    {
        size_t loc = analyzer_.findSymbol(t->name);
        genRM("LD", ac, loc, gp);
    }
        break;
    case OpK:
    {
        /* gen code for ac = left arg */
        genCode(t->child);
        /* gen code to push left operand */
        genRM("ST", ac, tmp_offset_--, mp);
        /* gen code for ac = right operand */
        genCode(t->child->sibling);
        /* now load left operand */
        genRM("LD",ac1,++tmp_offset_,mp);
        switch (t->op)
        {
        case ADD:
            genRO("ADD", ac, ac1, ac);
            break;
        case SUB:
            genRO("SUB", ac, ac1, ac);
            break;
        case MUL:
            genRO("MUL", ac, ac1, ac);
            break;
        case DIV:
            genRO("DIV", ac, ac1, ac);
            break;
        case LT :
            genRO("SUB", ac, ac1, ac);
            genRM("JLT", ac, 2, pc);
            genRM("LDC", ac, 0, ac);
            genRM("LDA", pc, 1, pc);
            genRM("LDC", ac, 1, ac);
            break;
         case EQ :
            genRO("SUB", ac, ac1, ac);
            genRM("JEQ", ac, 2, pc);
            genRM("LDC", ac, 0, ac);
            genRM("LDA", pc, 1, pc);
            genRM("LDC", ac, 1, ac);
            break;
        default:
            break;
        }
    }
    default:
        break;
    }
}

size_t Generator::skipLocation(size_t size)
{
    size_t ret = current_location_;
    current_location_ += size;
    if(highest_location_ < current_location_)
        highest_location_ = current_location_;
    return ret;
}

void Generator::backupLocation(size_t loc)
{
    current_location_ = loc;
}

void Generator::restoreLocation()
{
    current_location_ = highest_location_;
}

void Generator::increaseLocation()
{
    ++current_location_;
    if(highest_location_ < current_location_)
        highest_location_ = current_location_;
}

void Generator::genRO(const char *op, int r, int s, int t)
{
    os_ << std::setw(8) << current_location_ << ":    "
        << std::setw(4) << op << "    "
        << r << "," << s << "," << t << "\n";
    increaseLocation();
}

void Generator::genRM(const char * op, int r, int d, int s)
{
    os_ << std::setw(8) << current_location_ << ":    "
        << std::setw(4) << op << "    "
        << r << "," << d << "(" << s << ")\n";
    increaseLocation();
}

void Generator::genRMAbs(const char *op, int r, int a)
{
    genRM(op, r, a-(current_location_+1), pc);
}
