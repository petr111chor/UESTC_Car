#include "zf_common_headfile.h"

#define oled_show       (0)
#define USE_num ImageH * 3   //定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点

/*图像处理*/
uint8_t bin_thr = 0;
bool Pixle[ImageH][ImageW];       //二值化图像
static bool isVisited[ImageH][ImageW];//像素点是否访问过
Image_Data image;
volatile imageLine_t imageLine; // 图像边线与中线信息

/*图像拐点*/
short LTurnPoint_Row = EFFECTIVE_ROW, LTurnPoint = 0;
short RTurnPoint_Row = EFFECTIVE_ROW, RTurnPoint = 0;
float Kl_up = 0, Bl_up = 0, Kl_down = 0, Bl_down = 0;
float Kr_up = 0, Br_up = 0, Kr_down = 0, Br_down = 0;
uint8_t Error_Cnt_L = 0, Error_Cnt_R = 0;

/*中线误差*/
volatile uint8_t AIM_LINE_SET = 36;    //目标行28
volatile uint8_t RAMP_AIM_DELTA = 10;
volatile uint8_t AIM_LINE;
bool Flag_SpeedDown = false;
int biaos = 0;
#define SHOW_IMAGE                         (0)

uint8 C_Image[ImageH][ImageW];
uint16 threshold = 230;
uint16 light = 500;

uint16 data_stastics_l = 0;//统计左边找到点的个数
uint16 data_stastics_r = 0;//统计右边找到点的个数
uint8 hightest = 0;//最高点
uint8 start_point_l[2] = { 0 };//左边起点的x，y值
uint8 start_point_r[2] = { 0 };//右边起点的x，y值


/************************************图像采集及处理************************************************/
/*图像预处理函数打包*/
void ImageProcess(void)
{
    uint8 i, j;
    AIM_LINE = AIM_LINE_SET;

    //边线和中心线都不存在
    imageLine.Lost_Center = false;
    imageLine.Lost_Left = false;
    imageLine.Lost_Right = false;

    for (i = 0; i < ImageH; i++)
    {
        //每一行的左右边界点和中心点都不存在
        imageLine.Exist_Left[i] = false;
        imageLine.Exist_Right[i] = false;
        imageLine.Exist_Center[i] = false;

        //边界点和中心点设为初始位置
        imageLine.Point_Left[i] = 1;
        imageLine.Point_Right[i] = ImageW - 1;
        imageLine.Point_Center[i] = ImageW / 2;

        //每一行的白点个数清零
        imageLine.White_Num[i] = 0;

        for (j = 0; j < ImageW; j++)
        {
            isVisited[i][j] = false;//DFS用, 所有点都还没被遍历过
            C_Image[i][j] = mt9v03x_image[i * 2][j * 2];          //  压缩图像

        }
    }
    mt9v03x_finish_flag = 0;//清零标志位，图像重新采集完后重新置为1

    /*************大津法得到二值化的阈值**********************/
    if(!Flag.break_Road)
    {
        if(Flag.Fix_Thr)
        {
            bin_thr = threshold;
        }else if(Flag.Ostu_2){
            bin_thr = GetOSTU_2(C_Image);
        }else{
            bin_thr = GetOSTU(C_Image);
        }

    }


    /******************图像二值化*******************************/
    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
        {
            if (C_Image[i][j] > bin_thr)//白点 1；黑点 0
                Pixle[i][j] = true;
            else
                Pixle[i][j] = false;
        }
    }
    Pixle_Filter();             //过滤二值化图像噪点

    /*提取赛道边界*/
    image_filter(Pixle);      // 滤波
    image_draw_rectan(Pixle); // 预处理
    // 清零
    data_stastics_l = 0;
    data_stastics_r = 0;
    if (get_start_point(ImageH - 2)) // 找到起点了，再执行八领域，没找到就一直找
    {
        //printf("正在开始八领域\n");
        search_l_r((uint16)USE_num, Pixle, &data_stastics_l, &data_stastics_r, start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
        //printf("八邻域已结束\n");
        // 从爬取的边界线内提取边线 ， 这个才是最终有用的边线
        get_left(data_stastics_l);
        get_right(data_stastics_r);
        // 处理函数放这里，不要放到if外面去了，不要放到if外面去了，不要放到if外面去了，重要的事说三遍
    }


}
/*获取二值化阈值(大津法)*/
int GetOSTU(uint8 tmImage[ImageH][ImageW])
{
    int16 i, j;
    uint32 Amount = 0;//像素总数
    uint32 PixelBack = 0;//back像素点数
    uint32 PixelIntegralBack = 0;//back灰度值
    uint32 PixelIntegral = 0;//灰度值总数
    int32 PixelIntegralFore = 0;//fore灰度值
    int32 PixelFore = 0;//fore像素点数
    float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
    int16 MinValue, MaxValue;
    uint8 Threshold = 0;
    uint8 HistoGram[256];              //

    for (j = 0; j < 256; j++)  HistoGram[j] = 0; //初始化灰度直方图

    for (j = 0; j < ImageH; j++)
    {
        for (i = 0; i < ImageW; i++)
        {
            HistoGram[tmImage[j][i]]++; //统计灰度级中每个像素在整幅图像中的个数
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //获取最大灰度的值

    if (MaxValue == MinValue)     return MaxValue;         // 图像中只有一个颜色
    if (MinValue + 1 == MaxValue)  return MinValue;        // 图像中只有二个颜色

    for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  像素总数

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j;//灰度值总数 j是0-255 表示灰度
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)//从本次图像的最小灰度到最大灰度遍历
    {
        PixelBack = PixelBack + HistoGram[j];   //前景像素点数
        PixelFore = Amount - PixelBack;         //背景像素点数
        OmegaBack = (float)PixelBack / Amount;//前景像素百分比
        OmegaFore = (float)PixelFore / Amount;//背景像素百分比
        PixelIntegralBack += HistoGram[j] * j;  //前景灰度值
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
        MicroBack = (float)PixelIntegralBack / PixelBack;   //前景灰度百分比
        MicroFore = (float)PixelIntegralFore / PixelFore;   //背景灰度百分比
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//计算类间方差
        if (Sigma > SigmaB)                    //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
        {
            SigmaB = Sigma;
            Threshold = j;//j是0-255灰度值
        }
    }
    return Threshold;                        //返回最佳阈值;
}

/*获取二值化阈值(加权均值)*/
int GetOSTU_2(uint8 tmImage[ImageH][ImageW])
{
    short i, j;
    uint32 Amount = 0;//像素总数
    //uint32 PixelBack = 0;//back像素点数
    uint32 PixelIntegralBack = 0;//back灰度值
    uint32 PixelIntegral = 0;//灰度值总数
    int PixelIntegralFore = 0;//fore灰度值
    //int PixelFore = 0;//fore像素点数
    //float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
    short MinValue, MaxValue;
    uint8 Threshold = 0;
    uint8 HistoGram[256];
    //

    for (j = 0; j < 256; j++)  HistoGram[j] = 0; //初始化灰度直方图

    for (j = 0; j < ImageH; j++)
    {
        for (i = 0; i < ImageW; i++)
        {
            HistoGram[tmImage[j][i]]++; //统计灰度级中每个像素在整幅图像中的个数
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //获取最大灰度的值

    if (MaxValue == MinValue)     return MaxValue;         // 图像中只有一个颜色
    if (MinValue + 1 == MaxValue)  return MinValue;        // 图像中只有二个颜色

    for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  像素总数

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j;//灰度值总数 j是0-255 表示灰度
    }
    //SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)//从本次图像的最小灰度到最大灰度遍历
    {
        //PixelBack = PixelBack + HistoGram[j];   //前景像素点数
        //PixelFore = Amount - PixelBack;         //背景像素点数
        //OmegaBack = (float)PixelBack / Amount;//前景像素百分比
        //OmegaFore = (float)PixelFore / Amount;//背景像素百分比
        PixelIntegralBack += HistoGram[j] * j;  //前景灰度值
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
        //MicroBack = (float)PixelIntegralBack / (PixelBack);   //前景灰度百分比
        //MicroFore = (float)PixelIntegralFore / (PixelFore);   //背景灰度百分比
        //Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//计算类间方差
        if (PixelIntegralBack > 4*PixelIntegralFore)                    //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
        {
            //SigmaB = Sigma;
            Threshold = j;//j是0-255灰度值
            break;
        }
    }
    if (Threshold == 0)
        Threshold = 250;
    return Threshold;            //返回最佳阈值;
}


/*
函数名称：void get_start_point(uint8 start_row)
功能说明：寻找两个边界的边界点作为八邻域循环的起始点
参数说明：输入任意行数
函数返回：无
修改时间：2022年9月8日
备    注：
example：  get_start_point(image_h-2)
 */
uint8 get_start_point(uint8 start_row)
{
    uint8 i = 0,l_found = 0,r_found = 0;
    //清零
    start_point_l[0] = 0;//x
    start_point_l[1] = 0;//y

    start_point_r[0] = 0;//x
    start_point_r[1] = 0;//y

        //从中间往左边，先找起点
    for (i = ImageW / 2; i > 0; i--)
    {
        start_point_l[0] = i;//x
        start_point_l[1] = start_row;//y
        if (Pixle[start_row][i] == 255 && Pixle[start_row][i - 1] == 0)
        {
            //printf("找到左边起点image[%d][%d]\n", start_row,i);
            l_found = 1;
            break;
        }
    }

    for (i = ImageW / 2; i < ImageW; i++)
    {
        start_point_r[0] = i;//x
        start_point_r[1] = start_row;//y
        if (Pixle[start_row][i] == 255 && Pixle[start_row][i + 1] == 0)
        {
            //printf("找到右边起点image[%d][%d]\n",start_row, i);
            r_found = 1;
            break;
        }
    }

    if(l_found&&r_found)return 1;
    else {
        //printf("未找到起点\n");
        return 0;
    }
}

/*
函数名称：void search_l_r(uint16 break_flag, uint8(*image)[image_w],uint16 *l_stastic, uint16 *r_stastic,
                            uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y,uint8*hightest)

功能说明：八邻域正式开始找右边点的函数，输入参数有点多，调用的时候不要漏了，这个是左右线一次性找完。
参数说明：
break_flag_r            ：最多需要循环的次数
(*image)[image_w]       ：需要进行找点的图像数组，必须是二值图,填入数组名称即可
                       特别注意，不要拿宏定义名字作为输入参数，否则数据可能无法传递过来
*l_stastic              ：统计左边数据，用来输入初始数组成员的序号和取出循环次数
*r_stastic              ：统计右边数据，用来输入初始数组成员的序号和取出循环次数
l_start_x               ：左边起点横坐标
l_start_y               ：左边起点纵坐标
r_start_x               ：右边起点横坐标
r_start_y               ：右边起点纵坐标
hightest                ：循环结束所得到的最高高度
函数返回：无
修改时间：2022年9月25日
备    注：
example：
    search_l_r((uint16)USE_num,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
                start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
 */

 //存放点的x，y坐标
uint16 points_l[(uint16)USE_num][2] = { {  0 } };//左线
uint16 points_r[(uint16)USE_num][2] = { {  0 } };//右线
uint16 dir_r[(uint16)USE_num] = { 0 };//用来存储右边生长方向
uint16 dir_l[(uint16)USE_num] = { 0 };//用来存储左边生长方向
void search_l_r(uint16 break_flag, uint8(*image)[ImageW], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest)
{

    uint8 i = 0, j = 0;

    //左边变量
    uint8 search_filds_l[8][2] = { {  0 } };
    uint8 index_l = 0;
    uint8 temp_l[8][2] = { {  0 } };
    uint8 center_point_l[2] = {  0 };
    uint16 l_data_statics;//统计左边
    //定义八个邻域
    static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是顺时针

    //右边变量
    uint8 search_filds_r[8][2] = { {  0 } };
    uint8 center_point_r[2] = { 0 };//中心坐标点
    uint8 index_r = 0;//索引下标
    uint8 temp_r[8][2] = { {  0 } };
    uint16 r_data_statics;//统计右边
    //定义八个邻域
    static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是逆时针

    l_data_statics = *l_stastic;//统计找到了多少个点，方便后续把点全部画出来
    r_data_statics = *r_stastic;//统计找到了多少个点，方便后续把点全部画出来

    //第一次更新坐标点  将找到的起点值传进来
    center_point_l[0] = l_start_x;//x
    center_point_l[1] = l_start_y;//y
    center_point_r[0] = r_start_x;//x
    center_point_r[1] = r_start_y;//y

        //开启邻域循环
    while (break_flag--)
    {

        //左边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_l[l_data_statics][0] = center_point_l[0];//x
        points_l[l_data_statics][1] = center_point_l[1];//y
        l_data_statics++;//索引加一

        //右边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_r[r_data_statics][0] = center_point_r[0];//x
        points_r[r_data_statics][1] = center_point_r[1];//y

        index_l = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_l[i][0] = 0;//先清零，后使用
            temp_l[i][1] = 0;//先清零，后使用
        }

        //左边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
                && image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)
            {
                temp_l[index_l][0] = search_filds_l[(i)][0];
                temp_l[index_l][1] = search_filds_l[(i)][1];
                index_l++;
                dir_l[l_data_statics - 1] = (i);//记录生长方向
            }

            if (index_l)
            {
                //更新坐标点
                center_point_l[0] = temp_l[0][0];//x
                center_point_l[1] = temp_l[0][1];//y
                for (j = 0; j < index_l; j++)
                {
                    if (center_point_l[1] > temp_l[j][1])
                    {
                        center_point_l[0] = temp_l[j][0];//x
                        center_point_l[1] = temp_l[j][1];//y
                    }
                }
            }

        }
        if ((points_r[r_data_statics][0]== points_r[r_data_statics-1][0]&& points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
            && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
            ||(points_l[l_data_statics-1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics-1][0] == points_l[l_data_statics - 3][0]
                && points_l[l_data_statics-1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics-1][1] == points_l[l_data_statics - 3][1]))
        {
            //printf("三次进入同一个点，退出\n");
            break;
        }
        if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
            && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
            )
        {
            //printf("\n左右相遇退出\n");
            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点
            //printf("\n在y=%d处退出\n",*hightest);
            break;
        }
        if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
        {
            printf("\n如果左边比右边高了，左边等待右边\n");
            continue;//如果左边比右边高了，左边等待右边
        }
        if (dir_l[l_data_statics - 1] == 7
            && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
        {
            //printf("\n左边开始向下了，等待右边，等待中... \n");
            center_point_l[0] = points_l[l_data_statics - 1][0];//x
            center_point_l[1] = points_l[l_data_statics - 1][1];//y
            l_data_statics--;
        }
        r_data_statics++;//索引加一

        index_r = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_r[i][0] = 0;//先清零，后使用
            temp_r[i][1] = 0;//先清零，后使用
        }

        //右边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                && image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
            {
                temp_r[index_r][0] = search_filds_r[(i)][0];
                temp_r[index_r][1] = search_filds_r[(i)][1];
                index_r++;//索引加一
                dir_r[r_data_statics - 1] = (i);//记录生长方向
                //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
            }
            if (index_r)
            {

                //更新坐标点
                center_point_r[0] = temp_r[0][0];//x
                center_point_r[1] = temp_r[0][1];//y
                for (j = 0; j < index_r; j++)
                {
                    if (center_point_r[1] > temp_r[j][1])
                    {
                        center_point_r[0] = temp_r[j][0];//x
                        center_point_r[1] = temp_r[j][1];//y
                    }
                }

            }
        }


    }


    //取出循环次数
    *l_stastic = l_data_statics;
    *r_stastic = r_data_statics;

}

/*
函数名称：void get_left(uint16 total_L)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_L ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example： get_left(data_stastics_l );
 */
void get_left(uint16 total_L)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    //初始化
    for (i = 0;i<ImageH;i++)
    {
        imageLine.Point_Left[i] = 0;
    }
    h = ImageH - 2;
    //左边
    for (j = 0; j < total_L; j++)
    {
        //printf("%d\n", j);
        if (points_l[j][1] == h)
        {
            imageLine.Point_Left[h] = points_l[j][0]+1;
        }
        else continue; //每行只取一个点，没到下一行就不记录
        h--;
        if (h == 0)
        {
            break;//到最后一行退出
        }
    }
}

/*
函数名称：void get_right(uint16 total_R)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_R  ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example：get_right(data_stastics_r);
 */
void get_right(uint16 total_R)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    for (i = 0; i < ImageH; i++)
    {
        imageLine.Point_Right[i] = ImageH - 1;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
    }
    h = ImageH - 2;
    //右边
    for (j = 0; j < total_R; j++)
    {
        if (points_r[j][1] == h)
        {
            imageLine.Point_Right[h] = points_r[j][0] - 1;
        }
        else continue;//每行只取一个点，没到下一行就不记录
        h--;
        if (h == 0)break;//到最后一行退出
    }
}

//定义膨胀和腐蚀的阈值区间
#define threshold_max   255*5//此参数可根据自己的需求调节
#define threshold_min   255*2//此参数可根据自己的需求调节
void image_filter(uint8(*bin_image)[ImageW])//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
    uint16 i, j;
    uint32 num = 0;


    for (i = 1; i < ImageH - 1; i++)
    {
        for (j = 1; j < (ImageW - 1); j++)
        {
            //统计八个方向的像素值
            num =
                bin_image[i - 1][j - 1] + bin_image[i - 1][j] + bin_image[i - 1][j + 1]
                + bin_image[i][j - 1] + bin_image[i][j + 1]
                + bin_image[i + 1][j - 1] + bin_image[i + 1][j] + bin_image[i + 1][j + 1];


            if (num >= threshold_max && bin_image[i][j] == 0)
            {

                bin_image[i][j] = 255;//白  可以搞成宏定义，方便更改

            }
            if (num <= threshold_min && bin_image[i][j] == 255)
            {

                bin_image[i][j] = 0;//黑

            }

        }
    }

}

/*
函数名称：void image_draw_rectan(uint8(*image)[image_w])
功能说明：给图像画一个黑框
参数说明：uint8(*image)[image_w] 图像首地址
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_draw_rectan(bin_image);
 */
void image_draw_rectan(uint8(*image)[ImageW])
{

    uint8 i = 0;
    for (i = 0; i < ImageH; i++)
    {
        image[i][0] = 0;
        image[i][1] = 0;
        image[i][ImageW - 1] = 0;
        image[i][ImageW - 2] = 0;

    }
    for (i = 0; i < ImageW; i++)
    {
        image[0][i] = 0;
        image[1][i] = 0;
        //image[image_h-1][i] = 0;

    }
}

/*DFS巡线：深度优先搜索找出连通域内所有白点(DFS不后退)*/

