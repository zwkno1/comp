#include <iostream>
#include <fstream>

#include "scanner.h"
#include "parser.h"
#include "analyzer.h"

using namespace std;
namespace test
{

void scanner_test(const std::string & file_path)
{
    std::fstream fs(file_path);
    Scanner s(fs);

    int token;
    do
    {
        token = s.getToken();
        const char *arr[] =
        {
            "IF",
            "THEN",
            "ELSE",
            "END",
            "REPEAT",
            "UNTIL",
            "READ",
            "WRITE",
            "ASSIGN",
            "EQ",
            "LT",
            "ADD",
            "SUB",
            "MUL",
            "DIV",
            "LP",
            "RP",
            "SEMI",
            "NUM",
            "ID",
            "ERROR",
            "ENDFILE",
        };
        if(token == ID)
        {
            std::cout << arr[token] << ":" << s.id() << std::endl;
        }
        else if(token == NUM)
        {
            std::cout << arr[token] << ":" << s.num() << std::endl;
        }
        else
        {
            std::cout << arr[token] << std::endl;
        }
    }while(token != ERROR && token != ENDFILE);
}

void print_stmtseq(TreeNode * t);

void print_expr(TreeNode * t)
{
    switch (t->expr_kind)
    {
    case OpK:
        switch (t->op)
        {
        case ADD:
            print_expr(t->child);
            std::cout << "+ ";
            print_expr(t->child->sibling);
            break;
        case SUB:
            print_expr(t->child);
            std::cout << "- ";
            print_expr(t->child->sibling);
            break;
        case MUL:
            print_expr(t->child);
            std::cout << "* ";
            print_expr(t->child->sibling);
            break;
        case DIV:
            print_expr(t->child);
            std::cout << "/ ";
            print_expr(t->child->sibling);
            break;
        case LT:
            print_expr(t->child);
            std::cout << "< ";
            print_expr(t->child->sibling);
            break;
        case EQ:
            print_expr(t->child);
            std::cout << "= ";
            print_expr(t->child->sibling);
            break;
        default:
            break;
        }
        break;
    case ConstK:
        std::cout << t->value << " ";
        break;
    case IdK:
        std::cout << t->name << " ";
        break;
    default:
        std::cout << "??? ";
        break;
    }
}

void print_stmt(TreeNode * t)
{
    switch (t->stmt_kind)
    {
    case IfK :
        std::cout << "if ";
        print_expr(t->child);
        std::cout << " then\n";
        print_stmtseq(t->child->sibling);
        if(t->child->sibling->sibling)
        {
            std::cout << " else\n";
            print_stmtseq(t->child->sibling->sibling);
        }
        std::cout << "end\n";
        break;
    case RepeatK :
        std::cout << "repeat\n";
        print_stmtseq(t->child);
        std::cout << "until ";
        print_expr(t->child->sibling);
        std::cout << ";\n";
        break;
    case AssignK :
        std::cout << t->name << " := ";
        print_expr(t->child);
        std::cout << ";\n";
        break;
    case ReadK :
        std::cout << "read " << t->name << ";\n";
        break;
    case WriteK :
        std::cout << "write ";
        print_expr(t->child);
        std::cout << ";\n";
        break;
    default:
        std::cout << "???\n";
        break;
    }

}

void print_stmtseq(TreeNode *t)
{
    t = t->child;
    while(t)
    {
        print_stmt(t);
        t = t->sibling;
    }
}

void parser_test(const std::string & file_path)
{
    try
    {
        std::fstream fs(file_path);
        Scanner s(fs);
        Parser p(s);
        p.parse();
        TreeNode * root = p.root();
        Analyzer a(root);
        a.type_check();
        a.build_symbol_table();
        a.print_symbol_table();
        print_stmtseq(root);
    }
    catch(...)
    {
        return;
    }
}

}

int main(int argc, char *argv[])
{
    test::parser_test("/home/zwk/loucomp/comp/sample.tny");
}
