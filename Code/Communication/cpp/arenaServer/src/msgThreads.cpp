#include "dronePosVec.pb.h"
#include "arenaServer.h"
#include <thread>
#include <queue>
#include <cerrno>
/*
GENERIC THREAD RECV FUNCTION, OVERRIDE IF NEEDED
*/
void AbMessenger::recvThread()
{
    std::cout<<strName<<" recv thread up"<<std::endl;

    //dont start until main thread sends signal to start threads
    waitforProgStart_();

    dronePosVec::dronePosition data;
    setTimeout();
    setTimeout(5,0);
    ssize_t msgsize = 0;
    char errorStr[] = {'\0'};
    while(threadloop_)
    {
        data.Clear();
        msgsize = clientRecv(recvMsg_,bufferSize_);
        if (msgsize > 0)
        {            
            try
            {
                data.ParseFromArray(recvMsg_,msgsize);
                //std::cout<<strName<<" msg recieved: "<<std::string(recvMsg_,msgsize)<<"\r"; //TEMP
                appendToQueue_(*pq1,std::string(recvMsg_,msgsize));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n'; //incorrect message, possibly caused by port scanning or error on other process, thread should continue
                appendToQueue_(*pq1,std::string(errorStr,1)); //append empty string
            }
        }
        else
        {
            std::cout<<strName<<" recv fail, errno:"<<strerror(errno)<<": "<<errno<<std::endl;
            threadloop_ = false;
            break;
        }
        sleeptoInterval_(recvInterval_);
    }
    std::cout<<strName<<" recv thread ready to join"<<std::endl;
}

void AbMessenger::sendThread()
{
    std::cout<<strName<<" send Thread up"<<std::endl;

    //dont start until main thread sends signal to start threads
    waitforProgStart_();

    dronePosVec::dronePosition data;
    int r = 0;
    std::string msg;
    while(threadloop_)
    {
        //data.Clear();
        r = blockingGetQueue_(*pq2,msg,5000);
        if (r == -1) //timeout from blocking queue
        {
            std::cout<<strName<<" timeout from queue get,"<< " rval: "<<r<<std::endl;
            threadloop_ = false;
            break;
        }
        else if (msg[0] == '\0')
        {
            //will happen if any wrong messages get passed through, just ignore for now
            std::cout<<strName<<" incorrect message recieved from queue,"<< " rval: "<<r<<std::endl;
            pq2->pop();
        }
        else
        {
            //std::cout<<strName<<" msg sent: "<<msg<<std::endl; //TEMP
            clientSend(msg.c_str(),msg.size());
            pq2->pop();
        }
        sleeptoInterval_(sendInterval_);
    }
    std::cout<<strName<<" send Thread ready to join"<<std::endl;
}