/*得到每一行所有白点的个数*/
void Get_White_Num(void)
{
    uint8_t i, j;
    uint8_t white_num = 0;

    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
        {
            if (Pixle[i][j])
            {
                white_num++;
            }
        }
        imageLine.White_Num[i] = white_num;
        white_num = 0;
    }
}
void trackDFS(void)
{
    uint8_t i, j;
    uint8_t roadPointStackX[300];             //DFS堆栈存储寻找X坐标
    uint8_t roadPointStackY[300];             //DFS堆栈存储寻找Y坐标
    short stackTopPos = -1;

    if (!Flag.Left_Ring_Turn && !Flag.Right_Ring_Turn)
    {
        //选择图片下方中点作为起始点
        if (isWhite(ImageH - 2, ImageW / 2))          //若下方中点就是白点，入栈
        {
            stackTopPos++;
            roadPointStackX[stackTopPos] = ImageH - 2;
            roadPointStackY[stackTopPos] = ImageW / 2;
            isVisited[ImageH - 2][ImageW / 2] = true;
        }
        else         //如果下方中点不是白点，就从右往左搜索，如果连续五个点都是白点，则判定为dfs的起点
        {
            for (i = ImageW - 4; i >= 2; i--)//从右往左搜索
            {
                if (isWhite(ImageH - 2, i - 2) && isWhite(ImageH - 2, i - 1) && isWhite(ImageH - 2, i)
                    && isWhite(ImageH - 2, i + 1) && isWhite(ImageH - 2, i + 2))//连续5个白点
                {
                    //搜索到就入栈
                    stackTopPos++;                      //stackTopPos非零就表示栈非空
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = true;      //标记为该点已经遍历过
                    break;                              //一旦确定起点就break
                }
            }
        }

        i = j = 0;

        while (stackTopPos >= 0)                        //只要找到起点就开始
        {
            //出栈
            i = roadPointStackX[stackTopPos];
            j = roadPointStackY[stackTopPos];
            stackTopPos--;

            //处理出界，直接continue
            if ((i < EFFECTIVE_ROW) || (i > ImageH - 2) || (j < 1) || (j > ImageW - 1))
            {
                continue;
            }

            /*************以下操作原则是：遇白点入栈，遇黑点“初步”判断为边界点(后续还需要修正滤波补线等操作)**************/
            /**************************堆栈存储：...左右(上)左右(上)...******************************/
            /******************一般情况下，上的白点会先出栈(体现为先一直往图像上方搜索)，然后再出左右*****************/

            //向左搜索
            if (!isVisited[i][j - 1])                   //起点的左边一点是否遍历过
            {
                if (isWhite(i, j - 1))
                {
                    //白点入栈
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j - 1;
                    isVisited[i][j - 1] = true;         //标记为该点已经遍历过
                }
                else                                    //黑点就初步判断为边界点               (xzl: 感觉会出现覆盖的情况)
                {
                    if (isLeftPoint(i, j))
                    {
                        //这个条件得问问
                        if (!imageLine.Exist_Left[i] || (imageLine.Exist_Left[i] && imageLine.Point_Left[i] < j && j < 59))
                            imageLine.Point_Left[i] = j;//左边界点的横坐标

                        imageLine.Exist_Left[i] = 1;     //左边界点存在
                    }
                }
            }

            //向右搜索
            if (!isVisited[i][j + 1])                   //起点的右边一点是否遍历过
            {
                if (isWhite(i, j + 1))
                {
                    //白点入栈
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j + 1;
                    isVisited[i][j + 1] = true;         //标记为该点已经遍历过
                }
                else                                    //黑点就初步判断为边界点               (xzl: 感觉会出现覆盖的情况)
                {
                    if (isRightPoint(i, j))
                    {

                        if (!imageLine.Exist_Right[i] || (imageLine.Exist_Right[i] && imageLine.Point_Right[i] > j && j > 35))
                            imageLine.Point_Right[i] = j;//右边界点的横坐标
                        imageLine.Exist_Right[i] = true;
                    }
                }
            }

            //向上搜索(向上不判断边界点)
            if (!isVisited[i - 1][j])
            {
                if (isWhite(i - 1, j))
                {
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i - 1;
                    roadPointStackY[stackTopPos] = j;
                    isVisited[i - 1][j] = true;
                }
            }
        }
    }
    else
    {
        for (i = 0; i <= ImageW / 2; i++)//从右往左搜索
        {
            if (Flag.Left_Ring_Turn)
            {
                if (isWhite(ImageH - 2, i - 2) && isWhite(ImageH - 2, i - 1) && isWhite(ImageH - 2, i)
                    && isWhite(ImageH - 2, i + 1) && isWhite(ImageH - 2, i + 2)//连续5个白点
                    )
                {
                    //搜索到就入栈
                    stackTopPos++;//stackTopPos非零就表示栈非空
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = 1;
                }
            }
            else
            {
                if (isWhite(ImageH - 2, ImageW - i - 2) && isWhite(ImageH - 2, ImageW - i - 1) && isWhite(ImageH - 2, ImageW - i)
                    && isWhite(ImageH - 2, ImageW - i + 1) && isWhite(ImageH - 2, ImageW - i + 2)//连续5个白点
                    )
                {
                    //搜索到就入栈
                    stackTopPos++;//stackTopPos非零就表示栈非空
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = 1;
                }
            }
        }

        i = j = 0;

        while (stackTopPos >= 0)
        {
            //出栈
            i = roadPointStackX[stackTopPos];
            j = roadPointStackY[stackTopPos];
            stackTopPos--;

            //处理出界，直接continue
            if ((i < EFFECTIVE_ROW) || (i > ImageH - 2) || (j < 1) || (j > ImageW - 2))
            {
                continue;
            }

            /*************以下操作原则是：遇白点入栈，遇黑点初步判断为边界点(后续还需要修正滤波补线等操作)**************/
            //堆栈存储：...左右(上)左右(上)...
            //一般情况下，上的白点会先出栈(体现为先一直往图像上方搜索)，然后再出左右
            //向左搜索
            if (!isVisited[i][j - 1])
            {
                if (isWhite(i, j - 1))
                {
                    //白点入栈
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j - 1;
                    isVisited[i][j - 1] = 1;
                }
                else
                {
                    //黑点初步判断为边界点(xzl: 感觉会出现覆盖的情况)
                    if (isLeftPoint(i, j))
                    {
                        imageLine.Point_Left[i] = j;//左线轨迹
                        imageLine.Exist_Left[i] = 1;
                        imageLine.Lost_Left = 0;
                    }
                }
            }

            //向右搜索
            if (!isVisited[i][j + 1])
            {
                if (isWhite(i, j + 1))
                {
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j + 1;
                    isVisited[i][j + 1] = 1;
                }
                else//可能找到右边界
                {
                    if (isRightPoint(i, j))
                    {
                        imageLine.Point_Right[i] = j;//右线轨迹
                        imageLine.Exist_Right[i] = 1;
                        imageLine.Lost_Right = 0;
                    }
                }
            }
            //向上搜索(向上不判断边界点)
            if (!isVisited[i - 1][j])
            {
                if (isWhite(i - 1, j))
                {
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i - 1;
                    roadPointStackY[stackTopPos] = j;
                    isVisited[i - 1][j] = 1;
                }
            }
        }
    }
}

