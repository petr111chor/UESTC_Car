/*********************************************************************************************************************
* TC377 Opensourec Library ����TC377 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC377 ��Դ���һ����
*
* TC377 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          cpu0_main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.9.4
* ����ƽ̨          TC377TP
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-11-03       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��

// *************************** ����Ӳ������˵�� ***************************
// ʹ����ɿƼ� Ӣ���� ��������������
//      ֱ�ӽ���������ȷ�����ں��İ�ĵ������ؽӿڼ���
//
// ���� ����Wifi SPI ģ��
//      ģ��ܽ�            ��Ƭ���ܽ�
//      RST                 �鿴 zf_device_wifi_spi.h �� WIFI_SPI_RST_PIN �궨��
//      INT                 �鿴 zf_device_wifi_spi.h �� WIFI_SPI_INT_PIN �궨��
//      CS                  �鿴 zf_device_wifi_spi.h �� WIFI_SPI_CS_PIN �궨��
//      MISO                �鿴 zf_device_wifi_spi.h �� WIFI_SPI_MISO_PIN �궨��
//      SCK                 �鿴 zf_device_wifi_spi.h �� WIFI_SPI_SCK_PIN �궨��
//      MOSI                �鿴 zf_device_wifi_spi.h �� WIFI_SPI_MOSI_PIN �궨��
//      5V                  5V ��Դ
//      GND                 ��Դ��


// *************************** ����ʹ�ò���˵�� ***************************
// 1.����Ӳ������˵�����Ӻ�ģ�飬ʹ�õ�Դ����(����������ᵼ��ģ���ѹ����)
//
// 2.�鿴���������ӵ�wifi����¼wifi���ƣ����룬IP��ַ
//
// 3.���·��Ĵ����������޸ĺ궨�壬WIFI_SSID_TESTΪwifi���ƣ�WIFI_PASSWORD_TESTΪwifi����
//
// 4.��zf_device_wifi_spi.h���޸�WIFI_SPI_TARGET_IP�궨�壬����Ϊwifi��IP��ַ
//
// 5.�������̵���Ƭ���У��򿪴������֣����������Ĵ���
//
// 6.��������֣�����ΪTCP Server��ѡ����ʵı�����ַ��������


// *************************** ���̲���˵�� ***************************
// 1.�����̻�ͨ�� Debug �������������Ϣ ����ؽӺõ��Դ����Ա��ȡ������Ϣ
//
// 2.���Ӻ�ģ��ͺ��İ�󣨾���ʹ��������������Ա��⹩�粻������⣩ ��¼������ ���¸�λ�����ʼ����
//
// 3.���ģ��δ��������ʼ�� ��ͨ�� DEBUG �������δ�ܳɹ���ʼ����ԭ�� ������᳢�����³�ʼ�� һ����������Ի�ɹ�
//
// 4.���һֱ�� Debug ����������� ����Ҫ��鱨������ ���鿴���ļ��·��ĳ��������б�����Ų�
//
// 5.����Ĭ�ϲ����� WIFI_SPI_AUTO_CONNECT �궨�� ͨ�� main �����еĽӿڽ����������� �����Ҫ�̶����н������� ���Կ����ú궨��
//
// 6.��ģ���ʼ����ɺ��ͨ�� DEBUG ���������ǰģ�����Ҫ��Ϣ���̼��汾��IP��Ϣ��MAC��Ϣ��PORT��Ϣ
//
// 7.�������� TCP Client ���� ģ��ᱻ����Ϊ TCP Client ��Ҫ���ӵ��������ڵ� TCP Server ���ܽ���ͨ��
//   Ŀ�����ӵ� TCP Server �� IP ��˿�Ĭ��ʹ�� zf_device_wifi_spi.h �� WIFI_SPI_TARGET_IP �� WIFI_SPI_TARGET_PORT ����
//   ʵ�ʲ�����Ҫ�����ֳ� TCP Server ��ʵ�� IP ��ַ��˿�����
//
// 8.�������豸�ɹ����ӵ� TCP Server ���������ʹ�����������λ������ TCP Server ģʽ Ȼ�󱾻����ӵ����Ե� IP ��˿ڣ�
//   �����̻���ͨ�����Դ��ڷ���������Ϣ��Ȼ���� Server ����һ�β������ݣ� ֮�����ʹ����λ���������ݸ��豸���豸�յ�֮��Ὣ���ݷ��ظ���λ��
//
// �������������˵�����ز��� ����ձ��ļ����·� ���̳�������˵�� �����Ų�

// **************************** �������� ****************************

#define WIFI_SSID_TEST          "wzwz"
#define WIFI_PASSWORD_TEST      "20040829" // �����Ҫ���ӵ�WIFI û������ �滻Ϊ NULL


uint8 wifi_spi_test_buffer[] = "this is wifi spi test buffer";
uint8 wifi_spi_get_data_buffer[256];
uint8 buffer[10];
uint32 datalength;

// **************************** �������� ****************************
void wifi(uint32 speed)
{


    // �˴���д�û����� ���������ʼ�������
    //buffer[0]=(uint8)(speed/10);
    //buffer[1]=(uint8)(speed%10);

    func_uint_to_str(buffer,speed);
    while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
    {
        printf("\r\n connect wifi failed. \r\n");
        system_delay_ms(100);                                                   // ��ʼ��ʧ�� �ȴ� 100ms
    }

    printf("\r\n module version:%s",wifi_spi_version);                          // ģ��̼��汾
    printf("\r\n module mac    :%s",wifi_spi_mac_addr);                         // ģ�� MAC ��Ϣ
    printf("\r\n module ip     :%s",wifi_spi_ip_addr_port);                     // ģ�� IP ��ַ

    // zf_device_wifi_spi.h �ļ��ڵĺ궨����Ը���ģ������(����) WIFI ֮���Ƿ��Զ����� TCP ������������ UDP ����
    if(0 == WIFI_SPI_AUTO_CONNECT)                                              // ���û�п����Զ����� ����Ҫ�ֶ�����Ŀ�� IP
    {
        while(wifi_spi_socket_connect(                                          // ��ָ��Ŀ�� IP �Ķ˿ڽ��� TCP ����
            "TCP",                                                              // ָ��ʹ��TCP��ʽͨѶ
            WIFI_SPI_TARGET_IP,                                                 // ָ��Զ�˵�IP��ַ����д��λ����IP��ַ
            WIFI_SPI_TARGET_PORT,                                               // ָ��Զ�˵Ķ˿ںţ���д��λ���Ķ˿ںţ�ͨ����λ��Ĭ����8080
            WIFI_SPI_LOCAL_PORT))                                               // ָ�������Ķ˿ں�
        {
            // ���һֱ����ʧ�� ����һ���ǲ���û�н�Ӳ����λ
            printf("\r\n Connect TCP Servers error, try again.");
            system_delay_ms(100);                                               // ��������ʧ�� �ȴ� 100ms
        }
    }


    // ���Ͳ���������������
    datalength = wifi_spi_send_buffer(wifi_spi_test_buffer, sizeof(wifi_spi_test_buffer));
    if(!datalength)
    {
        printf("\r\n send success.");
    }
    else
    {
        printf("\r\n %ld bytes data send failed.", datalength);
    }

    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();         // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        //data_length = wifi_spi_read_buffer(wifi_spi_get_data_buffer, sizeof(wifi_spi_get_data_buffer));
        datalength = sizeof(buffer);
        if(datalength)                                                     // ������յ����� ��������������ж�
        {
            printf("\r\n Get data: <%s>.", wifi_spi_get_data_buffer);
            if(!wifi_spi_send_buffer(buffer, datalength))
            {
                printf("\r\n send success.");
                memset(wifi_spi_get_data_buffer, 0, datalength);           // ���ݷ������ �������
            }
            else
            {
                printf("\r\n %ld bytes data send failed.", datalength);
            }
        }
        system_delay_ms(100);
        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}
// **************************** �������� ****************************

// *************************** ���̳�������˵�� ***************************
// ����1������û������
//      �鿴�������ִ򿪵��Ƿ�����ȷ�Ĵ��� ���򿪵� COM ���Ƿ��Ӧ���� Debug UART(USB ת TTL ���� ����������) �� COM ��
//
// ����2��������������
//      �鿴��������ʹ�õĲ������Ƿ��� zf_common_debug.h �� DEBUG_UART_BAUDRATE �궨���һ�� Ĭ���� 115200
//
// ����3����λ���ղ������ݻ���һֱ��ʾConnect TCP Servers error, try again.
//      ���Ĭ��ʹ�õ�IP ��ַ��zf_device_wifi_spi.h �� WIFI_SPI_TARGET_IP���Ƿ�����λ���ı�����ַһ��
//      ���Ĭ��ʹ�õ�ͨ�Ŷ˿ڣ�zf_device_wifi_spi.h �� WIFI_SPI_TARGET_PORT���Ƿ�����λ���Ķ˿ں�һ��
//
// ����4��һֱ��ʾ connect wifi failed
//      �� WIFI_SSID_TEST �궨���޸�Ϊ�����ʹ�õ� Wi-Fi ����
//      �� WIFI_PASSWORD_TEST �궨���޸�Ϊ�����ʹ�õ� Wi-Fi ����
//  ��������У���������ĵ�
//      1.���wifi�Ƿ���2.4Gwifi��5G�޷�����
//      2.����Ƿ�ʹ�õ�ع���
//      3.�����Եķ���ǽ�Ƿ�ر�
//      4.���wifi���ƻ���������û�������ַ����еĻ�ȫ��ɾ����ֻ����Ӣ�ĺ�����

#pragma section all restore


// **************************** �������� ****************************
