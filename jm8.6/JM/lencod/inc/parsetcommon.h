
/*!
 **************************************************************************************
 * \file
 *    parsetcommon.h
 * \brief
 *    Picture and Sequence Parameter Sets, structures common to encoder and decoder
 *    This code reflects JVT version xxx
 *  \date 25 November 2002
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details) 
 *      - Stephan Wenger        <stewe@cs.tu-berlin.de>
 ***************************************************************************************
 */



// In the JVT syntax, frequently flags are used that indicate the presence of
// certain pieces of information in the NALU.  Here, these flags are also
// present.  In the encoder, those bits indicate that the values signalled to
// be present are meaningful and that this part of the syntax should be
// written to the NALU.  In the decoder, the flag indicates that information
// was received from the decoded NALU and should be used henceforth.
// The structure names were chosen as indicated in the JVT syntax

#ifndef _PARSETCOMMON_H_
#define _PARSETCOMMON_H_

#define MAXIMUMPARSETRBSPSIZE   1500
#define MAXIMUMPARSETNALUSIZE   1500
#define SIZEslice_group_id      (sizeof (int) * 60000)    // should be sufficient for HUGE pictures, need one int per MB in a picture

#define MAXSPS  32
#define MAXPPS  256

//! Boolean Type
typedef enum {
  FALSE,
  TRUE
} Boolean;

#define MAXIMUMVALUEOFcpb_cnt   32
typedef struct
{
  unsigned  cpb_cnt;                                          // ue(v)
  unsigned  bit_rate_scale;                                   // u(4)
  unsigned  cpb_size_scale;                                   // u(4)
    unsigned  bit_rate_value [MAXIMUMVALUEOFcpb_cnt];         // ue(v)
    unsigned  cpb_size_value[MAXIMUMVALUEOFcpb_cnt];          // ue(v)
    unsigned  vbr_cbr_flag[MAXIMUMVALUEOFcpb_cnt];            // u(1)
  unsigned  initial_cpb_removal_delay_length_minus1;          // u(5)
  unsigned  cpb_removal_delay_length_minus1;                  // u(5)
  unsigned  dpb_output_delay_length_minus1;                   // u(5)
  unsigned  time_offset_length;                               // u(5)
} hrd_parameters_t;


typedef struct
{
  Boolean      aspect_ratio_info_present_flag;                   // u(1)
    unsigned  aspect_ratio_idc;                               // u(8)
      unsigned  sar_width;                                    // u(16)
      unsigned  sar_height;                                   // u(16)
  Boolean      overscan_info_present_flag;                       // u(1)
    Boolean      overscan_appropriate_flag;                      // u(1)
  Boolean      video_signal_type_present_flag;                   // u(1)
    unsigned  video_format;                                   // u(3)
    Boolean      video_full_range_flag;                          // u(1)
    Boolean      colour_description_present_flag;                // u(1)
      unsigned  colour_primaries;                             // u(8)
      unsigned  transfer_characteristics;                     // u(8)
      unsigned  matrix_coefficients;                          // u(8)
  Boolean      chroma_location_info_present_flag;                // u(1)
    unsigned  chroma_location_frame;                          // ue(v)
    unsigned  chroma_location_field;                          // ue(v)
  Boolean      timing_info_present_flag;                         // u(1)
    unsigned  num_units_in_tick;                              // u(32)
    unsigned  time_scale;                                     // u(32)
    Boolean      fixed_frame_rate_flag;                          // u(1)
  Boolean      nal_hrd_parameters_present_flag;                  // u(1)
    hrd_parameters_t nal_hrd_parameters;                      // hrd_paramters_t
  Boolean      vcl_hrd_parameters_present_flag;                  // u(1)
    hrd_parameters_t vcl_hrd_parameters;                      // hrd_paramters_t
  // if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag))
    Boolean      low_delay_hrd_flag;                             // u(1)
  Boolean      bitstream_restriction_flag;                       // u(1)
    Boolean      motion_vectors_over_pic_boundaries_flag;        // u(1)
    unsigned  max_bytes_per_pic_denom;                        // ue(v)
    unsigned  max_bits_per_mb_denom;                          // ue(v)
    unsigned  log2_max_mv_length_vertical;                    // ue(v)
    unsigned  log2_max_mv_length_horizontal;                  // ue(v)
    unsigned  max_dec_frame_reordering;                       // ue(v)
    unsigned  max_dec_frame_buffering;                        // ue(v)
} vui_seq_parameters_t;