/****************************************滤波******************************************************/
/*滤除二值化图像噪点*/
void Pixle_Filter(void)
{
/*    uint8_t nr; //行
    uint8_t nc; //列

    //四领域
    for (nr = ImageH - 2; nr >= 1; nr--)
    {
        for (nc = 1; nc < ImageW - 1; nc++)
        {
            if ((Pixle[nr][nc] == 0) && (Pixle[nr - 1][nc] + Pixle[nr + 1][nc] + Pixle[nr][nc + 1] + Pixle[nr][nc - 1] > 2))
                Pixle[nr][nc] = true;
            else if ((Pixle[nr][nc] == 1) && (Pixle[nr - 1][nc] + Pixle[nr + 1][nc] + Pixle[nr][nc + 1] + Pixle[nr][nc - 1] < 2))
                Pixle[nr][nc] = false;
            //if ((Pixle[nr][nc] == 0) && (Pixle[nr - 1][nc - 1] + Pixle[nr][nc - 1] + Pixle[nr + 1][nc - 1] + Pixle[nr - 1][nc + 1]
            //    + Pixle[nr][nc + 1] + Pixle[nr + 1][nc + 1] > 4))
            //    Pixle[nr][nc] = true;
        }
    }
*/
    uint16_t nr, nc;
    uint8_t tmp[ImageH][ImageW];

    // 创建临时副本时扩展边界（避免边界判断）
    memcpy(tmp, Pixle, sizeof(tmp));

    // 使用八邻域判断+面积阈值
    for (nr = 1; nr < ImageH-1; nr++) {
        for (nc = 1; nc < ImageW-1; nc++) {
            // 八邻域计算（包含四个对角）
            const uint8_t sum_8 = Pixle[nr-1][nc-1] + Pixle[nr-1][nc] + Pixle[nr-1][nc+1]
                                + Pixle[nr][nc-1]   +                Pixle[nr][nc+1]
                                + Pixle[nr+1][nc-1] + Pixle[nr+1][nc] + Pixle[nr+1][nc+1];

            // 主过滤条件
            if (Pixle[nr][nc] == 1) {
                // 白点过滤规则：
                //1. 八邻域白色像素数 < 3：孤立点直接滤除
                //2. 3 <= 八邻域数 < 5：需要二次验证四邻域
                tmp[nr][nc] = (sum_8 < 3) ? 0 :
                             ((sum_8 < 5) ? ((Pixle[nr-1][nc] + Pixle[nr+1][nc]
                                           + Pixle[nr][nc-1] + Pixle[nr][nc+1]) >= 2)
                                          : 1);
            } else {
                // 黑点填充规则保持原逻辑
                tmp[nr][nc] = (Pixle[nr-1][nc] + Pixle[nr+1][nc]
                             + Pixle[nr][nc-1] + Pixle[nr][nc+1] > 2) ? 1 : 0;
            }
        }
    }

    memcpy(Pixle, tmp, sizeof(tmp));
}
/*深度遍历滤波*/
void Left_Right_Confusion_Filter(void)
{
    int8_t i = 0, j = 0;
    for (i = 59; i >= 0; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Left[i])
        {
            if (imageLine.Point_Left[i] > imageLine.Point_Right[i])
            {
                for (j = imageLine.Point_Left[i] + 1; j < 93; j++)
                {
                    if (!isWhite(i, j + 1))
                    {
                        if (isRightPoint(i, j))
                            imageLine.Point_Right[i] = j;
                        break;
                    }
                }
                for (j = imageLine.Point_Right[i] - 1; j > 0; j--)
                {
                    if (!isWhite(i, j - 1))
                    {
                        if (isLeftPoint(i, j))
                            imageLine.Point_Left[i] = j;
                        break;
                    }
                }
            }
        }
    }
    for (i = 59; i > 0; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] && (ABS(imageLine.Point_Right[i] - imageLine.Point_Right[i - 1])) > 5)
        {
            for (j = 5; j > 0; j--)
            {
                if (isRightPoint(i - 1, imageLine.Point_Right[i] + j))
                {
                    imageLine.Point_Right[i - 1] = imageLine.Point_Right[i - 1] + j;
                    break;
                }

                if (isRightPoint(i - 1, imageLine.Point_Right[i - 1] - j))
                {
                    imageLine.Point_Right[i - 1] = imageLine.Point_Right[i - 1] - j;
                    break;
                }
            }
        }
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] && (ABS(imageLine.Exist_Left[i] - imageLine.Exist_Left[i - 1])) > 5)
        {
            for (j = 5; j > 0; j--)
            {
                if (isLeftPoint(i - 1, imageLine.Point_Left[i] + j))
                {
                    imageLine.Point_Left[i - 1] = imageLine.Point_Left[i - 1] + j;
                    break;
                }

                if (isLeftPoint(i - 1, imageLine.Point_Left[i - 1] - j))
                {
                    imageLine.Point_Left[i - 1] = imageLine.Point_Left[i - 1] - j;
                    break;
                }
            }
        }
    }

}
/*filter函数打包*/
void doFilter(void)
{
    if (!Flag.Ramp && !Flag.Left_Ring && !Flag.Right_Ring && (!Flag.CrossRoad_Find || Flag.crossLeft || Flag.crossRight))
    {
        TurnFliter(LTurnPoint_Row, LTurnPoint, RTurnPoint_Row, RTurnPoint, 3, 3,
            Kl_up, Bl_up, Kl_down, Bl_down, Kr_up, Br_up, Kr_down, Br_down, Error_Cnt_L, Error_Cnt_R);
    }
    left_right_Limit();    //边界点越界限制
    lineChangeLimit();     //边界斜率限制
    singlePoint_Filter();  //单独点滤去
    lostLine_Filter();    //无效行过多滤去(丢边线判断)
}
/*无效行过多滤去(丢边线判断)*/
void lostLine_Filter(void)
{
    //对于左边界线的判断--------------------
    short count = 0;
    int8_t i = 0;

    for (i = 0; i < ImageH; i++)        //从上到下搜索
    {
        if (imageLine.Exist_Left[i])
            count++;
    }

    if (count < VALID_LINE_THRESHOLE)        //如果无效行超过阈值认为该边界线丢失
    {
        imageLine.Lost_Left = 1;
        for (i = 0; i < ImageH; i++)        //从上到下搜索
        {
            imageLine.Exist_Left[i] = 0;
        }
    }
    else
        imageLine.Lost_Left = 0;

    //对于右边界线的判断--------------------
    count = 0;
    for (i = 0; i < ImageH; i++)
    {
        if (imageLine.Exist_Right[i])
            count++;
    }

    if (count < VALID_LINE_THRESHOLE)        //如果无效行超过阈值认为该边界线丢失
    {
        imageLine.Lost_Right = 1;
        for (i = 0; i < ImageH; i++)        //从上到下搜索
        {
            imageLine.Exist_Right[i] = 0;
        }
    }
    else
        imageLine.Lost_Right = 0;
}
/*左右边界点越界滤波*/
void left_right_Limit(void)
{
    int8_t i = 0;

    for (i = 1; i < ImageH - 1; i++)//从左上到右下
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Right[i])
        {
            if (imageLine.Point_Left[i] > imageLine.Point_Right[i])
            {
                if (Flag.Right_Ring_Out || Flag.Left_Ring_Find || Flag.Left_Ring_Out_Mend2)
                {
                    imageLine.Exist_Right[i] = 0;
                    continue;
                }
                if (Flag.Left_Ring_Out || Flag.Right_Ring_Find || Flag.Right_Ring_Out_Mend2)
                {
                    imageLine.Exist_Left[i] = 0;
                    continue;
                }
                imageLine.Exist_Right[i] = 0;
                imageLine.Exist_Left[i] = 0;
            }
        }
    }
}
/*边界点不突变*/
void lineChangeLimit(void)
{
    int8_t i = 0, j = 0;
    float leftK = 0, rightK = 0;

    //左边界相邻两有效点斜率检测
    for (i = ImageH - 2; i > 0; i--)        //从下往上
    {
        if (imageLine.Exist_Left[i])        //先找到第一个有效点
        {
            for (j = i + 1; j < ImageH; j++)        //再向下找临近有效点
            {
                if (imageLine.Exist_Left[j])
                {
                    leftK = getLineK(i, imageLine.Point_Left[i], j, imageLine.Point_Left[j]);

                    if (ABS(leftK) > K_MAX_THRESHOLD)
                    {
                        imageLine.Exist_Left[i] = 0;
                        //imageLine.Exist_Left[j] = 0;
                    }
                    break;        //只要找到一个临近有效点，检测后就break到下一个i
                }
            }
        }
    }

    //右边界相邻两有效点斜率检测
    for (i = ImageH - 2; i > 0; i--)        //从下往上
    {
        if (imageLine.Exist_Right[i])        //先找到第一个有效点
        {
            for (j = i + 1; j < ImageH; j++)        //再向下找临近有效点
            {
                if (imageLine.Exist_Right[j])
                {
                    rightK = getLineK(i, imageLine.Point_Right[i], j, imageLine.Point_Right[j]);

                    if (ABS(rightK) > K_MAX_THRESHOLD)
                    {
                        imageLine.Exist_Right[i] = 0;
                        //imageLine.Exist_Right[j] = 0;
                    }
                    break;        //只要找到一个临近有效点，检测后就break到下一个i
                }
            }
        }
    }
}
/*单独边界点滤除*/
void singlePoint_Filter(void)
{
    uint8_t i = 0;

    for (i = EFFECTIVE_ROW; i < ImageH - 1; i++)
    {
        if (!imageLine.Exist_Left[i - 1] && imageLine.Exist_Left[i] && !imageLine.Exist_Left[i + 1])
        {
            imageLine.Exist_Left[i] = false;
        }
        if (!imageLine.Exist_Right[i - 1] && imageLine.Exist_Right[i] && !imageLine.Exist_Right[i + 1])
        {
            imageLine.Exist_Right[i] = false;
        }
    }
    for (i = EFFECTIVE_ROW; i < ImageH - 2; i++)
    {
        if (!imageLine.Exist_Left[i - 1] && imageLine.Exist_Left[i] && imageLine.Exist_Left[i + 1] && !imageLine.Exist_Left[i + 2])
        {
            imageLine.Exist_Left[i] = false;
            imageLine.Exist_Left[i + 1] = false;
        }
        if (!imageLine.Exist_Right[i - 1] && imageLine.Exist_Right[i] && imageLine.Exist_Right[i + 1] && !imageLine.Exist_Right[i + 2])
        {
            imageLine.Exist_Right[i] = false;
            imageLine.Exist_Right[i + 1] = false;
        }
    }
}
/****************************************补线******************************************************/
/*给中点赋值，并认为该行中线有效*/
void amplitudeLIMIT(uint16_t i, int16_t amp)
{
    imageLine.Exist_Center[i] = true;
    imageLine.Point_Center[i] = amp;
}
/*对得出的中线进行突变限幅*/
void limitCenter(void)
{
    for (int8_t i = ImageH - 2; i >= 1; i--)        //从下往上
    {
        if (imageLine.Exist_Center[i] && imageLine.Exist_Center[i + 1])
        {
            if (ABS(imageLine.Point_Center[i] - imageLine.Point_Center[i + 1]) > 6)
            {
                if ((imageLine.Exist_Center[i]) && (!imageLine.Exist_Center[i - 1]))
                    imageLine.Exist_Center[i] = 0;
            }
        }
        if (imageLine.Exist_Center[i] && imageLine.Exist_Center[i - 1])
        {
            if (ABS(imageLine.Point_Center[i] - imageLine.Point_Center[i - 1]) > 6)
            {
                if ((imageLine.Exist_Center[i]) && (!imageLine.Exist_Center[i + 1]))
                    imageLine.Exist_Center[i] = 0;
            }
        }
        if (imageLine.Exist_Center[i] && !imageLine.Exist_Center[i - 1] && !imageLine.Exist_Center[i + 1])
        {
            imageLine.Exist_Center[i] = 0;
        }
    }
}
/*补中线合集*/
void doMend(void)
{
    trackMend_startPart();
    trackMend_HalfPart();//丢边线半宽补，不丢线直接计算
}
/*元素外拐点滤波补线*/
void TurnFliter(short Lturnpoint_row, short Lturnpoint, short Rturnpoint_row, short Rturnpoint, float limitKl, float limitKr,
    float kl_up, float bl_up, float kl_down, float bl_down, float kr_up, float br_up, float kr_down, float br_down, uint8_t error_cnt_l, uint8_t error_cnt_r)
{
    short i = 0, j = 0;
    float DeltaKl = kl_up - kl_down, DeltaKr = kr_up - kr_down;
    bool flag_fliterL = false, flag_fliterR = false, count_flag = false;

    if (Flag.Left_Ring || Flag.Right_Ring)
    {
        Flag.CrossRoad_Find = false;
        Flag.CrossRoad_Block = false;
        return;
    }

    if (Lturnpoint_row != EFFECTIVE_ROW && Lturnpoint_row >= 16 && Lturnpoint_row <= 50)
    {
        short Endline_R = ImageH - 1;
        for (i = ImageH - 1; i >= EFFECTIVE_ROW; i--)
        {
            if ((i <= Lturnpoint_row + 5) && (!imageLine.Exist_Right[i] && !imageLine.Exist_Right[i - 1] && !imageLine.Exist_Right[i - 2]
                && !imageLine.Exist_Right[i - 3] && !imageLine.Exist_Right[i - 4] && !imageLine.Exist_Right[i - 5]))
                break;
            if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] &&
                ABS(imageLine.Point_Right[i - 1] - imageLine.Point_Right[i]) <= 10)
            {
                for (j = i; j >= EFFECTIVE_ROW; j--)
                {
                    if (imageLine.Exist_Right[j] && imageLine.Exist_Right[j - 1] &&
                        ABS(imageLine.Point_Right[j - 1] - imageLine.Point_Right[j]) <= 10)
                        Endline_R = j - 1;
                    else
                        break;
                    if (Endline_R < Lturnpoint_row && imageLine.Point_Right[Endline_R] < Lturnpoint + (roadK * i / 3 + roadB / 3))
                        break;
                }
            }
            if (Endline_R != ImageH - 1)
                break;
        }
        if (!(Endline_R < Lturnpoint_row && imageLine.Point_Right[Endline_R] < Lturnpoint + roadK * i / 3 + roadB / 3))
        {
            if (kl_up > 0)
            {
                if (ABS(DeltaKl) >= limitKl)
                    flag_fliterL = true;
                if (ABS(DeltaKl) <= 5)
                {
                    if ((error_cnt_l >= 5) && (Lturnpoint < ImageW - 15))
                        flag_fliterL = false;
                }
            }
            if (kl_up == 0)
            {
                if (kl_down < 0 && Lturnpoint >= 5)
                    flag_fliterL = true;
            }
        }
    }
    if (Rturnpoint_row != EFFECTIVE_ROW && Rturnpoint_row >= 16 && Rturnpoint_row <= 50)
    {
        short Endline_L = ImageH - 1;
        for (i = ImageH - 10; i >= EFFECTIVE_ROW; i--)
        {
            if ((i <= Rturnpoint_row + 5) && (!imageLine.Exist_Left[i] && !imageLine.Exist_Left[i - 1] && !imageLine.Exist_Left[i - 2]
                && !imageLine.Exist_Left[i - 3] && !imageLine.Exist_Left[i - 4] && !imageLine.Exist_Left[i - 5]))
                break;
            if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] &&
                ABS(imageLine.Point_Left[i - 1] - imageLine.Point_Left[i]) <= 10)
            {
                for (j = i; j >= EFFECTIVE_ROW; j--)
                {
                    if (imageLine.Exist_Left[j] && imageLine.Exist_Left[j - 1] &&
                        ABS(imageLine.Point_Left[j - 1] - imageLine.Point_Left[j]) <= 10)
                        Endline_L = j - 1;
                    else
                        break;
                    if (Endline_L < Rturnpoint_row && imageLine.Point_Left[Endline_L] > Rturnpoint - (roadK * i / 3 + roadB / 3))
                        break;
                }
            }
            if (Endline_L != ImageH - 1)
                break;
        }
        if (!(Endline_L < Rturnpoint_row && imageLine.Point_Left[Endline_L] > Rturnpoint - (roadK * i / 3 + roadB / 3)))
        {
            if (kr_up < 0)
            {
                if (ABS(DeltaKr) >= limitKr)
                    flag_fliterR = true;
                if (ABS(DeltaKr) <= 5)
                {
                    if ((error_cnt_r >= 5) && (Rturnpoint > 15))
                        flag_fliterR = false;
                }
            }
            if (kr_up == 0)
            {
                if (kr_down > 0 && Rturnpoint <= ImageW - 5)
                    flag_fliterR = true;
            }
        }
    }

    if (Lturnpoint_row != EFFECTIVE_ROW && Rturnpoint_row != EFFECTIVE_ROW
        && Lturnpoint_row >= 18 && Rturnpoint_row >= 18
        && Lturnpoint_row <= 45 && Rturnpoint_row <= 45)
    {
        if (ABS(Lturnpoint_row - Rturnpoint_row) <= 20 && ABS(Lturnpoint - Rturnpoint) >= 30)
        {
            for (i = Lturnpoint_row; i > EFFECTIVE_ROW; i--)
            {
                if (imageLine.Exist_Left[i] && imageLine.Point_Left[i] < Lturnpoint)
                    imageLine.Exist_Left[i] = false;
                if (!isWhite(i, Lturnpoint))
                    break;
            }
            for (i = Rturnpoint_row; i > EFFECTIVE_ROW; i--)
            {
                if (imageLine.Exist_Right[i] && imageLine.Point_Right[i] > Rturnpoint)
                    imageLine.Exist_Right[i] = false;
                if (!isWhite(i, Rturnpoint))
                    break;
            }
        }
        if (kl_down >= 0 && kr_down <= 0)
            return;
        if (kl_down < 0 && kr_down <= 0)
        {
            if ((Rturnpoint < ImageW - 10) && (kl_down > kr_down))
            {
                if ((Lturnpoint_row < Rturnpoint_row))
                {
                    flag_fliterR = true;
                }
            }
        }
        if (kl_down >= 0 && kr_down > 0)
        {
            if ((Lturnpoint > 10) && (kl_down < kr_down))
            {
                if ((Rturnpoint_row < Lturnpoint_row))
                {
                    flag_fliterL = true;
                }
            }
        }
        if (kl_down < 0 && kr_down > 0)
        {
            if (ABS(Lturnpoint_row - Rturnpoint_row) <= 10 && ABS(Lturnpoint - Rturnpoint) >= 30)
            {
                flag_fliterL = true;
                flag_fliterR = true;
            }
        }
        for (i = AIM_LINE + 1; i > AIM_LINE - 10; i--)
        {
            if (!(imageLine.Exist_Left[i] && imageLine.Exist_Right[i]))
            {
                count_flag = true;
                break;
            }
        }
        if (!count_flag)
        {
            flag_fliterL = false;
            flag_fliterR = false;
        }
    }
    //滤波补线
    if (flag_fliterL)
    {
        for (i = Lturnpoint_row - 1; i >= 0; i--)
        {
            imageLine.Exist_Left[i] = true;
            imageLine.Point_Left[i] = getLineValue(i, kl_down, bl_down);
            if (imageLine.Point_Right[i] < imageLine.Point_Left[i] + (roadK * i / 2 + roadB / 2) && !flag_fliterR)
                imageLine.Exist_Right[i] = false;
        }
    }
    if (flag_fliterR)
    {
        for (i = Rturnpoint_row - 1; i >= 0; i--)
        {
            imageLine.Exist_Right[i] = true;
            imageLine.Point_Right[i] = getLineValue(i, kr_down, br_down);
            if (imageLine.Point_Left[i] > imageLine.Point_Right[i] - (roadK * i / 2 + roadB / 2) && !flag_fliterL)
                imageLine.Exist_Left[i] = false;
        }
    }
}
/*计算中线*/
/*计算中线*/
void trackMend_startPart(void)
{
    int leftLine_startPoint = 0;
    int rightLine_startPoint = 0;

    int leftLine_stopPoint = 0;
    int rightLine_stopPoint = 0;

    int i;

    float k_left, b_left;
    float k_right, b_right;

    short MendBasis_left[2][5];
    short MendBasis_right[2][5];
    int count = 0;

    //------------------------补左线-------------------------------
    //找左线起始点
    for (i = ImageH - 1; i >= (ImageH_6 ); i--)
    {
        if (imageLine.Exist_Left[i])
        {
            leftLine_startPoint = i;
            break;
        }
    }

    //------------------------补右线-------------------------------
    //找右线起始点
    for (i = ImageH - 1; i >= (ImageH_6 ); i--)
    {
        if (imageLine.Exist_Right[i])
        {
            rightLine_startPoint = i;
            break;
        }
    }

    if (imageLine.Point_Right[rightLine_startPoint] - imageLine.Point_Left[leftLine_startPoint] >= 5)    //防止在三叉的位置补下部分线
    {
        //当左起始超过2/3行时(图像底部看不到边线的部分有点多啦),补线
        if (leftLine_startPoint >10)
        {
            for (i = leftLine_startPoint; i > 0; i--)
            {
                if (imageLine.Exist_Left[i])
                {
                    MendBasis_left[0][count] = (short)i;
                    MendBasis_left[1][count] = (short)imageLine.Point_Left[i];
                    count++;
                }
                if (count == 5)    //有5个点即可开始补线
                {
                    leastSquareMethod(MendBasis_left[0], MendBasis_left[1], 5, &k_left, &b_left);
                    if (ABS(k_left) > 2 && i > ImageH_6)
                    {
                        count = 0;
                        continue;
                    }
                    //开始补线
                    for (i=i+5; i < ImageH; i++)
                    {
                         imageLine.Point_Left[i] = constrain_int16(getLineValue(i, k_left, b_left), 0, ImageW - 1);
                         imageLine.Exist_Left[i] = 1;
                    }
                    break;
                }
            }

        }
        //当右起始超过2/3行时(图像底部看不到边线的部分有点多啦),补线
        count = 0;
        if (rightLine_startPoint >10)
        {
            for (i = rightLine_startPoint; i > 0; i--)
            {
                if (imageLine.Exist_Right[i])
                {
                    MendBasis_right[0][count] = (short)i;
                    MendBasis_right[1][count] = (short)imageLine.Point_Right[i];
                    count++;
                }
                if (count == 5)    //有5个点即可开始补线
                {
                    leastSquareMethod(MendBasis_right[0], MendBasis_right[1], 5, &k_right, &b_right);
                    if (ABS(k_right) > 2 && i < ImageH_6)
                    {
                        count = 0;
                        continue;
                    }
                    //开始补线
                    for (i = i + 5; i < ImageH; i++)
                    {
                        imageLine.Point_Right[i] = constrain_int16(getLineValue(i, k_right, b_right), 0, ImageW - 1);
                        imageLine.Exist_Right[i] = 1;
                    }
                    break;
                }
            }

        }
    }
    //补十字
    for (i = leftLine_startPoint; i >= (ImageH_6); i--)
   {
        if (!imageLine.Exist_Left[i])
        {
            leftLine_stopPoint = i+1;
            break;
        }
    }
    for (i = rightLine_startPoint; i >= (ImageH_6); i--)
    {
        if (!imageLine.Exist_Right[i])
        {
            rightLine_stopPoint = i+1;
            break;
        }
    }
    if (leftLine_stopPoint > ImageH_4)
    {
        for (i = leftLine_stopPoint ;i > ImageH_6;i--)
        {
            if (RoadLeft(i) > 90 && RoadLeft(i + 1) > 90 && RoadLeft(i + 2) > 90)//至少三个全白行
                break;
        }
        if (i != ImageH_6)
        {
            for (i = leftLine_stopPoint - 1; i > 0; i--)
            {
                if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i -1] && (ABS(imageLine.Exist_Left[i] - imageLine.Exist_Left[i-1]) <3))
                {
                    leftLine_stopPoint = i;
                    break;
                }
            }
            if (i != 0)
            {
                float k_Left = 0, b_Left = 0;
                k_Left = ((float)imageLine.Point_Left[leftLine_stopPoint] - (float)imageLine.Point_Left[leftLine_startPoint])
                    / ((float)leftLine_stopPoint - (float)leftLine_startPoint);
                b_Left = (float)imageLine.Point_Left[leftLine_startPoint] - k_Left * (leftLine_startPoint);
                for (i = leftLine_startPoint; i >= leftLine_stopPoint; i--)
                {
                    imageLine.Exist_Left[i] = true;
                    imageLine.Point_Left[i] = getLineValue(i, k_Left, b_Left);
                }
            }
        }

    }

    if (rightLine_stopPoint > ImageH_4)
    {
        for (i = rightLine_stopPoint;i > ImageH_6;i--)
        {
            if (RoadRight(i) > 90 && RoadRight(i + 1) > 90 && RoadRight(i + 2) > 90)//至少三个全白行
                break;
        }
        if (i != ImageH_6)
        {
            for (i = rightLine_stopPoint - 1; i > 0; i--)
            {
                if (imageLine.Exist_Right[i])
                {
                    rightLine_stopPoint = i;
                    break;
                }
            }
            if (i != 0)
            {

                float k_Left = 0, b_Left = 0;
                k_Left = ((float)imageLine.Point_Right[rightLine_stopPoint] - (float)imageLine.Point_Right[rightLine_startPoint])
                    / ((float)rightLine_stopPoint - (float)rightLine_startPoint);
                b_Left = (float)imageLine.Point_Right[rightLine_startPoint] - k_Left * (rightLine_startPoint);
                for (i = rightLine_startPoint; i >= rightLine_stopPoint; i--)
                {
                    imageLine.Exist_Right[i] = true;
                    imageLine.Point_Right[i] = getLineValue(i, k_Left, b_Left);
                }
            }
        }

    }
}

