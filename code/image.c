#include "zf_common_headfile.h"

#define oled_show       (0)
#define max_size (ImageW * ImageH)


/*ͼ����*/
uint8_t bin_thr = 0;
bool Pixle[ImageH][ImageW];       //��ֵ��ͼ��
static bool isVisited[ImageH][ImageW];//���ص��Ƿ���ʹ�
Image_Data image;
volatile imageLine_t imageLine; 

/*ͼ��յ�*/
short LTurnPoint_Row = EFFECTIVE_ROW, LTurnPoint = 0;
short RTurnPoint_Row = EFFECTIVE_ROW, RTurnPoint = 0;
float Kl_up = 0, Bl_up = 0, Kl_down = 0, Bl_down = 0;
float Kr_up = 0, Br_up = 0, Kr_down = 0, Br_down = 0;
uint8_t Error_Cnt_L = 0, Error_Cnt_R = 0;

/*�������*/
volatile uint8_t AIM_LINE_SET = 36;    //Ŀ����28
volatile uint8_t RAMP_AIM_DELTA = 10;
volatile uint8_t AIM_LINE;
bool Flag_SpeedDown = false;
int biaos = 0;
#define SHOW_IMAGE                         (0)

uint8 C_Image[ImageH][ImageW];
uint16 threshold = 50;
uint16 light = 500;

uint8 img_x = 0;
uint8 img_y = 0;
uint8 image_pross[ImageH][ImageW];
uint8 pixel_min_num = 10; // �����С��С��С�������С�Ĺ�㱻��Ϊ����
uint8 pixel_max_num = 200; // �������С�����������С�Ĺ�㱻��Ϊͼ��ɼ��쳣

