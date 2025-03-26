#ifndef _IMAGE_
#define _IMAGE_

#include"zf_common_headfile.h"

extern volatile uint8_t AIM_LINE_SET;                  //Ŀ����
extern volatile uint8_t RAMP_AIM_DELTA;
//extern volatile uint8_t RIGHT_RING_AIM_DELTA;
//extern volatile uint8_t LEFT_RING_AIM_DELTA;
extern volatile uint8_t AIM_LINE;
extern int biaos;
extern bool Flag_SpeedDown;


//ͼ��ɼ�������
#define ImageW                 94                      //ѹ�����ͼ���
#define ImageW_2               47                      
#define ImageW_3               31
#define ImageW_2_3             62
#define ImageH                 60                      //ѹ�����ͼ���
#define ImageH_2               30
#define ImageH_3               20
#define ImageH_4               15
#define ImageH_6               10
#define ImageH_2_3             40
#define ImageH_5_6             50
#define EFFECTIVE_ROW          4                       //����ͼƬ���ϲ����Ч��
#define TOTAL_POINT            ImageW*ImageH
#define center                 46.5

#define Shield_H               44
#define Shield_w1              15
#define Shield_w2               80
//�����˲�����
#define K_MAX_THRESHOLD          10                      //����Ч������б��
#define LOST_LINE_THRESHOLD      (60-EFFECTIVE_ROW )                 //���������ֵ
#define VALID_LINE_THRESHOLE     (60-LOST_LINE_THRESHOLD)  //��Ч����Сֵ
#define MINRoadLen               5                       //��С·��
#define roadK					(1.2653) 		 		 //ֱ����K  1.274
#define roadB					(1.347)			 		 //ֱ����B  1.789
#define SingleLineLeanAveERR_MAX 23                      //һ�߶��ߣ�δ����һ�����ƽ����б���
#define SingleLineLeanK          25                      //һ�߶��ߣ�δ����һ�߲���ϵ��(���۾���ʵ�����ߵľ���)

extern uint8 C_Image[ImageH][ImageW];
extern uint16 threshold;
extern uint16 light;


//�ṹ�嶨��
typedef struct
{
    volatile bool Exist_Left[ImageH];                 //ĳһ����߽߱�����
    volatile bool Exist_Right[ImageH];                //ĳһ���ұ߽߱�����
    volatile bool Exist_Center[ImageH];               //ĳһ�����ĵ����

    volatile bool Lost_Center;                           //�Ƿ�������
    volatile bool Lost_Left;                             //�Ƿ�������
    volatile bool Lost_Right;                            //�Ƿ�������

    volatile short Point_Left[ImageH];                //���ĳһ�е���߽��
    volatile short Point_Right[ImageH];               //���ĳһ�е��ұ߽��
    volatile short Point_Center[ImageH];              //���ĳһ�е����ĵ�

    volatile short White_Num[ImageH];                 //ĳһ�еİ׵����
}imageLine_t;


typedef struct
{
    volatile float cam_finalCenterERR[10];//����ƫ��
}Image_Data;

/************************************ͼ��ɼ�������************************************************/
extern uint8 bin_thr;
extern bool Pixle[ImageH][ImageW];
extern volatile imageLine_t imageLine;
extern Image_Data image;
void ImageProcess(void);
int GetOSTU(uint8 tmImage[ImageH][ImageW]);
int GetOSTU_2(uint8 tmImage[ImageH][ImageW]);
void trackDFS(void);
void Get_White_Num(void);
uint8 get_start_point(uint8 start_row);
void search_l_r(uint16 break_flag, uint8(*image)[ImageW], uint16 *l_stastic, uint16 *r_stastic,
        uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest);
void get_left(uint16 total_L);
void get_right(uint16 total_R);
void image_filter(uint8(*bin_image)[ImageW]);
void image_draw_rectan(uint8(*image)[ImageW]);

/****************************************�˲�******************************************************/
void Pixle_Filter(void);
void Left_Right_Confusion_Filter(void);
void doFilter(void);
void left_right_Limit(void);
void lineChangeLimit(void);
void singlePoint_Filter(void);
void lostLine_Filter(void);
/****************************************����******************************************************/
void amplitudeLIMIT(uint16_t i, int16_t amp);
void limitCenter(void);
void TurnFliter(short Lturnpoint_row, short Lturnpoint, short Rturnpoint_row, short Rturnpoint, float limitKl, float limitKr,
    float kl_up, float bl_up, float kl_down, float bl_down, float kr_up, float br_up, float kr_down, float br_down, uint8_t error_cnt_l, uint8_t error_cnt_r);
void doMend(void);
void trackMend_startPart(void);
void trackMend_HalfPart(void);
/**************************************�ײ㺯��****************************************************/
extern short LTurnPoint_Row, LTurnPoint;
extern short RTurnPoint_Row, RTurnPoint;
extern float Kl_up, Bl_up, Kl_down, Bl_down;
extern float Kr_up, Br_up, Kr_down, Br_down;
extern uint8_t Error_Cnt_L, Error_Cnt_R;
bool isWhite(uint16_t row, uint16_t line);
bool isLeftPoint(uint16_t i, uint16_t j);
bool isRightPoint(uint16_t i, uint16_t j);
bool LeftLine_Check(uint16_t EndLine);
bool RightLine_Check(uint16_t EndLine);
void TurnPointL_Find(short* TurnPoint_Row, short* TurnPoint, float* Kl_up, float* Bl_up, float* Kl_down, float* Bl_down, uint8_t* error_cnt_l);
void TurnPointR_Find(short* TurnPoint_Row, short* TurnPoint, float* Kr_up, float* Br_up, float* Kr_down, float* Br_down, uint8_t* error_cnt_r);
uint8_t road_right(uint8_t row);
uint8_t road_left(uint8_t row);
uint8_t RoadRight(uint8_t row);
int8_t RoadLeft(uint8_t row);

/****************************************Բ��******************************************************/
void Left_Ring(void);
void Left_Ring_Find(void);
void Left_Ring_Turn(void);
void Left_Ring_Out(void);
void Left_Ring_Out_Mend1(void);
void Left_Ring_Out_Mend2(void);
void Right_Ring(void);
void Right_Ring_Find(void);
void Right_Ring_Turn(void);
void Right_Ring_Out(void);
void Right_Ring_Out_Mend1(void);
void Right_Ring_Out_Mend2(void);

/***************************************��·******************************************************/
void Break_Road(void);
void Break_Road_2(void);


void updateMediumLine(void);//�������ߴ��
bool MediumLineCal(volatile float* final);//�������ߴ��
void show_line();


#endif