void trackMend_HalfPart(void)
{
    int8_t i = 0, j = 0;
    float err = 0, aveErr = 0, k_l = 0, b_l = 0, k_r = 0, b_r = 0;
    uint8_t count = 0;
    uint8_t centerCompensation = 0;
    bool loseL_flag = false, loseR_flag = false, loseL_First = true, loseR_First = true, recordL = true, recordR = true;
    short basis_length = 0, basis_row = 0, basis_length_2 = 0, basis_row_2 = 0;

    //(一) 两边丢线(没救了)
    if (imageLine.Lost_Left && imageLine.Lost_Right)
    {
        imageLine.Lost_Center = true;
    }

    //(二) 只丢左边线
    else if (imageLine.Lost_Left)
    {
        imageLine.Lost_Center = false;
        //1. 计算中线补偿
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Right[i])
            {
                err += (((ImageW) / 2 + roadK * i / 2 + roadB / 2) - imageLine.Point_Right[i]);
                count++;
            }
        }
        //2. 计算平均误差
        aveErr = (float)(err / count);
        //3. 根据右边线补中线(路宽+中线补偿)
        if (count >= 5 && aveErr > 0)//点数足够且确定右边界线往左倾斜
            centerCompensation = LIMIT2MIN(aveErr, SingleLineLeanAveERR_MAX) / SingleLineLeanAveERR_MAX * SingleLineLeanK / 2;//补偿计算

        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Right[i])
            {
                //计算中线并限幅
                imageLine.Exist_Center[i] = true;
                amplitudeLIMIT(i, imageLine.Point_Right[i] - centerCompensation - roadK * i / 2 - roadB / 2 - (59 - i) * 0.15);
            }
        }
        limitCenter();//对得出的中线进行突变限幅
    }
    //(三) 只丢右边线
    else if (imageLine.Lost_Right)
    {
        imageLine.Lost_Center = false;
        //1. 计算中线补偿
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i])
            {
                err += (imageLine.Point_Left[i] - (ImageW / 2 - roadK * i / 2 - roadB / 2));
                count++;
            }
        }

        //2. 计算平均误差
        aveErr = (float)(err / count);
        //3. 根据左边线补中线(路宽+中线补偿)
        if (count >= 5 && aveErr > 0)//点数足够且确定右边界线往左倾斜
            centerCompensation = LIMIT2MIN(aveErr, SingleLineLeanAveERR_MAX) / SingleLineLeanAveERR_MAX * SingleLineLeanK / 2;//补偿计算

        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i])
            {
                //计算中线并限幅
                imageLine.Exist_Center[i] = true;
                amplitudeLIMIT(i, imageLine.Point_Left[i] + centerCompensation + roadK * i / 2 + roadB / 2 + (59 - i) * 0.15);
            }
        }
        limitCenter();//对得出的中线进行突变限幅
    }
    //(四)两边都没有丢线
    else if (!imageLine.Lost_Left && !imageLine.Lost_Right)
    {
        imageLine.Lost_Center = false;
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i] && imageLine.Exist_Right[i])
            {
                if (imageLine.Point_Right[i] - imageLine.Point_Left[i] < MINRoadLen)
                    continue;
                else
                {
                    loseL_flag = false; loseR_flag = false; loseL_First = true;
                    loseR_First = true; recordL = true; recordR = true;
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, (imageLine.Point_Left[i] + imageLine.Point_Right[i]) / 2);
                }
            }
            else if ((!imageLine.Exist_Left[i] && imageLine.Exist_Right[i]))       // && i < 40 ImageH / 2
            {
                if (loseL_First)
                {
                    if (imageLine.Exist_Left[MIN(i + 1, ImageH - 1)])
                    {
                        for (j = i - 1; j >= 2; j--)
                        {
                            if (imageLine.Exist_Left[j] && imageLine.Exist_Right[j] && imageLine.Exist_Right[j - 1]
                                && ABS(imageLine.Point_Left[j] - imageLine.Point_Left[j - 1]) < 5)
                            {
                                loseL_flag = false;
                                break;
                            }
                            loseL_flag = true;
                        }
                        loseL_First = false;
                    }
                }
                if (!loseL_First && loseL_flag)
                {
                    if (recordL)
                    {
                        basis_length = imageLine.Point_Right[i + 1] - imageLine.Point_Center[i + 1];
                        basis_row = i + 1;
                        recordL = false;
                    }
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, (imageLine.Point_Right[i] - basis_length + (basis_row - i) * 0.3));
                }
                else if (!loseL_First && !loseL_flag)
                {
                    if (recordL)
                    {
                        basis_length = imageLine.Point_Right[i + 1] - imageLine.Point_Center[i + 1];
                        basis_row = i + 1;
                        basis_length_2 = imageLine.Point_Right[j] - (imageLine.Point_Left[j] + imageLine.Point_Right[j]) / 2;
                        basis_row_2 = j;
                        k_l = getLineK(basis_row, basis_length, basis_row_2, basis_length_2);
                        b_l = basis_length - k_l * basis_row;
                        recordL = false;
                    }
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, imageLine.Point_Right[i] - (k_l * i + b_l));
                }
                else
                    amplitudeLIMIT(i, (imageLine.Point_Right[i] - (roadK * i + roadB) / 2));
                if (ABS(imageLine.Point_Center[i] - center) > 50)
                    break;
            }
            else if ((imageLine.Exist_Left[i] && !imageLine.Exist_Right[i]))       // && i < ImageH / 2
            {
                if (loseR_First)
                {
                    if (imageLine.Exist_Right[MIN(i + 1, ImageH - 1)])
                    {
                        for (j = i - 1; j >= 2; j--)
                        {
                            if (imageLine.Exist_Left[j] && imageLine.Exist_Right[j] &&
                                imageLine.Exist_Left[j - 1] && imageLine.Exist_Right[j - 1] &&
                                ABS(imageLine.Point_Right[j] - imageLine.Point_Right[j - 1]) < 5)
                            {
                                loseR_flag = false;
                                break;
                            }
                            loseR_flag = true;
                        }
                        loseR_First = false;
                    }
                }
                if (!loseR_First && loseR_flag)
                {
                    if (recordR)
                    {
                        basis_length = imageLine.Point_Center[i + 1] - imageLine.Point_Left[i + 1];
                        basis_row = i + 1;
                        recordR = false;
                    }
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, (imageLine.Point_Left[i] + basis_length - (basis_row - i) * 0.3));
                }
                else if (!loseR_First && !loseR_flag)
                {
                    if (recordR)
                    {
                        basis_length = imageLine.Point_Center[i + 1] - imageLine.Point_Left[i + 1];
                        basis_row = i + 1;
                        basis_length_2 = (imageLine.Point_Left[j] + imageLine.Point_Right[j]) / 2 - imageLine.Point_Left[j];
                        basis_row_2 = j;
                        k_r = getLineK(basis_row, basis_length, basis_row_2, basis_length_2);
                        b_r = basis_length - k_r * basis_row;
                        recordR = false;
                    }
                    imageLine.Exist_Center[i] = true;
                    amplitudeLIMIT(i, imageLine.Point_Left[i] + (k_r * i + b_r));
                }
                else
                    amplitudeLIMIT(i, (imageLine.Point_Left[i] + (roadK * i + roadB) / 2));
                if (ABS(imageLine.Point_Center[i] - center) > 50)
                    break;
            }
            else continue;
        }
        limitCenter();
    }
}
/**************************************底层函数****************************************************/
/*判断是否为白点*/
bool isWhite(uint16_t row, uint16_t line)
{
    //出界判断
    if (!(row >= 0 && row < ImageH && line >= 0 && line < ImageW))
        return false;

    //判断白点黑点
    if (Pixle[row][line])
        return true;
    else
        return false;
}
/*判断是否为左右边界点*/
bool isLeftPoint(uint16_t i, uint16_t j)
{
    if (j < 2 || j >= ImageW - 3 || i<0 || i>ImageH - 1)//图像边缘
        return false;
    //右边一定不能出现蓝布
    if (((!isWhite(i, j)) || (!isWhite(i, j + 1)) || (!isWhite(i, j + 2)) || (!isWhite(i, j + 3)) || (!isWhite(i, j + 4)) || (!isWhite(i, j + 5))))
        return false;
    //左边一定不能出现路
    if (isWhite(i, j - 1) || isWhite(i, j - 2) || isWhite(i, j - 3) || isWhite(i, j - 4) || isWhite(i, j - 5))
        return false;
    if (j < 0 || j >= ImageW)// 列索引超出范围，直接返回无效
        return false;

    return true;
}
bool isRightPoint(uint16_t i, uint16_t j)
{
    if (j < 2 || j >= ImageW - 3 || i<0 || i>ImageH - 1)//图像边缘
        return false;
    //左边一定不能出现蓝布
    if (((!isWhite(i, j)) || (!isWhite(i, j - 1)) || (!isWhite(i, j - 2)) || (!isWhite(i, j - 3)) || (!isWhite(i, j - 4)) || (!isWhite(i, j - 5))))
        return false;
    //右边一定不能出现路
    if (isWhite(i, j + 1) || isWhite(i, j + 2) || isWhite(i, j + 3) || isWhite(i, j + 4) || isWhite(i, j + 5))
        return false;
    if (j < 0 || j >= ImageW)// 列索引超出范围，直接返回无效
        return false;

    return true;
}
/*边界线是否为直线 (EndLine 判断结束行)*/
bool LeftLine_Check(uint16_t EndLine)
{
    short i = 0, j = 0;
    short StartLine = 0;
    //检查拐点
    if (LTurnPoint_Row >= EndLine)
        return false;
    //找到左边线起始点
    for (i = ImageH - 1; i >= ImageH / 2; i--)
    {
        if (imageLine.Exist_Left[i])
        {
            StartLine = i;
            break;
        }
    }
    if (StartLine == 0)
        return false;
    //判断左线位置是否正常
    short LeftLineMAX = 0;
    for (i = StartLine; i > EndLine; i--)
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] && imageLine.Exist_Left[i - 2] &&
            (ABS((imageLine.Point_Left[i] + imageLine.Point_Left[i - 2]) / 2 - imageLine.Point_Left[i - 1]) < 5))
        {
            if (imageLine.Point_Left[i - 2] > LeftLineMAX)
                LeftLineMAX = imageLine.Point_Left[i - 2];
        }
        else
        {
            for (j = i - 3; j > EndLine; j--)
            {
                if (imageLine.Exist_Left[j] && (ABS(imageLine.Point_Left[j] - imageLine.Point_Left[i - 1]) < 10))
                {
                    i = j;
                    break;
                }
            }
        }
    }
    if (LeftLineMAX < 10 || LeftLineMAX >ImageW_2_3)
        return false;
    //判断左边界点的数量是否正常
    short LeftLine_Cnt = 0, e_cnt = 0;
    for (i = StartLine; i > EndLine; i--)
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] &&
            ABS(imageLine.Point_Left[i - 1] - imageLine.Point_Left[i]) <= 5)
            LeftLine_Cnt++;
        else
        {
            e_cnt++;
            if (e_cnt > 5)
                return false;
        }
    }
    if (LeftLine_Cnt < 4 * (StartLine - EndLine) / 5)
        return false;
    //判断左边线斜率是否正常
    float k_left = 0, b_left = 0;
    short Basis_left[2][5];
    short count = 0;
    for (i = EndLine; i < StartLine; i++)
    {
        if (imageLine.Exist_Left[i])
        {
            Basis_left[0][count] = (short)i;
            Basis_left[1][count] = (short)imageLine.Point_Left[i];
            count++;
        }
        if (count == 5)
            break;
    }
    if (count == 5)
    {
        leastSquareMethod(Basis_left[0], Basis_left[1], 5, &k_left, &b_left);
        if (k_left < -1.5 || k_left > 0.5)
            return false;
        else
            return true;
    }
    else
        return false;
}
bool RightLine_Check(uint16_t EndLine)
{
    short i = 0, j = 0;
    short StartLine = 0;
    //检查拐点
    if (RTurnPoint_Row >= EndLine)
        return false;
    //找到右边线起始点
    for (i = ImageH - 1; i >= ImageH / 2; i--)
    {
        if (imageLine.Exist_Right[i])
        {
            StartLine = i;
            break;
        }
    }
    if (StartLine == 0)
        return false;
    //判断右线位置是否正常
    short rightLineMAX = 94;
    for (i = StartLine; i > EndLine; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] && imageLine.Exist_Right[i - 2] &&
            (ABS((imageLine.Point_Right[i] + imageLine.Point_Right[i - 2]) / 2 - imageLine.Point_Right[i - 1]) < 5))
        {
            if (imageLine.Point_Right[i - 2] < rightLineMAX)
                rightLineMAX = imageLine.Point_Right[i - 2];
        }
        else
        {
            for (j = i - 3; j > EndLine; j--)
            {
                if (imageLine.Exist_Right[j] && (ABS(imageLine.Point_Right[j] - imageLine.Point_Right[i - 1]) < 10))
                {
                    i = j;
                    break;
                }
            }
        }
    }
    if (rightLineMAX < ImageW_3 || rightLineMAX > 84)
        return false;
    //判断右边界点的数量是否正常
    short RightLine_Cnt = 0, e_cnt = 0;
    for (i = StartLine; i > EndLine; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] &&
            ABS(imageLine.Point_Right[i - 1] - imageLine.Point_Right[i]) <= 5)
            RightLine_Cnt++;
        else
        {
            e_cnt++;
            if (e_cnt > 5)
                return false;
        }
    }
    if (RightLine_Cnt < 4 * (StartLine - EndLine) / 5)
        return false;
    //判断右边线斜率是否正常
    float k_right = 0, b_right = 0;
    short Basis_right[2][5];
    short count = 0;
    for (i = EndLine; i < StartLine; i++)
    {
        if (imageLine.Exist_Right[i])
        {
            Basis_right[0][count] = (short)i;
            Basis_right[1][count] = (short)imageLine.Point_Right[i];
            count++;
        }
        if (count == 5)
            break;
    }
    if (count == 5)
    {
        leastSquareMethod(Basis_right[0], Basis_right[1], 5, &k_right, &b_right);
        if (k_right > 1.5 || k_right < -0.5)
            return false;
        else
            return true;
    }
    else
        return false;
}
/*图像找拐点*/
void TurnPointL_Find(short* TurnPoint_Row, short* TurnPoint, float* kl_up, float* bl_up, float* kl_down, float* bl_down, uint8_t* error_cnt_l)
{
    uint8_t err_cnt = 0, StartLineL = 0;
    short i = 0, j = 0, k = 0;
    short BasisLine_Up[3] = { 0,0,0 }, BasisPoint_Up[3] = { 0,0,0 };
    short BasisLine_Down[3] = { 0,0,0 }, BasisPoint_Down[3] = { 0,0,0 };
    short BreakLine[2] = { 0,0 }, BreakPoint[2] = { 0,0 };
    bool flag_left2 = false, flag_enough = false, flag_right1 = false, flag_break = false, flag_err1 = false, flag_err2 = false;

    singlePoint_Filter();  //单独点滤去
    *TurnPoint_Row = EFFECTIVE_ROW; *TurnPoint = 0;
    *kl_up = 0; *bl_up = 0; *kl_down = 0; *bl_down = 0;
    *error_cnt_l = 0;
    if (!Flag.Left_Ring && !Flag.Right_Ring)
    {
        for (i = ImageH_6; i < ImageH; i++)
        {
            if (imageLine.Exist_Left[i])
            {
                StartLineL = i;
                break;
            }
        }
    }
    for (i = ImageH - 2; i >= ImageH_6; i--)
    {
        //45行以上连续6行没有边界点直接退出
        if ((i <= ImageH - 20) && (!imageLine.Exist_Left[i] && !imageLine.Exist_Left[i - 1] && !imageLine.Exist_Left[i - 2]
            && !imageLine.Exist_Left[i - 3] && !imageLine.Exist_Left[i - 4] && !imageLine.Exist_Left[i - 5]))
            break;
        //状态1：未找到逆趋势点。
        //任务：找到3个逆趋势点，并记录下来用于斜率判别
        //      或者找到断点
        if (!flag_break && !flag_left2 && imageLine.Exist_Left[i])
        {
            //先找1个逆趋势点
            for (j = i - 1; j >= MAX(i - 3, ImageH_6); j--)
            {
                if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                    break;
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] < imageLine.Point_Left[i])
                    {
                        BasisLine_Up[0] = i;
                        BasisPoint_Up[0] = imageLine.Point_Left[i];
                        BasisLine_Up[1] = j;
                        BasisPoint_Up[1] = imageLine.Point_Left[j];
                    }
                    break;
                }
            }
            //找到1个逆趋势点后，向上再找两个
            if (BasisLine_Up[0] != 0)
            {
                for (j = BasisLine_Up[0] - 1; j >= MAX(BasisLine_Up[0] - 7, ImageH_6); j--)
                {
                    if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                        break;
                    if (imageLine.Exist_Left[j])
                    {
                        for (k = j - 1; k >= MAX(j - 3, ImageH_6); k--)
                        {
                            if (imageLine.Exist_Left[k])
                            {
                                if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                                {
                                    BasisLine_Up[1] = j;
                                    BasisPoint_Up[1] = imageLine.Point_Left[j];
                                    BasisLine_Up[2] = k;
                                    BasisPoint_Up[2] = imageLine.Point_Left[k];
                                    flag_left2 = true;
                                }
                                if (imageLine.Point_Left[k] > imageLine.Point_Left[j])//找到3个逆趋势点之前出现趋势逆转，判定为假拐点
                                {
                                    flag_err1 = true;
                                    flag_left2 = false;
                                }
                                break;
                            }
                        }
                        if (flag_left2 || flag_err1)
                            break;
                    }
                }
                if (flag_left2)
                {
                    //检验逆趋势点上方是否为白
                    for (j = BasisLine_Up[0] - 1; j >= BasisLine_Up[0] - 3; j--)
                    {
                        if (!isWhite(j, BasisPoint_Up[0]))
                        {
                            flag_left2 = false;
                            break;
                        }
                    }
                }
                else
                {
                    if (flag_err1)
                    {
                        i = k + 1;
                        BasisLine_Up[0] = 0; BasisLine_Up[1] = 0; BasisLine_Up[2] = 0;
                        BasisPoint_Up[0] = 0; BasisPoint_Up[1] = 0; BasisPoint_Up[2] = 0;
                        flag_err1 = false; flag_left2 = false;
                        continue;
                    }
                    else
                    {
                        for (j = BasisLine_Up[1] - 1; j >= MAX(BasisLine_Up[1] - 10, ImageH_6); j--)
                        {
                            if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                            {
                                if (imageLine.Point_Left[i] < ImageW_2_3)
                                {
                                    BreakLine[1] = BasisLine_Up[0];
                                    BreakPoint[1] = BasisPoint_Up[0];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            //没有找到逆趋势点，检查是否为断点
            else if (imageLine.Point_Left[i] < ImageW_2_3 && !imageLine.Exist_Left[i - 1] &&
                !imageLine.Exist_Left[i - 2] && !imageLine.Exist_Left[i - 3])
            {
                BreakLine[1] = i;
                BreakPoint[1] = imageLine.Point_Left[i];
            }
            //找到1个断点后，向上判断是否是白的
            if (BreakLine[1] != 0)
            {
                if (BreakLine[1] < StartLineL + 3)
                {
                    BreakLine[1] = 0;
                    BreakPoint[1] = 0;
                }
                if (BreakLine[1] != 0)
                {
                    for (j = BreakLine[1] - 1; j >= BreakLine[1] - 4; j--)
                    {
                        if (!isWhite(j, BreakPoint[1] + 1))
                        {
                            BreakLine[1] = 0;
                            BreakPoint[1] = 0;
                            break;
                        }
                    }
                }
                if (BreakLine[1] != 0)
                {
                    for (k = BreakLine[1]; k <= ImageH - 1; k++)
                    {
                        if (imageLine.Point_Left[k] == BreakPoint[1])
                        {
                            BreakLine[0] = k;
                            BreakPoint[0] = imageLine.Point_Left[k];
                        }
                        else
                            break;
                    }
                    flag_break = true;
                }
            }
        }
        //状态2：找到3个逆趋势点。
        //任务：在逆趋势点下方找到3个正趋势点，并记录下来用于斜率判别
        if (flag_left2 && !flag_right1)
        {
            //先找逆趋势点下方最下点
            for (j = BasisLine_Up[0]; j <= ImageH - 1; j++)
            {
                if (imageLine.Exist_Left[j])
                {
                    for (k = j + 1; k <= MIN(j + 3, ImageH - 1); k++)
                    {
                        if (imageLine.Exist_Left[k])
                        {
                            if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                            {
                                BasisLine_Down[0] = j;
                                BasisPoint_Down[0] = imageLine.Point_Left[j];
                                BasisLine_Down[1] = k;
                                BasisPoint_Down[1] = imageLine.Point_Left[k];
                            }
                            break;
                        }
                    }
                    if (BasisLine_Down[0] != 0)
                        break;
                }
            }
            //找到最下点后，向下找2个正趋势点
            if (BasisLine_Down[0] != 0)
            {
                for (j = BasisLine_Down[0] + 1; j <= MIN(BasisLine_Down[0] + 7, ImageH - 1); j++)
                {
                    if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                        break;
                    if (imageLine.Exist_Left[j])
                    {
                        for (k = j + 1; k <= MIN(j + 3, ImageH - 1); k++)
                        {
                            if (imageLine.Exist_Left[k])
                            {
                                if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                                {
                                    BasisLine_Down[1] = j;
                                    BasisPoint_Down[1] = imageLine.Point_Left[j];
                                    BasisLine_Down[2] = k;
                                    BasisPoint_Down[2] = imageLine.Point_Left[k];
                                    flag_right1 = true;
                                }
                                if (imageLine.Point_Left[k] > imageLine.Point_Left[j])//找到3个正趋势点之前出现趋势逆转，判定为假拐点
                                {
                                    flag_err2 = true;
                                    flag_right1 = false;
                                }
                                break;
                            }
                        }
                        if (flag_right1 || flag_err2)
                            break;
                    }
                }
            }
            if (!flag_right1)//没有找到3个正趋势点，清缓存，继续找
            {
                i = BasisLine_Up[2] + 1;
                BasisLine_Up[0] = 0; BasisLine_Up[1] = 0; BasisLine_Up[2] = 0;
                BasisPoint_Up[0] = 0; BasisPoint_Up[1] = 0; BasisPoint_Up[2] = 0;
                BasisLine_Down[0] = 0; BasisLine_Down[1] = 0; BasisLine_Down[2] = 0;
                BasisPoint_Down[0] = 0; BasisPoint_Down[1] = 0; BasisPoint_Down[2] = 0;
                flag_err1 = false; flag_err2 = false; flag_left2 = false; flag_right1 = false;
                continue;
            }
        }
        //状态3：找到断点。
        //任务：在断点下方找到3个正趋势点。
        if (flag_break && !flag_right1)
        {
            if (BreakLine[0] - BreakLine[1] > 5)
            {
                BasisLine_Down[0] = BreakLine[1];
                BasisPoint_Down[0] = BreakPoint[1];
            }
            else
            {
                BasisLine_Down[0] = BreakLine[0];
                BasisPoint_Down[0] = BreakPoint[0];
            }
            for (j = BreakLine[0] + 1; j <= ImageH - 1; j++)
            {
                if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2])
                {
                    flag_err2 = true;
                    break;
                }
                if (imageLine.Exist_Left[j])
                {
                    BasisLine_Down[1] = j;
                    BasisPoint_Down[1] = imageLine.Point_Left[j];
                    break;
                }
            }
            if (BasisLine_Down[1] != 0)
            {
                for (j = BasisLine_Down[1]; j <= ImageH - 1; j++)
                {
                    if (!imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2] && !imageLine.Exist_Left[j - 3])
                    {
                        flag_err2 = true;
                        break;
                    }
                    if (imageLine.Exist_Left[j])
                    {
                        for (k = j + 1; k <= ImageH - 1; k++)
                        {
                            if (!imageLine.Exist_Left[k] && !imageLine.Exist_Left[k - 1] && !imageLine.Exist_Left[k - 2] && !imageLine.Exist_Left[k - 3])
                                break;
                            if (imageLine.Exist_Left[k])
                            {
                                if (BasisPoint_Down[1] > BasisPoint_Down[0])
                                {
                                    if (imageLine.Point_Left[k] > imageLine.Point_Left[j])
                                    {
                                        BasisLine_Down[2] = k;
                                        BasisPoint_Down[2] = imageLine.Point_Left[k];
                                        flag_enough = true;
                                    }
                                    if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                                        flag_err2 = true;
                                }
                                else
                                {
                                    if (imageLine.Point_Left[k] < imageLine.Point_Left[j])
                                    {
                                        BasisLine_Down[2] = k;
                                        BasisPoint_Down[2] = imageLine.Point_Left[k];
                                        flag_enough = true;
                                    }
                                    if (imageLine.Point_Left[k] > imageLine.Point_Left[j])
                                        flag_err2 = true;
                                }
                                break;
                            }
                        }
                        if (flag_enough || flag_err2)
                            break;
                    }
                }
            }
            else
            {
                BasisLine_Down[1] = BreakLine[0];
                BasisPoint_Down[1] = BreakPoint[0];
                BasisLine_Down[2] = BreakLine[0];
                BasisPoint_Down[2] = BreakPoint[0];
                flag_enough = true;
            }
            if (!flag_enough)
            {
                if (!flag_err2)
                {
                    BasisLine_Down[2] = BasisLine_Down[1];
                    BasisPoint_Down[2] = BasisPoint_Down[1];
                    flag_enough = true;
                }
                else
                    return;
            }
        }
        //状态4：找到3个逆趋势点，且其下方有3个正趋势点。
        //任务：返回上、下拐点(或断点)、k、b
        if (flag_left2 && flag_right1)
        {
            *TurnPoint_Row = BasisLine_Down[0];
            *TurnPoint = BasisPoint_Down[0];
            for (j = BasisLine_Up[1]; j > MAX(BasisLine_Up[1] - 10, ImageH_6); j--)
            {
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] == BasisPoint_Up[1])
                        BasisLine_Up[1] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Up[2]; j > MAX(BasisLine_Up[2] - 10, ImageH_6); j--)
            {
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] == BasisPoint_Up[2])
                        BasisLine_Up[2] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Down[1]; j < MIN(BasisLine_Down[1] + 10, ImageW); j++)
            {
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] == BasisPoint_Down[1])
                        BasisLine_Down[1] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Down[2]; j < MIN(BasisLine_Down[2] + 10, ImageW); j++)
            {
                if (imageLine.Exist_Left[j])
                {
                    if (imageLine.Point_Left[j] == BasisPoint_Down[2])
                        BasisLine_Down[2] = j;
                    else
                        break;
                }
            }
            leastSquareMethod(BasisLine_Up, BasisPoint_Up, 3, kl_up, bl_up);
            leastSquareMethod(BasisLine_Down, BasisPoint_Down, 3, kl_down, bl_down);
            for (j = BasisLine_Down[2]; j > BasisLine_Up[2]; j--)
            {
                if (imageLine.Exist_Left[j] && imageLine.Exist_Left[j - 1] &&
                    ABS(imageLine.Point_Left[j] - imageLine.Point_Left[j - 1]) < 2)
                    err_cnt++;
            }
            *error_cnt_l = err_cnt;
            return;
        }
        //状态5：找到断点。
        //任务：返回断点、k、b
        if (flag_break && flag_enough)
        {
            if (BasisLine_Down[1] != BreakLine[0])
            {
                *TurnPoint_Row = BreakLine[1];
                *TurnPoint = BreakPoint[1];
                for (j = BasisLine_Down[1]; j < MIN(BasisLine_Down[1] + 10, ImageH); j++)
                {
                    if (imageLine.Exist_Left[j])
                    {
                        if (imageLine.Point_Left[j] == BasisPoint_Down[1])
                            BasisLine_Down[1] = j;
                        else
                            break;
                    }
                }
                for (j = BasisLine_Down[2]; j < MIN(BasisLine_Down[2] + 10, ImageH); j++)
                {
                    if (imageLine.Exist_Left[j])
                    {
                        if (imageLine.Point_Left[j] == BasisPoint_Down[2])
                            BasisLine_Down[2] = j;
                        else
                            break;
                    }
                }
                *kl_up = 0; *bl_up = 0;
                leastSquareMethod(BasisLine_Down, BasisPoint_Down, 3, kl_down, bl_down);
            }
            else
            {
                *TurnPoint_Row = BreakLine[1];
                *TurnPoint = BreakPoint[1];
                *kl_up = 0; *bl_up = 0; *kl_down = 0; *bl_down = BreakPoint[1];
            }
            return;
        }
    }
}
void TurnPointR_Find(short* TurnPoint_Row, short* TurnPoint, float* kr_up, float* br_up, float* kr_down, float* br_down, uint8_t* error_cnt_r)
{
    uint8_t err_cnt = 0, StartLineR = 0;
    short i = 0, j = 0, k = 0;
    short BasisLine_Up[3] = { 0,0,0 }, BasisPoint_Up[3] = { 0,0,0 };
    short BasisLine_Down[3] = { 0,0,0 }, BasisPoint_Down[3] = { 0,0,0 };
    short BreakLine[2] = { 0,0 }, BreakPoint[2] = { 0,0 };
    bool flag_left1 = false, flag_enough = false, flag_right2 = false, flag_break = false, flag_err1 = false, flag_err2 = false;

    singlePoint_Filter();  //单独点滤去
    *TurnPoint_Row = EFFECTIVE_ROW; *TurnPoint = 0;
    *kr_up = 0; *br_up = 0; *kr_down = 0; *br_down = 0;
    *error_cnt_r = 0;
    if (!Flag.Left_Ring && !Flag.Right_Ring)
    {
        for (i = ImageH_6; i < ImageH; i++)
        {
            if (imageLine.Exist_Right[i])
            {
                StartLineR = i;
                break;
            }
        }
    }
    for (i = ImageH - 2; i >= ImageH_6; i--)
    {
        //45行以上连续6行没有边界点直接退出
        if ((i <= ImageH - 20) && (!imageLine.Exist_Right[i] && !imageLine.Exist_Right[i - 1] && !imageLine.Exist_Right[i - 2]
            && !imageLine.Exist_Right[i - 3] && !imageLine.Exist_Right[i - 4] && !imageLine.Exist_Right[i - 5]))
            break;
        //状态1：未找到逆趋势点。
        //任务：找到3个逆趋势点，并记录下来用于斜率判别
        //      或者找到断点
        if (!flag_break && !flag_right2 && imageLine.Exist_Right[i])
        {
            //先找1个逆趋势点
            for (j = i - 1; j >= MAX(i - 7, ImageH_6); j--)
            {
                if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                    break;
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] > imageLine.Point_Right[i])
                    {
                        BasisLine_Up[0] = i;
                        BasisPoint_Up[0] = imageLine.Point_Right[i];
                        BasisLine_Up[1] = j;
                        BasisPoint_Up[1] = imageLine.Point_Right[j];
                    }
                    break;
                }
            }
            //找到1个逆趋势点后，向上再找两个
            if (BasisLine_Up[0] != 0)
            {
                for (j = BasisLine_Up[0] - 1; j >= MAX(BasisLine_Up[0] - 7, ImageH_6); j--)
                {
                    if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                        break;
                    if (imageLine.Exist_Right[j])
                    {
                        for (k = j - 1; k >= MAX(j - 3, ImageH_6); k--)
                        {
                            if (imageLine.Exist_Right[k])
                            {
                                if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                                {
                                    BasisLine_Up[1] = j;
                                    BasisPoint_Up[1] = imageLine.Point_Right[j];
                                    BasisLine_Up[2] = k;
                                    BasisPoint_Up[2] = imageLine.Point_Right[k];
                                    flag_right2 = true;
                                }
                                if (imageLine.Point_Right[k] < imageLine.Point_Right[j])//找到3个逆趋势点之前出现趋势逆转，判定为假拐点
                                {
                                    flag_err1 = true;
                                    flag_right2 = false;
                                }
                                break;
                            }
                        }
                        if (flag_right2 || flag_err1)
                            break;
                    }
                }
                if (flag_right2)
                {
                    for (j = BasisLine_Up[0] - 1; j >= BasisLine_Up[0] - 3; j--)
                    {
                        if (!isWhite(j, BasisPoint_Up[0]))
                        {
                            flag_right2 = false;
                            break;
                        }
                    }
                }
                else//没有找到三个逆趋势点，判断是否存在断点，如果没有则清缓存，继续找
                {
                    if (flag_err1)
                    {
                        i = k + 1;
                        BasisLine_Up[0] = 0; BasisLine_Up[1] = 0; BasisLine_Up[2] = 0;
                        BasisPoint_Up[0] = 0; BasisPoint_Up[1] = 0; BasisPoint_Up[2] = 0;
                        flag_err1 = false; flag_right2 = false;
                        continue;
                    }
                    else
                    {
                        for (j = BasisLine_Up[1] - 1; j >= MAX(BasisLine_Up[1] - 10, ImageH_6); j--)
                        {
                            if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                            {
                                if (imageLine.Point_Right[i] > ImageW_3)
                                {
                                    BreakLine[1] = i;
                                    BreakPoint[1] = imageLine.Point_Right[i];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            //没有找到逆趋势点，检查是否为断点
            else if (imageLine.Point_Right[i] > ImageW_3 && !imageLine.Exist_Right[i - 1] &&
                !imageLine.Exist_Right[i - 2] && !imageLine.Exist_Right[i - 3])
                 {
                      BreakLine[1] = i;
                      BreakPoint[1] = imageLine.Point_Right[i];
                 }
            //找到1个断点后，向上判断是否是白的
            if (BreakLine[1] != 0)
            {
                if (BreakLine[1] < StartLineR + 3)
                {
                    BreakLine[1] = 0;
                    BreakPoint[1] = 0;
                }
                if (BreakLine[1] != 0)
                {
                    for (j = BreakLine[1] - 1; j >= BreakLine[1] - 4; j--)
                    {
                        if (!isWhite(j, BreakPoint[1] - 1))
                        {
                            BreakLine[1] = 0;
                            BreakPoint[1] = 0;
                            break;
                        }
                    }
                }
                if (BreakLine[1] != 0)
                {
                    for (k = BreakLine[1]; k <= ImageH - 1; k++)
                    {
                        if (imageLine.Point_Right[k] == BreakPoint[1])
                        {
                            BreakLine[0] = k;
                            BreakPoint[0] = imageLine.Point_Right[k];
                        }
                        else
                            break;
                    }
                    flag_break = true;
                }
            }
        }
        //状态2：找到3个逆趋势点。
        //任务：在逆趋势点下方找到3个正趋势点，并记录下来用于斜率判别
        if (flag_right2 && !flag_left1)
        {
            //先找逆趋势点下方最下点
            for (j = BasisLine_Up[0]; j <= ImageH - 1; j++)
            {
                if (imageLine.Exist_Right[j])
                {
                    for (k = j + 1; k <= MIN(j + 3, ImageH - 1); k++)
                    {
                        if (imageLine.Exist_Right[k])
                        {
                            if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                            {
                                BasisLine_Down[0] = j;
                                BasisPoint_Down[0] = imageLine.Point_Right[j];
                                BasisLine_Down[1] = k;
                                BasisPoint_Down[1] = imageLine.Point_Right[k];
                            }
                            break;
                        }
                    }
                    if (BasisLine_Down[0] != 0)
                        break;
                }
            }
            if (BasisLine_Down[0] != 0)
            {
                //找到最下点后，向下找2个正趋势点
                for (j = BasisLine_Down[0] + 1; j <= MIN(BasisLine_Down[0] + 7, ImageH - 1); j++)
                {
                    if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                        break;
                    if (imageLine.Exist_Right[j])
                    {
                        for (k = j + 1; k <= MIN(j + 3, ImageH - 1); k++)
                        {
                            if (imageLine.Exist_Right[k])
                            {
                                if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                                {
                                    BasisLine_Down[1] = j;
                                    BasisPoint_Down[1] = imageLine.Point_Right[j];
                                    BasisLine_Down[2] = k;
                                    BasisPoint_Down[2] = imageLine.Point_Right[k];
                                    flag_left1 = true;
                                }
                                if (imageLine.Point_Right[k] < imageLine.Point_Right[j])//找到3个正趋势点之前出现趋势逆转，判定为假拐点
                                {
                                    flag_err2 = true;
                                    flag_left1 = false;
                                }
                                break;
                            }
                        }
                        if (flag_left1 || flag_err2)
                            break;
                    }
                }
            }
            if (!flag_left1)//没有找到3个正趋势点，清缓存，继续找
            {
                i = BasisLine_Up[2] + 1;
                BasisLine_Up[0] = 0; BasisLine_Up[1] = 0; BasisLine_Up[2] = 0;
                BasisPoint_Up[0] = 0; BasisPoint_Up[1] = 0; BasisPoint_Up[2] = 0;
                BasisLine_Down[0] = 0; BasisLine_Down[1] = 0; BasisLine_Down[2] = 0;
                BasisPoint_Down[0] = 0; BasisPoint_Down[1] = 0; BasisPoint_Down[2] = 0;
                flag_err1 = false; flag_err2 = false; flag_right2 = false; flag_left1 = false;
                continue;
            }
        }
        //状态3：找到断点。
        //任务：在断点下方找到3个正趋势点。
        if (flag_break && !flag_left1)
        {
            if (BreakLine[0] - BreakLine[1] > 5)
            {
                BasisLine_Down[0] = BreakLine[1];
                BasisPoint_Down[0] = BreakPoint[1];
            }
            else
            {
                BasisLine_Down[0] = BreakLine[0];
                BasisPoint_Down[0] = BreakPoint[0];
            }
            for (j = BreakLine[0] + 1; j <= ImageH - 1; j++)
            {
                if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                {
                    flag_err2 = true;
                    break;
                }
                if (imageLine.Exist_Right[j])
                {
                    BasisLine_Down[1] = j;
                    BasisPoint_Down[1] = imageLine.Point_Right[j];
                    break;
                }
            }
            if (BasisLine_Down[1] != 0)
            {
                for (j = BasisLine_Down[1]; j <= ImageH - 1; j++)
                {
                    if (!imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2])
                    {
                        flag_err2 = true;
                        break;
                    }
                    if (imageLine.Exist_Right[j])
                    {
                        for (k = j + 1; k <= ImageH - 1; k++)
                        {
                            if (!imageLine.Exist_Right[k] && !imageLine.Exist_Right[k - 1] && !imageLine.Exist_Right[k - 2])
                                break;
                            if (imageLine.Exist_Right[k])
                            {
                                if (BasisPoint_Down[1] > BasisPoint_Down[0])
                                {
                                    if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                                    {
                                        BasisLine_Down[2] = k;
                                        BasisPoint_Down[2] = imageLine.Point_Right[k];
                                        flag_enough = true;
                                    }
                                    if (imageLine.Point_Right[k] < imageLine.Point_Right[j])
                                        flag_err2 = true;
                                }
                                else
                                {
                                    if (imageLine.Point_Right[k] < imageLine.Point_Right[j])
                                    {
                                        BasisLine_Down[2] = k;
                                        BasisPoint_Down[2] = imageLine.Point_Right[k];
                                        flag_enough = true;
                                    }
                                    if (imageLine.Point_Right[k] > imageLine.Point_Right[j])
                                        flag_err2 = true;
                                }
                                break;
                            }
                        }
                        if (flag_enough || flag_err2)
                            break;
                    }
                }
            }
            else
            {
                BasisLine_Down[1] = BreakLine[0];
                BasisPoint_Down[1] = BreakPoint[0];
                BasisLine_Down[2] = BreakLine[0];
                BasisPoint_Down[2] = BreakPoint[0];
                flag_enough = true;
            }
            if (!flag_enough)
            {
                if (!flag_err2)
                {
                    BasisLine_Down[2] = BasisLine_Down[1];
                    BasisPoint_Down[2] = BasisPoint_Down[1];
                    flag_enough = true;
                }
                else
                    return;
            }
        }
        //状态4：找到3个逆趋势点，且其下方有3个正趋势点。
        //任务：返回上、下拐点、k、b
        if (flag_right2 && flag_left1)
        {
            *TurnPoint_Row = BasisLine_Down[0];
            *TurnPoint = BasisPoint_Down[0];
            for (j = BasisLine_Up[1]; j > MAX(BasisLine_Up[1] - 10, ImageH_6); j--)
            {
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] == BasisPoint_Up[1])
                        BasisLine_Up[1] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Up[2]; j > MAX(BasisLine_Up[2] - 10, ImageH_6); j--)
            {
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] == BasisPoint_Up[2])
                        BasisLine_Up[2] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Down[1]; j < MIN(BasisLine_Down[1] + 10, ImageW); j++)
            {
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] == BasisPoint_Down[1])
                        BasisLine_Down[1] = j;
                    else
                        break;
                }
            }
            for (j = BasisLine_Down[2]; j < MIN(BasisLine_Down[2] + 10, ImageW); j++)
            {
                if (imageLine.Exist_Right[j])
                {
                    if (imageLine.Point_Right[j] == BasisPoint_Down[2])
                        BasisLine_Down[2] = j;
                    else
                        break;
                }
            }
            leastSquareMethod(BasisLine_Up, BasisPoint_Up, 3, kr_up, br_up);
            leastSquareMethod(BasisLine_Down, BasisPoint_Down, 3, kr_down, br_down);
            for (j = BasisLine_Down[2]; j > BasisLine_Up[2]; j--)
            {
                if (imageLine.Exist_Right[j] && imageLine.Exist_Right[j - 1] &&
                    ABS(imageLine.Point_Right[j] - imageLine.Point_Right[j - 1]) < 2)
                    err_cnt++;
            }
            *error_cnt_r = err_cnt;
            return;
        }
        //状态5：找到断点。
        //任务：返回断点、k、b
        if (flag_break && flag_enough)
        {
            if (BasisLine_Down[1] != BreakLine[0])
            {
                *TurnPoint_Row = BreakLine[1];
                *TurnPoint = BreakPoint[1];
                for (j = BasisLine_Down[1]; j < MIN(BasisLine_Down[1] + 10, ImageH); j++)
                {
                    if (imageLine.Exist_Right[j])
                    {
                        if (imageLine.Point_Right[j] == BasisPoint_Down[1])
                            BasisLine_Down[1] = j;
                        else
                            break;
                    }
                }
                for (j = BasisLine_Down[2]; j < MIN(BasisLine_Down[2] + 10, ImageH); j++)
                {
                    if (imageLine.Exist_Right[j])
                    {
                        if (imageLine.Point_Right[j] == BasisPoint_Down[2])
                            BasisLine_Down[2] = j;
                        else
                            break;
                    }
                }
                *kr_up = 0; *br_up = 0;
                leastSquareMethod(BasisLine_Down, BasisPoint_Down, 3, kr_down, br_down);
            }
            else
            {
                *TurnPoint_Row = BreakLine[1];
                *TurnPoint = BreakPoint[1];
                *kr_up = 0; *br_up = 0; *kr_down = 0; *br_down = BreakPoint[1];
            }
            return;
        }
    }
}


