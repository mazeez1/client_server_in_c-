
//Moses Azeez
//mma348 
//09-19-19
//Sources 
//http://www.cplusplus.com/forum/general/248809/
//https://www.geeksforgeeks.org/socket-programming-cc/
//https://www.thecrazyprogrammer.com/2017/06/socket-programming.html

#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <cmath>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int main(int argc, char const *argv[])
{
    
    cout<<"Server Port is open...Waiting for client \n\n";

    struct hostent *host_name; // Create a list of ports
    int sock_server = 0;    // initate a socket creation
    sock_server = socket(AF_INET,SOCK_STREAM,0);
    int port_num = atoi(argv[1]); //get the port from terminal 

    //Each port is allowed to be between 1024 and 65535 (must be aggreed on)
    srand(time(NULL));
    int port_rand =0;
    port_rand = rand()%64511+1024; 
  

    struct sockaddr_in server_address;    //specify the address for the socket
    //We are using IPv4 for this implemenation.
    server_address.sin_family = AF_INET;  
    //We are finding a port number from the addresses given 
    server_address.sin_port = htons(port_num); 
   // The host is local host.  If we do not recieve this input, return wrong 
    host_name = gethostbyname("localhost"); //set the host 

    //b copy is used to move between source and destination.
    bcopy((char *)host_name->h_addr,(char *)&server_address.sin_addr.s_addr,host_name->h_length); 
    //Part 2: Bind a socket to port. 
    int BinderTCP;  //Initate a binder for our port. 
    BinderTCP = bind(sock_server,(struct sockaddr*) &server_address,sizeof(server_address));

    //Listen for handshake 
    listen(sock_server,5);

    //create a socket to connect to
    int sock_client;
    sock_client = accept(sock_server,NULL,NULL);

    if(sock_client>=0)
    { 
        cout<<"Handshake Detected. Selected the random port number: "<<port_rand<<"\n\n";
    }
   

    int carry2;
    int carry1;
    int msg;

    carry1 = recv(sock_client, &msg, sizeof(msg),0);
    carry2 = send(sock_client,&port_rand,sizeof(port_rand),0);
    //(5)Close the TCP socket
    close(sock_server);

    //Stage 2 using UDP sockets.  Set them up

     int UDPsocket,addr_length;
     int length_1;
     struct sockaddr_in server_address2;
     struct sockaddr_in src_adrr;
     socklen_t src_addr_len; 
     
    //Begin by creating a UDP socket
    UDPsocket = socket(AF_INET,SOCK_DGRAM,0);
    //if(UDPsocket<0){ cout<<"error on create socket\n"; }

    addr_length = sizeof(server_address2);  //Check if size matches each other
    bzero(&server_address2,addr_length);      //Zero the server so the message can be uninterupted 

    //This specifies the address for the socket.  The IPv4 family once again.
    server_address2.sin_family = AF_INET; 

    server_address2.sin_port = htons(port_rand); 

    host_name = gethostbyname("localhost"); //set the host 
    if(host_name<0){cout<<"Unknown host\n";}
    bcopy((char *)host_name->h_addr,(char *)&server_address.sin_addr.s_addr,host_name->h_length); //copy from src to dest

    //Use the UDP method of socket binding 
    int BinderUDP;
    BinderUDP = bind(UDPsocket,(struct sockaddr*)&server_address2,addr_length);

    src_addr_len = sizeof(struct sockaddr_in);
    int num=0;
    const int buffer_size = 4; // was a const
    int number = 0; 
    char buffer[buffer_size];
    char buffer_cap[buffer_size]; 
    bool check = true;
    
    // Begin by creating a file for transmission
    string st = "";
    FILE *file_path;
    file_path = fopen("dataRecieved.txt","w");
    
    bzero(buffer,buffer_size);// clear the buffer

  // receiving chunk & Captalization & Sending ACK
    while(check == true) 
    {
        length_1 = recvfrom(UDPsocket,buffer,buffer_size,0,(struct sockaddr *)&src_adrr,&src_addr_len); //receive chunks from client

        while(number!=4){
            buffer_cap[number]=toupper(buffer[number]);
            st = st+buffer[number];
            number++;
        }
        number = 0;

        for(int i=0; i<4; i++)
        {
            if(buffer_cap[i+1]=='#')
            {
                check = false; // to stop send & receiving 
                buffer_cap[i+1]='^';
                length_1 = sendto(UDPsocket,buffer_cap,buffer_size,0,(struct sockaddr *)&src_adrr,src_addr_len);
                goto lable;
            }  
        }
        length_1 = sendto(UDPsocket,buffer_cap,buffer_size,0,(struct sockaddr *)&src_adrr,src_addr_len);
        num++;
    }

    lable: 
    string file_String="";
    for(int i=0; i < st.length()-1; i++) //take off the '#' sign
    {
       file_String = file_String +st[i];
       if(st[i+1]=='#')
       {
           break;
       }
        
    }
    //Clean the path and close the socket
    fprintf(file_path,"%s",file_String.c_str());
    fclose(file_path); 
    close(UDPsocket); 

    return 0;
}