/*
    SPECIFIC THREAD FUNCTIONS:
*/
/*
//CAMERA --------------------------------------------------------------------------------
void CameraMessenger::recvThread()
{
    std::cout<<"Camera recvThread up"<<std::endl;
    dronePosVec::dronePosition data; 
    setTimeout(5,0);//too high for 100ms interval
    ssize_t msgsize = 0;
    while(threadloop_)
    {
        data.Clear();
        msgsize = clientRecv(recvMsg_,bufferSize_);
        if (msgsize > 0)
        {
            data.ParseFromArray(recvMsg_,msgsize);
            appendToQueue_(q,std::string(recvMsg_,msgsize));
            std::cout<<data.rotation().size()<<std::endl; //TODO: test, remove
            if (data.rotation().size() > 0)
            {
                std::cout<<data.rotation().Get(0)<<std::endl; //TODO: test, remove
            }
            
            //q.push(data.SerializeAsString()); //uncomment
        }
        else
        {
            std::cout<<"errno:"<<strerror(errno)<<": "<<errno<<std::endl;
            threadloop_ = false; //this will end the sending thread too
            break;
        }
        sleeptoInterval_(recvInterval_);
    }
    std::cout<<"Camera recvThread ready to join"<<std::endl;
}

void CameraMessenger::sendThread(){ } 

//ESTIMATOR --------------------------------------------------------------------------------
void EstimatorMessenger::recvThread(){ }

void EstimatorMessenger::sendThread()
{
    std::cout<<"estimator sendThread up"<<std::endl;
    //dronePosVec::dataTransfers data; 
    int r = 0;
    std::string msg;
    while(threadloop_)
    {
        r = blockingGetQueue_(q2,msg,5);
        if (r == 0)
        {
            clientSend(msg.c_str(),msg.size());
            q2.pop();
        }
        else //timeout from blocking queue
        {
            std::cout<<"Estimator send fail, errno:"<<strerror(errno)<<" : "<<errno<< " rval: "<<r<<std::endl;
            threadloop_ = false;
            break;
        }

        // comment later if needed
        sleeptoInterval_(sendInterval_);
        if(q.empty() != true)
        {
            msg = q.front();
            clientSend(msg.c_str(),msg.length());
            q.pop();
        }
        
    }
    std::cout<<"estimator sendThread ready to join"<<std::endl;
}

//DRONE --------------------------------------------------------------------------------

void DroneMessenger::recvThread()
{
    std::cout<<"drone recvThread up"<<std::endl;
    dronePosVec::dronePosition data; 
    setTimeout(5,0);//too high for 100ms interval
    ssize_t msgsize = 0;
    while(threadloop_)
    {
        data.Clear();
        msgsize = clientRecv(recvMsg_,bufferSize_);
        if (msgsize > 0)
        {
            data.ParseFromArray(recvMsg_,msgsize);
            //std::cout<<"rotmatrix[1]: "<<data.rotmatrix()<<std::endl;
            std::cout<<data.rotation().size()<<std::endl; //TODO: test, remove
            if (data.rotation().size() > 0)
            {
                std::cout<<data.rotation().Get(0)<<std::endl; //TODO: test, remove
            }
            
            //q.push(data.SerializeAsString()); //uncomment
        }
        else
        {
            std::cout<<"Drone recv fail, errno:"<<strerror(errno)<<": "<<errno<<std::endl;
            threadloop_ = false;
            break;
        }
        sleeptoInterval_(recvInterval_);
    }
    std::cout<<"drone recvThread ready to join"<<std::endl;
}

void DroneMessenger::sendThread()
{

}
// --------------------------------------------------RL ---------------------------------------------------------------

void RLMessenger::recvThread()
{
    //q = motor info to drone, q2 = data from camera
    std::cout<<"RL recvThread up"<<std::endl;
    dronePosVec::dronePosition data;
    setTimeout(5,0);//too high for 100ms interval
    ssize_t msgsize = 0;
    while(threadloop_)
    {
        data.Clear();
        msgsize = clientRecv(recvMsg_,bufferSize_);
        if (msgsize > 0)
        {            
            try
            {
                data.ParseFromArray(recvMsg_,msgsize);
                appendToQueue_(q,std::string(recvMsg_,msgsize));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n'; //incorrect message, possibly caused by port scanning or error on other process, thread should continue
                appendToQueue_(q,std::string(0)); //append empty string
            }
        }
        else
        {
            std::cout<<"RL recv fail, errno:"<<strerror(errno)<<": "<<errno<<std::endl;
            threadloop_ = false;
            break;
        }
        sleeptoInterval_(recvInterval_);
    }
    std::cout<<"RL recvThread ready to join"<<std::endl;
}

void RLMessenger::sendThread()
{
    //q = motor info to drone, q2 = data from camera
    std::cout<<"RL send Thread up"<<std::endl;
    dronePosVec::dronePosition data;
    int r = 0;
    std::string msg;
    while(threadloop_)
    {
        //data.Clear();
        r = blockingGetQueue_(q2,msg,5);
        if (r == 0)
        {
            clientSend(msg.c_str(),msg.size());
            q2.pop();
        }
        else //timeout from blocking queue
        {
            std::cout<<"RL send fail, errno:"<<strerror(errno)<<" : "<<errno<< " rval: "<<r<<std::endl;
            threadloop_ = false;
            break;
        }
        sleeptoInterval_(recvInterval_);
    }
    std::cout<<"RL send Thread ready to join"<<std::endl;
}

*/