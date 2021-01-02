#include <iostream>
#include <SQLAPI.h>

using namespace std;

int main(int argc, char* argv[])
{
    SAConnection con; // connection object

    try{
        con.Connect(_TSA("oracle0.ugr.es:1521/practbd.oracle0.ugr.es"),
                _TSA("x7149055"), 
                _TSA("x7149055"), 
                SA_Oracle_Client);
        
        cout << "Connected" << endl;
    }
    catch(SAException &x)
    {
        // SAConnection::Rollback()
        // can also throw an exception
        // (if a network error for example),
        // we will be ready
        try
        {
            // on error rollback changes
            con.Rollback();
        }
        catch(SAException &)
        {
        }
        // print error message
        printf("%s\n", x.ErrText().GetMultiByteChars());  
    }
   
   return 0;
}