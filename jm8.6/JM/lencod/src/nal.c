
/*!
 **************************************************************************************
 * \file
 *    nal.c
 * \brief
 *    Handles the operations on converting String of Data Bits (SODB)
 *    to Raw Byte Sequence Payload (RBSP), and then 
 *    onto Encapsulate Byte Sequence Payload (EBSP).
 *  \date 14 June 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details) 
 *      - Shankar Regunathan                  <shanre@microsoft.de>
 *      - Stephan Wenger                      <stewe@cs.tu-berlin.de>
 ***************************************************************************************
 */


#include "contributors.h"

#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "defines.h"
#include "global.h"

 /*!
 ************************************************************************
 * \brief
 *    Converts String Of Data Bits (SODB) to Raw Byte Sequence 
 *  ��SODBԭʼ����bit��ת�����ַ��غ���
 *   sodb+rbsp_trailing_bits=rbsp
 *  rbsp�������е�sodb���������sodb����bit��rbsp_trailing_bits
 *  ���һ��bit��1�����������0ֱ�������ַ���
 *    Packet (RBSP)
 * \param currStream
 *        Bitstream which contains data bits.
 * \return None
 * \note currStream is byte-aligned at the end of this function
 *    
 ************************************************************************
*/

static byte *NAL_Payload_buffer;
//
void SODBtoRBSP(Bitstream *currStream)
{
  //������������ǽ�sodbԭʼ����bit������1bitλ1������sodb ת rbsp��׼
  currStream->byte_buf <<= 1;//
  currStream->byte_buf |= 1;
  //��ǰ�ֽ�ʣ��bit��1
  currStream->bits_to_go--;//��ǰ�����ֽ�ʣ���ʣ���λ��
  //sodb�ӵĵ�һ��bit��1��ʣ�඼��0��ʣ��λȫ�����Ʋ�0
  currStream->byte_buf <<= currStream->bits_to_go;
  //�������ֽ�8λ���ؽ�������
  currStream->streamBuffer[currStream->byte_pos++] = currStream->byte_buf;
  //�����Ѿ�����ǰ�����ֽ�����λ�����ˣ�������������ʾ����������һ���ֽ�
  currStream->bits_to_go = 8;
  //��ջ����ֽ�
  currStream->byte_buf = 0;
}


/*!
************************************************************************
*  \brief
*     This function converts a RBSP payload to an EBSP payload
*     
*  \param streamBuffer
*       pointer to data bits
*  \param begin_bytepos
*            The byte position after start-code, after which stuffing to
*            prevent start-code emulation begins.
*  \param end_bytepos
*           Size of streamBuffer in bytes.
*  \param min_num_bytes
*           Minimum number of bytes in payload. Should be 0 for VLC entropy
*           coding mode. Determines number of stuffed words for CABAC mode.
*  \return 
*           Size of streamBuffer after stuffing.
*  \note
*      NAL_Payload_buffer is used as temporary buffer to store data.
*
*
************************************************************************
*/
//�˺���Ϊ�˷�ֹ����
int RBSPtoEBSP(byte *streamBuffer, int begin_bytepos, int end_bytepos, int min_num_bytes)
{
  
  int i, j, count;

  for(i = begin_bytepos; i < end_bytepos; i++)
  	//��ʱ����NAL��Ԫ���ݵ�
    NAL_Payload_buffer[i] = streamBuffer[i];//NAL_Payload_buffer��lencode��main�����г�ʼ������ռ�

  count = 0;
  j = begin_bytepos;
  for(i = begin_bytepos; i < end_bytepos; i++) 
  { //NAL_Payload_buffer ������6λ��ȫ��0 ���ұߵı��ʽ��Ϊ0 ȫ��0���ұߵı��ʽΪ1
	//H.264���ڴ洢�����е����� ����0x000001��ʼ ��0����
	//����������г���ͬ����˳���ֽ�����������һ��0x03�ֽ�
	//0X000001--0X00000301
    if(count == ZEROBYTES_SHORTSTARTCODE && !(NAL_Payload_buffer[i] & 0xFC))//NAL_Payload_buffer=0~3 �����
    {//˵�� ���������������ֽڵ�0x00 ���ҽ��������ֽڵĸ�6λ��0 �����һ��0x03
      streamBuffer[j] = 0x03;//
      j++;//��ʱj>i
      count = 0;   
    }
    streamBuffer[j] = NAL_Payload_buffer[i];//j>=i
    if(NAL_Payload_buffer[i] == 0x00)
      count++;
    else 
      count = 0;
    j++;
  }
  while (j < begin_bytepos+min_num_bytes) {
    streamBuffer[j] = 0x00; // cabac stuffing word
    streamBuffer[j+1] = 0x00;
    streamBuffer[j+2] = 0x03;
    j += 3;
    stat->bit_use_stuffingBits[img->type]+=16;
  }
  return j;
}

 /*!
 ************************************************************************
 * \brief
 *    Initializes NAL module (allocates NAL_Payload_buffer)
 ************************************************************************
*/

void AllocNalPayloadBuffer()
{
  //���뻺���С
  const int buffer_size = (input->img_width * input->img_height * 4); // AH 190202: There can be data expansion with 
                                                          // low QP values. So, we make sure that buffer 
                                                          // does not everflow. 4 is probably safe multiplier.
  FreeNalPayloadBuffer();//ȷ��NAL_Payload_buffer�����ڴ涼���ͷ�

  //����NAL��С
  NAL_Payload_buffer = (byte *) calloc(buffer_size, sizeof(byte));//���·����ڴ�
  assert (NAL_Payload_buffer != NULL);//�ж�NAL_Payload_buffer�ڴ��Ƿ񱻷���
}


 /*!
 ************************************************************************
 * \brief
 *   Finits NAL module (frees NAL_Payload_buffer)
 ************************************************************************
*/

void FreeNalPayloadBuffer()
{
  if(NAL_Payload_buffer)
  {
    free(NAL_Payload_buffer);
    NAL_Payload_buffer=NULL;
  }
}
