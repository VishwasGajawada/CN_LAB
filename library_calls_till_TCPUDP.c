HEADER FILES
--------------------------------------------------------------------------------------------------------------------------------

#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>

 

				SHARED MEMORY
-----------------------------------------------------------------------------------------------------------------------------------

	int state=1;
	key_t h=ftok(".",state++);	// value of state should on every program where this share memory is used
 	int shmid=shmget(h,sizeof(int),IPC_CREAT|0666);
 	share_memory=shmat(shmid,(const void*)0,0);
 	
 	
 				SEMAPHORE
-----------------------------------------------------------------------------------------------------------------------------------
 	
	void sem_wait(int semid)
	{
		struct sembuf sb;
		sb.sem_num=0;
		sb.sem_op=-1;
		sb.sem_flg=0;
		if((semop(semid,&sb,1))==-1)
		{
			perror("\nFailed to acquire semaphore.");
			exit(0);
		}
	}

	void sem_try_wait(int semid)
	{
		struct sembuf sb;
		sb.sem_num=0;
		sb.sem_op=-1;
		sb.sem_flg=IPC_NOWAIT;;
		return semop(semid,&sb,1);
	}

	void sem_signal(int semid)
	{
		struct sembuf sb;
		sb.sem_num=0;
		sb.sem_op=1;
		sb.sem_flg=0;
		if((semop(semid,&sb,1))==-1)
		{
			perror("\nFailed to release semaphore.");
			exit(0);
		}
	}

	int state=1;
	key_t h=ftok(".",state++);	// value of state should on every program where this semaphore is used
	int sem_id;
	if((sem_id=semget(h,1,0666|IPC_CREAT))==-1)
 	{
		printf("error in creation semaphore\n");
		exit(0);
	}
	
	int semaphore_value=1;

	if((semctl(sem_id,0,SETVAL,semaphore_value))==-1)
	{
		printf("error to set value\n");
	}
	
	(OR)
	
#define sname "/mysem"

sem_t *sem = sem_open(sname, O_CREAT, 0644, 0);
sem_t *sem = sem_open(sname,1);

sem_wait(sem);
sem_post(sem);

----------------------------------------------------------------------------------------------------------------------------------
				MSG QUEUE
----------------------------------------------------------------------------------------------------------------------------------
	struct mymsg
	{
		long type;
		char msg[20];
	};

	struct mymsg msg1;
	key_t  key;
	int    mqpid;
	int    ret;
	int    len;
	system("touch f1.txt");
	if((key=ftok("f1.txt",'B')) == -1)
	{
		perror("key");
		exit(1);
	}
	if((mqpid=msgget(key,0644|IPC_CREAT))==-1)
	{
		perror("Key");
		exit(1);
	}
	
	if(msgsnd( mqpid ,&msg1 ,len+1 , 0) == -1)
	{
		perror("msgsnd");
		exit(1);
	}

	memset(msg1.msg,'\0',sizeof(msg1.msg));
	if(msgrcv( mqpid , &msg1 , sizeof(msg1.msg),1 ,0) == -1)
	{
		perror("msgrcv");
		exit(1);
	}	
----------------------------------------------------------------------------------------------------------------------------------
				FIFO
----------------------------------------------------------------------------------------------------------------------------------
	char name[50];
	if(mkfifo(name,0666)==-1)
	{
		perror("mkfifo()1");
		exit(1);
	}
	if((wfd=open("./wellknownfifo",O_WRONLY))==-1)
	{
		perror("open()");
		exit(1);
	}


	write(wfd,buffer,sizeof(buffer));

	
	char buffer[50];
	if(mkfifo("./wellknownfifo",0666)==-1)
	{
		perror("mkfifo()");
		exit(1);
	}
	if((rfd=open("./wellknownfifo",O_RDONLY))==-1)
	{
		perror("open()");
		exit(1);
	}

	read(rfd,buffer,50);

 
				pthread
----------------------------------------------------------------------------------------------------------------------------------
 	
 	void do_thread_service(void *arg)
	{
		int *args= (int*)arg ;
 	
	}
	
	pthread_t t_service;
 	if(pthread_create(&t_service,NULL,(void*)&do_thread_service ,(void*)args)!=0)
	perror("\npthread_create ");
 	



		           CONNECTION ORIENTED SERVER	( usage -:  "./a.out port_no")
