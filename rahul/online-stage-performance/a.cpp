#include "header.h"

int gpid(char s[]){
    char temp[100]="pidof ./";
    strcat(temp,s);
    int fd = fileno(popen(temp, "r"));
    char s1[100];
    read(fd, &s1, 100);
    int pid = atoi(s1);
    return pid;
}
int flag=0;
ACE_SOCK_Stream sfd[3];
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

    // ACE_DEBUG ((LM_INFO, ACE_TEXT ("%S occukbhbjhbjkbrred\n"), signum));
    
    srand(time(0));
    int r = rand()%3;
    char s[100]="j",temp[100];
    sprintf(temp,"%d",r+1);
    strcat(s,temp);
    ACE_OS::kill(gpid(s),SIGUSR1);
    for(int i=0;i<3;i++){
        strcpy(s,"j");
        sprintf(temp,"%d",i+1);
        strcat(s,temp);
        ACE_OS::kill(gpid(s),SIGUSR2);
    }
    int score=0;
    for(int i=0;i<3;i++){
        char buff[100];
        int sz = sfd[i].recv_n(buff,100);
        buff[sz] = '\0';
        score+=atoi(buff);
    }
    printf("Score from all the judges is: %d\n",score);
    return 0;
  }

private:
  int signum_;
};

int max(int a,int b){
    return a>b?a:b;
}
int main(){
    MySignalHandler h (SIGUSR1);
    ACE_Sig_Handler handler;
    handler.register_handler (SIGUSR1, &h);

    ACE_INET_Addr addr(8082, INADDR_LOOPBACK);
	

	ACE_SOCK_Acceptor acc[3];
    for(int i=0;i<3;i++){
        acc[i]=ACE_SOCK_Acceptor(addr);
        acc[i].accept(sfd[i]);
    }
    while(1);
    
    
    return 0;
}