//每个图像的最大条带组数
#define MAXnum_slice_groups_minus1  8
//图像参数集合序列
typedef struct
{
  //指示图像参数是否有效
  Boolean   Valid;                  // indicates the parameter set is valid
  //用以指明本图像参数集的序号，本ID将在各个片的片头被引用
  unsigned  pic_parameter_set_id;                             // ue(v)
  //指明本图像参数集引用的序列参数集的序列号
  unsigned  seq_parameter_set_id;                             // ue(v)
  //=0 CAVLC 编码格式 =1 CABAC编码 
  Boolean   entropy_coding_mode_flag;                         // u(1)
  // if( pic_order_cnt_type < 2 )  in the sequence parameter set
  //POC的计算需要一些其他的语法元素作为参数，
  //=1时 表示片头会出现指明这些参数，=0时表示片头不会给出这些参数
  Boolean      pic_order_present_flag;                           // u(1)
  //图像是否分组和图像分组的数目，0时表示整个图片在一个组内大于0时表示
  unsigned  num_slice_groups_minus1;                          // ue(v)
  //规定了片组的映射类型，取值是0-6 包括0交错 1散乱 2前景和背景 
  //3box-out 4光栅扫描 5手绢 6显示 
  unsigned  slice_group_map_type;                        // ue(v)
    // if( slice_group_map_type = = 0 )如果是交错分组
    //交错分组的情况下每个分组的大小
	//for(igroup=0;igroup<=num_slice_groups_minus1;igroup++)
      unsigned  run_length_minus1[MAXnum_slice_groups_minus1]; // ue(v)
    // else if( slice_group_map_type = = 2 )    
	//for(igroup=0;igroup<=num_slice_groups_minus1;igroup++)为循环为每个前景背景分配
    //前景和背景的情况下 指明矩形区域的左上位置和右下位置
      unsigned  top_left[MAXnum_slice_groups_minus1];         // ue(v)
      unsigned  bottom_right[MAXnum_slice_groups_minus1];     // ue(v)
	//这三种是同种类型的类似连续栅格扫描 3类似螺旋 4光栅(水平扫描) 5手绢(垂直扫描)
    // else if( slice_group_map_type = = 3 || 4 || 5 这三种模式下图像只有两个片组 0 1
    //指明了片组的扫描方向 
      Boolean   slice_group_change_direction_flag;            // u(1)
    //同上面的语法元素确认分组类型 指明一个片组大小从一个图像到下一个的改变倍数 以map_units
    //为单位的，确定0 1组的大小要与片参数集合的slice_group_change_cycle一起来确定片组的大小
      unsigned  slice_group_change_rate_minus1;               // ue(v)
    // else if( slice_group_map_type = = 6 )
      unsigned  pic_size_in_map_units_minus1;	                // ue(v)
	//片组ID号
      unsigned  *slice_group_id;                              // complete MBAmap u(v)
  //加1后指明目前参考帧队列的长度，下面两条分别
  //参考帧list0 list1的长度包括长和短期，当帧场模式的情况下要X2
  //与num_ref_frames区别是，num_ref_frames指的是参考最大值
  //而下面的是只现存的参考数量。
  int       num_ref_idx_l0_active_minus1;                     // ue(v)
  int       num_ref_idx_l1_active_minus1;                     // ue(v)
  //用以指明是否允许P和SP片的加权预测。允许则在片头出现
  Boolean   weighted_pred_flag;                               // u(1)
  //指明B(bipred)帧预测是否允许加权预测，
  Boolean   weighted_bipred_idc;                              // u(2)
  //+26 指明图像亮度分量QP量化步长的初始值
  int       pic_init_qp_minus26;                              // se(v)
  //+26 指明SP 和 SI QP值
  int       pic_init_qs_minus26;                              // se(v)
  //色度分量的量化参数是根据亮度分量的量化参数计算出出来的
  int       chroma_qp_index_offset;                           // se(v)
  //编码器显示的控制滤波强度
  Boolean   deblocking_filter_control_present_flag;           // u(1)
  //P和B片中 帧内编码的宏块是否允许以相邻的帧间编码的宏块。
  //如果=1，表示必须以相邻的帧间编码宏块为参考
  Boolean   constrained_intra_pred_flag;                      // u(1)
  //表示是否出现redundant_pic_cnt语法元素
  Boolean   redundant_pic_cnt_present_flag;                   // u(1)
  Boolean   vui_pic_parameters_flag;                          // u(1)
} pic_parameter_set_rbsp_t;