// **************************************************************十字**************************************
/*返回某行右半边或左半边的白点数*/
uint8_t road_right(uint8_t row)
{
    uint8_t i, white = 0;
    for (i = 47; i < ImageW; i++)
    {
        if (isWhite(row, i))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white;
}
uint8_t road_left(uint8_t row)
{
    uint8_t i, white = 0;
    for (i = 47; i > 0; i--)
    {
        if (isWhite(row, i))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white;
}


uint8_t Point_LeftRight(uint8_t row)//从右边开始找第一个左边界
{
    uint8 i = 0, j = 0;
    for (i = ImageW - 1;i > 0;i--)
    {
        if (isWhite(row, i))//右边第一个白点
            break;
    }
    for (j = i - 1;j < ImageW;j--)
    {
        if (!isWhite(row, j))
            break;

    }
    return j + 1;
}
uint8_t Point_RightLeft(uint8_t row)//从左边开始找第一个右边界
{
    uint8 white = 0, i = 0, j = 0;
    for (i = 0;i < ImageW;i++)
    {

        if (isWhite(row, i))//左边第一个白点
            break;
    }
    for (j = i + 1;j < ImageW;j++)
    {
        if (isWhite(row, j))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white + i;
}
/***************************************************************************
 * 函数名称：RoadRight
 * 函数说明：从右边第一个黑点开始计算的宽度
 * 参数说明：row:行数
 * 函数返回：该行的右边路宽
 * 函数备注：*
 ***************************************************************************/

uint8_t RoadRight(uint8_t row)
{
    uint8 white = 0, i = 0, j = 0;
    for (i = ImageW - 1;i > 0;i--)
    {
        if (isWhite(row, i))//右边第一个白点
            break;
    }
    for (j = i;j < ImageW;j--)
    {
        if (isWhite(row, j))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white;
}

/***************************************************************************
 * 函数名称：RoadLeft
 * 函数说明：从左边第一个白点开始计算的宽度
 * 参数说明：row:行数
 * 函数返回：该行的左边路宽
 * 函数备注：*
 ***************************************************************************/
int8_t RoadLeft(uint8_t row)
{
    uint8 white = 0, i = 0, j = 0;
    for (i = 0;i < ImageW;i++)
    {
        if (isWhite(row, i))//左边第一个白点
            break;
    }
    for (j = i;j < ImageW;j++)
    {
        if (isWhite(row, j))
        {
            white++;
        }
        else
        {
            break;
        }
    }
    return white;
}


/****************************************圆环******************************************************/
/*左圆环打包函数*/
void Left_Ring(void)
{
    if (Flag.Right_Ring || ring_cam.Left_Ring_Clc <= 0)
        return;

    Left_Ring_Find();
    Left_Ring_Turn();
    Left_Ring_Out();
    Left_Ring_Out_Mend1();
    Left_Ring_Out_Mend2();

    if (Flag.Left_Ring)
    {
        AIM_LINE = AIM_LINE_SET - ring_cam.Ring_AIM_DELTA[(ring_cam.Left_Ring_Type >>(ring_cam.Left_Ring_Clc-1 )) & 1];
    }

}
/*判断左圆环及出口补线*/
void Left_Ring_Find(void)
{
    short i = 0, j = 0;
    short Down_Point = EFFECTIVE_ROW, Divide_Point = EFFECTIVE_ROW, Mid_Point = EFFECTIVE_ROW, Top_Point = EFFECTIVE_ROW;
    short L_Max = 0;

    if (Flag.Left_Ring_Turn || Flag.Left_Ring_Out || Flag.Left_Ring_Out_Mend1 || Flag.Left_Ring_Out_Mend2)//在圆环的后状态则不执行找圆环
        return;
    /************************找环岛下直道端点***********************/
    if (LTurnPoint_Row >= ImageH_3 && Kl_down < 0)
        Down_Point = LTurnPoint_Row;
    if (Down_Point != EFFECTIVE_ROW)
    {
        short e_cnt = 0;
        for (i = Down_Point; i >= Down_Point - 15; i--)
        {
            if (imageLine.Exist_Left[i] && imageLine.Point_Left[i] <= imageLine.Point_Left[Down_Point]
                && imageLine.Point_Left[Down_Point] - imageLine.Point_Left[i] < 10)
                e_cnt++;
        }
        if (e_cnt > 7)
            Down_Point = EFFECTIVE_ROW;
    }
    /************************找环岛下方直道分界点***********************/
    if (Down_Point != EFFECTIVE_ROW)
    {
        for (i = Down_Point - 3; i > ImageH_6; i--)
        {
            if (imageLine.Exist_Left[i] && imageLine.Point_Left[i] > imageLine.Point_Left[Down_Point])
            {
                Divide_Point = i;
                break;
            }
        }
    }
    else if (Flag.Left_Ring_Find)
    {
        for (j = ImageH - 15; j > ImageH_6; j--)
        {
            if (imageLine.Exist_Left[j] && imageLine.Point_Left[j] > 2)
            {
                Divide_Point = j;
                break;
            }
        }
    }
    else
    {
        bool count_flag = false;
        for (i = ImageH - 5; i >= ImageH - 10; i--)
        {
            count_flag = true;
            for (j = 1; j <= 10; j++)
            {
                if (!isWhite(i, j))
                {
                    count_flag = false;
                    break;
                }
            }
            if (count_flag)
                break;
        }
        if (!count_flag)
            return;
    }
    /************************找环岛转折点***********************/
    if (Divide_Point != EFFECTIVE_ROW)
    {
        for (i = Divide_Point; i > ImageH_6; i--)
        {
            if (imageLine.Exist_Left[i] && imageLine.Point_Left[i] > L_Max)
            {
                Mid_Point = i;
                L_Max = imageLine.Point_Left[i];
            }
            if (!imageLine.Exist_Left[i] || imageLine.Point_Left[i] < L_Max)
                break;
        }
    }
    /************************识别到圆环之后丢失转折点***********************/
    if ((Mid_Point == EFFECTIVE_ROW || imageLine.Point_Left[Mid_Point] <= 2)
        && Down_Point != EFFECTIVE_ROW && Flag.Left_Ring_Find)
    {
        float k_Left = 0, b_Left = 0;
        k_Left = ((float)imageLine.Point_Left[Down_Point] - (float)5)
            / ((float)Down_Point - (float)(ImageH - 1));
        b_Left = (float)5 - k_Left * (ImageH - 1);
        for (i = Down_Point; i >= 0; i--)
        {
            imageLine.Exist_Left[i] = true;
            imageLine.Point_Left[i] = getLineValue(i, k_Left, b_Left);
        }
        return;
    }
    /************************找环岛上直道端点***********************/
    for (i = Mid_Point - 1; i > EFFECTIVE_ROW; i--)
    {
        if (!imageLine.Exist_Left[i] || (imageLine.Point_Left[i] - imageLine.Point_Left[Mid_Point] < -5))
        {
            for (j = i - 1; j > EFFECTIVE_ROW; j--)
            {
                if (imageLine.Exist_Left[j] && (imageLine.Point_Left[Mid_Point] - imageLine.Point_Left[j] < -3))
                {
                    Top_Point = j;
                    break;
                }
            }
        }
        if (Top_Point != EFFECTIVE_ROW)
            break;
    }
    /************************防止误触***********************/
    if (!Flag.Left_Ring_Find)
    {
        if (Flag.Garage_Find)
            return;
        if (Mid_Point == EFFECTIVE_ROW || imageLine.Point_Left[Mid_Point] <= 2)
            return;
        if (!RightLine_Check(Top_Point))
            return;
        if (Down_Point != EFFECTIVE_ROW && (ABS(Down_Point - Mid_Point) < 15 ||
            ABS(imageLine.Point_Left[Mid_Point] - imageLine.Point_Left[Down_Point]) > 30))
            return;
        if (Top_Point != EFFECTIVE_ROW)
        {
            if ((Down_Point - Mid_Point) - (Mid_Point - Top_Point) <= 10)
                return;
        }
    }
    else if (Flag.Garage_Go)
    {
        Flag.Left_Ring_Find = false;
        Flag.Left_Ring = false;
        return;
    }
    /************************状态转换***********************/
    if (Flag.Left_Ring_Find && Top_Point != EFFECTIVE_ROW && imageLine.Point_Left[Top_Point] > 2
        && (Down_Point == EFFECTIVE_ROW || imageLine.Point_Left[Down_Point] <= 2))
    {
        if (((ring_cam.Left_Ring_Type >> (ring_cam.Left_Ring_Clc-1 )) & 1) == 0)
        {
            if (Mid_Point >= ring_cam.StartTurn_Line[0])
            {
                Flag.Left_Ring_Find = false;
                Flag.Left_Ring_Turn = true;
                return;
            }
        }
        else if (((ring_cam.Left_Ring_Type >> (ring_cam.Left_Ring_Clc -1)) & 1) == 1)
        {
            if (Mid_Point >= ring_cam.StartTurn_Line[1])
            {
                Flag.Left_Ring_Find = false;
                Flag.Left_Ring_Turn = true;
                return;
            }
        }
    }
    /************************补线及滤波***********************/
    if (Down_Point != EFFECTIVE_ROW)//滤除拐点上方的左右边界点 防止导致中线拟合出问题
    {
        for (i = Down_Point - 1; i >= 0; i--)
        {
            imageLine.Exist_Left[i] = false;
        }
        float k_Left = 0, b_Left = 0;//用上下拐点来补线
        k_Left = ((float)imageLine.Point_Left[Down_Point] - (float)imageLine.Point_Left[Mid_Point])
            / ((float)Down_Point - (float)Mid_Point);
        b_Left = (float)imageLine.Point_Left[Mid_Point] - k_Left * Mid_Point;
        //开始补线
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (!imageLine.Exist_Left[i] || (i<Down_Point))
            {
                imageLine.Exist_Left[i] = true;
                imageLine.Point_Left[i] = getLineValue(i, k_Left, b_Left);
                Flag.Left_Ring_Find = true;
                Flag.Left_Ring = true;
            }
        }
    }
    else //下方拐点丢失 从上方拐点进行向下拉线补线
    {
        float k_left = 0, b_left = 0;
        k_left = ((float)10 - (float)imageLine.Point_Left[Mid_Point])
            / ((float)(ImageH - 1) - (float)Mid_Point);
        b_left = (float)imageLine.Point_Left[Mid_Point] - k_left * Mid_Point;

        //开始补线
        for (i = ImageH - 1; i >= 0; i--)
        {
            imageLine.Exist_Left[i] = true;
            imageLine.Point_Left[i] = getLineValue(i, k_left, b_left);
            Flag.Left_Ring_Find = true;
            Flag.Left_Ring = true;
        }
    }
}
/*补线进左圆环*/
void Left_Ring_Turn(void)
{
    short i = 0, j = 0;
    short Top_Point = ImageW, Top_Point_row = EFFECTIVE_ROW, Divide_Point = EFFECTIVE_ROW;
    short Endpoint_R = ImageW, Startline_R = 0;
    bool count_flag = false;

    if (!Flag.Left_Ring_Turn)
        return;
    /************************找环岛上端点***********************/
    for (i = ImageH_5_6; i > ImageH_6; i--)
    {
        if (imageLine.Exist_Left[i] && !imageLine.Exist_Left[i - 1] && !imageLine.Exist_Left[i - 2])
        {
            Divide_Point = i;
            break;
        }
        if (!imageLine.Exist_Left[i] && !imageLine.Exist_Left[i - 1] && !imageLine.Exist_Left[i - 2] &&
            !imageLine.Exist_Left[i - 3] && !imageLine.Exist_Left[i - 4] && !imageLine.Exist_Left[i - 5])
            break;
    }
    /************************找环岛上直道断点***********************/
    if (Divide_Point != EFFECTIVE_ROW)
    {
        for (i = Divide_Point - 1; i > EFFECTIVE_ROW; i--)
        {
            if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] &&
                imageLine.Point_Left[i] > ImageW / 4 && imageLine.Point_Left[i - 1] > ImageW / 4 &&
                (imageLine.Point_Left[i] - imageLine.Point_Left[i - 1] >= -2))
            {
                Top_Point_row = i;
                Top_Point = imageLine.Point_Left[i];
                break;
            }
        }
    }
    else
    {
        for (i = ImageH_5_6; i > EFFECTIVE_ROW; i--)
        {
            if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] &&
                imageLine.Point_Left[i] > ImageW / 4 && imageLine.Point_Left[i - 1] > ImageW / 4 &&
                (imageLine.Point_Left[i] - imageLine.Point_Left[i - 1] >= -2))
            {
                Top_Point_row = i;
                Top_Point = imageLine.Point_Left[i];
                break;
            }
        }
    }
    for (i = Top_Point_row; i < ImageH; i++)
    {
        for (j = Top_Point; j > Top_Point - 15; j--)
        {
            count_flag = true;
            if (!isWhite(i, j))
            {
                count_flag = false;
                break;
            }
        }
        if (count_flag)
        {
            Top_Point_row = i - 1;
            break;
        }
    }
    for (j = Top_Point + 1; j > Top_Point - 15; j--)
    {
        if (isWhite(Top_Point_row, j) && !isWhite(Top_Point_row, j - 1))
        {
            Top_Point = j;
            break;
        }
    }
    /************************状态转换***********************/
    for (i = ImageH - 5; i > ImageH_4; i--)
    {
        if (imageLine.Exist_Right[i])
        {
            Startline_R = i;
            break;
        }
    }

    for (j = Startline_R - 1; j > ImageH_4; j--)
    {
        if (imageLine.Exist_Right[j] && imageLine.Exist_Right[j + 1] && imageLine.Exist_Right[j - 1] &&
            (ABS((imageLine.Point_Right[j - 1] + imageLine.Point_Right[j + 1]) / 2 - imageLine.Point_Right[j]) < 10))
        {
            Endpoint_R = imageLine.Point_Right[j - 1];
        }
        else
            break;
    }
    if (Endpoint_R < ImageW_3 && Startline_R >= ImageH_5_6)
    {
        Flag.Left_Ring_Turn = false;
        Flag.Left_Ring_Out = true;
        return;
    }
    /************************补线及滤波***********************/
    if (Top_Point_row == EFFECTIVE_ROW)
        return;

    float k_right = 0, b_right = 0;
    k_right = ((float)constrain_int16((Top_Point + 30), 89, 109) - (float)Top_Point)
        / ((float)(ImageH - 1) - (float)Top_Point_row);
    b_right = (float)constrain_int16((Top_Point + 30), 89, 109) - k_right * (ImageH - 1);
    for (i = ImageH - 1; i >= 0; i--)
    {
        if (i >= Top_Point_row)
        {
            imageLine.Exist_Right[i] = true;
            imageLine.Point_Right[i] = getLineValue(i, k_right, b_right);
        }
        else
        {
            if (((ring_cam.Left_Ring_Type >> (ring_cam.Left_Ring_Clc-1 )) & 1) == 0)
            {
                imageLine.Exist_Right[i] = true;
                imageLine.Point_Right[i] = (ring_cam.Turn_Upslope[0] * k_right) * i +
                    (b_right - (ring_cam.Turn_Upslope[0] - 1) * k_right * Top_Point_row);
                if (imageLine.Point_Left[i] >= imageLine.Point_Right[i] - 15)
                    imageLine.Exist_Left[i] = false;
            }
            else if (((ring_cam.Left_Ring_Type >> (ring_cam.Left_Ring_Clc-1 )) & 1) == 1)
            {
                imageLine.Exist_Right[i] = true;
                imageLine.Point_Right[i] = (ring_cam.Turn_Upslope[1] * k_right) * i +
                    (b_right - (ring_cam.Turn_Upslope[1] - 1) * k_right * Top_Point_row);
                if (imageLine.Point_Left[i] >= imageLine.Point_Right[i] - 15)
                    imageLine.Exist_Left[i] = false;
            }
        }
    }
}
/*出左圆环补线*/
void Left_Ring_Out(void)
{
    short i = 0, j = 0, k = 0;
    short TurnPoint_Row = EFFECTIVE_ROW, StratLine_R = EFFECTIVE_ROW, EndLine_R = EFFECTIVE_ROW;

    if (!Flag.Left_Ring_Out)
        return;
    /************************找拐点***********************/
    TurnPoint_Row = RTurnPoint_Row;
    /************************状态转换***********************/
    for (i = ImageH - 1; i >= ImageH_2; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] && imageLine.Exist_Right[i - 2])
        {
            StratLine_R = i;
            break;
        }
    }
    if (StratLine_R <= 40  || TurnPoint_Row >= 50)
    {
        Flag.Left_Ring_Out = false;
        Flag.Left_Ring_Out_Mend1 = true;
        return;
    }
    /************************特殊滤波***********************/
    for (j = StratLine_R; j >= EFFECTIVE_ROW; j--)
    {
        if (imageLine.Exist_Right[j] && !imageLine.Exist_Right[j - 1] && !imageLine.Exist_Right[j - 2] &&
            !imageLine.Exist_Right[j - 3] && !imageLine.Exist_Right[j - 4] && !imageLine.Exist_Right[j - 5])
        {
            EndLine_R = j;
            break;
        }
    }
    if (EndLine_R != EFFECTIVE_ROW)
    {
        for (k = EndLine_R + 5; k >= 0; k--)
        {
            if (k <= EndLine_R)
                imageLine.Exist_Right[k] = false;
            imageLine.Exist_Left[k] = false;
        }
    }
    if (TurnPoint_Row == EFFECTIVE_ROW)
        return;
    /************************补线及滤波***********************/
    float k_right = 0, b_right = 0;
    short MendBasis_Right[2][5];
    short count = 0;
    for (i = TurnPoint_Row; i <= TurnPoint_Row + 15; i++)
    {
        if (imageLine.Exist_Right[i])
        {
            MendBasis_Right[0][count] = (short)i;
            MendBasis_Right[1][count] = (short)imageLine.Point_Right[i];
            count++;
        }
        if (count == 5)
            break;
    }
    if (count == 5)    //有5个点即可开始补线
    {
        leastSquareMethod(MendBasis_Right[0], MendBasis_Right[1], 5, &k_right, &b_right);
        for (i = TurnPoint_Row - 1; i >= 0; i--)
        {
            if (((ring_cam.Left_Ring_Type >> (ring_cam.Left_Ring_Clc-1 )) & 1) == 0)
            {
                imageLine.Exist_Right[i] = true;
                imageLine.Point_Right[i] = (ring_cam.Out_Slope[0] * k_right) * i +
                    (b_right - (ring_cam.Out_Slope[0] - 1) * k_right * TurnPoint_Row);
            }
            else if (((ring_cam.Left_Ring_Type >> (ring_cam.Left_Ring_Clc-1 )) & 1) == 1)
            {
                imageLine.Exist_Right[i] = true;
                imageLine.Point_Right[i] = (ring_cam.Out_Slope[1] * k_right) * i +
                    (b_right - (ring_cam.Out_Slope[1] - 1) * k_right * TurnPoint_Row);
            }
            if (!isWhite(i, imageLine.Point_Right[i]))
            {
                imageLine.Exist_Right[i] = false;
                break;
            }
        }
        for (j = i + 5; j >= 0; j--)
        {
            if (j >= ImageH_4)
            {
                if (imageLine.Exist_Left[j] && isWhite(j + 3, imageLine.Point_Left[j]))
                    imageLine.Exist_Left[j] = false;
            }
            else
                imageLine.Exist_Left[j] = false;
            if (j <= i)
            {
                if (imageLine.Exist_Right[j] >= imageLine.Point_Right[i])
                    imageLine.Exist_Right[j] = false;
            }
        }
    }
}
/*出左圆环无图像固定打角*/
void Left_Ring_Out_Mend1(void)
{
    short i = 0, j = 0;
    short StartLine_R = EFFECTIVE_ROW;

    if (!Flag.Left_Ring_Out_Mend1)
        return;
    /************************排除右下角边界的影响***********************/
    for (i = ImageH - 1; i >= ImageH - 20; i--)
    {
        if (imageLine.Exist_Right[i])
        {
            for (j = i - 1; j >= i - 3; j--)
            {
                if (isWhite(j, imageLine.Point_Right[i]))
                    return;
            }
        }
    }
    /************************最下右边界点位置***********************/
    for (i = ImageH - 1; i >= ImageH_6; i--)
    {
        if (imageLine.Exist_Right[i])
        {
            StartLine_R = i;
            break;
        }
    }
    /************************状态转换***********************/
    if (StartLine_R >= ImageH - 15)
    {
        Flag.Left_Ring_Out_Mend1 = false;
        Flag.Left_Ring_Out_Mend2 = true;
    }
}
/*出左圆环入口补线*/
void Left_Ring_Out_Mend2(void)
{
    short i = 0, j = 0, W_cnt = 0;
    bool count_flag = 0;
    short Top_Point = EFFECTIVE_ROW, Startline_L = EFFECTIVE_ROW;

    if (!Flag.Left_Ring_Out_Mend2)
        return;
    /************************找圆环出口以作分界***********************/
    for (i = ImageH - 5; i >= ImageH_4; i--)
    {
        for (j = 1; j <= 20; j++)
        {
            if (!isWhite(i, j))
            {
                count_flag = false;
                break;
            }
            count_flag = true;
        }
        if (count_flag)
        {
            W_cnt++;
            if (W_cnt >= 5)
            {
                Startline_L = i;
                break;
            }
        }
    }
    /************************找圆环上端点***********************/
    for (i = Startline_L; i > ImageH_6; i--)
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] && imageLine.Exist_Left[i - 2] &&
            imageLine.Point_Left[i] > 10 && imageLine.Point_Left[i - 1] > 10 && imageLine.Point_Left[i - 2] > 10 &&
            ABS(imageLine.Point_Left[i - 1] - imageLine.Point_Left[i]) < 2 &&
            ABS(imageLine.Point_Left[i - 2] - imageLine.Point_Left[i - 1]) < 2)
        {
            Top_Point = i;
            break;
        }
    }
    /************************状态转换***********************/
    if (Top_Point >= 45 || Startline_L == EFFECTIVE_ROW)
    {
        Flag.Left_Ring_Out_Mend2 = false;
        Flag.Left_Ring_OnlyOnce = false;
        ring_cam.Left_Ring_Clc--;
        Flag.Left_Ring = false;
        Flag.Left_Ring_Out_Record = true;
        
        //if(fork_param.ForkRoad_Position == 1)                         //三叉状态更新
            //Flag.ForkRoad_OnlyOnce = true;
        return;
    }
    /************************补线滤波***********************/
    if (Top_Point == EFFECTIVE_ROW)
        return;

    float k_left = 0, b_left = 0;
    short MendBasis_left[2][5];
    int count = 0;
    for (i = Top_Point; i >= Top_Point - 10; i--)
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] &&
            ABS(imageLine.Point_Left[i - 1] - imageLine.Point_Left[i]) < 2)
        {
            MendBasis_left[0][count] = (short)i;
            MendBasis_left[1][count] = (short)imageLine.Point_Left[i];
            count++;
        }
        if (count == 5)
            break;
    }
    if (count == 5)    //有5个点即可开始补线
    {
        leastSquareMethod(MendBasis_left[0], MendBasis_left[1], 5, &k_left, &b_left);
        if (k_left >= 0.5)
        {
            k_left = ((float)5 - (float)imageLine.Point_Left[Top_Point])
                / ((float)(ImageH - 1) - (float)Top_Point);
            b_left = (float)imageLine.Point_Left[Top_Point] - k_left * Top_Point;
        }
    }
    else
    {
        k_left = ((float)5 - (float)imageLine.Point_Left[Top_Point])
            / ((float)(ImageH - 1) - (float)Top_Point);
        b_left = (float)imageLine.Point_Left[Top_Point] - k_left * Top_Point;
    }
    //开始补线
    for (i = ImageH - 1; i >= Top_Point; i--)
    {
        imageLine.Exist_Left[i] = true;
        imageLine.Point_Left[i] = getLineValue(i, k_left, b_left);
    }
}
/*右圆环打包函数*/
void Right_Ring(void)
{
    if (Flag.Left_Ring  || ring_cam.Right_Ring_Clc <= 0)
        return;

    Right_Ring_Find();
    Right_Ring_Turn();
    Right_Ring_Out();
    Right_Ring_Out_Mend1();
    Right_Ring_Out_Mend2();
    if (Flag.Right_Ring)
    {
        AIM_LINE = AIM_LINE_SET-ring_cam.Ring_AIM_DELTA[(ring_cam.Right_Ring_Type >>(ring_cam.Right_Ring_Clc-1 )) & 1];
    }

}
/*判断右圆环及出口补线*/
void Right_Ring_Find(void)
{
    short i = 0, j = 0;
    short Down_Point = EFFECTIVE_ROW, Divide_Point = EFFECTIVE_ROW, Mid_Point = EFFECTIVE_ROW, Top_Point = EFFECTIVE_ROW;
    short R_Min = ImageW;

    if (Flag.Right_Ring_Turn || Flag.Right_Ring_Out || Flag.Right_Ring_Out_Mend1 || Flag.Right_Ring_Out_Mend2)//在圆环的后状态则不执行找圆环
        return;
    /************************找环岛下直道端点***********************/
    if (RTurnPoint_Row >= ImageH_3 && Kr_down > 0)
        Down_Point = RTurnPoint_Row;
    if (Down_Point != EFFECTIVE_ROW)
    {
        short e_cnt = 0;
        for (i = Down_Point; i >= Down_Point - 15; i--)
        {
            if (imageLine.Exist_Right[i] && imageLine.Point_Right[i] >= imageLine.Point_Right[Down_Point]
                && imageLine.Point_Right[i] - imageLine.Point_Right[Down_Point] < 10)
                e_cnt++;
        }
        if (e_cnt > 7)
            Down_Point = EFFECTIVE_ROW;
    }
    /************************找环岛下方直道分界点***********************/
    if (Down_Point != EFFECTIVE_ROW)
    {
        for (i = Down_Point - 3; i > ImageH_6; i--)
        {
            if (imageLine.Exist_Right[i] && imageLine.Point_Right[i] < imageLine.Point_Right[Down_Point])
            {
                Divide_Point = i;
                break;
            }
        }
    }
    else if (Flag.Right_Ring_Find)
    {
        for (j = ImageH - 15; j > ImageH_6; j--)
        {
            if (imageLine.Exist_Right[j] && imageLine.Point_Right[j] < ImageW - 2)
            {
                Divide_Point = j;
                break;
            }
        }
    }
    else
    {
        bool count_flag = false;
        for (i = ImageH - 5; i >= ImageH - 10; i--)
        {
            count_flag = true;
            for (j = ImageW - 1; j >= ImageW - 10; j--)
            {
                if (!isWhite(i, j))
                {
                    count_flag = false;
                    break;
                }
            }
            if (count_flag)
                break;
        }
        if (!count_flag)
            return;
    }
    /************************找环岛转折点***********************/
    if (Divide_Point != EFFECTIVE_ROW)
    {
        for (i = Divide_Point; i > ImageH_6; i--)
        {
            if (imageLine.Exist_Right[i] && imageLine.Point_Right[i] < R_Min)
            {
                Mid_Point = i;
                R_Min = imageLine.Point_Right[i];
            }
            if (!imageLine.Exist_Right[i] || imageLine.Point_Right[i] > R_Min)
                break;
        }
    }
    /************************识别到圆环之后丢失转折点***********************/
    if ((Mid_Point == EFFECTIVE_ROW || imageLine.Point_Right[Mid_Point] >= ImageW - 2)
        && Down_Point != EFFECTIVE_ROW && Flag.Right_Ring_Find)
    {
        float k_Right = 0, b_Right = 0;
        k_Right = ((float)imageLine.Point_Right[Down_Point] - (float)89)
            / ((float)Down_Point - (float)ImageH);
        b_Right = (float)89 - k_Right * ImageH;
        for (i = Down_Point; i >= 0; i--)
        {
            imageLine.Exist_Right[i] = true;
            imageLine.Point_Right[i] = getLineValue(i, k_Right, b_Right);
        }
        return;
    }
    /************************找环岛上直道端点***********************/
    for (i = Mid_Point - 1; i > EFFECTIVE_ROW; i--)
    {
        if (!imageLine.Exist_Right[i] || (imageLine.Point_Right[i] - imageLine.Point_Right[Mid_Point] > 5))
        {
            for (j = i - 1; j > EFFECTIVE_ROW; j--)
            {
                if (imageLine.Exist_Right[j] && (imageLine.Point_Right[Mid_Point] - imageLine.Point_Right[j] > 3))
                {
                    Top_Point = j;
                    break;
                }
            }
        }
        if (Top_Point != EFFECTIVE_ROW)
            break;
    }
    /************************防止误触***********************/
    if (!Flag.Right_Ring_Find)
    {
        if (Flag.Garage_Find)
            return;
        if (Mid_Point == EFFECTIVE_ROW || imageLine.Point_Right[Mid_Point] >= ImageW - 2)
            return;
        if (!LeftLine_Check(Top_Point))
            return;
        if (Down_Point != EFFECTIVE_ROW && (ABS(Down_Point - Mid_Point) < 15 ||
            ABS(imageLine.Point_Right[Mid_Point] - imageLine.Point_Right[Down_Point]) > 30))
            return;
        if (Top_Point != EFFECTIVE_ROW)
        {
            if ((Down_Point - Mid_Point) - (Mid_Point - Top_Point) <= 8)
                return;
        }
    }
    else if (Flag.Garage_Go)
    {
        Flag.Right_Ring_Find = false;
        Flag.Right_Ring = false;
        return;
    }
    /************************状态转换***********************/
    if (Flag.Right_Ring_Find && Top_Point != EFFECTIVE_ROW && imageLine.Point_Right[Top_Point] < ImageW - 2
        && (Down_Point == EFFECTIVE_ROW || imageLine.Point_Right[Down_Point] >= ImageW - 2))
    {
        if (((ring_cam.Right_Ring_Type >> (ring_cam.Right_Ring_Clc -1)) & 1) == 0)
        {
            if (Mid_Point >= ring_cam.StartTurn_Line[0])
            {
                Flag.Right_Ring_Find = false;
                Flag.Right_Ring_Turn = true;
                return;
            }
        }
        else if (((ring_cam.Right_Ring_Type >> (ring_cam.Right_Ring_Clc -1)) & 1) == 1)
        {
            if (Mid_Point >= ring_cam.StartTurn_Line[1])
            {
                Flag.Right_Ring_Find = false;
                Flag.Right_Ring_Turn = true;
                return;
            }
        }
    }
    /************************补线及滤波***********************/
    if (!Flag.Right_Ring_Find)
        Flag.Right_Ring_Find_Record = true;
    else
        Flag.Right_Ring_Find_Record = false;
    if (Down_Point != EFFECTIVE_ROW)//滤除拐点上方的左右边界点 防止导致中线拟合出问题
    {
        for (i = Down_Point - 1; i >= 0; i--)
        {
            imageLine.Exist_Right[i] = false;
        }
        float k_Right = 0, b_Right = 0;//用上下拐点来补线
        k_Right = ((float)imageLine.Point_Right[Down_Point] - (float)imageLine.Point_Right[Mid_Point])
            / ((float)Down_Point - (float)Mid_Point);
        b_Right = (float)imageLine.Point_Right[Mid_Point] - k_Right * Mid_Point;
        //开始补线
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (!imageLine.Exist_Right[i] || (i< Down_Point))
            {
                imageLine.Exist_Right[i] = true;
                imageLine.Point_Right[i] = getLineValue(i, k_Right, b_Right);
                Flag.Right_Ring_Find = true;
                Flag.Right_Ring = true;
            }
        }
    }
    else //下方拐点丢失 从上方拐点进行向下拉线补线
    {
        float k_right = 0, b_right = 0;
        k_right = ((float)84 - (float)imageLine.Point_Right[Mid_Point])
            / ((float)(ImageH - 1) - (float)Mid_Point);
        b_right = (float)imageLine.Point_Right[Mid_Point] - k_right * Mid_Point;
        //开始补线
        for (i = ImageH - 1; i >= 0; i--)
        {
            imageLine.Exist_Right[i] = true;
            imageLine.Point_Right[i] = getLineValue(i, k_right, b_right);
            Flag.Right_Ring_Find = true;
            Flag.Right_Ring = true;
        }
    }
}
/*补线进右圆环*/
void Right_Ring_Turn(void)
{
    short i = 0, j = 0;
    short Top_Point = ImageW, Top_Point_row = EFFECTIVE_ROW, Divide_Point = EFFECTIVE_ROW;
    short Endpoint_L = 0, Startline_L = 0;
    bool count_flag = false;

    if (!Flag.Right_Ring_Turn)
        return;
    /************************找环岛上端点***********************/
    for (i = ImageH_5_6; i > ImageH_6; i--)
    {
        if (imageLine.Exist_Right[i] && !imageLine.Exist_Right[i - 1] && !imageLine.Exist_Right[i - 2])
        {
            Divide_Point = i;
            break;
        }
        if (!imageLine.Exist_Right[i] && !imageLine.Exist_Right[i - 1] && !imageLine.Exist_Right[i - 2] &&
            !imageLine.Exist_Right[i - 3] && !imageLine.Exist_Right[i - 4] && !imageLine.Exist_Right[i - 5])
            break;
    }
    /************************找环岛上直道断点***********************/
    if (Divide_Point != EFFECTIVE_ROW)
    {
        for (i = Divide_Point - 1; i > EFFECTIVE_ROW; i--)
        {
            if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] &&
                imageLine.Point_Right[i] < 3 * ImageW / 4 && imageLine.Point_Right[i - 1] < 3 * ImageW / 4 &&
                (imageLine.Point_Right[i] - imageLine.Point_Right[i - 1] <= 2))
            {
                Top_Point_row = i;
                Top_Point = imageLine.Point_Right[i];
                break;
            }
        }
    }
    else
    {
        for (i = ImageH_5_6; i > EFFECTIVE_ROW; i--)
        {
            if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] &&
                imageLine.Point_Right[i] < 3 * ImageW / 4 && imageLine.Point_Right[i - 1] < 3 * ImageW / 4 &&
                (imageLine.Point_Right[i] - imageLine.Point_Right[i - 1] <= 2))
            {
                Top_Point_row = i;
                Top_Point = imageLine.Point_Right[i];
                break;
            }
        }
    }


    for (i = Top_Point_row; i < ImageH; i++)
    {
        for (j = Top_Point; j < Top_Point + 15; j++)
        {
            count_flag = true;
            if (!isWhite(i, j))
            {
                count_flag = false;
                break;
            }
        }
        if (count_flag)
        {
            Top_Point_row = i - 1;
            break;
        }
    }
    for (j = Top_Point - 1; j < Top_Point + 15; j++)
    {
        if (isWhite(Top_Point_row, j) && !isWhite(Top_Point_row, j + 1))
        {
            Top_Point = j;
            break;
        }
    }
    /************************状态转换***********************/
    for (i = ImageH - 5; i > ImageH_4; i--)
    {
        if (imageLine.Exist_Left[i])
        {
            Startline_L = i;
            break;
        }
    }

    for (j = Startline_L - 1; j > ImageH_4; j--)
    {
        if (imageLine.Exist_Left[j] && imageLine.Exist_Left[j + 1] && imageLine.Exist_Left[j - 1] &&
            (ABS((imageLine.Point_Left[j - 1] + imageLine.Point_Left[j + 1]) / 2 - imageLine.Point_Left[j]) < 10))
        {
            Endpoint_L = imageLine.Point_Left[j - 1];
        }
        else
            break;
    }
    if (Endpoint_L > ImageW_2_3 && Startline_L >= ImageH_5_6)
    {
        Flag.Right_Ring_Turn = false;
        Flag.Right_Ring_Out = true;
//        Flag.Ui_Stop_Flag = 1;
        return;
    }
    /************************补线及滤波***********************/
    if (Top_Point_row == EFFECTIVE_ROW)
        return;

    float k_left = 0, b_left = 0;
    k_left = ((float)constrain_int16((Top_Point - 30), -15, 5) - (float)Top_Point)
        / ((float)(ImageH - 1) - (float)Top_Point_row);
    b_left = (float)constrain_int16((Top_Point - 30), -15, 5) - k_left * (ImageH - 1);
    for (i = ImageH - 1; i >= 0; i--)
    {
        if (i >= Top_Point_row)
        {
            imageLine.Exist_Left[i] = true;
            imageLine.Point_Left[i] = getLineValue(i, k_left, b_left);
        }
        else
        {
            if (((ring_cam.Right_Ring_Type >> (ring_cam.Right_Ring_Clc-1 )) & 1) == 0)
            {
                imageLine.Exist_Left[i] = true;
                imageLine.Point_Left[i] = (ring_cam.Turn_Upslope[0] * k_left) * i +
                    (b_left - (ring_cam.Turn_Upslope[0] - 1) * k_left * Top_Point_row);
                if (imageLine.Point_Right[i] <= imageLine.Point_Left[i] + 15)
                    imageLine.Exist_Right[i] = false;
            }
            else if (((ring_cam.Right_Ring_Type >> (ring_cam.Right_Ring_Clc-1 )) & 1) == 1)
            {
                imageLine.Exist_Left[i] = true;
                imageLine.Point_Left[i] = (ring_cam.Turn_Upslope[1] * k_left) * i +
                    (b_left - (ring_cam.Turn_Upslope[1] - 1) * k_left * Top_Point_row);
                if (imageLine.Point_Right[i] <= imageLine.Point_Left[i] + 15)
                    imageLine.Exist_Right[i] = false;
            }
        }
    }
}
/*出右圆环补线*/
void Right_Ring_Out(void)
{
    short i = 0, j = 0, k = 0;
    short TurnPoint_Row = EFFECTIVE_ROW, StratLine_L = EFFECTIVE_ROW, EndLine_L = EFFECTIVE_ROW;

    if (!Flag.Right_Ring_Out)
        return;
    /************************找拐点***********************/
    TurnPoint_Row = LTurnPoint_Row;
    /************************状态转换***********************/
    for (i = ImageH - 1; i >= ImageH_2; i--)
    {
        if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i - 1] && imageLine.Exist_Left[i - 2])
        {
            StratLine_L = i;
            break;
        }
    }
    if (StratLine_L <= 40 || TurnPoint_Row >= 50)
    {
        Flag.Right_Ring_Out = false;
        Flag.Right_Ring_Out_Mend1 = true;
        return;
    }
    /************************特殊滤波***********************/
    for (j = StratLine_L; j >= EFFECTIVE_ROW; j--)
    {
        if (imageLine.Exist_Left[j] && !imageLine.Exist_Left[j - 1] && !imageLine.Exist_Left[j - 2] &&
            !imageLine.Exist_Left[j - 3] && !imageLine.Exist_Left[j - 4] && !imageLine.Exist_Left[j - 5])
        {
            EndLine_L = j;
            break;
        }
    }
    if (EndLine_L != EFFECTIVE_ROW)
    {
        for (k = EndLine_L + 5; k >= 0; k--)
        {
            if (k <= EndLine_L)
                imageLine.Exist_Left[k] = false;
            imageLine.Exist_Right[k] = false;
        }
    }
    if (TurnPoint_Row == EFFECTIVE_ROW)
        return;
    /************************补线及滤波***********************/
    float k_left = 0, b_left = 0;
    short MendBasis_Left[2][5];
    short count = 0;
    for (i = TurnPoint_Row; i <= TurnPoint_Row + 10; i++)
    {
        if (imageLine.Exist_Left[i])
        {
            MendBasis_Left[0][count] = (short)i;
            MendBasis_Left[1][count] = (short)imageLine.Point_Left[i];
            count++;
        }
        if (count == 5)
            break;
    }
    if (count == 5)    //有5个点即可开始补线
    {
        leastSquareMethod(MendBasis_Left[0], MendBasis_Left[1], 5, &k_left, &b_left);
        for (i = TurnPoint_Row - 1; i >= 0; i--)
        {
            if (((ring_cam.Right_Ring_Type >> (ring_cam.Right_Ring_Clc-1 )) & 1) == 0)
            {
                imageLine.Exist_Left[i] = true;
                imageLine.Point_Left[i] = (ring_cam.Out_Slope[0] * k_left) * i +
                    (b_left - (ring_cam.Out_Slope[0] - 1) * k_left * TurnPoint_Row);
            }
            else if (((ring_cam.Right_Ring_Type >> (ring_cam.Right_Ring_Clc-1 )) & 1) == 1)
            {
                imageLine.Exist_Left[i] = true;
                imageLine.Point_Left[i] = (ring_cam.Out_Slope[1] * k_left) * i +
                    (b_left - (ring_cam.Out_Slope[1] - 1) * k_left * TurnPoint_Row);
            }
            if (!isWhite(i, imageLine.Point_Left[i]))
            {
                imageLine.Exist_Left[i] = false;
                break;
            }
        }
        for (j = i + 5; j >= 0; j--)
        {
            if (j >= ImageH_4)
            {
                if (imageLine.Exist_Right[j] && isWhite(j + 3, imageLine.Point_Right[j]))
                    imageLine.Exist_Right[j] = false;
            }
            else
                imageLine.Exist_Right[j] = false;
            if (j <= i)
            {
                if (imageLine.Exist_Left[j] <= imageLine.Point_Left[i])
                    imageLine.Exist_Left[j] = false;
            }
        }
    }
}
/*出右圆环无图像固定打角*/
void Right_Ring_Out_Mend1(void)
{
    short i = 0, j = 0;
    short StartLine_L = EFFECTIVE_ROW;

    if (!Flag.Right_Ring_Out_Mend1)
        return;
    /************************排除左下角边界的影响***********************/
    for (i = ImageH - 1; i >= ImageH - 20; i--)
    {
        if (imageLine.Exist_Left[i])
        {
            for (j = i - 1; j >= i - 3; j--)
            {
                if (isWhite(j, imageLine.Point_Left[i]))
                    return;
            }
        }
    }
    /************************最下左边界点位置***********************/
    for (i = ImageH - 1; i >= ImageH_6; i--)
    {
        if (imageLine.Exist_Left[i])
        {
            StartLine_L = i;
            break;
        }
    }
    /************************状态转换***********************/
    if (StartLine_L >= ImageH - 10)
    {
        Flag.Right_Ring_Out_Mend1 = false;
        Flag.Right_Ring_Out_Mend2 = true;
    }
}
/*出右圆环入口补线*/
void Right_Ring_Out_Mend2(void)
{
    short i = 0, j = 0, W_cnt = 0;
    bool count_flag = 0;
    short Top_Point = EFFECTIVE_ROW, Startline_R = EFFECTIVE_ROW;

    if (!Flag.Right_Ring_Out_Mend2)
        return;
    /************************找圆环出口以作分界***********************/
    for (i = ImageH - 5; i >= ImageH_4; i--)
    {
        for (j = ImageW - 1; j >= ImageW - 20; j--)
        {
            if (!isWhite(i, j))
            {
                count_flag = false;
                break;
            }
            count_flag = true;
        }
        if (count_flag)
        {
            W_cnt++;
            if (W_cnt >= 5)
            {
                Startline_R = i;
                break;
            }
        }
    }
    /************************找圆环上端点***********************/
    for (i = Startline_R; i > ImageH_6; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] && imageLine.Exist_Right[i - 2] &&
            imageLine.Point_Right[i] < ImageW - 10 && imageLine.Point_Right[i - 1] < ImageW - 10 && imageLine.Point_Right[i - 2] < ImageW - 10 &&
            ABS(imageLine.Point_Right[i - 1] - imageLine.Point_Right[i]) < 2 &&
            ABS(imageLine.Point_Right[i - 2] - imageLine.Point_Right[i - 1]) < 2)
        {
            Top_Point = i;
            break;
        }
    }
    /************************状态转换***********************/
    if (Top_Point >= 45 || Startline_R == EFFECTIVE_ROW)
    {
        Flag.Right_Ring_Out_Mend2 = false;
        Flag.Right_Ring_OnlyOnce = false;
        ring_cam.Right_Ring_Clc--;
        Flag.Right_Ring = false;
        Flag.Right_Ring_Out_Record = true;
        //if (fork_param.ForkRoad_Position == 2)
            //Flag.ForkRoad_OnlyOnce = true;
        return;
    }
    /************************补线滤波***********************/
    if (Top_Point == EFFECTIVE_ROW)
        return;

    float k_right = 0, b_right = 0;
    short MendBasis_right[2][5];
    int count = 0;
    for (i = Top_Point; i >= Top_Point - 10; i--)
    {
        if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i - 1] &&
            ABS(imageLine.Point_Right[i] - imageLine.Point_Right[i - 1]) < 2)
        {
            MendBasis_right[0][count] = (short)i;
            MendBasis_right[1][count] = (short)imageLine.Point_Right[i];
            count++;
        }
        if (count == 5)
            break;
    }
    if (count == 5)    //有5个点即可开始补线
    {
        leastSquareMethod(MendBasis_right[0], MendBasis_right[1], 5, &k_right, &b_right);
        if (k_right < -0.5)
        {
            k_right = ((float)89 - (float)imageLine.Point_Right[Top_Point])
                / ((float)(ImageH - 1) - (float)Top_Point);
            b_right = (float)imageLine.Point_Right[Top_Point] - k_right * Top_Point;
        }
    }
    else
    {
        k_right = ((float)89 - (float)imageLine.Point_Right[Top_Point])
            / ((float)(ImageH - 1) - (float)Top_Point);
        b_right = (float)imageLine.Point_Right[Top_Point] - k_right * Top_Point;
    }
    //开始补线
    for (i = ImageH - 1; i >= Top_Point; i--)
    {
        imageLine.Exist_Right[i] = true;
        imageLine.Point_Right[i] = getLineValue(i, k_right, b_right);
    }
}

