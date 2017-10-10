#include "msg.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "malloc.h"
#include "stdlib.h"
#include "stdio.h"
int  msg_recv_data( Msg_Ncb_st *fs,unsigned char *buffer,unsigned int len);
int  msg_memcpy(unsigned char * src,unsigned char *des,unsigned int len);
int  msg_find_node(Msg_Ncb_st *fs, unsigned char id, struct Msg_Node_st **pp);
int  msg_convet_data( Msg_Ncb_st *fs,unsigned char *buff, unsigned int len);
int  msg_delete( Msg_Ncb_st *fs);
unsigned char msg_crc_check(unsigned char* pdata,unsigned int len);
int  msg_check_id(Msg_Ncb_st *fs, unsigned char id);
int  msg_subscribe( Msg_Ncb_st *fs, unsigned char id, unsigned char mode, void *pdata, msg_call_back_st callback);
int  msg_recv_data( Msg_Ncb_st *fs,unsigned char *buffer,unsigned int len)
{
    if((fs == 0)||(fs->is_initialize == 0)||(fs->rx_buffer == 0))
        return 0;
    for(unsigned int i = 0; i < len; i++)
    {
        unsigned char data = buffer[i];

        if( data == MSG_FRAME_FLAG1)
        {
            fs->rx_flag |= MSG_PROTOCL_HEAD;
            fs->rx_buffer[fs->rx_index++]=data;
        }
        else if(data == MSG_FRAME_FLAG2)
        {
            if( fs->rx_flag & MSG_PROTOCL_HEAD)
            {
                fs->rx_index = 0;
                fs->rx_flag&=~MSG_PROTOCL_OVER;
            }
            else
            {
                fs->rx_buffer[fs->rx_index++]=data;
            }
            fs->rx_flag&=~MSG_PROTOCL_HEAD;
        }
        else
        {
           fs->rx_buffer[fs->rx_index++]=data;
           fs->rx_flag&=~MSG_PROTOCL_HEAD;
           if(fs->rx_index == 2)
           {
                fs->rx_index_size =*(unsigned short int *)(fs->rx_buffer);
           }
           if(( fs->rx_index_size == fs->rx_index )&&(fs->rx_index > 1 ))
             {
                    fs->rx_flag |=  MSG_PROTOCL_OVER;
            //			if((MTSCheckCRC(&fs->rx_buffer_size[1],fs->rx_buffer_size[0]-2) == fs->rx_buffer_size[fs->rx_index])&& //??
            //				((fs->rx_buffer_size[1] < RequCmdOrderNumMax)&&(fs->rx_buffer_size[1] > 0)))
                    {
                        int res = msg_convet_data(fs,fs->rx_buffer,fs->rx_index_size);
                        fs->rx_index = 0;
                        fs->rx_flag&=~ MSG_PROTOCL_OVER;
                    }
              }
        }
        if(fs->rx_index == fs->rx_buffer_size)
            fs->rx_index--;
    }
    return 1;
}
int  msg_memcpy(unsigned char * src,unsigned char *des,unsigned int len)
{
    for(unsigned int i =0; i < len; i++)
        des[i] = src[i];
    return 1;
}
int  msg_find_node( Msg_Ncb_st *fs, unsigned char id,struct Msg_Node_st **pp)
{
    struct Msg_Node_st *pN;
    if((fs == 0)||(id == 0)||(fs->is_initialize == 0)||(id >= MSG_ID_MAX))
        return 0;
    pN = fs->phead;
    for(unsigned int i = 0; i < fs->node_size; i++)
    {
        if( pN->id == id)
        {
            *pp = pN;
            return 1;
        }
        else
            pN = pN->next;
    }
    return -1;
}
int  msg_convet_data( Msg_Ncb_st *fs,unsigned char *buff, unsigned int len)
{
    unsigned short Size = *(unsigned short int *)(buff + 0) ;
    unsigned char Crc = *(unsigned char *)(buff + 2) ;
    unsigned char Cmd =  *(unsigned char *)(buff + 3) ;
    unsigned char Ctrlword = *(buff + 4);
    unsigned char *pdata = (buff + 5);
    struct Msg_Node_st *pN = 0;
    if(msg_find_node(fs,Cmd,&pN) != 1)
        return -1;
    if(pN == 0)
        return -1;
    if(pN->mode == MSG_MODE_CALLBACK)
    {
        if(pN->callback != 0)
        {
            pN->callback(pdata);
            pN->size = Size;
        }
        else
            return -1;
    }
    else if (pN->mode == MSG_MODE_COPY)
    {
        msg_memcpy((unsigned char *)pdata,(unsigned char *)(pN->pdata),Size);
        pN->size = Size;
    }
    return 1;
}
int  msg_create( Msg_Ncb_st **pfs,unsigned int buffer_size, msg_write_st write)
{
    Msg_Ncb_st *fs;
    fs = ( Msg_Ncb_st *)malloc(sizeof( Msg_Ncb_st));
    if(fs == 0)
        return -2;
    fs->node_size  = 1;
    fs->rx_buffer_size = buffer_size;
    fs->rx_buffer = 0;
    fs->rx_buffer = (unsigned char *)malloc(buffer_size);
    fs->rx_flag = 0;
    fs->rx_index = 0;
    fs->write = write;
    fs->phead = (struct Msg_Node_st *)malloc(sizeof(struct Msg_Node_st));
    fs->phead->callback = 0;
    fs->phead->id = 0;
    fs->phead->mode = 0;
    fs->phead->next = 0;
    fs->phead->prev = 0;
    fs->phead->pdata = 0;
    fs->node_size = 1;
    fs->ptail =  fs->phead;
    fs->is_initialize = 1;
    *pfs = fs;
    return 1;
}
int  msg_delete( Msg_Ncb_st *fs)
{
    struct Msg_Node_st *p;
    struct Msg_Node_st *pnext;
    if(fs == 0)
        return 1;
    free(fs->rx_buffer);
    p = fs->phead;
    for(unsigned int i = 0; i < fs->node_size; i++)
    {
        pnext = p->next;
        free(p);
        p = pnext;
    }
    free(fs);
    fs = 0;
    return 1;
}
unsigned char msg_crc_check(unsigned char* pdata,unsigned int len)
{
    unsigned char sum = 0;
    for(unsigned int i = 0; i < len; i++)
    {
        sum += pdata[i];
    }
    return sum;
}