#define MAXnum_ref_frames_in_pic_order_cnt_cycle  256
//SPS序列参数集结构体，描述了所有有关其所有元素
typedef struct
{
  //指示此参数序列是否有效，不是参数序列集的元素
  Boolean   Valid;                  // indicates the parameter set is valid
  //指明编码集合。 基本base
  unsigned  profile_idc;                                      // u(8) 8位无符号 不编码
  //配置约束协议 有三个 0 1 2 分别是否符合标准A.2.1 A2.2 A2.3约束条件
  Boolean   constrained_set0_flag;                            // u(1)
  Boolean   constrained_set1_flag;                            // u(1)
  Boolean   constrained_set2_flag;                            // u(1)
  //编码级别主要和图像分辨率内存大小有关
  unsigned  level_idc;                                        // u(8)
  //指明本序列参数集的ID号 本值是0-31 ，此值是采用指数哥伦布编码
  unsigned  seq_parameter_set_id;                             // ue(v)
  //经过计算可以推到出最大帧数
  unsigned  log2_max_frame_num_minus4;                        // ue(v)
  //指明图像的播放顺序的类型，现在主要学习=0 依赖帧类型                                                                                                       
  unsigned pic_order_cnt_type;
  // if( pic_order_cnt_type == 0 ) //默认走的是这个路径
    unsigned log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
  // else if( pic_order_cnt_type == 1 )//这条支路暂时不分析
    Boolean delta_pic_order_always_zero_flag;               // u(1)
    int     offset_for_non_ref_pic;                         // se(v) se代表有符号指数哥伦布编码
    int     offset_for_top_to_bottom_field;                 // se(v)
    unsigned  num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
    // for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
      int   offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   // se(v)
	
  //制定参考帧的最大长度 缓冲区依据这个来分配空间，存放解码的的数据帧，最多16个
  unsigned  num_ref_frames;                                   // ue(v)
  //=1时表示允许fram_num不连续 =0必须连续
  Boolean   gaps_in_frame_num_value_allowed_flag;             // u(1)
  //图像的宽度-1，以宏块为单位
  unsigned  pic_width_in_mbs_minus1;                          // ue(v)
  //图像的高度，图像的高度与图像是帧或场有关(frame_mbs_only_flag)
  //场模式是以宏块对出现的，帧式是以宏块出现的
  //所以framheightInMbs(以宏块为单位图像的高度)=(2-frame_mbs_only_flag)*pic_height_in_map_units_minus1
  unsigned  pic_height_in_map_units_minus1;                   // ue(v)
  //=1时，表示图像中的编码模式都是以帧，没有场模式
  Boolean   frame_mbs_only_flag;               //默认情况下=1               // u(1)
  // if( !frame_mbs_only_flag ) 对下面的语法判断，下面的语法是要倒退一个字符
    Boolean   mb_adaptive_frame_field_flag;		// u(1) 是否采用帧场自适应
  //指明B帧的直接预测和  SKIP模式 基本模式暂时不看 暂时不看B帧
  Boolean   direct_8x8_inference_flag;                        // u(1)
  //是否对图像对图像进行剪裁
  Boolean   frame_cropping_flag;                              // u(1)
  //如果剪裁指明其上下左右的偏移
    unsigned  frame_cropping_rect_left_offset;                // ue(v)
    unsigned  frame_cropping_rect_right_offset;               // ue(v)
    unsigned  frame_cropping_rect_top_offset;                 // ue(v)
    unsigned  frame_cropping_rect_bottom_offset;              // ue(v)
  Boolean   vui_parameters_present_flag;                      // u(1)
	//指明VUI子结构是否允许出现在码流中
	vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
} seq_parameter_set_rbsp_t;

pic_parameter_set_rbsp_t *AllocPPS ();
seq_parameter_set_rbsp_t *AllocSPS ();
void FreePPS (pic_parameter_set_rbsp_t *pps);
void FreeSPS (seq_parameter_set_rbsp_t *sps);

#endif