/***************************************断路2******************************************************/

void Break_Road_2(void)
{
    if(Total_time_2 > break_time)
    {
        bluetooth_ch9141_send_string("1br");
        Flag.break_Road_in = false;
        beep_set_sound(BEEP_OFF);
        Flag.Run_Time2 = false;
        gpio_set_level(LED1, GPIO_HIGH);
    }

    if(Bar_break_order < 16 && (Bar_break_order >> (Bar_break_count - 1) & 1) == 1)
    {
        return;
    }

    if ( Flag.barricade || Flag.Garage_Out || Flag.Ramp || (Flag.Left_Ring && !Flag.Left_Ring_Find) || (Flag.Right_Ring && !Flag.Right_Ring_Find))//|| Flag.Garage_Out
        return;
    short i,L_Exit_Line = EFFECTIVE_ROW, R_Exit_Line = EFFECTIVE_ROW;
    int road_wide = 0;
    short flag = 0;
    static int break_cnt = 0;



    if (!Flag.break_Road && dl1b_distance_mm > 1500)
    {

        for (i = ImageH-1; i > 3; i--)
        {
            if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i + 1] && imageLine.Exist_Left[i + 2] && (!imageLine.Exist_Left[i - 1]) && (!imageLine.Exist_Left[i - 2]) && (!imageLine.Exist_Left[i - 3]))
            {
                L_Exit_Line = i;

            }
        }

        for (i = ImageH-1; i > 3; i--)
        {
            if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i + 1] && imageLine.Exist_Right[i + 2] && (!imageLine.Exist_Right[i - 1]) && (!imageLine.Exist_Right[i - 2]) && (!imageLine.Exist_Right[i - 3]))
            {
                R_Exit_Line = i;
            }
        }



        if ((R_Exit_Line > Break_Line) && (L_Exit_Line > Break_Line) && (ABS(R_Exit_Line - L_Exit_Line) < 3)
            && ((imageLine.Point_Right[R_Exit_Line] - imageLine.Point_Left[L_Exit_Line]) < 10) && imageLine.Point_Left[L_Exit_Line] > 0 + 5&& imageLine.Point_Right[R_Exit_Line] <  ImageW - 5)
        {
            break_cnt++;
            for (int i = L_Exit_Line; i < ImageH - 10; i++)
              {
                  flag = 0;
                  for (int j = 0; j < 3; j++)
                  {
                      if ((((imageLine.Point_Left[i + j + 1] - imageLine.Point_Left[i + j]) >= 0) || (!imageLine.Exist_Left[i + j] || !imageLine.Exist_Left[i + j +1])) &&(i - L_Exit_Line) >2)
                      {
                          flag++;
                      }


                  }
                  if (flag == 3)
                  {
                      road_wide = 0;
                      for (int i = L_Exit_Line; i < L_Exit_Line + 3; i++)
                      {
                          road_wide += ABS(imageLine.Point_Left[i + 1] - imageLine.Point_Left[i]);
                      }
                      break;
                  }


              }

            if((road_wide <= 8 && road_wide > 0) || break_cnt >= 3)
            {
                CHARGE_OFF;
                Flag.break_Road_in = true;
                Bar_break_count--;
                Flag.break_Road = true;
                gpio_set_level(LED1, GPIO_LOW);
                beep_set_sound(BEEP_ON);
                Flag.turnWAY_state = 8;
                Flag.Run_Time2 = true;
                bluetooth_ch9141_send_string("1in");
                return;
            }
            road_wide = 0;
              for (int i = R_Exit_Line; i < ImageH - 10; i++)
              {
                  flag = 0;
                  for (int j = 0; j < 3; j++)
                  {
                      if ((((imageLine.Point_Right[i + j + 1] - imageLine.Point_Right[i + j]) <=  0) || (!imageLine.Exist_Right[i + j] || !imageLine.Exist_Right[i + j + 1])) && (i - R_Exit_Line) > 2)
                      {
                          flag++;
                      }

                  }
                  if (flag == 3)
                  {

                      for (int i = R_Exit_Line; i < R_Exit_Line + 3; i++)
                      {
                          road_wide += ABS(imageLine.Point_Right[i + 1] - imageLine.Point_Right[i]);
                      }
                      break;
                  }
              }

              if(road_wide <= 8 && road_wide > 0)
              {
                  CHARGE_OFF;
                  Flag.break_Road_in = true;
                  Bar_break_count--;
                  Flag.break_Road = true;
                  gpio_set_level(LED1, GPIO_LOW);
                  beep_set_sound(BEEP_ON);
                  Flag.turnWAY_state = 8;
                  Flag.Run_Time2 = true;
                  bluetooth_ch9141_send_string("1in");
              }

//            // 看是左拐断路还是右拐断路
//            if (LTurnPoint_Row != EFFECTIVE_ROW)
//            {
//                for (int i = L_Exit_Line; i < L_Exit_Line + 3; i++)
//                {
//                    road_wide += ABS(imageLine.Point_Left[i+1] - imageLine.Point_Left[i]);
//                }
//            }
//            else if (RTurnPoint_Row != EFFECTIVE_ROW)
//            {
//                for (int i = R_Exit_Line; i < R_Exit_Line + 3; i++)
//                {
//                    road_wide += ABS(imageLine.Point_Right[i + 1] - imageLine.Point_Right[i]);
//                }
//            }
//            else{
//                return;
//            }
//
//
//            if(road_wide <= 8 && road_wide > 0)
//            {
//                CHARGE_OFF;
//                Flag.break_Road_in = true;
//                Bar_break_count--;
//                Flag.break_Road = true;
//                gpio_set_level(LED1, GPIO_LOW);
//                beep_set_sound(BEEP_ON);
//                Flag.turnWAY_state = 8;
//                Flag.Run_Time2 = true;
//                bluetooth_ch9141_send_string("1in");
//            }


//            if(Flag.Fix_Thr_in_break)
//            {
//                Flag.Fix_Thr = true;
//            }
        }
        else
        {
            break_cnt = 0;
        }
    }

}


