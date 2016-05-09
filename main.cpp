#include <iostream>
#include <fstream>

#include "scanner.h"

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

}

int main(int argc, char *argv[])
{
    test::scanner_test("/home/zwk/loucomp/mytiny/sample.tny");
}
