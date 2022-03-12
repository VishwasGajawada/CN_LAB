#include "header.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Log_Msg.h"
#include "ace/Signal.h"
#include "ace/Sig_Handler.h"

int flag=0;
ACE_SOCK_Stream sfd1;
class MySignalHandler : public ACE_Event_Handler
{
public:
  MySignalHandler (int signum) : signum_(signum)
  { }

  virtual ~MySignalHandler()
  { }

  virtual int handle_signal (int signum, siginfo_t * = 0, ucontext_t * = 0)
  {
    // ACE_TRACE ("MySignalHandler::handle_signal");

    // // Make sure the right handler was called back.
    // ACE_ASSERT (signum == this->signum_);
        printf("Signal received from anchor\n");
    ACE_INET_Addr addr(8081, INADDR_LOOPBACK);
    ACE_SOCK_Stream sfd;
    ACE_SOCK_Connector con;

    
    switch (signum)
    {
    case SIGUSR1:{
        con.connect(sfd, addr);
        char buff[100]="J2's question: What is your name??\n";
        // ACE_DEBUG ((LM_INFO, ACE_TEXT ("%S occurred\n"), signum));
        sfd.send_n(buff,100);
        flag=1;
        break;
    }
    case SIGUSR2:{
        srand(0);
        int r = rand()%10;
        char buff[100];
        sprintf(buff,"%d\n",r);
        printf("score :%s\n",buff);
        sfd1.send_n(buff,100);
        break;
    }
    }
    return 0;
  }

private:
  int signum_;
};

int main(){
    MySignalHandler h1 (SIGUSR1), h2 (SIGUSR2);
    ACE_Sig_Handler handler;
    handler.register_handler (SIGUSR1, &h1);
    handler.register_handler (SIGUSR2, &h2);

    ACE_INET_Addr addr(8082, INADDR_LOOPBACK);
	ACE_SOCK_Connector con;

	con.connect(sfd1, addr);

    while(true);

    return 0;
}