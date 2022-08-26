//
// Created by rubo on 2022/1/2.
//

#ifndef MAIN_CPP_TY_H
#define MAIN_CPP_TY_H
#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;
Mat src;
#define debug 0
int StartScanRow = 1;
int right_way[8];
Mat image_show = Mat::zeros(100, 114, CV_8UC1);
int RoadWide0 = 45;
const int COL = 114;
int CenterCol = COL / 2;
const int ROW = 100;
int point_center = 0, point_center0 = 0, point_center1 = 0, point_center2 = 0;
void dfs_left(int start_x, int start_y);
int left_lost_cnt = 0, right_lost_cnt = 0;
struct size_point {
    int x0;
    int y0;
};
void cal_middle_line(void);
int Center[ROW];
uint8_t left_line[ROW];
char left_line_find[ROW];
uint8_t right_line[ROW];
char right_line_find[ROW];
void draw(int x, int y, int flag) {
    if (flag == 1) //B
    {
        src.at<Vec3b>(x, y)[0] = 255;
        src.at<Vec3b>(x, y)[1] = 0;
        src.at<Vec3b>(x, y)[2] = 0;
    } else if (flag == 2)//G
    {
        src.at<Vec3b>(x, y)[0] = 0;
        src.at<Vec3b>(x, y)[1] = 255;
        src.at<Vec3b>(x, y)[2] = 0;
    } else if (flag == 3) {
        src.at<Vec3b>(x, y)[0] = 0;
        src.at<Vec3b>(x, y)[1] = 0;
        src.at<Vec3b>(x, y)[2] = 255;
    } else if (flag == 4) {
        src.at<Vec3b>(x, y)[0] = 0;
        src.at<Vec3b>(x, y)[1] = 255;
        src.at<Vec3b>(x, y)[2] = 255;
    } else if (flag == 5) {
        src.at<Vec3b>(x, y)[0] = 255;
        src.at<Vec3b>(x, y)[1] = 255;
        src.at<Vec3b>(x, y)[2] = 0;
    }
}
struct size_point stack_seed[1000];//栈
struct size_point stack_seed1[1000];//栈

