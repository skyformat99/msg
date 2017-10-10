#include <QCoreApplication>

#include "iostream"
#include "msg.h"
Msg_Ncb_st *p = 0;
 int sendbytes(unsigned char * data,unsigned int len)
{
//     printf("sendbytes size is%d\r\n",len);
//     for(unsigned int i = 0; i < len; i++)
//         printf("%x\r\n",data[i]);
    msg_recv_data(p,data,len);
    return 1;
}
typedef struct
{
    float A;
    char B;
    long C;
} package1_st;
 typedef struct
 {
     int A;
     short B;
     double C;
 } package2_st;
 typedef struct
 {
     float A;
     short B;
     short C;
 } package3_st;
 typedef struct
 {
     int A;
     char B;
     float C;
 } package4_st;
int msg_1(void *p);
int msg_2(void *p);
int msg_3(void *p);
int msg_4(void *p);
int main(int argc, char *argv[])
{
    unsigned char id;
    int res;
    package1_st pack1;
    package2_st pack2;
    package3_st pack3;
    package4_st pack4;
    package1_st pack1_des;
    pack1.A = 1.1;
    pack2.C = 1.2;
    pack3.A = 1.3;
    pack4.C = 1.4;
    /*create one msg fs*/
    msg_create(&p,1000,sendbytes);
    printf("*pfs = :%d\r\n",p);
    /*subscribe the msg witrh the callback*/
    id = 1;
    res = msg_subscribe(p,id,MSG_MODE_CALLBACK,0,msg_1);
    printf("msg_subscribe res = %d, id = %d \r\n",res,id);
    id = 2;
    res = msg_subscribe(p,id,MSG_MODE_CALLBACK,0,msg_2);
    printf("msg_subscribe res = %d, id = %d \r\n",res,id);
    id = 3;
    res = msg_subscribe(p,id,MSG_MODE_CALLBACK,0,msg_3);
    printf("msg_subscribe res = %d, id = %d \r\n",res,id);
    id = 4;
    res = msg_subscribe(p,id,MSG_MODE_CALLBACK,0,msg_4);
    printf("msg_subscribe res = %d, id = %d \r\n",res,id);
    id = 4;
    res = msg_subscribe(p,id,MSG_MODE_CALLBACK,0,msg_4);
    printf("msg_subscribe res = %d, id = %d \r\n",res,id);
    /*
      publish and you can get the msg from the call back
      brcasuse i put the msg_recv_data in the send_byte
    */
    msg_publish(p,1,(unsigned char *)&pack1,sizeof(pack1));
    msg_publish(p,2,(unsigned char *)&pack2,sizeof(pack2));
    msg_publish(p,3,(unsigned char *)&pack3,sizeof(pack3));
    msg_publish(p,4,(unsigned char *)&pack4,sizeof(pack4));

    /*copy mode no msg calback */
    id = 5;
    res = msg_subscribe(p,id,MSG_MODE_COPY,(unsigned char *)&pack1_des,0);
    printf("msg_subscribe res = %d, id = %d \r\n",res,id);
    msg_publish(p,5,(unsigned char *)&pack1,sizeof(pack1));
    printf("pack1_des A = %f,\r\n",pack1_des.A);
    msg_delete(p);
    while(1)
    {

    }
    return 1;
}
int msg_1(void *p)
{
    package1_st *pd = (package1_st *)p;
    printf("msg_1 is %f\r\n",pd->A);
    return 1;
}
int msg_2(void *p)
{
    package2_st *pd = (package2_st *)p;
    printf("msg_2 is %f\r\n",pd->C);
    return 1;
}
int msg_3(void *p)
{
    package3_st *pd = (package3_st *)p;
    printf("msg_3 is %f\r\n",pd->A);
    return 1;
}
int msg_4(void *p)
{
    package4_st *pd = (package4_st *)p;
    printf("msg_4 is %f\r\n",pd->C);
    return 1;
}
