
/*!
 **************************************************************************************
 * \file
 *    nalucommon.h.h
 * \brief
 *    NALU handling common to encoder and decoder
 *  \date 25 November 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details) 
 *      - Stephan Wenger        <stewe@cs.tu-berlin.de>
 ***************************************************************************************
 */


#ifndef _NALUCOMMON_H_
#define _NALUCOMMON_H_
//RBSP最大的长度是 64000个字节
//原始字节序列负载 raw byte sequence payload
//其是由原始数据比特SODB 和rbsp_trailing_bits得到的，以可以码流能字节对其
#define MAXRBSPSIZE 64000
//有关nal_unit_type 类型
#define NALU_TYPE_SLICE    1//NAL为片 不分区 非IDR帧图像
#define NALU_TYPE_DPA      2//A片
#define NALU_TYPE_DPB      3//B片
#define NALU_TYPE_DPC      4//C片
#define NALU_TYPE_IDR      5//不分区 非IDR图像的片
#define NALU_TYPE_SEI      6//图像增强信息单元
#define NALU_TYPE_SPS      7//序列参数集
#define NALU_TYPE_PPS      8//图像参数集
#define NALU_TYPE_AUD      9//分界符
#define NALU_TYPE_EOSEQ    10//序列结束符
#define NALU_TYPE_EOSTREAM 11//码流结束符
#define NALU_TYPE_FILL     12//填充

/*
NAL 优先级
最大时3 最小0
值越大则优先级越高

*/
#define NALU_PRIORITY_HIGHEST     3
#define NALU_PRIORITY_HIGH        2
#define NALU_PRIRITY_LOW          1
#define NALU_PRIORITY_DISPOSABLE  0

//网络抽象层的语法结构
typedef struct 
{
  //开始代码的固定前缀
  int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
  unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
  //nal单元的最大长度
  unsigned max_size;            //! Nal Unit Buffer size
  //指明当前的nal类型0-31 
  /*
	1 不分区 非IDR图像片
	5 IDR图像中的片
	6 补充增强信息单元SEI
	7..详细见上面的宏
  */
  int nal_unit_type;            //! NALU_TYPE_xxxx
  //NAL优先级
  int nal_reference_idc;        //! NALU_PRIORITY_xxxx
  int forbidden_bit;            //! should be always FALSE
  byte *buf;        //! conjtains the first byte followed by the EBSP
} NALU_t;


NALU_t *AllocNALU();
void FreeNALU(NALU_t *n);

#endif
