#include <filesystem>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

std::string readFile(std::filesystem::path const &filePath)
{
    std::ifstream inFile(filePath);

    if (!inFile.is_open())
    {
        throw std::runtime_error("Error: Could not open file " + filePath.string());
    }

    if (filePath.extension() != ".b" && filePath.extension() != ".bf")
    {
        throw std::runtime_error("Error: File type must be .b or .bf");
    }

    std::string instructions;

    char c;
    while (inFile.get(c))
    {
        if (std::string("><+-.,[]").find(c) != std::string::npos)
        {
            instructions.push_back(c);
        }
    }

    return instructions;
}

void interpret(std::string const &instructions, char &lastPrinted)
{
    std::vector<char> data(30000, 0);
    auto p_data = data.begin();
    auto p_instr = instructions.begin();
    std::stack<decltype(p_instr)> stack;

    while (p_instr != instructions.end())
    {
        switch (*p_instr)
        {
        case '>':
            p_data++;
            if (p_data == data.end())
            {
                data.push_back(0);
                p_data = --data.end();
            }
            break;
        case '<':
            p_data--;
            if (p_data < data.begin())
            {
                throw std::runtime_error("Error: Invalid data pointer position");
            }
            break;
        case '+':
            (*p_data)++;
            break;
        case '-':
            (*p_data)--;
            break;
        case '.':
            lastPrinted = *p_data;
            std::cout << *p_data;
            break;
        case ',':
            std::cin.get(*p_data);
            break;
        case '[':
            if (*p_data == 0)
            {
                size_t numOpen = 0;
                while (++p_instr != instructions.end())
                {
                    if (*p_instr == '[')
                    {
                        numOpen++;
                    }
                    else if (*p_instr == ']')
                    {
                        if (numOpen == 0)
                        {
                            break;
                        }
                        else
                        {
                            numOpen--;
                        }
                    }
                }
                if (p_instr == instructions.end())
                {
                    throw std::runtime_error("Error: Could not find matching ]");
                }
            }
            else
            {
                stack.push(p_instr);
            }
            break;
        case ']':
            if (*p_data != 0)
            {
                if (stack.empty())
                {
                    throw std::runtime_error("Error: Could not find matching [");
                }
                p_instr = stack.top();
            }
            else
            {
                stack.pop();
            }
            break;
        }
        p_instr++;
    }

    if (!stack.empty())
    {
        throw std::runtime_error("Error: Could not find matching ]");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: bf input_file" << std::endl;
        return EXIT_FAILURE;
    }

    char lastPrinted = '\n';

    try
    {
        interpret(readFile(argv[1]), lastPrinted);
    }
    catch (std::exception &err)
    {
        if (lastPrinted != '\n')
        {
            std::cerr << '\n';
        }
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
