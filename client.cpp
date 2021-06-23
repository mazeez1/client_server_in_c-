//Moses Azeez
//mma348 
//09-19-19
//Sources 
//http://www.cplusplus.com/forum/general/248809/
//https://www.geeksforgeeks.org/socket-programming-cc/
//https://www.thecrazyprogrammer.com/2017/06/socket-programming.html
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

int main(int argc, char *argv[]) 
{

	//1 Create and open the TCP port.  
    struct hostent *host1;
	int sock_network = 0; 
	sock_network = socket(AF_INET,SOCK_STREAM,0); 
    int port_number = atoi(argv[2]);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port_number); 
    host1 = gethostbyname(argv[1]); 
 
    bcopy((char *)host1->h_addr,(char *)&server_address.sin_addr.s_addr,host1->h_length); 
    //(2)connect to the server 
    int check;
    check = connect(sock_network,(struct sockaddr *) &server_address,sizeof(server_address));

    // sends 117 over to the server for the handshake
    int var = 117,carry1;
    carry1 = send(sock_network,&var,sizeof(var),0);
    
    //recieve data from server
    int port_rand,carry2;
    carry2 = recv(sock_network, &port_rand, sizeof(port_rand),0);  //receive data from server

    //show the choosen random port.  
    cout<<"Random Port number: "<< port_rand<<"\n";

    //close the TCP socket
    close(sock_network);

	//Begin the UDP process
    int UDPsocket, addr_length, n;
    struct sockaddr_in server_address2;
    struct sockaddr_in  src_addr;
    socklen_t src_addr_len;
    UDPsocket = socket(AF_INET,SOCK_DGRAM,0);

    //struct hostent *host1;

    //for file reading
    char fileName[] = "./file";
    ifstream my_file (argv[3]);
    int count = 0;
   
    
    //specify the address for the socket
    server_address2.sin_family = AF_INET;  
    server_address2.sin_port = htons(port_rand);  

    host1 = gethostbyname(argv[1]);  
    bcopy((char *)host1->h_addr,(char *)&server_address2.sin_addr.s_addr,host1->h_length);  

    addr_length = sizeof(struct sockaddr_in);
    src_addr_len = sizeof(struct sockaddr_in);

    const int size_buffer = 4;
    char buffer[size_buffer];

  
    string my_string; // Begin setting up the file for reading 
    string follow_str ="";
    if(my_file){

        while(getline(my_file,my_string)){   
            follow_str=follow_str+my_string;
        }
        while(follow_str[count]!='\0') {
            count++;
        }
        int size = count+1;
        char array[size];

        for(int i = 0; i<count; i++)  //go through each bit
        {
            array[i] = follow_str[i];
        }
        array[count]='#';  
        
        int number=0;
        bzero(buffer,size_buffer); 

        for(int i=0; i<size;)
        {
        	//continues checking for file btyes 
            while(number !=4 && array[i-1]!='#')  
            {  
                buffer[number]=buffer[number]+array[i];
                number++;
                i++;
            }

            n = sendto(UDPsocket,buffer,strlen(buffer),0,(struct sockaddr *)&server_address2,addr_length); //send the chunk to the server 
            bzero(buffer,size_buffer);// clear the buffer
            n = recvfrom(UDPsocket,buffer,size_buffer,0,NULL,NULL); // receive Cap chunk from the server  

            //check the end of receving session and close
            for(int i=0; i<size_buffer;i++) 
            {
                if(buffer[i] =='^')
                {
                    cout<<endl;
                    goto lable;
                }
                cout<<buffer[i];
            }
            cout<<endl;
           //stops checking, the buffer is completed
            number = 0;  
            bzero(buffer,size_buffer);    
        }
        lable:
        my_file.close(); // close the file (the btyes have ended)
    }

    //Clean the path and close the socket
    close(UDPsocket);    
    return 0;
}
