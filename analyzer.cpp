#include "analyzer.h"
#include "treenode.h"
#include <iostream>

Analyzer::Analyzer(Parser & parser)
    : parser_(parser)
    , location_(0)
{
}

void Analyzer::analysis()
{
    typeCheck();
    buildSymbolTable();
}

void Analyzer::typeCheck()
{
    typeCheck(root());
}

void Analyzer::buildSymbolTable()
{
    buildSymbolTable(root());
}

void Analyzer::typeCheck(TreeNode * t)
{
    traverse(t, [this](TreeNode *)
    {
    },
    [this](TreeNode * t)
    {
        checkNode(t);
    });
}

void Analyzer::buildSymbolTable(TreeNode *t)
{
    traverse(root(), [this](TreeNode *t)
    {
        switch (t->kind)
        {
        case StmtK:
            switch (t->stmt_kind)
            {
            case AssignK:
            case ReadK:
                insertSymbol(t);
                break;
            default:
                break;
            }
            break;
        case ExprK:
            switch (t->expr_kind)
            {
            case IdK:
                insertSymbol(t);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    },
    [this](TreeNode * )
    {
    });

}

void Analyzer::traverse(TreeNode * t, auto pre, auto post)
{
    if(t)
    {
        pre(t);
        if(t->child)
        {
            TreeNode * c = t->child;
            while(c)
            {
                traverse(c, pre, post);
                c = c->sibling;
            }
        }
        post(t);
    }
}

    
void Analyzer::checkNode(TreeNode * t)
{
    switch (t->kind) 
    {
    case ExprK:
        switch (t->expr_kind) 
        {
        case OpK:
            switch (t->op) 
            {
            case EQ:
            case LT:
                t->expr_type = Boolean;
                break;
            default:
                t->expr_type = Integer;
                break;
            }
            break;
        case ConstK:
        case IdK:
            t->expr_type = Integer;
            break;
        default:
            break;
        }
        
        break;
    case StmtK:
        switch (t->stmt_kind) 
        {
        case IfK:
            if (t->child->expr_type != Boolean)
                typeCheckError(t->child, "if test is not Boolean");
            break;
        case RepeatK:
            if(t->child->sibling->expr_type != Boolean)
                typeCheckError(t->child, "repeat test is not Boolean");
            break;
        case AssignK:
            if(t->child->expr_type != Integer)
                typeCheckError(t->child, "assignment of non-integer value");
            break;
        case WriteK:
            if(t->child->expr_type != Integer)
                typeCheckError(t->child, "write of non-integer value");
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void Analyzer::typeCheckError(TreeNode * t, const char * err)
{
    std::cout << "line " << t->line_no << " >>>> " << err <<std::endl;
    throw;
}

void Analyzer::insertSymbol(TreeNode * t)
{
    auto it = symbol_table_.find(t->name);
    if(it == symbol_table_.end())
    {
        symbol_table_[t->name] = Symbol{ t->name, location_++, { t->line_no } };
    }
    else
    {
        it->second.lines.push_back(t->line_no);
    }
}

size_t Analyzer::findSymbol(const std::string & name)
{
    auto iter = symbol_table_.find(name);
    if(iter != symbol_table_.end())
    {
        return iter->second.location;
    }
    return 0;
}

void Analyzer::printSymbolTable()
{
    for(auto & i : symbol_table_)
    {
        std::cout << i.second.name << " >>> location: " << i.second.location << "\n";
        for(auto & j : i.second.lines)
        {
            std::cout << "\tline: " << j+1 << "\n";
        }
    }
    std::cout << std::endl;
}