int  msg_publish( Msg_Ncb_st *fs,unsigned int id,unsigned char *pdata, unsigned int len)
{
    typedef union
    {
        unsigned short int	usdata;
        unsigned char       udata[2];
    }u16_u;
    unsigned char temp[7];
    int res;
    u16_u  size;
    unsigned char Crc;
    unsigned char Cmd = id;
    unsigned char CtrlWord = 0;
    if((fs == 0)||(fs->is_initialize == 0))
        return -2;
    size.usdata = len + 5;
    Crc = msg_crc_check(pdata,len);
    Crc += Cmd;
    Crc += CtrlWord;
    temp[0] = MSG_FRAME_FLAG1;
    temp[1] = MSG_FRAME_FLAG2;
    temp[2] = size.udata[0];
    temp[3] = size.udata[1];
    temp[4] = Crc;
    temp[5] = Cmd;
    temp[6] = CtrlWord;
    if(fs->write == 0)
        return -1;
    res = fs->write((unsigned char *)(temp), 7);
    if(res != 1)
        return res;
    return fs->write((unsigned char *)(pdata), len);
}
int  msg_check_id( Msg_Ncb_st *fs, unsigned char id)
{
    struct Msg_Node_st *pN;
    if((fs == 0)||(id == 0)||(fs->is_initialize == 0)||(id >= MSG_ID_MAX))
        return 0;
    pN = fs->phead;
    for(unsigned int i = 0; i <  fs->node_size; i++)
    {
        if( pN->id == id)
        {
            return 1;
        }
        else
            pN = pN->next;
    }
    return 0;
}
int  msg_subscribe( Msg_Ncb_st *fs,unsigned char id,unsigned char mode,void *pdata,msg_call_back_st callback)
{
    struct Msg_Node_st *pNode;
    if((fs == 0)||(id == 0)||(fs->is_initialize == 0)||(id >= MSG_ID_MAX)||(mode >= MSG_MODE_MAX))
        return -1;
    if(msg_check_id(fs,id) == 1)  /*the link already have the id*/
        return -1;
    pNode = (struct Msg_Node_st *)malloc(sizeof(struct Msg_Node_st));
    pNode->id = id;
    pNode->mode = mode;
    pNode->callback = callback;
    pNode->pdata = pdata;
    pNode->prev = fs->ptail;
    fs->ptail->next = pNode;
    fs->ptail = pNode;
    fs->node_size++;
    return 1;
}
#ifdef __cplusplus
}
#endif
