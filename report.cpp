#include <string>
#include <iostream>

#include "common.h"

using namespace std;

void report_error(string message)
{
    cout << "error: " << message << endl;
}

void report_warning(string message)
{
    cout << "warning: " << message << endl;
}

void report_cur_line()
{
    cout << cur_line_at << ": " << cur_line << endl;
}

