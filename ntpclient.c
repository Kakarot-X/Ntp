#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define NTP_TIMESTAMP_DELTA 2208988800ull

int main(){

char *host_name="us.pool.ntp.org";
  
typedef struct
{

  uint8_t leapI_versionN_mode;      // 8 bits. leap indicater, version number, and mode.
  
  uint8_t stratum;      
  uint8_t poll;          
  uint8_t precision;      

  uint32_t rootDelay;      
  uint32_t rootDispersion;
  uint32_t refId;          

  uint32_t refTm_sec;        
  uint32_t refTm_fsec;        

  uint32_t originateTm_sec;       
  uint32_t originateTm_fsec;       

  uint32_t recTm_sec;       
  uint32_t recTm_fsec;       

  uint32_t transTm_s;         
  uint32_t transTm_fsec;        

} NTP_PACKET;

NTP_PACKET packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

*((char *) &packet) =0x1b; // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.


struct sockaddr_in serv_addr;//server address data structure
struct hostent *server;//server data structure]


int sockfd =socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);//udp socket


if (sockfd<0){
 printf("SOCKET CREATION FAILED");
 exit(1);

}

server = gethostbyname(host_name);


if (server == NULL){
  printf("FAILED TO RETRIEVE HOST ADDRESS NO  SUCH HOST");
  exit(0);
}

memset(&serv_addr,0,sizeof(serv_addr)) ;

serv_addr.sin_family = AF_INET;

memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);


serv_addr.sin_port = htons(123);



if ( connect( sockfd, (struct sockaddr*) &serv_addr, sizeof(NTP_PACKET)  )<0 ){
  printf("ERROR CONNECTING ");
}

int n;
n  = write(sockfd,(char *)&packet,sizeof(NTP_PACKET));


if ( n < 0 ){
  printf( "failed to write data to socket" );
  exit(0);}


n = read(sockfd,(char *)&packet,sizeof(NTP_PACKET));


if ( n < 0 ){
  printf( "failed to read server message" );
  exit(0);}


packet.transTm_s = ntohl( packet.transTm_s );
packet.transTm_fsec = ntohl( packet.transTm_fsec );


time_t txTm = ( time_t ) ( packet.transTm_s - NTP_TIMESTAMP_DELTA );
printf( "Time: %s", ctime( ( const time_t* ) &txTm ) );


close(sockfd);
return 0;

}
