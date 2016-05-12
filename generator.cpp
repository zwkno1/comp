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


void Generator::gen_code()
{
    gen_rm("LD", mp, 0, ac);
    gen_rm("ST", ac, 0, ac);
    gen_code(analyzer_.root());
    gen_ro("HALT", 0, 0, 0);
}

void Generator::gen_code(TreeNode * t)
{
    if(t)
    {
        switch (t->kind)
        {
        case StmtSeqK:
            gen_stmt_seq(t);
            break;
        case ExprK:
            gen_expr(t);
            break;
        case StmtK:
            gen_stmt(t);
            break;
        default:
            break;
        }

    }
}

void Generator::gen_stmt_seq(TreeNode * t)
{
    TreeNode *child = t->child;
    while(child)
    {
        gen_code(child);
        child = child->sibling;
    }
}


void Generator::gen_stmt(TreeNode * t)
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
        gen_code(t->child);

        /* save location for if */
        size_t loc1 = skip_location(1);

        /* recurse on then part */
        gen_code(t->child->sibling);

        /* save location for esle */
        size_t loc2 = skip_location(1);

        /* generate code for if jmp */
        size_t current_loc = skip_location(0);
        backup_location(loc1);
        gen_rm_abs("JEQ", ac, current_loc);
        restore_location();

        /* recurse on else part */
        gen_code(t->child->sibling->sibling);

        /* generate code for else jmp */
        current_loc = skip_location(0);
        backup_location(loc2);
        gen_rm_abs("LDA", ac, current_loc);
        restore_location();
    }
        break;
    case RepeatK :
    {
        /* save location for repeat */
        size_t loc = skip_location(0);
        gen_code(t->child);
        gen_code(t->child->sibling);
        gen_rm_abs("JEQ", ac, loc);
    }
        break;
    case AssignK :
    {
        gen_code(t->child);
        size_t loc = analyzer_.find_symbol(t->name);
        gen_rm("ST", ac, loc, gp);
    }
        break;
    case ReadK :
    {
        gen_ro("IN", ac, 0, 0);
        size_t loc = analyzer_.find_symbol(t->name);
        gen_rm("ST", ac, loc, gp);
    }
        break;
    case WriteK :
    {
        gen_code(t->child);
        gen_ro("OUT", ac, 0, 0);
    }
        break;
    default:
        break;
    }
}

void Generator::gen_expr(TreeNode * t)
{
    switch (t->expr_kind)
    {
    case ConstK:
    {
        gen_rm("LDC", ac, t->value, 0);
    }
        break;
    case IdK:
    {
        size_t loc = analyzer_.find_symbol(t->name);
        gen_rm("LD", ac, loc, gp);
    }
        break;
    case OpK:
    {
        /* gen code for ac = left arg */
        gen_code(t->child);
        /* gen code to push left operand */
        gen_rm("ST", ac, tmp_offset_--, mp);
        /* gen code for ac = right operand */
        gen_code(t->child->sibling);
        /* now load left operand */
        gen_rm("LD",ac1,++tmp_offset_,mp);
        switch (t->op)
        {
        case ADD:
            gen_ro("ADD", ac, ac1, ac);
            break;
        case SUB:
            gen_ro("SUB", ac, ac1, ac);
            break;
        case MUL:
            gen_ro("MUL", ac, ac1, ac);
            break;
        case DIV:
            gen_ro("DIV", ac, ac1, ac);
            break;
        case LT :
            gen_ro("SUB", ac, ac1, ac);
            gen_rm("JLT", ac, 2, pc);
            gen_rm("LDC", ac, 0, ac);
            gen_rm("LDA", pc, 1, pc);
            gen_rm("LDC", ac, 1, ac);
            break;
         case EQ :
            gen_ro("SUB", ac, ac1, ac);
            gen_rm("JEQ", ac, 2, pc);
            gen_rm("LDC", ac, 0, ac);
            gen_rm("LDA", pc, 1, pc);
            gen_rm("LDC", ac, 1, ac);
            break;
        default:
            break;
        }
    }
    default:
        break;
    }
}

size_t Generator::skip_location(size_t size)
{
    size_t ret = current_location_;
    current_location_ += size;
    if(highest_location_ < current_location_)
        highest_location_ = current_location_;
    return ret;
}

void Generator::backup_location(size_t loc)
{
    current_location_ = loc;
}

void Generator::restore_location()
{
    current_location_ = highest_location_;
}

void Generator::increase_location()
{
    ++current_location_;
    if(highest_location_ < current_location_)
        highest_location_ = current_location_;
}

void Generator::gen_ro(const char *op, int r, int s, int t)
{
    os_ << std::setw(8) << current_location_ << ":    "
        << std::setw(4) << op << "    "
        << r << "," << s << "," << t << "\n";
    increase_location();
}

void Generator::gen_rm(const char * op, int r, int d, int s)
{
    os_ << std::setw(8) << current_location_ << ":    "
        << std::setw(4) << op << "    "
        << r << "," << d << "(" << s << ")\n";
    increase_location();
}

void Generator::gen_rm_abs(const char *op, int r, int a)
{
    gen_rm(op, r, a-(current_location_+1), pc);
}
