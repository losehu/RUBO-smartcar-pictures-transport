#include <iostream>
#include <WinSock2.h>
#include "opencv2/opencv.hpp"
#include "ty.h"
#pragma comment(lib, "ws2_32.lib")        //加载ws2_32.dll
using namespace cv;
using namespace std;
#define CIRCLE_MAX 400000//5e6
#define BUFF_MAX 4000
#define COL 188
#define ROW 120
 char cRecvBuff[BUFF_MAX];                               //定义接收缓冲区

 char solve_Buff[BUFF_MAX];                               //定义接收缓冲区
 char circle_rec_buff[CIRCLE_MAX];//
#define img_info solve_Buff
long long byte_sum = 0;
long long circle_recv_add = 0;
long long circle_read_add = 0;
long long step_byte = 0;
void circle_ram_store ( char *rec_buff, int len) {
    for (int i = 0; i < len; i++) {
        byte_sum++;
        circle_rec_buff[circle_recv_add] = rec_buff[i];
        circle_recv_add++;
        if (circle_recv_add == CIRCLE_MAX)circle_recv_add = 0;
    }
}
bool find_flag = 0;

bool get_circle_ram( char *get_buff) {
    long long tmp_step_byte=step_byte,tmp_circle_read_add=circle_read_add;
    find_flag=0;
    while (step_byte < byte_sum && find_flag == 0) {
        if(step_byte +6+ROW*COL/8> byte_sum)
        {
            step_byte=tmp_step_byte;
            circle_read_add=tmp_circle_read_add;
            return  0;
        }
        if (circle_rec_buff[circle_read_add%CIRCLE_MAX] =='9' && circle_rec_buff[(circle_read_add + 1) % CIRCLE_MAX] == '9'&&
            circle_rec_buff[(circle_read_add + 2) % CIRCLE_MAX] == '9'&&  circle_rec_buff[(circle_read_add + 3+ROW*COL/8) % CIRCLE_MAX] == '9'&&
                circle_rec_buff[(circle_read_add + 4+ROW*COL/8) % CIRCLE_MAX] == '9'&& circle_rec_buff[(circle_read_add + 5+ROW*COL/8) % CIRCLE_MAX] == '9') {

           // cout<<circle_read_add<<endl;
            circle_read_add+=3;
            step_byte+=3;
            find_flag=1;
            for (int i = 0; i < ROW*COL/8; i++) {
                get_buff[i] = circle_rec_buff[circle_read_add ];
                step_byte++;
                circle_read_add++;
                if (circle_read_add >= CIRCLE_MAX)circle_read_add = 0;
            }
             return 1;
        }
        step_byte++;
        circle_read_add++;
        if (circle_read_add >= CIRCLE_MAX)circle_read_add = 0;
    }
    step_byte=tmp_step_byte;
    circle_read_add=tmp_circle_read_add;
    return 0;
}

int main(int argc, char *argv[]) {
    namedWindow("result", 0);
    resizeWindow("result", COL * 4, ROW * 4); //创建一个500*500大小的窗口


    WSADATA wsaData;                                   //指向WinSocket信息结构的指针
    SOCKET sockListener;
    SOCKADDR_IN sin, saClient;                          //设置两个地址，sin用来绑定
    int nSize, nbSize, nbSize1;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)           //进行WinSocket的初始化
    {
        printf("Can't initiates windows socket!Program stop.\n");//初始化失败返回-1
        return -1;
    }
    sockListener = socket(AF_INET, SOCK_DGRAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);             //发送端使用的发送端口，可以根据需要更改
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockListener, (SOCKADDR FAR *) &sin, sizeof(sin)) != 0) {
        printf("Can't bind socket to local port!Program stop.\n");//初始化失败返回-1
        return -1;
    }
    while (1) {
      memset(cRecvBuff,0,sizeof (cRecvBuff));
           memset(img_info,0,sizeof (img_info));

        nSize = sizeof(SOCKADDR_IN);
            if ((nbSize = recvfrom(sockListener, cRecvBuff, BUFF_MAX, 0,
                                   (SOCKADDR
                                   FAR *) &saClient, &nSize)) == SOCKET_ERROR) //若接收失败则提示错误
            {
                printf("Recive Error");
                break;
            }
//        cRecvBuff[0]='9',cRecvBuff[1]='9',cRecvBuff[2]='9';
//        for(int i=3;i<1428;i++)cRecvBuff[i]=0x55;
//       //     printf("%x",cRecvBuff[i]);
//    //   waitKey(0);
//        nbSize=1428;
        circle_ram_store(cRecvBuff, nbSize);
        while (get_circle_ram(img_info)) {
//            cout<<"huanchong"<<endl;
//for(int i=0;i<CIRCLE_MAX;i++)
//   printf("%x",circle_rec_buff[i]);
            int hang = 0, lie = 0;
            for (int i = 0; i < ROW*COL/8; i++) {
                for (int j = 0; j < 8; j++) {
                    image_show.at<uchar>(hang, lie) = ((((unsigned char) img_info[i]) >> (7 - j)) & 1) * 255;
                    lie++;
                    if (lie == COL) {
                        lie = 0;
                        hang++;
                    }
                }
            }
         //   imwrite("../out.bmp", image_show);
         //  src = image_show;
          //  cvtColor(src, src, COLOR_GRAY2RGB);
          //  mat_to_v(image_show, ImageUsed);
            //
//cout<<1/((double)(endd-start)/CLOCKS_PER_SEC)<<endl;
            imshow("result", image_show);
//                    for(int i=0;i<1425;i++)
//            printf("%x",img_info[i]);
            waitKey(1);
        }
    }
    return 0;
}
