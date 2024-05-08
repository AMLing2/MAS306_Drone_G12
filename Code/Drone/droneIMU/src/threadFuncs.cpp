#include "arenaHeader.h"

void ClientClass::recvThread()
{

}

void ClientClass::sendThread() 
{
    std::cout<<"drone sendThread up"<<std::endl;
    setTimeout(5,0);
    int i = 0;
    while(threadloop)
    {
        sleeptoInterval_(sendInterval_);
        //this whole section is wierd and kind of bad to be honest, works well in python but maybe not so well when ported to c++
        //TODO: redo
        try
        {
            if (sendQueue.size() >= 1) //TODO: ideally should be blocking with a timeout
            {
                readingQueue = true; //prevents string from being deleted while it is wanted
                clientSend(sendQueue.front().c_str(),sendQueue.front().size());
                sendQueue.pop();
                readingQueue = false;
                //i = 0;
            }
        }
        catch(const std::exception& e) //this wont catch like desired because no timeout from blocking...
        {
            std::cerr << e.what() << '\n';
            threadloop = false;
        }
        i++;
        if (i > 5) //TODO: temporary and kinda bad exit method but will "timeout" after 5 x n interval ms
        {
            threadloop = false;
        }
    }
    std::cout<<"drone sendThread done"<<std::endl;
}