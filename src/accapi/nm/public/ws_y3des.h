/*******************************************************************************
Copyright (C) Autelan Technology


This software file is owned and distributed by Autelan Technology 
********************************************************************************


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* ws_y3des.h
*
*
* CREATOR:
* autelan.software.Network Dep. team
* zhouym@autelan.com
*
* DESCRIPTION:
* function for des3  
*
*
*******************************************************************************/
#ifndef _WS_Y3DES_H                            
#define _WS_Y3DES_H

#include <string.h>


//�๹�캯��
extern void ymDES2_Initialize(); 

//����:����16��28λ��key
//����:Դ8λ���ַ���(key),���key�����0-1
//���:����������private CreateSubKey���������char SubKeys[keyN][16][48]
extern void ymDES2_InitializeKey(char* srcBytes,unsigned int keyN);

//����:����8λ�ַ���
//����:8λ�ַ���,ʹ��Key�����0-1
//���:���������ܺ��������private szCiphertext[16]
//      �û�ͨ������Ciphertext�õ�
extern void ymDES2_EncryptData(char* _srcBytes,unsigned int keyN);

//����:����16λʮ�������ַ���
//����:16λʮ�������ַ���,ʹ��Key�����0-1
//���:���������ܺ��������private szPlaintext[8]
//      �û�ͨ������Plaintext�õ�
extern void ymDES2_DecryptData(char* _srcBytes,unsigned int keyN);

//����:�������ⳤ���ַ���
//����:���ⳤ���ַ���,����,ʹ��Key�����0-1
//���:���������ܺ��������private szFCiphertextAnyLength[8192]
//      �û�ͨ������CiphertextAnyLength�õ�
extern void ymDES2_EncryptAnyLength(char* _srcBytes,unsigned int _bytesLength,unsigned int keyN);

//����:�������ⳤ��ʮ�������ַ���
//����:���ⳤ���ַ���,����,ʹ��Key�����0-1
//���:���������ܺ��������private szFPlaintextAnyLength[8192]
//      �û�ͨ������PlaintextAnyLength�õ�
extern void ymDES2_DecryptAnyLength(char* _srcBytes,unsigned int _bytesLength, unsigned int keyN);

//����:Bytes��Bits��ת��,
//����:���任�ַ���,���������Ż�����ָ��,Bits��������С
extern void ymDES2_Bytes2Bits(char *srcBytes, char* dstBits, unsigned int sizeBits);

//����:Bits��Bytes��ת��,
//����:���任�ַ���,���������Ż�����ָ��,Bits��������С
extern void ymDES2_Bits2Bytes(char *dstBytes, char* srcBits, unsigned int sizeBits);

//����:Int��Bits��ת��,
//����:���任�ַ���,���������Ż�����ָ��
extern void ymDES2_Int2Bits(unsigned int srcByte, char* dstBits);
		
//����:Bits��Hex��ת��
//����:���任�ַ���,���������Ż�����ָ��,Bits��������С
extern void ymDES2_Bits2Hex(char *dstHex, char* srcBits, unsigned int sizeBits);
		
//����:Bits��Hex��ת��
//����:���任�ַ���,���������Ż�����ָ��,Bits��������С
extern void ymDES2_Hex2Bits(char *srcHex, char* dstBits, unsigned int sizeBits);

//szCiphertextInBinary��get����
extern char* ymDES2_GetCiphertextInBinary();

//szCiphertextInHex��get����
extern char* ymDES2_GetCiphertextInHex();

//Ciphertext��get����
extern char* ymDES2_GetCiphertextInBytes();

//Plaintext��get����
extern char* ymDES2_GetPlaintext();

//CiphertextAnyLength��get����
extern char* ymDES2_GetCiphertextAnyLength();

//PlaintextAnyLength��get����
extern char* ymDES2_GetPlaintextAnyLength();

//����:��������Կ
//����:����PC1�任��56λ�������ַ���,���ɵ�szSubKeys���0-1
//���:��������char szSubKeys[16][48]
extern void ymDES2_CreateSubKey(char* sz_56key,unsigned int keyN);

//����:DES�е�F����,
//����:��32λ,��32λ,key���(0-15),ʹ�õ�szSubKeys���0-1
//���:���ڱ任����32λ
extern void ymDES2_FunctionF(char* sz_Li,char* sz_Ri,unsigned int iKey,unsigned int keyN);

//����:IP�任
//����:�������ַ���,����������ָ��
//���:�����ı�ڶ�������������
extern void ymDES2_InitialPermuteData(char* _src,char* _dst);

//����:����32λ������չλ48λ,
//����:ԭ32λ�ַ���,��չ�������ָ��
//���:�����ı�ڶ�������������
extern void ymDES2_ExpansionR(char* _src,char* _dst);

//����:�����,
//����:�����Ĳ����ַ���1,�ַ���2,����������,����������ָ��
//���: �����ı���ĸ�����������
extern void ymDES2_XOR(char* szParam1,char* szParam2, unsigned int uiParamLength, char* szReturnValueBuffer);

//����:S-BOX , ����ѹ��,
//����:48λ�������ַ���,
//���:���ؽ��:32λ�ַ���
extern void ymDES2_CompressFuncS(char* _src48, char* _dst32);

//����:IP��任,
//����:���任�ַ���,����������ָ��
//���:�����ı�ڶ�������������
extern void ymDES2_PermutationP(char* _src,char* _dst);

                         
#endif
 