/**************************************中线误差****************************************************/
/*计算中线打角*/
bool MediumLineCal(volatile float* final)
{
    uint8 i;
    static float lastCenter = 47;
    short temp_dev = 0, SpeedDown_Line = 0;

    //大弯道检测
    for (i = ImageH_2; i > EFFECTIVE_ROW; i--)
    {
        if (imageLine.Exist_Center[i])
        {
            temp_dev = imageLine.Point_Center[i] - center;
            if (ABS(temp_dev) >= 15)
            {
                SpeedDown_Line = i;
                break;
            }
        }
    }
    if (SpeedDown_Line > AIM_LINE - 10)
        Flag_SpeedDown = true;
    else
        Flag_SpeedDown = false;

    if (imageLine.Exist_Center[AIM_LINE] == false && !imageLine.Lost_Center)
    {
        uint8_t up_cnt = 0, down_cnt = 0;
        //假如控制行处中线不存在，往下找15个点
        for (i = AIM_LINE; i < AIM_LINE + 15; i++)
        {
            down_cnt++;
            if (imageLine.Exist_Center[i])
            {
                AIM_LINE = i;
                break;
            }
        }
        for (i = AIM_LINE; i > AIM_LINE - 10; i--)
        {
            up_cnt++;
            if (imageLine.Exist_Center[i])
            {
                if(up_cnt < down_cnt)
                    AIM_LINE = i;
                break;
            }
        }
//        imageLine.Lost_Center = true;

    }

    if (!imageLine.Lost_Center)
    {
        if (imageLine.Exist_Center[AIM_LINE])
        {
            if (my_abs_float(lastCenter - (float)imageLine.Point_Center[AIM_LINE]) > 50.0f)
            {
                lastCenter = imageLine.Point_Center[AIM_LINE];
                return true;
            }
            else
            {
                //更新中线误差
                for (i = 0; i < 10; i++)
                    *(final + 10 - i) = *(final + 9 - i);

                *(final) = (imageLine.Point_Center[AIM_LINE] - (center + biaos));
                lastCenter = imageLine.Point_Center[AIM_LINE];
                return true;
            }
        }
    }
    else
    {
        *(final) = *(final + 1);        // 丢线则保持上次的误差
        return false;
    }
    return false;
}