/************************************ͼ��ɼ�������************************************************/
/*ͼ��Ԥ���������*/
void ImageProcess(void)
{
    uint8 averx = 255;
        uint8 visitnum = 0;
        uint16 totalnum = 0;
        uint32 sumx = 0;
        uint32 sumy = 0;
        uint32 visit_y = 0;
        uint8 i, j;
        bool rst = true;
        static uint8_t lostTime = 0;
        for (i = 0; i < ImageH; i++)
        {
            //

            for (j = 0; j < ImageW; j++)
            {
                C_Image[i][j] = 0;          //  ѹ��ͼ��
            }
        }
        for (i = 10; i < 30; i++)
        {
            //

            for (j = 15; j < 80; j++)
            {
                C_Image[i][j] = mt9v03x_image[i * 2][j * 2];          //  ѹ��ͼ��
            }
        }
        mt9v03x_finish_flag = 0;//�����־λ��ͼ�����²ɼ����������Ϊ1
        bin_thr = GetOSTU_2(C_Image);

        for (uint8 j = 0; j < ImageW; j++)
        {
            for (uint8 i = 0; i < ImageH; i++)
            {
                if (C_Image[i][j] > bin_thr)
                {
                    Pixle[i][j] = true;
                }
                else
                    Pixle[i][j] = false;
            }
        }
        rst = two_pass();
        if (rst)
        {
            lostTime = 0;
            for (uint8 j = 0; j < ImageW; j++)
            {
                for (uint8 i = 0; i < ImageH; i++)
                {
                    if (Pixle[i][j])
                    {
                        visitnum++;
                        visit_y += i;
                    }
                }

                sumx += j * visitnum;
                sumy += visit_y;
                totalnum += visitnum;
                visitnum = 0;
                visit_y = 0;

            }
            if (totalnum)
            {
                averx = (uint8)(sumx / totalnum);
                visit_y = (uint8)(sumy / totalnum);
            }
            img_x = averx;
            img_y = visit_y;
            MediumLineCal_2(image.cam_finalCenterERR);
            ips200_show_int(25, 250, image.cam_finalCenterERR[0], 3);
            ips200_show_int(40, 250, image.cam_finalCenterERR[1], 3);
            ips200_show_int(65, 250, image.cam_finalCenterERR[2], 3);
            ips200_show_int(80, 250, image.cam_finalCenterERR[3], 3);
            ips200_show_int(95, 250, image.cam_finalCenterERR[4], 3);
            ips200_show_int(110, 250, image.cam_finalCenterERR[5], 3);
            ips200_show_int(125, 250, image.cam_finalCenterERR[6], 3);
            ips200_show_int(140, 250, image.cam_finalCenterERR[7], 3);
            ips200_show_int(155, 250, image.cam_finalCenterERR[8], 3);
            ips200_show_int(170, 250, image.cam_finalCenterERR[9], 3);
        }
        else if (!rst && (lostTime <= 50))
        {
            lostTime++;
        }
        else {
            Flag.Duty_Stop_Flag = 1;
            Flag.Ui_Stop_Flag = 1;
        }


}
/*��ȡ��ֵ����ֵ(���)*/
int GetOSTU(uint8 tmImage[ImageH][ImageW])
{
    int16 i, j;
    uint32 Amount = 0;//��������
    uint32 PixelBack = 0;//back���ص���
    uint32 PixelIntegralBack = 0;//back�Ҷ�ֵ
    uint32 PixelIntegral = 0;//�Ҷ�ֵ����
    int32 PixelIntegralFore = 0;//fore�Ҷ�ֵ
    int32 PixelFore = 0;//fore���ص���
    float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��;
    int16 MinValue, MaxValue;
    uint8 Threshold = 0;
    uint8 HistoGram[256];              //

    for (j = 0; j < 256; j++)  HistoGram[j] = 0; //��ʼ���Ҷ�ֱ��ͼ

    for (j = 0; j < ImageH; j++)
    {
        for (i = 0; i < ImageW; i++)
        {
            HistoGram[tmImage[j][i]]++; //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //��ȡ��С�Ҷȵ�ֵ
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //��ȡ���Ҷȵ�ֵ

    if (MaxValue == MinValue)     return MaxValue;         // ͼ����ֻ��һ����ɫ
    if (MinValue + 1 == MaxValue)  return MinValue;        // ͼ����ֻ�ж�����ɫ

    for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  ��������

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j;//�Ҷ�ֵ���� j��0-255 ��ʾ�Ҷ�
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)//�ӱ���ͼ�����С�Ҷȵ����Ҷȱ���
    {
        PixelBack = PixelBack + HistoGram[j];   //ǰ�����ص���
        PixelFore = Amount - PixelBack;         //�������ص���
        OmegaBack = (float)PixelBack / Amount;//ǰ�����ذٷֱ�
        OmegaFore = (float)PixelFore / Amount;//�������ذٷֱ�
        PixelIntegralBack += HistoGram[j] * j;  //ǰ���Ҷ�ֵ
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//�����Ҷ�ֵ
        MicroBack = (float)PixelIntegralBack / PixelBack;   //ǰ���ҶȰٷֱ�
        MicroFore = (float)PixelIntegralFore / PixelFore;   //�����ҶȰٷֱ�
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//������䷽��
        if (Sigma > SigmaB)                    //����������䷽��g //�ҳ������䷽���Լ���Ӧ����ֵ
        {
            SigmaB = Sigma;
            Threshold = j;//j��0-255�Ҷ�ֵ
        }
    }
    return Threshold;                        //���������ֵ;
}

/*��ȡ��ֵ����ֵ(��Ȩ��ֵ)*/
int GetOSTU_2(uint8 tmImage[ImageH][ImageW])
{
    short i, j;
    uint32 Amount = 0;//��������
    //uint32 PixelBack = 0;//back���ص���
    uint32 PixelIntegralBack = 0;//back�Ҷ�ֵ
    uint32 PixelIntegral = 0;//�Ҷ�ֵ����
    int PixelIntegralFore = 0;//fore�Ҷ�ֵ
    //int PixelFore = 0;//fore���ص���
    //float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��;
    short MinValue, MaxValue;
    uint8 Threshold = 0;
    uint8 HistoGram[256];
    //

    for (j = 0; j < 256; j++)  HistoGram[j] = 0; //��ʼ���Ҷ�ֱ��ͼ

    for (j = 0; j < ImageH; j++)
    {
        for (i = 0; i < ImageW; i++)
        {
            HistoGram[tmImage[j][i]]++; //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //��ȡ��С�Ҷȵ�ֵ
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //��ȡ���Ҷȵ�ֵ

    if (MaxValue == MinValue)     return MaxValue;         // ͼ����ֻ��һ����ɫ
    if (MinValue + 1 == MaxValue)  return MinValue;        // ͼ����ֻ�ж�����ɫ

    for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  ��������

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j;//�Ҷ�ֵ���� j��0-255 ��ʾ�Ҷ�
    }
    //SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)//�ӱ���ͼ�����С�Ҷȵ����Ҷȱ���
    {
        //PixelBack = PixelBack + HistoGram[j];   //ǰ�����ص���
        //PixelFore = Amount - PixelBack;         //�������ص���
        //OmegaBack = (float)PixelBack / Amount;//ǰ�����ذٷֱ�
        //OmegaFore = (float)PixelFore / Amount;//�������ذٷֱ�
        PixelIntegralBack += HistoGram[j] * j;  //ǰ���Ҷ�ֵ
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//�����Ҷ�ֵ
        //MicroBack = (float)PixelIntegralBack / (PixelBack);   //ǰ���ҶȰٷֱ�
        //MicroFore = (float)PixelIntegralFore / (PixelFore);   //�����ҶȰٷֱ�
        //Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//������䷽��
        if (PixelIntegralBack > 4*PixelIntegralFore)                    //����������䷽��g //�ҳ������䷽���Լ���Ӧ����ֵ
        {
            //SigmaB = Sigma;
            Threshold = j;//j��0-255�Ҷ�ֵ
            break;
        }
    }
    if (Threshold == 0)
        Threshold = 250;
    return Threshold;            //���������ֵ;
}




/*DFSѲ�ߣ�������������ҳ���ͨ�������а׵�(DFS������)*/

/*�õ�ÿһ�����а׵�ĸ���*/
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
    uint8_t roadPointStackX[300];             //DFS��ջ�洢Ѱ��X����
    uint8_t roadPointStackY[300];             //DFS��ջ�洢Ѱ��Y����
    short stackTopPos = -1;

    if (!Flag.Left_Ring_Turn && !Flag.Right_Ring_Turn)
    {
        //ѡ��ͼƬ�·��е���Ϊ��ʼ��
        if (isWhite(ImageH - 2, ImageW / 2))          //���·��е���ǰ׵㣬��ջ
        {
            stackTopPos++;
            roadPointStackX[stackTopPos] = ImageH - 2;
            roadPointStackY[stackTopPos] = ImageW / 2;
            isVisited[ImageH - 2][ImageW / 2] = true;
        }
        else         //����·��е㲻�ǰ׵㣬�ʹ������������������������㶼�ǰ׵㣬���ж�Ϊdfs�����
        {
            for (i = ImageW - 4; i >= 2; i--)//������������
            {
                if (isWhite(ImageH - 2, i - 2) && isWhite(ImageH - 2, i - 1) && isWhite(ImageH - 2, i)
                    && isWhite(ImageH - 2, i + 1) && isWhite(ImageH - 2, i + 2))//����5���׵�
                {
                    //����������ջ
                    stackTopPos++;                      //stackTopPos����ͱ�ʾջ�ǿ�
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = true;      //���Ϊ�õ��Ѿ�������
                    break;                              //һ��ȷ������break
                }
            }
        }

        i = j = 0;

        while (stackTopPos >= 0)                        //ֻҪ�ҵ����Ϳ�ʼ
        {
            //��ջ
            i = roadPointStackX[stackTopPos];
            j = roadPointStackY[stackTopPos];
            stackTopPos--;

            //������磬ֱ��continue
            if ((i < EFFECTIVE_ROW) || (i > ImageH - 2) || (j < 1) || (j > ImageW - 1))
            {
                continue;
            }

            /*************���²���ԭ���ǣ����׵���ջ�����ڵ㡰�������ж�Ϊ�߽��(��������Ҫ�����˲����ߵȲ���)**************/
            /**************************��ջ�洢��...����(��)����(��)...******************************/
            /******************һ������£��ϵİ׵���ȳ�ջ(����Ϊ��һֱ��ͼ���Ϸ�����)��Ȼ���ٳ�����*****************/

            //��������
            if (!isVisited[i][j - 1])                   //�������һ���Ƿ������
            {
                if (isWhite(i, j - 1))
                {
                    //�׵���ջ
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j - 1;
                    isVisited[i][j - 1] = true;         //���Ϊ�õ��Ѿ�������
                }
                else                                    //�ڵ�ͳ����ж�Ϊ�߽��               (xzl: �о�����ָ��ǵ����)
                {
                    if (isLeftPoint(i, j))
                    {
                        //�������������
                        if (!imageLine.Exist_Left[i] || (imageLine.Exist_Left[i] && imageLine.Point_Left[i] < j && j < 59))
                            imageLine.Point_Left[i] = j;//��߽��ĺ�����

                        imageLine.Exist_Left[i] = 1;     //��߽�����
                    }
                }
            }

            //��������
            if (!isVisited[i][j + 1])                   //�����ұ�һ���Ƿ������
            {
                if (isWhite(i, j + 1))
                {
                    //�׵���ջ
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j + 1;
                    isVisited[i][j + 1] = true;         //���Ϊ�õ��Ѿ�������
                }
                else                                    //�ڵ�ͳ����ж�Ϊ�߽��               (xzl: �о�����ָ��ǵ����)
                {
                    if (isRightPoint(i, j))
                    {

                        if (!imageLine.Exist_Right[i] || (imageLine.Exist_Right[i] && imageLine.Point_Right[i] > j && j > 35))
                            imageLine.Point_Right[i] = j;//�ұ߽��ĺ�����
                        imageLine.Exist_Right[i] = true;
                    }
                }
            }

            //��������(���ϲ��жϱ߽��)
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
        for (i = 0; i <= ImageW / 2; i++)//������������
        {
            if (Flag.Left_Ring_Turn)
            {
                if (isWhite(ImageH - 2, i - 2) && isWhite(ImageH - 2, i - 1) && isWhite(ImageH - 2, i)
                    && isWhite(ImageH - 2, i + 1) && isWhite(ImageH - 2, i + 2)//����5���׵�
                    )
                {
                    //����������ջ
                    stackTopPos++;//stackTopPos����ͱ�ʾջ�ǿ�
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = 1;
                }
            }
            else
            {
                if (isWhite(ImageH - 2, ImageW - i - 2) && isWhite(ImageH - 2, ImageW - i - 1) && isWhite(ImageH - 2, ImageW - i)
                    && isWhite(ImageH - 2, ImageW - i + 1) && isWhite(ImageH - 2, ImageW - i + 2)//����5���׵�
                    )
                {
                    //����������ջ
                    stackTopPos++;//stackTopPos����ͱ�ʾջ�ǿ�
                    roadPointStackX[stackTopPos] = ImageH - 2;
                    roadPointStackY[stackTopPos] = i;
                    isVisited[ImageH - 2][i] = 1;
                }
            }
        }

        i = j = 0;

        while (stackTopPos >= 0)
        {
            //��ջ
            i = roadPointStackX[stackTopPos];
            j = roadPointStackY[stackTopPos];
            stackTopPos--;

            //������磬ֱ��continue
            if ((i < EFFECTIVE_ROW) || (i > ImageH - 2) || (j < 1) || (j > ImageW - 2))
            {
                continue;
            }

            /*************���²���ԭ���ǣ����׵���ջ�����ڵ�����ж�Ϊ�߽��(��������Ҫ�����˲����ߵȲ���)**************/
            //��ջ�洢��...����(��)����(��)...
            //һ������£��ϵİ׵���ȳ�ջ(����Ϊ��һֱ��ͼ���Ϸ�����)��Ȼ���ٳ�����
            //��������
            if (!isVisited[i][j - 1])
            {
                if (isWhite(i, j - 1))
                {
                    //�׵���ջ
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j - 1;
                    isVisited[i][j - 1] = 1;
                }
                else
                {
                    //�ڵ�����ж�Ϊ�߽��(xzl: �о�����ָ��ǵ����)
                    if (isLeftPoint(i, j))
                    {
                        imageLine.Point_Left[i] = j;//���߹켣
                        imageLine.Exist_Left[i] = 1;
                        imageLine.Lost_Left = 0;
                    }
                }
            }

            //��������
            if (!isVisited[i][j + 1])
            {
                if (isWhite(i, j + 1))
                {
                    stackTopPos++;
                    roadPointStackX[stackTopPos] = i;
                    roadPointStackY[stackTopPos] = j + 1;
                    isVisited[i][j + 1] = 1;
                }
                else//�����ҵ��ұ߽�
                {
                    if (isRightPoint(i, j))
                    {
                        imageLine.Point_Right[i] = j;//���߹켣
                        imageLine.Exist_Right[i] = 1;
                        imageLine.Lost_Right = 0;
                    }
                }
            }
            //��������(���ϲ��жϱ߽��)
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

/****************************************�˲�******************************************************/
/*�˳���ֵ��ͼ�����*/
void Pixle_Filter(void)
{
    uint8_t nr; //��
    uint8_t nc; //��

    //������
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
}
/*��ȱ����˲�*/
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
/*filter�������*/
void doFilter(void)
{
    if (!Flag.Ramp && !Flag.Left_Ring && !Flag.Right_Ring && (!Flag.CrossRoad_Find || Flag.crossLeft || Flag.crossRight))
    {
        TurnFliter(LTurnPoint_Row, LTurnPoint, RTurnPoint_Row, RTurnPoint, 3, 3,
            Kl_up, Bl_up, Kl_down, Bl_down, Kr_up, Br_up, Kr_down, Br_down, Error_Cnt_L, Error_Cnt_R);
    }
    left_right_Limit();    //�߽��Խ������
    lineChangeLimit();     //�߽�б������
    singlePoint_Filter();  //��������ȥ
    lostLine_Filter();    //��Ч�й�����ȥ(�������ж�)
}
/*��Ч�й�����ȥ(�������ж�)*/
void lostLine_Filter(void)
{
    //������߽��ߵ��ж�--------------------
    short count = 0;
    int8_t i = 0;

    for (i = 0; i < ImageH; i++)        //���ϵ�������
    {
        if (imageLine.Exist_Left[i])
            count++;
    }

    if (count < VALID_LINE_THRESHOLE)        //�����Ч�г�����ֵ��Ϊ�ñ߽��߶�ʧ
    {
        imageLine.Lost_Left = 1;
        for (i = 0; i < ImageH; i++)        //���ϵ�������
        {
            imageLine.Exist_Left[i] = 0;
        }
    }
    else
        imageLine.Lost_Left = 0;

    //�����ұ߽��ߵ��ж�--------------------
    count = 0;
    for (i = 0; i < ImageH; i++)
    {
        if (imageLine.Exist_Right[i])
            count++;
    }

    if (count < VALID_LINE_THRESHOLE)        //�����Ч�г�����ֵ��Ϊ�ñ߽��߶�ʧ
    {
        imageLine.Lost_Right = 1;
        for (i = 0; i < ImageH; i++)        //���ϵ�������
        {
            imageLine.Exist_Right[i] = 0;
        }
    }
    else
        imageLine.Lost_Right = 0;
}
/*���ұ߽��Խ���˲�*/
void left_right_Limit(void)
{
    int8_t i = 0;

    for (i = 1; i < ImageH - 1; i++)//�����ϵ�����
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
/*�߽�㲻ͻ��*/
void lineChangeLimit(void)
{
    int8_t i = 0, j = 0;
    float leftK = 0, rightK = 0;

    //��߽���������Ч��б�ʼ��
    for (i = ImageH - 2; i > 0; i--)        //��������
    {
        if (imageLine.Exist_Left[i])        //���ҵ���һ����Ч��
        {
            for (j = i + 1; j < ImageH; j++)        //���������ٽ���Ч��
            {
                if (imageLine.Exist_Left[j])
                {
                    leftK = getLineK(i, imageLine.Point_Left[i], j, imageLine.Point_Left[j]);

                    if (ABS(leftK) > K_MAX_THRESHOLD)
                    {
                        imageLine.Exist_Left[i] = 0;
                        //imageLine.Exist_Left[j] = 0;
                    }
                    break;        //ֻҪ�ҵ�һ���ٽ���Ч�㣬�����break����һ��i
                }
            }
        }
    }

    //�ұ߽���������Ч��б�ʼ��
    for (i = ImageH - 2; i > 0; i--)        //��������
    {
        if (imageLine.Exist_Right[i])        //���ҵ���һ����Ч��
        {
            for (j = i + 1; j < ImageH; j++)        //���������ٽ���Ч��
            {
                if (imageLine.Exist_Right[j])
                {
                    rightK = getLineK(i, imageLine.Point_Right[i], j, imageLine.Point_Right[j]);

                    if (ABS(rightK) > K_MAX_THRESHOLD)
                    {
                        imageLine.Exist_Right[i] = 0;
                        //imageLine.Exist_Right[j] = 0;
                    }
                    break;        //ֻҪ�ҵ�һ���ٽ���Ч�㣬�����break����һ��i
                }
            }
        }
    }
}
/*�����߽���˳�*/
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
/****************************************����******************************************************/
/*���е㸳ֵ������Ϊ����������Ч*/
void amplitudeLIMIT(uint16_t i, int16_t amp)
{
    imageLine.Exist_Center[i] = true;
    imageLine.Point_Center[i] = amp;
}
/*�Եó������߽���ͻ���޷�*/
void limitCenter(void)
{
    for (int8_t i = ImageH - 2; i >= 1; i--)        //��������
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

/*�����ߺϼ�*/
void doMend(void)
{
    trackMend_startPart();
    trackMend_HalfPart();//�����߰����������ֱ�Ӽ���
}
/*Ԫ����յ��˲�����*/
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
    //�˲�����
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
/*��������*/
/*��������*/
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

    //------------------------������-------------------------------
    //��������ʼ��
    for (i = ImageH - 1; i >= (ImageH_6 ); i--)
    {
        if (imageLine.Exist_Left[i])
        {
            leftLine_startPoint = i;
            break;
        }
    }

    //------------------------������-------------------------------
    //��������ʼ��
    for (i = ImageH - 1; i >= (ImageH_6 ); i--)
    {
        if (imageLine.Exist_Right[i])
        {
            rightLine_startPoint = i;
            break;
        }
    }

    if (imageLine.Point_Right[rightLine_startPoint] - imageLine.Point_Left[leftLine_startPoint] >= 5)    //��ֹ�������λ�ò��²�����
    {
        //������ʼ����2/3��ʱ(ͼ��ײ����������ߵĲ����е����),����
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
                if (count == 5)    //��5���㼴�ɿ�ʼ����
                {
                    leastSquareMethod(MendBasis_left[0], MendBasis_left[1], 5, &k_left, &b_left);
                    if (ABS(k_left) > 2 && i > ImageH_6)
                    {
                        count = 0;
                        continue;
                    }
                    //��ʼ����
                    for (i=i+5; i < ImageH; i++)
                    {
                         imageLine.Point_Left[i] = getLineValue(i, k_left, b_left);
                         imageLine.Exist_Left[i] = 1;
                    }
                    break;
                }
            }

        }
        //������ʼ����2/3��ʱ(ͼ��ײ����������ߵĲ����е����),����
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
                if (count == 5)    //��5���㼴�ɿ�ʼ����
                {
                    leastSquareMethod(MendBasis_right[0], MendBasis_right[1], 5, &k_right, &b_right);
                    if (ABS(k_right) > 2 && i < ImageH_6)
                    {
                        count = 0;
                        continue;
                    }
                    //��ʼ����
                    for (i = i + 5; i < ImageH; i++)
                    {
                        imageLine.Point_Right[i] = getLineValue(i, k_right, b_right);
                        imageLine.Exist_Right[i] = 1;
                    }
                    break;
                }
            }

        }
    }
    //��ʮ��
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
            if (RoadLeft(i) > 90 && RoadLeft(i + 1) > 90 && RoadLeft(i + 2) > 90)//��������ȫ����
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
            if (RoadRight(i) > 90 && RoadRight(i + 1) > 90 && RoadRight(i + 2) > 90)//��������ȫ����
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

    //(һ) ���߶���(û����)
    if (imageLine.Lost_Left && imageLine.Lost_Right)
    {
        imageLine.Lost_Center = true;
    }

    //(��) ֻ�������
    else if (imageLine.Lost_Left)
    {
        imageLine.Lost_Center = false;
        //1. �������߲���
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Right[i])
            {
                err += (((ImageW) / 2 + roadK * i / 2 + roadB / 2) - imageLine.Point_Right[i]);
                count++;
            }
        }
        //2. ����ƽ�����
        aveErr = (float)(err / count);
        //3. �����ұ��߲�����(·��+���߲���)
        if (count >= 5 && aveErr > 0)//�����㹻��ȷ���ұ߽���������б
            centerCompensation = LIMIT2MIN(aveErr, SingleLineLeanAveERR_MAX) / SingleLineLeanAveERR_MAX * SingleLineLeanK / 2;//��������

        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Right[i])
            {
                //�������߲��޷�
                imageLine.Exist_Center[i] = true;
                amplitudeLIMIT(i, imageLine.Point_Right[i] - centerCompensation - roadK * i / 2 - roadB / 2 - (59 - i) * 0.15);
            }
        }
        limitCenter();//�Եó������߽���ͻ���޷�
    }
    //(��) ֻ���ұ���
    else if (imageLine.Lost_Right)
    {
        imageLine.Lost_Center = false;
        //1. �������߲���
        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i])
            {
                err += (imageLine.Point_Left[i] - (ImageW / 2 - roadK * i / 2 - roadB / 2));
                count++;
            }
        }

        //2. ����ƽ�����
        aveErr = (float)(err / count);
        //3. ��������߲�����(·��+���߲���)
        if (count >= 5 && aveErr > 0)//�����㹻��ȷ���ұ߽���������б
            centerCompensation = LIMIT2MIN(aveErr, SingleLineLeanAveERR_MAX) / SingleLineLeanAveERR_MAX * SingleLineLeanK / 2;//��������

        for (i = ImageH - 1; i >= 0; i--)
        {
            if (imageLine.Exist_Left[i])
            {
                //�������߲��޷�
                imageLine.Exist_Center[i] = true;
                amplitudeLIMIT(i, imageLine.Point_Left[i] + centerCompensation + roadK * i / 2 + roadB / 2 + (59 - i) * 0.15);
            }
        }
        limitCenter();//�Եó������߽���ͻ���޷�
    }
    //(��)���߶�û�ж���
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
/**************************************�ײ㺯��****************************************************/
/*�ж��Ƿ�Ϊ�׵�*/
bool isWhite(uint16_t row, uint16_t line)
{
    //�����ж�
    if (!(row >= 0 && row < ImageH && line >= 0 && line < ImageW))
        return false;

    //�жϰ׵�ڵ�
    if (Pixle[row][line])
        return true;
    else
        return false;
}
/*�ж��Ƿ�Ϊ���ұ߽��*/
bool isLeftPoint(uint16_t i, uint16_t j)
{
    if (j < 2 || j >= ImageW - 3 || i<0 || i>ImageH - 1)//ͼ���Ե
        return false;
    //�ұ�һ�����ܳ�������
    if (((!isWhite(i, j)) || (!isWhite(i, j + 1)) || (!isWhite(i, j + 2)) || (!isWhite(i, j + 3)) || (!isWhite(i, j + 4)) || (!isWhite(i, j + 5))))
        return false;
    //���һ�����ܳ���·
    if (isWhite(i, j - 1) || isWhite(i, j - 2) || isWhite(i, j - 3) || isWhite(i, j - 4) || isWhite(i, j - 5))
        return false;

    return true;
}
bool isRightPoint(uint16_t i, uint16_t j)
{
    if (j < 2 || j >= ImageW - 3 || i<0 || i>ImageH - 1)//ͼ���Ե
        return false;
    //���һ�����ܳ�������
    if (((!isWhite(i, j)) || (!isWhite(i, j - 1)) || (!isWhite(i, j - 2)) || (!isWhite(i, j - 3)) || (!isWhite(i, j - 4)) || (!isWhite(i, j - 5))))
        return false;
    //�ұ�һ�����ܳ���·
    if (isWhite(i, j + 1) || isWhite(i, j + 2) || isWhite(i, j + 3) || isWhite(i, j + 4) || isWhite(i, j + 5))
        return false;

    return true;
}
/*�߽����Ƿ�Ϊֱ�� (EndLine �жϽ�����)*/
bool LeftLine_Check(uint16_t EndLine)
{
    short i = 0, j = 0;
    short StartLine = 0;
    //���յ�
    if (LTurnPoint_Row >= EndLine)
        return false;
    //�ҵ��������ʼ��
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
    //�ж�����λ���Ƿ�����
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
    //�ж���߽��������Ƿ�����
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
    //�ж������б���Ƿ�����
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
    //���յ�
    if (RTurnPoint_Row >= EndLine)
        return false;
    //�ҵ��ұ�����ʼ��
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
    //�ж�����λ���Ƿ�����
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
    //�ж��ұ߽��������Ƿ�����
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
    //�ж��ұ���б���Ƿ�����
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
/*ͼ���ҹյ�*/
void TurnPointL_Find(short* TurnPoint_Row, short* TurnPoint, float* kl_up, float* bl_up, float* kl_down, float* bl_down, uint8_t* error_cnt_l)
{
    uint8_t err_cnt = 0, StartLineL = 0;
    short i = 0, j = 0, k = 0;
    short BasisLine_Up[3] = { 0,0,0 }, BasisPoint_Up[3] = { 0,0,0 };
    short BasisLine_Down[3] = { 0,0,0 }, BasisPoint_Down[3] = { 0,0,0 };
    short BreakLine[2] = { 0,0 }, BreakPoint[2] = { 0,0 };
    bool flag_left2 = false, flag_enough = false, flag_right1 = false, flag_break = false, flag_err1 = false, flag_err2 = false;

    singlePoint_Filter();  //��������ȥ
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
        //45����������6��û�б߽��ֱ���˳�
        if ((i <= ImageH - 20) && (!imageLine.Exist_Left[i] && !imageLine.Exist_Left[i - 1] && !imageLine.Exist_Left[i - 2]
            && !imageLine.Exist_Left[i - 3] && !imageLine.Exist_Left[i - 4] && !imageLine.Exist_Left[i - 5]))
            break;
        //״̬1��δ�ҵ������Ƶ㡣
        //�����ҵ�3�������Ƶ㣬����¼��������б���б�
        //      �����ҵ��ϵ�
        if (!flag_break && !flag_left2 && imageLine.Exist_Left[i])
        {
            //����1�������Ƶ�
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
            //�ҵ�1�������Ƶ��������������
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
                                if (imageLine.Point_Left[k] > imageLine.Point_Left[j])//�ҵ�3�������Ƶ�֮ǰ����������ת���ж�Ϊ�ٹյ�
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
                    //���������Ƶ��Ϸ��Ƿ�Ϊ��
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
            //û���ҵ������Ƶ㣬����Ƿ�Ϊ�ϵ�
            else if (imageLine.Point_Left[i] < ImageW_2_3 && !imageLine.Exist_Left[i - 1] &&
                !imageLine.Exist_Left[i - 2] && !imageLine.Exist_Left[i - 3])
            {
                BreakLine[1] = i;
                BreakPoint[1] = imageLine.Point_Left[i];
            }
            //�ҵ�1���ϵ�������ж��Ƿ��ǰ׵�
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
        //״̬2���ҵ�3�������Ƶ㡣
        //�����������Ƶ��·��ҵ�3�������Ƶ㣬����¼��������б���б�
        if (flag_left2 && !flag_right1)
        {
            //���������Ƶ��·����µ�
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
            //�ҵ����µ��������2�������Ƶ�
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
                                if (imageLine.Point_Left[k] > imageLine.Point_Left[j])//�ҵ�3�������Ƶ�֮ǰ����������ת���ж�Ϊ�ٹյ�
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
            if (!flag_right1)//û���ҵ�3�������Ƶ㣬�建�棬������
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
        //״̬3���ҵ��ϵ㡣
        //�����ڶϵ��·��ҵ�3�������Ƶ㡣
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
        //״̬4���ҵ�3�������Ƶ㣬�����·���3�������Ƶ㡣
        //���񣺷����ϡ��¹յ�(��ϵ�)��k��b
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
        //״̬5���ҵ��ϵ㡣
        //���񣺷��ضϵ㡢k��b
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

    singlePoint_Filter();  //��������ȥ
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
        //45����������6��û�б߽��ֱ���˳�
        if ((i <= ImageH - 20) && (!imageLine.Exist_Right[i] && !imageLine.Exist_Right[i - 1] && !imageLine.Exist_Right[i - 2]
            && !imageLine.Exist_Right[i - 3] && !imageLine.Exist_Right[i - 4] && !imageLine.Exist_Right[i - 5]))
            break;
        //״̬1��δ�ҵ������Ƶ㡣
        //�����ҵ�3�������Ƶ㣬����¼��������б���б�
        //      �����ҵ��ϵ�
        if (!flag_break && !flag_right2 && imageLine.Exist_Right[i])
        {
            //����1�������Ƶ�
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
            //�ҵ�1�������Ƶ��������������
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
                                if (imageLine.Point_Right[k] < imageLine.Point_Right[j])//�ҵ�3�������Ƶ�֮ǰ����������ת���ж�Ϊ�ٹյ�
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
                else//û���ҵ����������Ƶ㣬�ж��Ƿ���ڶϵ㣬���û�����建�棬������
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
            //û���ҵ������Ƶ㣬����Ƿ�Ϊ�ϵ�
            else if (imageLine.Point_Right[i] > ImageW_3 && !imageLine.Exist_Right[i - 1] &&
                !imageLine.Exist_Right[i - 2] && !imageLine.Exist_Right[i - 3])
                 {
                      BreakLine[1] = i;
                      BreakPoint[1] = imageLine.Point_Right[i];
                 }
            //�ҵ�1���ϵ�������ж��Ƿ��ǰ׵�
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
        //״̬2���ҵ�3�������Ƶ㡣
        //�����������Ƶ��·��ҵ�3�������Ƶ㣬����¼��������б���б�
        if (flag_right2 && !flag_left1)
        {
            //���������Ƶ��·����µ�
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
                //�ҵ����µ��������2�������Ƶ�
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
                                if (imageLine.Point_Right[k] < imageLine.Point_Right[j])//�ҵ�3�������Ƶ�֮ǰ����������ת���ж�Ϊ�ٹյ�
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
            if (!flag_left1)//û���ҵ�3�������Ƶ㣬�建�棬������
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
        //״̬3���ҵ��ϵ㡣
        //�����ڶϵ��·��ҵ�3�������Ƶ㡣
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
        //״̬4���ҵ�3�������Ƶ㣬�����·���3�������Ƶ㡣
        //���񣺷����ϡ��¹յ㡢k��b
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
        //״̬5���ҵ��ϵ㡣
        //���񣺷��ضϵ㡢k��b
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

// **************************************************************ʮ��**************************************
/*����ĳ���Ұ�߻����ߵİ׵���*/
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


uint8_t Point_LeftRight(uint8_t row)//���ұ߿�ʼ�ҵ�һ����߽�
{
    uint8 i = 0, j = 0;
    for (i = ImageW - 1;i > 0;i--)
    {
        if (isWhite(row, i))//�ұߵ�һ���׵�
            break;
    }
    for (j = i - 1;j < ImageW;j--)
    {
        if (!isWhite(row, j))
            break;

    }
    return j + 1;
}
uint8_t Point_RightLeft(uint8_t row)//����߿�ʼ�ҵ�һ���ұ߽�
{
    uint8 white = 0, i = 0, j = 0;
    for (i = 0;i < ImageW;i++)
    {

        if (isWhite(row, i))//��ߵ�һ���׵�
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
 * �������ƣ�RoadRight
 * ����˵�������ұߵ�һ���ڵ㿪ʼ����Ŀ��
 * ����˵����row:����
 * �������أ����е��ұ�·��
 * ������ע��*
 ***************************************************************************/

uint8_t RoadRight(uint8_t row)
{
    uint8 white = 0, i = 0, j = 0;
    for (i = ImageW - 1;i > 0;i--)
    {
        if (isWhite(row, i))//�ұߵ�һ���׵�
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
 * �������ƣ�RoadLeft
 * ����˵��������ߵ�һ���׵㿪ʼ����Ŀ��
 * ����˵����row:����
 * �������أ����е����·��
 * ������ע��*
 ***************************************************************************/
int8_t RoadLeft(uint8_t row)
{
    uint8 white = 0, i = 0, j = 0;
    for (i = 0;i < ImageW;i++)
    {
        if (isWhite(row, i))//��ߵ�һ���׵�
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


/****************************************Բ��******************************************************/
/*��Բ���������*/
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
/*�ж���Բ�������ڲ���*/
void Left_Ring_Find(void)
{
    short i = 0, j = 0;
    short Down_Point = EFFECTIVE_ROW, Divide_Point = EFFECTIVE_ROW, Mid_Point = EFFECTIVE_ROW, Top_Point = EFFECTIVE_ROW;
    short L_Max = 0;

    if (Flag.Left_Ring_Turn || Flag.Left_Ring_Out || Flag.Left_Ring_Out_Mend1 || Flag.Left_Ring_Out_Mend2)//��Բ���ĺ�״̬��ִ����Բ��
        return;
    /************************�һ�����ֱ���˵�***********************/
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
    /************************�һ����·�ֱ���ֽ��***********************/
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
    /************************�һ���ת�۵�***********************/
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
    /************************ʶ��Բ��֮��ʧת�۵�***********************/
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
    /************************�һ�����ֱ���˵�***********************/
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
    /************************��ֹ��***********************/
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
    /************************״̬ת��***********************/
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
    /************************���߼��˲�***********************/
    if (Down_Point != EFFECTIVE_ROW)//�˳��յ��Ϸ������ұ߽�� ��ֹ����������ϳ�����
    {
        for (i = Down_Point - 1; i >= 0; i--)
        {
            imageLine.Exist_Left[i] = false;
        }
        float k_Left = 0, b_Left = 0;//�����¹յ�������
        k_Left = ((float)imageLine.Point_Left[Down_Point] - (float)imageLine.Point_Left[Mid_Point])
            / ((float)Down_Point - (float)Mid_Point);
        b_Left = (float)imageLine.Point_Left[Mid_Point] - k_Left * Mid_Point;
        //��ʼ����
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
    else //�·��յ㶪ʧ ���Ϸ��յ�����������߲���
    {
        float k_left = 0, b_left = 0;
        k_left = ((float)10 - (float)imageLine.Point_Left[Mid_Point])
            / ((float)(ImageH - 1) - (float)Mid_Point);
        b_left = (float)imageLine.Point_Left[Mid_Point] - k_left * Mid_Point;

        //��ʼ����
        for (i = ImageH - 1; i >= 0; i--)
        {
            imageLine.Exist_Left[i] = true;
            imageLine.Point_Left[i] = getLineValue(i, k_left, b_left);
            Flag.Left_Ring_Find = true;
            Flag.Left_Ring = true;
        }
    }
}
/*���߽���Բ��*/
void Left_Ring_Turn(void)
{
    short i = 0, j = 0;
    short Top_Point = ImageW, Top_Point_row = EFFECTIVE_ROW, Divide_Point = EFFECTIVE_ROW;
    short Endpoint_R = ImageW, Startline_R = 0;
    bool count_flag = false;

    if (!Flag.Left_Ring_Turn)
        return;
    /************************�һ����϶˵�***********************/
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
    /************************�һ�����ֱ���ϵ�***********************/
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
    /************************״̬ת��***********************/
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
    /************************���߼��˲�***********************/
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
/*����Բ������*/
void Left_Ring_Out(void)
{
    short i = 0, j = 0, k = 0;
    short TurnPoint_Row = EFFECTIVE_ROW, StratLine_R = EFFECTIVE_ROW, EndLine_R = EFFECTIVE_ROW;

    if (!Flag.Left_Ring_Out)
        return;
    /************************�ҹյ�***********************/
    TurnPoint_Row = RTurnPoint_Row;
    /************************״̬ת��***********************/
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
    /************************�����˲�***********************/
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
    /************************���߼��˲�***********************/
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
    if (count == 5)    //��5���㼴�ɿ�ʼ����
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
/*����Բ����ͼ��̶����*/
void Left_Ring_Out_Mend1(void)
{
    short i = 0, j = 0;
    short StartLine_R = EFFECTIVE_ROW;

    if (!Flag.Left_Ring_Out_Mend1)
        return;
    /************************�ų����½Ǳ߽��Ӱ��***********************/
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
    /************************�����ұ߽��λ��***********************/
    for (i = ImageH - 1; i >= ImageH_6; i--)
    {
        if (imageLine.Exist_Right[i])
        {
            StartLine_R = i;
            break;
        }
    }
    /************************״̬ת��***********************/
    if (StartLine_R >= ImageH - 15)
    {
        Flag.Left_Ring_Out_Mend1 = false;
        Flag.Left_Ring_Out_Mend2 = true;
    }
}
/*����Բ����ڲ���*/
void Left_Ring_Out_Mend2(void)
{
    short i = 0, j = 0, W_cnt = 0;
    bool count_flag = 0;
    short Top_Point = EFFECTIVE_ROW, Startline_L = EFFECTIVE_ROW;

    if (!Flag.Left_Ring_Out_Mend2)
        return;
    /************************��Բ�����������ֽ�***********************/
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
    /************************��Բ���϶˵�***********************/
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
    /************************״̬ת��***********************/
    if (Top_Point >= 45 || Startline_L == EFFECTIVE_ROW)
    {
        Flag.Left_Ring_Out_Mend2 = false;
        Flag.Left_Ring_OnlyOnce = false;
        ring_cam.Left_Ring_Clc--;
        Flag.Left_Ring = false;
        Flag.Left_Ring_Out_Record = true;

        //if(fork_param.ForkRoad_Position == 1)                         //����״̬����
            //Flag.ForkRoad_OnlyOnce = true;
        return;
    }
    /************************�����˲�***********************/
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
    if (count == 5)    //��5���㼴�ɿ�ʼ����
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
    //��ʼ����
    for (i = ImageH - 1; i >= Top_Point; i--)
    {
        imageLine.Exist_Left[i] = true;
        imageLine.Point_Left[i] = getLineValue(i, k_left, b_left);
    }
}
/*��Բ���������*/
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
/*�ж���Բ�������ڲ���*/
void Right_Ring_Find(void)
{
    short i = 0, j = 0;
    short Down_Point = EFFECTIVE_ROW, Divide_Point = EFFECTIVE_ROW, Mid_Point = EFFECTIVE_ROW, Top_Point = EFFECTIVE_ROW;
    short R_Min = ImageW;

    if (Flag.Right_Ring_Turn || Flag.Right_Ring_Out || Flag.Right_Ring_Out_Mend1 || Flag.Right_Ring_Out_Mend2)//��Բ���ĺ�״̬��ִ����Բ��
        return;
    /************************�һ�����ֱ���˵�***********************/
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
    /************************�һ����·�ֱ���ֽ��***********************/
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
    /************************�һ���ת�۵�***********************/
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
    /************************ʶ��Բ��֮��ʧת�۵�***********************/
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
    /************************�һ�����ֱ���˵�***********************/
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
    /************************��ֹ��***********************/
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
    /************************״̬ת��***********************/
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
    /************************���߼��˲�***********************/
    if (!Flag.Right_Ring_Find)
        Flag.Right_Ring_Find_Record = true;
    else
        Flag.Right_Ring_Find_Record = false;
    if (Down_Point != EFFECTIVE_ROW)//�˳��յ��Ϸ������ұ߽�� ��ֹ����������ϳ�����
    {
        for (i = Down_Point - 1; i >= 0; i--)
        {
            imageLine.Exist_Right[i] = false;
        }
        float k_Right = 0, b_Right = 0;//�����¹յ�������
        k_Right = ((float)imageLine.Point_Right[Down_Point] - (float)imageLine.Point_Right[Mid_Point])
            / ((float)Down_Point - (float)Mid_Point);
        b_Right = (float)imageLine.Point_Right[Mid_Point] - k_Right * Mid_Point;
        //��ʼ����
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
    else //�·��յ㶪ʧ ���Ϸ��յ�����������߲���
    {
        float k_right = 0, b_right = 0;
        k_right = ((float)84 - (float)imageLine.Point_Right[Mid_Point])
            / ((float)(ImageH - 1) - (float)Mid_Point);
        b_right = (float)imageLine.Point_Right[Mid_Point] - k_right * Mid_Point;
        //��ʼ����
        for (i = ImageH - 1; i >= 0; i--)
        {
            imageLine.Exist_Right[i] = true;
            imageLine.Point_Right[i] = getLineValue(i, k_right, b_right);
            Flag.Right_Ring_Find = true;
            Flag.Right_Ring = true;
        }
    }
}
/*���߽���Բ��*/
void Right_Ring_Turn(void)
{
    short i = 0, j = 0;
    short Top_Point = ImageW, Top_Point_row = EFFECTIVE_ROW, Divide_Point = EFFECTIVE_ROW;
    short Endpoint_L = 0, Startline_L = 0;
    bool count_flag = false;

    if (!Flag.Right_Ring_Turn)
        return;
    /************************�һ����϶˵�***********************/
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
    /************************�һ�����ֱ���ϵ�***********************/
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
    /************************״̬ת��***********************/
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
    /************************���߼��˲�***********************/
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
/*����Բ������*/
void Right_Ring_Out(void)
{
    short i = 0, j = 0, k = 0;
    short TurnPoint_Row = EFFECTIVE_ROW, StratLine_L = EFFECTIVE_ROW, EndLine_L = EFFECTIVE_ROW;

    if (!Flag.Right_Ring_Out)
        return;
    /************************�ҹյ�***********************/
    TurnPoint_Row = LTurnPoint_Row;
    /************************״̬ת��***********************/
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
    /************************�����˲�***********************/
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
    /************************���߼��˲�***********************/
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
    if (count == 5)    //��5���㼴�ɿ�ʼ����
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
/*����Բ����ͼ��̶����*/
void Right_Ring_Out_Mend1(void)
{
    short i = 0, j = 0;
    short StartLine_L = EFFECTIVE_ROW;

    if (!Flag.Right_Ring_Out_Mend1)
        return;
    /************************�ų����½Ǳ߽��Ӱ��***********************/
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
    /************************������߽��λ��***********************/
    for (i = ImageH - 1; i >= ImageH_6; i--)
    {
        if (imageLine.Exist_Left[i])
        {
            StartLine_L = i;
            break;
        }
    }
    /************************״̬ת��***********************/
    if (StartLine_L >= ImageH - 10)
    {
        Flag.Right_Ring_Out_Mend1 = false;
        Flag.Right_Ring_Out_Mend2 = true;
    }
}
/*����Բ����ڲ���*/
void Right_Ring_Out_Mend2(void)
{
    short i = 0, j = 0, W_cnt = 0;
    bool count_flag = 0;
    short Top_Point = EFFECTIVE_ROW, Startline_R = EFFECTIVE_ROW;

    if (!Flag.Right_Ring_Out_Mend2)
        return;
    /************************��Բ�����������ֽ�***********************/
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
    /************************��Բ���϶˵�***********************/
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
    /************************״̬ת��***********************/
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
    /************************�����˲�***********************/
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
    if (count == 5)    //��5���㼴�ɿ�ʼ����
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
    //��ʼ����
    for (i = ImageH - 1; i >= Top_Point; i--)
    {
        imageLine.Exist_Right[i] = true;
        imageLine.Point_Right[i] = getLineValue(i, k_right, b_right);
    }
}
/****************************************�µ�******************************************************/



void Ramp_task(void)
{
    if (Flag.Ramp)
    {
        AIM_LINE = Ramp_AimLine;
    }
      static uint8_t ramp_time_cnt = 0;
      short lost_cnt = 20,i= 0;
      if (Ramp_Cnt <= 0 || Flag.barricade || Flag.Garage_Out || Flag.break_Road || Flag.Right_Ring_Find || Flag.Right_Ring_Turn
              || Flag.Left_Ring_Find || Flag.Left_Ring_Turn)
          return;
      switch (ramp_task_state)
      {
          case 0: //ʶ��״̬
              if (dl1b_distance_mm < Ramp_Dis_up)
                  {
                  for(i=20;i<40;i++)
                      {
                      if(imageLine.Exist_Center[i] && ABS(imageLine.Point_Center[i] - center) < 10)
                          lost_cnt--;
                      }
                  if(lost_cnt < 5)//20-40�в��������߲�����5��
                      ramp_time_cnt++;
                  }
              else
                  ramp_time_cnt = 0;

              if (ramp_time_cnt > 2)
              {
                  // ʶ���µ����ı�Ŀ����
                  Flag.Ramp = true;
                  AIM_LINE = Ramp_AimLine;
                  ramp_task_state ++;
                  eulerAngle.pitch = 0;
                  beep_set_sound(BEEP_ON);
              }
              break;
          case 1: //ƽ·�������º����״̬
              if (eulerAngle.pitch > Ramp_pitch_1 )
              {
                  ramp_task_state++;
              }
              break;
          case 2://���½�����ʼ����
              if (eulerAngle.pitch < -Ramp_pitch_2)
              {
                  ramp_task_state++;
              }
              break;
          case 3://���½���������������״̬������
              if (ABS(eulerAngle.pitch) < Ramp_pitch_3)
              {
                  Flag.Ramp = false;
                  ramp_task_state = 0;
                  Ramp_Cnt--;
                  beep_set_sound(BEEP_OFF);

              }
              break;
          default:
              break;
      }

//      oled_show_int(1, 1, ramp_task_state, 1);
}


void  barricade_by_image(void)
{
    if (Barr_Cnt <= 0 || Flag.Ramp|| Flag.Garage_Out || Flag.Right_Ring|| Flag.Left_Ring)
    {
        Flag.barricade = 0;
        return;
    }

    if(Bar_break_order < 16 && (Bar_break_order >> (Bar_break_count - 1) & 1) == 0)
    {
        return;
    }




    float k, b, err;
    short x[2], y[2];


    short i, L_Exit_Line = EFFECTIVE_ROW, R_Exit_Line = EFFECTIVE_ROW;

    if (!Flag.barricade)//ʶ��·��
    {
        if (dl1b_distance_mm < Barr_Dis_up)
          {

              for (i = ImageH;i > 3;i--)
              {
                  if (imageLine.Exist_Left[i] && imageLine.Exist_Left[i + 1] && imageLine.Exist_Left[i + 2] && (!imageLine.Exist_Left[i - 1]) && (!imageLine.Exist_Left[i - 2]) && (!imageLine.Exist_Left[i - 3]))
                  {
                      L_Exit_Line = i;
                  }
              }

              for (i = ImageH;i > 3;i--)
              {
                  if (imageLine.Exist_Right[i] && imageLine.Exist_Right[i + 1] && imageLine.Exist_Right[i + 2] && (!imageLine.Exist_Right[i - 1]) && (!imageLine.Exist_Right[i - 2]) && (!imageLine.Exist_Right[i - 3]))
                  {
                      R_Exit_Line = i;
                  }
              }
              if ((R_Exit_Line > barricade_Line) && (L_Exit_Line > barricade_Line) && (ABS(R_Exit_Line - L_Exit_Line) < 5)  && ((imageLine.Point_Right[R_Exit_Line] - imageLine.Point_Left[L_Exit_Line]) > 8))
              {
                  Flag.barricade = true;
                  barr_state += 1;

                  beep_set_sound(BEEP_ON);

              }
          }

    }


    if (Flag.barricade && barr_state == 1)
    {
        // �ڵ׶����ߺͶ��ߵ�����߲���

        x[0] = ImageH - 1;
        y[0] = ImageW / 2;


        for (i = x[0]; i >= 0; i--)
        {
            if (!imageLine.Exist_Center[i])
            {
                x[1]   = i+1;
                if(Barr_Direction == 0)
                {
                    y[1]   = imageLine.Point_Left[x[1]] - bar_width;
                    if(y[1] < 0)
                        y[1] = 0;
                }
                else
                {
                    y[1]   = imageLine.Point_Right[x[1]] + bar_width;
                    if(y[1] > ImageW-1)
                        y[1] = ImageW-1;
                }


                break;
            }
        }

        leastSquareMethod(x, y, 2, &k, &b);
        err = getLeastSquareMethodERROR(x, y, 2, k, b);
        if (ABS(err) > 2)
        {
            barr_state += 1;
            eulerAngle.yaw = 0;
        }
        else
        {
            // ����
            for (i = ImageH - 1; i >= 0; i--)
            {
                imageLine.Exist_Center[i] = true;
                imageLine.Point_Center[i] = getLineValue(i, k, b);
            }
        }


    }

    if (Flag.barricade && barr_state == 2)
      {
          x[0] = ImageH - 1;
          y[0] = ImageW / 2;

          x[1] = 0;

          if(Barr_Direction == 0)
          {
              y[1] = ImageW + bar_width2;
          }
          else
          {
              y[1] = 0 - bar_width2;
          }

          leastSquareMethod(x, y, 2, &k, &b);

              // ����
          for (i = ImageH - 1; i >= 0; i--)
          {
              imageLine.Exist_Center[i] = true;
              imageLine.Lost_Center = false;
              imageLine.Point_Center[i] = getLineValue(i, k, b);
          }

          if(Barr_Direction == 0)
          {
              if ((ABS(eulerAngle.yaw) > Barr_Fixd_Dis_up))
              {
                  barr_state += 1;
                  eulerAngle.yaw = 0;
              }
          }
          else
          {
              if ((ABS(eulerAngle.yaw) > Barr_Fixd_Dis_up))
              {
                  barr_state += 1;
                  eulerAngle.yaw = 0;
              }
          }

      }


    if (Flag.barricade && barr_state == 3)
      {
        for (i = ImageH - 1; i >= 0; i--)
           {
               imageLine.Exist_Center[i] = true;
               imageLine.Lost_Center = false;
               imageLine.Point_Center[i] = 47;
           }
           if (Barr_Direction == 0)
           {
               if ( imageLine.Exist_Left[ImageH - 1] && (imageLine.Point_Left[ImageH - 1] > 0))// &&
               {
                   Flag.barricade = 0;
                   Barr_Cnt--;
                   Bar_break_count--;

               }
           }
           else
           {
               if ( imageLine.Exist_Right[ImageH - 1] && (imageLine.Point_Right[ImageH - 1] < ImageW))// &&
               {
                   Flag.barricade = 0;
                   Barr_Cnt--;
                   Bar_break_count--;
               }
           }


      }



}

/***************************************������*****************************************************/
/*Ѱ�Һڵ����жϰ�����*/
void zebra_found_zz(void)
{
    uint8_t i = 0, j = 0;
    uint8_t count1 = 0, count2 = 0;
    uint16_t add1 = 0, add2 = 0;
    uint8_t bianli_qidian1 = 0;
    uint8_t bianli_qidian2 = 0;
    uint8_t poat_num = 0;
    static uint8_t lose_cnt = 0;

    if ((Flag.Left_Ring && !Flag.Left_Ring_Find) || (Flag.Right_Ring && !Flag.Right_Ring_Find)  || Flag.Ramp || Flag.Garage_Out || Flag.barricade || Flag.break_Road || Flag.Begin_run)
        return;
    for (i = 46 - 2; i < 46 + 2; i++)
    {
        //�õ���Ҫ�������з�Χ
        if (imageLine.Point_Left[i] < 70 && imageLine.Point_Left[i]>10)
        {
            add1 = add1 + imageLine.Point_Left[i];
            count1++;
        }

        if (imageLine.Point_Right[i] > 60)
        {
            add2 = add2 + imageLine.Point_Right[i];
            count2++;
        }
    }

    if (count1)  //��ֹһ���߽�㶼û��
        bianli_qidian1 = add1 / count1;
    if (count2)  //��ֹһ���߽�㶼û��
        bianli_qidian2 = add2 / count2;


    for (i = 20; i < ImageH - 10; i++)
    {
        for (j = bianli_qidian1; j < bianli_qidian2; j++)
        {
            if ((Pixle[i][j] == 0
                && Pixle[i + 3][j] == 1
                && Pixle[i - 1][j] == 1
                && Pixle[i][j + 3] == 1
                && Pixle[i][j - 1] == 1)
                ||
                (
                    Pixle[i][j] == 0
                    && Pixle[i + 1][j] == 0
                    && Pixle[i][j + 1] == 0
                    && Pixle[i + 1][j + 1] == 0

                    && Pixle[i - 1][j - 1] == 1
                    && Pixle[i - 1][j] == 1
                    && Pixle[i - 1][j + 1] == 1
                    && Pixle[i - 1][j + 2] == 1

                    && Pixle[i][j - 1] == 1

                    && Pixle[i + 1][j - 1] == 1

                    && Pixle[i + 2][j - 1] == 1

                    && Pixle[i + 3][j] == 1
                    && Pixle[i][j + 3] == 1
                    && Pixle[i + 3][j + 3] == 1
                    )
                )
                poat_num++;
            if (poat_num >= 5)
                break;
        }
        if (poat_num >= 5)
        {
            if(i > 30 && Garage_Clc == 1)
            {
                bluetooth_ch9141_send_string("2ga");
            }
            break;
        }

    }




    if (poat_num >= 5)
    {
        Flag.Garage_Find = true;

//        sprintf(txt, "%2d", i);
//        bluetooth_ch9141_send_string(txt);

    }
    else if (Flag.Garage_Find && poat_num < 1)
    {
        lose_cnt++;

        if (lose_cnt > 12)
        {
            lose_cnt = 0;
            Flag.Garage_Find = false;
            Flag.Garage_Go = true;
            Garage_Clc--;
            if(Garage_Clc == 0)
            {
                beep_set_sound(BEEP_ON);
                Flag.Ui_Stop_Flag = 1;
            }
        }
    }
    else
        Flag.Garage_Go = false;

}

/***************************************��·2******************************************************/

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
        }
        else
        {
            break_cnt = 0;
        }
    }

}

/**************************************�������****************************************************/
/*�������ߴ��*/
bool MediumLineCal(volatile float* final)
{
    uint8 i;
    static float lastCenter = 47;
    short temp_dev = 0, SpeedDown_Line = 0;

    //��������
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
        //��������д����߲����ڣ�������15����
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
                //�����������
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
        *(final) = *(final + 1);        // �����򱣳��ϴε����
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

/*�������ߴ��*/
void updateMediumLine(void)
{
    bool rst = true;
    static uint8_t lostTime = 0;

    if(Flag.break_Road_in )
    {
        return;
    }

    /************************�������ߴ��*****************************/
    rst = MediumLineCal(image.cam_finalCenterERR);

    servo_param.Servo_filter =
        0.7f * image.cam_finalCenterERR[0] + 0.15f * image.cam_finalCenterERR[1]
        + 0.1f * image.cam_finalCenterERR[2] + 0.05f * image.cam_finalCenterERR[3];

    //���ߴ��
    if (rst)//����ͷ���
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
    else if (!rst && (lostTime <= 3))//���߲��࣬������ͷ֮ǰ��ֵ��
    {
        lostTime++;
        Flag.turnWAY_state = 1;
    }
    /*else if(!Flag.Garage_Out && !Flag.Right_Ring && !Flag.Left_Ring)    //����̫�࣬�����
    {
        CHARGE_OFF;
        Flag.break_Road = 1;
        Flag.turnWAY_state = 8;
    }*/
    if(longStraightRoadJudge() && !longStraightFlag && !Flag.Ramp && !Flag.Garage_Find)//�жϳ�ֱ��
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

/**************************************��****************************************************/

int parent[max_size] = { 0 };
int kind[max_size] = { 0 };
// �ҵ�label x�ĸ��ڵ�
int find(int x) {
    int i = x;
    while (0 != parent[i])
        i = parent[i];
    return i;
}

// ��label x �� label y�ϲ���ͬһ����ͨ��
void union_label(int x, int y) {
    int i = x;
    int j = y;
    while (0 != parent[i])
        i = parent[i];
    while (0 != parent[j])
        j = parent[j];
    if (i != j)
        parent[MAX(i,j)] = MIN(i,j);
}

int array_max(int arr[], int size) {
    int max_index = 0;
    int max_value = arr[0];

    for (int i = 1; i < size; i++) {
        if (arr[i] > max_value) {
            max_value = arr[i];
            max_index = i;
        }
    }
    return max_index;
}


bool two_pass(void)
{
    int i = 0;
    int j = 0;
    uint8 label = 1;
    for (j = 0; j < max_size; j++)
    {
        parent[j] = 0;
        kind[j] = 0;
    }
    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
            image_pross[i][j] = 0;

    }

    for (i = 1; i < ImageH; i++)
    {
        for (j = 1; j < ImageW; j++)
        {
            if (Pixle[i][j])
            {
                if (!image_pross[i][j - 1] && !image_pross[i - 1][j])
                {
                    image_pross[i][j] = label;
                    label++;
                }
                else if (image_pross[i][j - 1] && !image_pross[i - 1][j])
                {
                    image_pross[i][j] = image_pross[i][j - 1];
                }
                else if(!image_pross[i][j - 1] && image_pross[i - 1][j])
                    image_pross[i][j] = image_pross[i - 1][j];
                else
                {
                    if(image_pross[i][j - 1] == image_pross[i - 1][j])
                        image_pross[i][j] = image_pross[i][j - 1];
                    else
                    {
                        image_pross[i][j] = MIN(image_pross[i][j - 1], image_pross[i - 1][j]);
                        union_label(image_pross[i][j - 1], image_pross[i - 1][j]);
                    }
                }
            }
        }
    }


    for (i = 1; i < ImageH; i++)
    {
        for (j = 1; j < ImageW; j++)
        {
            image_pross[i][j] = find(image_pross[i][j]);
        }
    }
    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
        {
            if(image_pross[i][j])
                kind[image_pross[i][j]]++;
        }
    }
    label = array_max(kind, max_size);
    if (kind[label] < pixel_min_num || kind[label] > pixel_max_num)
        return 0;
    for (i = 0; i < ImageH; i++)
    {
        for (j = 0; j < ImageW; j++)
        {
            if (image_pross[i][j] == label)
                Pixle[i][j] = true;
            else
                Pixle[i][j] = false;
        }
    }
    return 1;


}

bool MediumLineCal_2(volatile float* final)
{
    uint8 i;
    for (i = 0; i < 10; i++)
        *(final + 10 - i) = *(final + 9 - i);

    *(final) = (img_x - (center + biaos));///(ImageH - img_y)*(ImageH - Break_Line)
    return true;
}
