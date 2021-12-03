#include "Protocol5.h"
#include <iostream>
#include <omp.h>
#include <string>
// using namespace std;
int main()
{
    Physical_Layer pl; /*initiate Physical Layer*/
    Protocol5 s(&pl);  /*initiate sender instance*/
    Protocol5 r(&pl);  /*initiate receiver instance*/
    std::string message = "";
    printf("Enter Message to send: \n");
    std::getline(std::cin, message);
    s.send_message(message);

#pragma omp parallel num_threads(2)
    {
        if (omp_get_thread_num() == 1)
        {
            r.name = "rec";
            r.Start();
        }
        if (omp_get_thread_num() == 0)
        {
            s.name = "sender";
            s.Start();
        }
    }

    return 0;
}