uint8_t stack_line[1000][2];//栈
int cnt = 0;
uint16_t stack_top = 0;
uint16_t stack_top1 = 0;
uint8_t ImageUsed[ROW][COL];
void v_to_mat(Mat &image1, unsigned char b[][COL]) { // 数组转矩阵
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            image1.at<uchar>(i, j) = b[i][j] * 255;
}
void mat_to_v(Mat &image1, unsigned char b[][COL]) { // 数组转矩阵
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            b[i][j] = image1.at<uchar>(i, j);
}
int max_top = 0;
int stack_max = 0;
bool pull_stack(int x, int y, uint8_t img_tmp[][COL]) //入栈
{
    if (stack_top == 300)return 1;
    img_tmp[y][x] = 1;
    stack_seed[stack_top].x0 = x;
    stack_seed[stack_top].y0 = y;
    stack_top++;
    //  if (stack_top > max_top)max_top = stack_top, cout << max_top << endl;
    return 0;
}
size_point push_stack() //出栈
{
    stack_seed[stack_top].y0 = 0;
    stack_seed[stack_top].x0 = 0;
    return stack_seed[--stack_top];
}
bool pull_stack1(uint8_t x, uint8_t y, uint8_t img_tmp[][COL]) //入栈
{
    if (stack_top1 == 300)return 1;
    img_tmp[y][x] = 1;
    stack_seed1[stack_top1].x0 = x;
    stack_seed1[stack_top1].y0 = y;
    stack_top1++;
    // if (stack_top1 > max_top)max_top = stack_top1, cout << max_top << endl;
    return 0;
}
size_point push_stack1() //出栈
{
    stack_seed1[stack_top1].y0 = 0;
    stack_seed1[stack_top1].x0 = 0;
    return stack_seed1[--stack_top1];
}
struct size_point connects[16] = {    //八领域扫点
        {0,  -1},//上
        {-1, -1},//左上
        {-1, 0},//左
        {-1, 1},//左下
        {0,  1},//下
        {1,  1},//右下
        {1,  0},//右
        {1,  -1},//右上



};
void SignalProcess_grayfine_fill(void) {
    // start_line=10;

    int error_right_tmp = 0, error_left_tmp = 0;
    bool error_right_flag = 0, error_left_flag = 0;
    int px, py;
    memset(left_line, 0, sizeof(left_line));
    memset(right_line, COL - 1, sizeof(right_line));
    uint8_t img_tmp[100][114];
    struct size_point center_seed;
    stack_top = 0;
    stack_top1 = 0;
    int start_line = 10;
    memset(img_tmp, 0, sizeof(img_tmp));
    for (int i = 0; i < 8; i++)right_way[i] = 0;
    while (start_line >= 0 && (stack_top == 0 || stack_top1 == 0)) {
        for (int i = COL / 2 + 10; i >= 0; i--) {
            if ((ImageUsed[ROW - 1 - start_line][i]
                 != ImageUsed[ROW - 1 - start_line][i + 1]
                 && ImageUsed[ROW - 1 - start_line][i] == 0)
                || (i == 0 && ImageUsed[ROW - 1 - start_line][i] != 0
                    && ImageUsed[ROW - 1 - start_line][i + RoadWide0 / 2]
                       != 0)) {
                if (ImageUsed[ROW - 1 - start_line][i + 1]
                    == ImageUsed[ROW - 1 - start_line][i + 2]) {
                    if (pull_stack(i + 1, ROW - 1 - start_line, img_tmp))
                        return;
                    break;
                }
            }
        }
        for (int i = COL / 2 - 10; i <= COL - 1; i++) {
            if ((ImageUsed[ROW - 1 - start_line][i]
                 != ImageUsed[ROW - 1 - start_line][i - 1]
                 && ImageUsed[ROW - 1 - start_line][i] == 0)
                || (i == COL - 1 && ImageUsed[ROW - 1 - start_line][i] != 0
                    && ImageUsed[ROW - 1 - start_line][i - RoadWide0 / 2]
                       != 0)) {
                if (ImageUsed[ROW - 1 - start_line][i - 1]
                    == ImageUsed[ROW - 1 - start_line][i - 2]) {
                    if (pull_stack1(i - 1, ROW - 1 - start_line, img_tmp))
                        return;
                    break;
                }
            }
        }
        start_line--;
    }
    if (stack_top == 0 || stack_top1 == 0 || start_line < 0)
        return;
    int max_py = 999, max_py1 = 999;
    bool left_line_flag[ROW] = {0};
    bool right_line_flag[ROW] = {0};
    bool one_way = 1;
    while (stack_top + stack_top1 != 0 && !(max_py == max_py1 && max_py == 99)) {
        while (stack_top != 0 && (max_py >= max_py1 || stack_top1 == 0)) {
            center_seed = push_stack();
            for (int i = 0; i < 8; i++) {
                px = center_seed.x0 + connects[i].x0;
                py = center_seed.y0 + connects[i].y0;
                if (!(px < 0 || py < 0 || px >= COL || py >= ROW)) {
                    if (img_tmp[py][px] == 0) {
                        if (ImageUsed[py][px] != ImageUsed[center_seed.y0][center_seed.x0] ||
                            ((px == 0 || (py == 0 && i != 6)) && ImageUsed[py][px] != 0)) {
                            if (pull_stack((uint8_t) px, (uint8_t) py, img_tmp))return;
                            //   if (ImageUsed[py][px] == 0)while (px < COL - 1 && ImageUsed[py][px] == 0)px++;
                            if (ImageUsed[py][px] != 0)while (px > 0 && ImageUsed[py][px - 1] != 0)px--;
                            if (left_line[py] < px)left_line[py] = px;
                            if (py < max_py)max_py = py;
                            if (debug) {
                                draw(py, px, 1);
                                imshow("result", src);
                                waitKey(1);
                            }
                        }
                    }
                }
            }
        }
        while (stack_top1 != 0 && (max_py <= max_py1 || stack_top == 0)) {
            center_seed = push_stack1();
            for (int i = 0; i < 8; i++) {
                px = center_seed.x0 + connects[i].x0;
                py = center_seed.y0 + connects[i].y0;
                if (!(px < 0 || py < 0 || px >= COL || py >= ROW)) {
                    if (img_tmp[py][px] == 0) {
                        if (ImageUsed[py][px] != ImageUsed[center_seed.y0][center_seed.x0] ||
                            ((px == COL - 1 || (py == 0 && i != 2)) && ImageUsed[py][px] != 0)) {
                            if (pull_stack1((uint8_t) px, (uint8_t) py, img_tmp))return;
                            //  if (ImageUsed[py][px] == 0)while (px > 0 && ImageUsed[py][px] != 0)px--;
                            if (ImageUsed[py][px] != 0)while (px < COL - 1 && ImageUsed[py][px + 1] != 0)px++;
                            if (right_line[py] > px)right_line[py] = px;
                            if (py < max_py1)max_py1 = py;
                            right_way[i]++;
                            if (debug) {
                                draw(py, px, 2);
                                imshow("result", src);
                                waitKey(1);
                            }
//                        src=image_show;
//                        cvtColor(src, src, COLOR_GRAY2RGB);
                            // break;
                        }
                    }
                }
            }
        }
    }
    if (debug) {
        src = image_show;
        cvtColor(src, src, COLOR_GRAY2RGB);
    }
    int lose_error = 1;
    left_lost_cnt = 0, right_lost_cnt = 0;
    for (int i = 0; i < ROW; i++) {
        if (left_line[i] < 0 + lose_error
            || left_line[i] > COL - 1 - lose_error) {
            left_line_find[i] = 'F';
            left_lost_cnt++;
            if (debug) {
                draw(i, left_line[i], 2);
                imshow("result", src);
                waitKey(1);
            }
        } else {
            left_line_find[i] = 'T';
            if (debug) {
                draw(i, left_line[i], 3);
                imshow("result", src);
                waitKey(1);
            }
        }
        if (right_line[i] < 0 + lose_error
            || right_line[i] > COL - 1 - lose_error) {
            right_line_find[i] = 'F';
            right_lost_cnt++;
            if (debug) {
                draw(i, right_line[i], 4);
                imshow("result", src);
                waitKey(1);
            }
        } else {
            right_line_find[i] = 'T';
            if (debug) {
                draw(i, right_line[i], 5);
                imshow("result", src);
                waitKey(1);
            }
        }
    }
    if (debug) {
        waitKey(0);
    }
    for (int i = 0; i < ROW / 2; i++) {
        uint8_t tmp;
        tmp = left_line[i], left_line[i] = left_line[ROW - 1 - i];
        left_line[ROW - 1 - i] = tmp;
        tmp = left_line_find[i], left_line_find[i] = left_line_find[ROW - 1 - i];
        left_line_find[ROW - 1 - i] = tmp;
        tmp = right_line[i], right_line[i] = right_line[ROW - 1 - i];
        right_line[ROW - 1 - i] = tmp;
        tmp = right_line_find[i], right_line_find[i] = right_line_find[ROW - 1 - i];
        right_line_find[ROW - 1 - i] = tmp;
    }
    for (int Ysite = 0; Ysite < ROW; Ysite++) {
        if (Ysite < StartScanRow + 1) {
            //  if (right_line_find[Ysite] == 'T' && left_line_find[Ysite] == 'T')
            Center[Ysite] = CenterCol;        //底部赋中值保持平滑

            right_line_find[Ysite] = 'T';
            left_line_find[Ysite] = 'T';
        } else {
            //此处应对有丢边界的中线进行处理
            if (right_line_find[Ysite] == 'T'
                && left_line_find[Ysite] == 'T') //无丢线
            {
                Center[Ysite] = (left_line[Ysite]
                                 + right_line[Ysite]) / 2;
            } else if (right_line_find[Ysite] == 'F'
                       && left_line_find[Ysite] == 'F') //两边都丢
                Center[Ysite] = CenterCol;
            else if (right_line_find[Ysite] == 'F'
                     && left_line_find[Ysite] == 'T') //丢了右边线
            {
                Center[Ysite] = left_line[Ysite] + RoadWide0 / 2;
            } else if (right_line_find[Ysite] == 'T'
                       && left_line_find[Ysite] == 'F')  //丢了左边
            {
                Center[Ysite] = right_line[Ysite] - RoadWide0 / 2;
            }
        }
        if (debug) {
            draw(ROW - 1 - Ysite, Center[Ysite], 1);
            imshow("result", src);
            waitKey(1);
        }
    }
    if (debug) {
        waitKey(0);
    }
    cal_middle_line();
}
void cal_middle_line(void) {
    //计算当前行的偏移量
//    float excursion[ROW];
    float sumcenter = 0;
#define CenterCalMinRow 12
#define CenterCalMaxRow 25
    float cnt = 0;
    for (int Ysite = CenterCalMinRow; Ysite < CenterCalMaxRow; Ysite++) {
        if (left_line_find[Ysite] == 'T'
            || right_line_find[Ysite] == 'T') {
            sumcenter += (float) (Center[Ysite] - (COL / 2))
                         / (float) (RoadWide0 / 2) * 100;
        }
    }
    point_center0 = (int) (sumcenter / (cnt) * 2);
    point_center1 = point_center0;
    point_center2 = point_center1;
    point_center = point_center0 * 0.7f + point_center1 * 0.2f
                   + point_center2 * 0.1f;
}
void circle_judge() {
    if (abs(left_lost_cnt - right_lost_cnt) < RoadWide0 / 3 * 2 && left_lost_cnt >= ROW / 4)return;
    int search_col = COL - 5;
    if (!(ImageUsed[ROW - 1][search_col] == ImageUsed[ROW - 2][search_col]
          && ImageUsed[ROW - 1][search_col] == ImageUsed[ROW - 1 - 5][search_col]
          && ImageUsed[ROW - 1][search_col] != 0
          && left_line_find[ROW - 1] == left_line_find[ROW - 2]
          && left_line_find[ROW - 1 - 5] == left_line_find[ROW - 1 - 5]
          && left_line_find[ROW - 1] == 'T'
          && ImageUsed[0][search_col] == ImageUsed[1][search_col]
          && ImageUsed[0][search_col] == ImageUsed[0 + 5][search_col]
          && ImageUsed[0][search_col] != 0
    ))return;



}
void init_img(string path);
int bin_to_dec(char *img_info_tmp, int start_num, int start_bit, int cnt,int arr_max) ;
int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);
bool get_circle_ram(char *get_buff);
void circle_ram_store(char *rec_buff,int len);




#endif //MAIN_CPP_TY_H
