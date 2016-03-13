
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
 *  将SODB原始数据bit流转化成字符载荷流
 *   sodb+rbsp_trailing_bits=rbsp
 *  rbsp包含所有的sodb并且在最后sodb几个bit和rbsp_trailing_bits
 *  其第一个bit是1，接下来填充0直到对其字符串
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
  //下面两条语句是将sodb原始数据bit流接上1bit位1。符合sodb 转 rbsp标准
  currStream->byte_buf <<= 1;//
  currStream->byte_buf |= 1;
  //当前字节剩余bit减1
  currStream->bits_to_go--;//当前缓冲字节剩余的剩余的位数
  //sodb接的第一个bit是1，剩余都是0，剩余位全部左移补0
  currStream->byte_buf <<= currStream->bits_to_go;
  //将缓冲字节8位加载进码流中
  currStream->streamBuffer[currStream->byte_pos++] = currStream->byte_buf;
  //上面已经将当前缓冲字节所有位都用了，所以这条语句表示码流进入下一个字节
  currStream->bits_to_go = 8;
  //清空缓冲字节
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
//此函数为了防止竞争
int RBSPtoEBSP(byte *streamBuffer, int begin_bytepos, int end_bytepos, int min_num_bytes)
{
  
  int i, j, count;

  for(i = begin_bytepos; i < end_bytepos; i++)
  	//临时保存NAL单元数据的
    NAL_Payload_buffer[i] = streamBuffer[i];//NAL_Payload_buffer在lencode的main函数中初始化分配空间

  count = 0;
  j = begin_bytepos;
  for(i = begin_bytepos; i < end_bytepos; i++) 
  { //NAL_Payload_buffer 如果其高6位不全是0 则右边的表达式则为0 全是0则右边的表达式为1
	//H.264中在存储介质中的码流 是以0x000001开始 以0结束
	//如果在码流中出现同样的顺序字节则将码流插入一个0x03字节
	//0X000001--0X00000301
    if(count == ZEROBYTES_SHORTSTARTCODE && !(NAL_Payload_buffer[i] & 0xFC))//NAL_Payload_buffer=0~3 ，如果
    {//说明 在连续出现两个字节的0x00 并且接下来的字节的高6位是0 则插入一个0x03
      streamBuffer[j] = 0x03;//
      j++;//此时j>i
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
  //编码缓存大小
  const int buffer_size = (input->img_width * input->img_height * 4); // AH 190202: There can be data expansion with 
                                                          // low QP values. So, we make sure that buffer 
                                                          // does not everflow. 4 is probably safe multiplier.
  FreeNalPayloadBuffer();//确保NAL_Payload_buffer所有内存都被释放

  //分配NAL大小
  NAL_Payload_buffer = (byte *) calloc(buffer_size, sizeof(byte));//重新分配内存
  assert (NAL_Payload_buffer != NULL);//判断NAL_Payload_buffer内存是否被分配
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
