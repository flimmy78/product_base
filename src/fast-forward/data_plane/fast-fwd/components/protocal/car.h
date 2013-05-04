#ifndef  _CAR_H_
#define   _CAR_H_

#include "cvmx-config.h"
#include "cvmx.h"
#include "cvmx-spinlock.h"
#include "cvmx-fpa.h"
#include "cvmx-pip.h"
#include "cvmx-ipd.h"
#include "cvmx-pko.h"
#include "cvmx-dfa.h"
#include "cvmx-pow.h"
#include "cvmx-gmx.h"
#include "cvmx-sysinfo.h"
#include "cvmx-coremask.h"
#include "cvmx-malloc.h"
#include "cvmx-bootmem.h"
#include "cvmx-helper.h"

#define CVM_CAR_SCALE_FACTOR_BIT_SHIFT  16
#define CVM_CAR_SCALE_FACTOR            (1 << CVM_CAR_SCALE_FACTOR_BIT_SHIFT)
#define CVM_CAR_CRC_LEN                 4
#define CVM_CAR_MTU                     1500
#define CVM_CAR_CBS                     (3 * CVM_CAR_MTU)   /* Max Token Bucket depth = 3 * MTU */

#define CVM_CAR_PASS_PKT             1
#define CVM_CAR_DROP_PKT             (! CVM_CAR_PASS_PKT)

#define METER_TEMPLATE_NUM    64 /*CARģ������*/
/*
����Ͱ���㷨����RFC2697����������ɫ��ǣ�ʵ�֡�
��������CIR  ��Committed Information Rate������������µ���С����
����ͻ������CBS��Committed Burst Size������λΪ�ֽڣ�����˲����Գ���CIR��������
����ͻ������EBS��Excess Burst Size��       ����λΪ�ֽڣ�������ͻ������CBS�϶��������ͻ������
��������ֵͨ�����������ã��Ƽ����òο����£�
CBS = CIR * (1 byte)/(8 bits) * 1.5 seconds
EBS = 2 * CBS
TC(t)��tʱ��CͰ��ʣ������t=0ʱ��TC(0)��CBS
TP(t)��tʱ��PͰ��ʣ������t=0ʱ��TP(0)��EBS
����CIR��ÿ��IP���ĵ��ֽ���������IPͷ���������������ͷ����but not link specific headers����
CBS��EBS��λҲ���ֽ�����CBS��EBS������һ�����������0���Ƽ����ã�
��CBS��EBS���ô���0ʱ���������Ϊ���ڻ����IP���ĵ���������

ע: EBSĿǰû��ʵ��
*/
typedef struct _cvm_car_tb
{
    uint32_t   depth;       /*CBS in bytes*/
    uint32_t   rate;        /* CIR in kbps*/

    uint64_t   rate_in_cycles_per_byte;
    uint64_t   depth_in_cycles;
    uint64_t   cycles_prev;

} cvm_car_tb_t; 

int car_init(void);
int cvm_car_tb_set(user_item_t *user, int speed);
inline int cvm_car_result(int bytes, uint32_t usr_idx, uint16_t usr_link_idx);
void cvm_car_show(uint32_t car_index);
void cvm_car_dump(void);
void cvm_car_show_template(void);
int cvm_car_set_template(uint32_t index, uint32_t cir_val, uint32_t cbs_val);


#endif 
