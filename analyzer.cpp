#include "analyzer.h"
#include "treenode.h"
#include <iostream>

Analyzer::Analyzer(TreeNode * t)
    : root_(t)
    , location_(0)
{
}

void Analyzer::type_check()
{
    type_check(root_);
}

void Analyzer::build_symbol_table()
{
    build_symbol_table(root_);
}

void Analyzer::type_check(TreeNode * t)
{
    traverse(t, [this](TreeNode *)
    {
    },
    [this](TreeNode * t)
    {
        check_node(t);
    });
}

void Analyzer::build_symbol_table(TreeNode *t)
{
    traverse(root_, [this](TreeNode *t)
    {
        switch (t->kind)
        {
        case StmtK:
            switch (t->stmt_kind)
            {
            case AssignK:
            case ReadK:
                insert_symbol(t);
                break;
            default:
                break;
            }
            break;
        case ExprK:
            switch (t->expr_kind)
            {
            case IdK:
                insert_symbol(t);
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

    
void Analyzer::check_node(TreeNode * t)
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
                type_check_error(t->child, "if test is not Boolean");
            break;
        case RepeatK:
            if(t->child->sibling->expr_type != Boolean)
                type_check_error(t->child, "repeat test is not Boolean");
            break;
        case AssignK:
            if(t->child->expr_type != Integer)
                type_check_error(t->child, "assignment of non-integer value");
            break;
        case WriteK:
            if(t->child->expr_type != Integer)
                type_check_error(t->child, "write of non-integer value");
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void Analyzer::type_check_error(TreeNode * t, const char * err)
{
    std::cout << "line " << t->line_no << " >>>> " << err <<std::endl;
    throw;
}

void Analyzer::insert_symbol(TreeNode * t)
{
    auto it = symbol_table_.find(t->name);
    if(it == symbol_table_.end())
    {
        symbol_table_[t->name] = std::list<Symbol>{ Symbol{ t->line_no, t->name, ++location_} };
    }
    else
    {
        it->second.push_back(Symbol{ t->line_no, t->name, 0});
    }
}

size_t Analyzer::find_symbol(const std::string & name)
{
    auto iter = symbol_table_.find(name);
    if(iter != symbol_table_.end() && !iter->second.empty())
    {
        return iter->second.front().location;
    }
    return 0;
}

void Analyzer::print_symbol_table()
{
    for(auto & i : symbol_table_)
    {
        for(auto & j : i.second)
        {
            std::cout << j.name << " >>> location: " << j.location << ", line: " << j.line_no << std::endl;
        }
    }
}
