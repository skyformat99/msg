#ifndef MSG_H
#define MSG_H
#ifdef __cplusplus
extern "C" {
#endif
    const int MSG_PROTOCL_HEAD  =   0x80;
    const int MSG_PROTOCL_OVER  =   0x40;
    const int MSG_FRAME_FLAG1   =   0xA5;
    const int MSG_FRAME_FLAG2   =   0x5A;
    const int MSG_ID_MAX    =      255;
    const int MSG_MODE_CALLBACK =  0;
    const int MSG_MODE_COPY    =   1;
    const int MSG_MODE_MAX    =    2;
    /*user call back*/
    typedef  int (*msg_call_back_st)(void *);
    /*user write function point*/
    typedef  int (*msg_write_st)(unsigned char *,unsigned int);
    /*one msg node including information about the msg*/
    struct Msg_Node_st
    {
        unsigned char                id;             /*command*/
        unsigned char               mode;          /*mode*/
        void                        *pdata;
        unsigned int                size;
        msg_call_back_st            callback;
        struct Msg_Node_st *          prev;
        struct Msg_Node_st *          next;
    };
    /*Message Node Control block*/
    typedef struct
    {
        struct Msg_Node_st *phead;          /*link head*/
        struct Msg_Node_st *ptail;           /*link tail*/
        unsigned int        node_size;
        msg_write_st        write;
        unsigned int        rx_index;
        unsigned short int   rx_index_size; /* local use*/
        unsigned char       rx_flag;
        unsigned char       *rx_buffer;
        unsigned int        rx_buffer_size;
        unsigned char       is_initialize ;
    }Msg_Ncb_st;
    extern  int msg_create(Msg_Ncb_st **pfs, unsigned int buffer_size, msg_write_st write);
    extern  int msg_delete( Msg_Ncb_st *fs);
    extern  int msg_publish(Msg_Ncb_st *fs, unsigned int id, unsigned char *pdata, unsigned int len);
    extern  int msg_subscribe( Msg_Ncb_st *fs,unsigned char id,unsigned char mode,void *pdata,msg_call_back_st callback);
    extern  int msg_recv_data( Msg_Ncb_st *fs, unsigned char *buffer, unsigned int len);
#ifdef __cplusplus
}
#endif
#endif // MSG_H