static uint8_t longStraightRoadJudge(void)
{
    short i;
    for(i = 20; i < 50; i++)
    {
        if((imageLine.Exist_Center[i] && ABS(imageLine.Point_Center[i] - center) > 0.3*i) || !imageLine.Exist_Center[i])
            return 0;
    }
    return 1;
}

static uint8_t longStraightRoadClear(void)
{
    short i;
    for(i = 15; i < 50; i++)
    {
        if((imageLine.Exist_Center[i] && ABS(imageLine.Point_Center[i] - center) > 0.3*i) || !imageLine.Exist_Center[i])
            return 1;
    }
    return 0;
}

/*更新中线打角*/
void updateMediumLine(void)
{
    bool rst = true;
    static uint8_t lostTime = 0;

    if(Flag.break_Road_in )
    {
        return;
    }

    /************************计算中线打角*****************************/
    rst = MediumLineCal(image.cam_finalCenterERR);

    servo_param.Servo_filter =
        0.7f * image.cam_finalCenterERR[0] + 0.15f * image.cam_finalCenterERR[1]
        + 0.1f * image.cam_finalCenterERR[2] + 0.05f * image.cam_finalCenterERR[3];

    //中线打角
    if (rst &&!Flag.turnWAY_state)//摄像头打角
    {
        if(Flag.break_Road)
        {
            CHARGE_ON;
            Flag.break_Road = 0;
            Flag.Fix_Thr = false;
            bluetooth_ch9141_send_string("1ot");
        }
        lostTime = 0;
        Flag.turnWAY_state = 0;
    }
    else if (!rst && (lostTime <= 3))//丢线不多，用摄像头之前的值跑
    {
        lostTime++;
        Flag.turnWAY_state = 1;
    }
    else if (!rst && (lostTime >= 20))
    {
        Flag.turnWAY_state = 2;
    }
    /*else if(!Flag.Garage_Out && !Flag.Right_Ring && !Flag.Left_Ring)    //丢线太多，电感跑
    {
        CHARGE_OFF;
        Flag.break_Road = 1;
        Flag.turnWAY_state = 8;
    }*/
    if(longStraightRoadJudge() && !longStraightFlag && !Flag.Ramp && !Flag.Garage_Find)//判断长直道
    {
        if(longStraightCount < 15)
            longStraightCount++;
    }
    else
        longStraightCount = 0;
    if(longStraightCount == 10)
        longStraightFlag = 1;
    if(longStraightFlag)
    {
        if(longStraightRoadClear())
            longStraightClearCount++;
        if(longStraightClearCount == 3 || Flag.Left_Ring || Flag.Right_Ring || Flag.Ramp || Flag.Garage_Find)
        {
            longStraightClearCount = 0;
            longStraightCount = 0;
            longStraightFlag = 0;
        }
    }
    servo_param.cam_servo_temp_1 = servo_param.Servo_filter;
#if oled_show
    oled_show_string(32, 1, "   ");
    oled_show_int(32, 1, (int16_t)(servo_param.cam_servo_temp_1),3);
#endif
}


void show_line(){
    uint8 i;
    uint8 j=0;
    uint8 leftLine, rightLine, centerLine;
    for(i = 50 ; i < 50 + ImageH ; i++){
        if(imageLine.Point_Left[j] > ImageW ){
            leftLine = ImageW ;
        }
        else{
            leftLine = imageLine.Point_Left[j];
            //ips200_show_int(110, 225, leftLine, 3);
        }
        if(imageLine.Point_Right[j] > ImageW ){
            rightLine = ImageW ;
        }
        else{
            rightLine = imageLine.Point_Right[j];
        }
        if(imageLine.Point_Center[j] > ImageW ){
            centerLine = ImageW ;
        }
        else{
            centerLine = imageLine.Point_Center[j];
        }
        ips200_draw_point(25 + leftLine, i, RGB565_BLUE);
        ips200_draw_point(25 + rightLine, i, RGB565_GREEN);
        ips200_draw_point(25 + centerLine, i, RGB565_RED);
        j++;
    }
}