---------------------------------------------------------------------------------------------------------------------------------
	if(argc!=2)
	printf("\n usage ./a.out port_no");

	int sfd;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;
	int port_no=atoi(argv[1]);

	if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	int opt=1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("\n bind successful ");

	listen(sfd,10);

	cli_len=sizeof(cli_addr);
	int nsfd;
	if((nsfd = accept(sfd , (struct sockaddr *)&cli_addr , &cli_len))==-1)
	perror("\n accept ");
	else printf("\n accept successful");
	//break after exec in child


		          CONNECTION ORIENTED CLIENT	( usage -:  "./a.out port_no")
---------------------------------------------------------------------------------------------------------------------------------

	if(argc!=2)
	printf("\n usage ./a.out port_no");

	int sfd;
	struct sockaddr_in serv_addr;
	int port_no=atoi(argv[1]);

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_STREAM , 0))==-1)
	perror("\n socket");
	else printf("\n socket created successfully\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	//serv_addr.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET,"127.0.0.1", &serv_addr.sin_addr);

	if(connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr))==-1)
	perror("\n connect : ");
	else printf("\nconnect succesful");



			           CONNECTION LESS SERVER	( usage -:  "./a.out port_no")
---------------------------------------------------------------------------------------------------------------------------------
	if(argc!=2)
	printf("\n usage ./a.out port_no");

	int sfd;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;
	int port_no=atoi(argv[1]);

	if((sfd = socket(AF_INET,SOCK_DGRAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("\n bind successful ");

	cli_len = sizeof(cli_addr);

	fgets( buffer , 256 , stdin );
	sendto(sfd , buffer , 256 , 0 , ( struct sockaddr * ) &cli_addr ,  cli_len);
	recvfrom(sfd , buffer , 256 , 0 , ( struct sockaddr * ) &cli_addr , & cli_len );

		          CONNECTION LESS CLIENT	( usage -:  "./a.out port_no")
---------------------------------------------------------------------------------------------------------------------------------

	if(argc!=2)
	printf("\n usage ./a.out port_no");

	int sfd;
	struct sockaddr_in serv_addr;
	int port_no=atoi(argv[1]);
	char buffer[256];

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_DGRAM , 0))==-1)
	perror("\n socket");
	else printf("\n socket created successfully\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	socklen_t serv_len = sizeof(serv_addr);
	
	fgets( buffer , 256 , stdin );
	sendto(sfd , buffer , 256 , 0 , ( struct sockaddr * ) &serv_addr ,  serv_len);
	recvfrom(sfd , buffer , 256 , 0 , ( struct sockaddr * ) &serv_addr , & serv_len );

		            
	

				GETPEERNAME (usage: only after accept; only on nsfd)
---------------------------------------------------------------------------------------------------------------------------------

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <stdio.h>


{
   int s;
   struct sockaddr_in peer;
   int peer_len;
   
   peer_len = sizeof(peer);
     
   if (getpeername(s, &peer, &peer_len) == -1) {
      perror("getpeername() failed");
      return -1;
   }

      /* Print it.    */
   printf("Peer's IP address is: %s\n", inet_ntoa(peer.sin_addr));
   printf("Peer's port is: %d\n", (int) ntohs(peer.sin_port));
   
}

PASSING ARGUMENTS THROUGH EXEC
-------------------------------------------------------------------------------------------------------------------------------
string msg;
char **arg=new char*[2];
arg[0]=strdup(msg.c_str());
arg[1]=NULL;
int c=fork();
if(c>0);
else if(c==0)
{
	if(execvp("./s",arg)==-1)
	cout<<"eroor"<<endl;
	exit(1);
}


//retrieving in child

int main(int argc, char const *argv[])
{
	string info=argv[argc];
}


 MKFIFO
 ------------------------------------------------------------------------------------------------------------------------------
 
 #include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

  int fd;
  mkfifo("fifo1.fifo",0666);
  fd=open("./fifo1.fifo",O_RDONLY);
             
             POLL
--------------------------------------------------------------------------------------------------------------------------------
	int size;
	struct pollfd fds[size];
	fds[i]=open(" ", 0666);
	fds[i].events=POLLIN;
	
	int ret=poll(fds, size, timeout);
	if(fds[i].revents & POLLIN) 
	{
	
	}
	
				 
To know pid of a program by knowing its name

 
int fd = fileno(popen("pidof ./S", "r"));	
char s[1000];
	read(fd, &s, 1000);
	X = atoi(s);	
int fd = fileno(popen("pidof ./P2.exe", "r"));
	char s[1000];
	read(fd, &s, 1000);
	X = atoi(s);

 
