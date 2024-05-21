#include "arenaHeader.h"

void ClientClass::recvThread()
{
    std::cout<<"drone recv Thread up"<<std::endl;
    dronePosVec::droneControl dc;
    waitForStartSignal_(true);
    std::cout<<"drone recv Thread start"<<std::endl;

    setTimeout(5,0);
    ssize_t msglen = 0; //needs to be signed
    while(threadloop)
    {
        sleeptoInterval_(sendInterval_);
        try
        {
            msglen = clientRecv(recvMsg_,bufferSize_);
            if ( (msglen == -1) || ((msglen <= 2) & ((recvMsg_[0] == '\0') || (recvMsg_[0] == '\4'))))//msg of size 1 (0 sent) or -1 might be sent in case of error
            {
                threadloop = false;
                break;
            }
            else
            {
                dc.Clear();
                dc.ParseFromArray(recvMsg_,msglen);
                std::cout<<"msg recieved: "<<dc.motorfl()<<"\r"; //TEMP
            }
            //std::cout<<"msg recieved: "<<std::string(recvMsg_,msglen)<<"\r"; //TEMP
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            threadloop = false;
            break;
        }
    }
    std::cout<<"drone recvThread done"<<std::endl;
}

void ClientClass::sendThread() 
{
    std::cout<<"drone send Thread up"<<std::endl;

    waitForStartSignal_(false);
    std::cout<<"drone sendThread start"<<std::endl;
    setTimeout(5,0);
    std::string msg;
    while(threadloop)
    {
        sleeptoInterval_(sendInterval_);
        try
        {
            readingQueue = true; //prevents string from being deleted while it is wanted
            blockingGetQueue_(sendQueue,msg,5000);
            clientSend(msg.c_str(),msg.size());
            sendQueue.pop();
            readingQueue = false;
        }
        catch(const std::exception& e)
        {
            std::cout<<"error"<<std::endl;
            std::cerr << e.what() << '\n';
            threadloop = false;
            break;
        }
    }
    std::cout<<"drone sendThread done"<<std::endl;
}