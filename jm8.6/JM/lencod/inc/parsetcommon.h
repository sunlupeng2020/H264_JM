
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

//ÿ��ͼ��������������
#define MAXnum_slice_groups_minus1  8
//ͼ�������������
typedef struct
{
  //ָʾͼ������Ƿ���Ч
  Boolean   Valid;                  // indicates the parameter set is valid
  //����ָ����ͼ�����������ţ���ID���ڸ���Ƭ��Ƭͷ������
  unsigned  pic_parameter_set_id;                             // ue(v)
  //ָ����ͼ����������õ����в����������к�
  unsigned  seq_parameter_set_id;                             // ue(v)
  //=0 CAVLC �����ʽ =1 CABAC���� 
  Boolean   entropy_coding_mode_flag;                         // u(1)
  // if( pic_order_cnt_type < 2 )  in the sequence parameter set
  //POC�ļ�����ҪһЩ�������﷨Ԫ����Ϊ������
  //=1ʱ ��ʾƬͷ�����ָ����Щ������=0ʱ��ʾƬͷ���������Щ����
  Boolean      pic_order_present_flag;                           // u(1)
  //ͼ���Ƿ�����ͼ��������Ŀ��0ʱ��ʾ����ͼƬ��һ�����ڴ���0ʱ��ʾ
  unsigned  num_slice_groups_minus1;                          // ue(v)
  //�涨��Ƭ���ӳ�����ͣ�ȡֵ��0-6 ����0���� 1ɢ�� 2ǰ���ͱ��� 
  //3box-out 4��դɨ�� 5�־� 6��ʾ 
  unsigned  slice_group_map_type;                        // ue(v)
    // if( slice_group_map_type = = 0 )����ǽ������
    //�������������ÿ������Ĵ�С
	//for(igroup=0;igroup<=num_slice_groups_minus1;igroup++)
      unsigned  run_length_minus1[MAXnum_slice_groups_minus1]; // ue(v)
    // else if( slice_group_map_type = = 2 )    
	//for(igroup=0;igroup<=num_slice_groups_minus1;igroup++)Ϊѭ��Ϊÿ��ǰ����������
    //ǰ���ͱ���������� ָ���������������λ�ú�����λ��
      unsigned  top_left[MAXnum_slice_groups_minus1];         // ue(v)
      unsigned  bottom_right[MAXnum_slice_groups_minus1];     // ue(v)
	//��������ͬ�����͵���������դ��ɨ�� 3�������� 4��դ(ˮƽɨ��) 5�־�(��ֱɨ��)
    // else if( slice_group_map_type = = 3 || 4 || 5 ������ģʽ��ͼ��ֻ������Ƭ�� 0 1
    //ָ����Ƭ���ɨ�跽�� 
      Boolean   slice_group_change_direction_flag;            // u(1)
    //ͬ������﷨Ԫ��ȷ�Ϸ������� ָ��һ��Ƭ���С��һ��ͼ����һ���ĸı䱶�� ��map_units
    //Ϊ��λ�ģ�ȷ��0 1��Ĵ�СҪ��Ƭ�������ϵ�slice_group_change_cycleһ����ȷ��Ƭ��Ĵ�С
      unsigned  slice_group_change_rate_minus1;               // ue(v)
    // else if( slice_group_map_type = = 6 )
      unsigned  pic_size_in_map_units_minus1;	                // ue(v)
	//Ƭ��ID��
      unsigned  *slice_group_id;                              // complete MBAmap u(v)
  //��1��ָ��Ŀǰ�ο�֡���еĳ��ȣ����������ֱ�
  //�ο�֡list0 list1�ĳ��Ȱ������Ͷ��ڣ���֡��ģʽ�������ҪX2
  //��num_ref_frames�����ǣ�num_ref_framesָ���ǲο����ֵ
  //���������ֻ�ִ�Ĳο�������
  int       num_ref_idx_l0_active_minus1;                     // ue(v)
  int       num_ref_idx_l1_active_minus1;                     // ue(v)
  //����ָ���Ƿ�����P��SPƬ�ļ�ȨԤ�⡣��������Ƭͷ����
  Boolean   weighted_pred_flag;                               // u(1)
  //ָ��B(bipred)֡Ԥ���Ƿ������ȨԤ�⣬
  Boolean   weighted_bipred_idc;                              // u(2)
  //+26 ָ��ͼ�����ȷ���QP���������ĳ�ʼֵ
  int       pic_init_qp_minus26;                              // se(v)
  //+26 ָ��SP �� SI QPֵ
  int       pic_init_qs_minus26;                              // se(v)
  //ɫ�ȷ��������������Ǹ������ȷ������������������������
  int       chroma_qp_index_offset;                           // se(v)
  //��������ʾ�Ŀ����˲�ǿ��
  Boolean   deblocking_filter_control_present_flag;           // u(1)
  //P��BƬ�� ֡�ڱ���ĺ���Ƿ����������ڵ�֡�����ĺ�顣
  //���=1����ʾ���������ڵ�֡�������Ϊ�ο�
  Boolean   constrained_intra_pred_flag;                      // u(1)
  //��ʾ�Ƿ����redundant_pic_cnt�﷨Ԫ��
  Boolean   redundant_pic_cnt_present_flag;                   // u(1)
  Boolean   vui_pic_parameters_flag;                          // u(1)
} pic_parameter_set_rbsp_t;


#define MAXnum_ref_frames_in_pic_order_cnt_cycle  256
//SPS���в������ṹ�壬�����������й�������Ԫ��
typedef struct
{
  //ָʾ�˲��������Ƿ���Ч�����ǲ������м���Ԫ��
  Boolean   Valid;                  // indicates the parameter set is valid
  //ָ�����뼯�ϡ� ����base
  unsigned  profile_idc;                                      // u(8) 8λ�޷��� ������
  //����Լ��Э�� ������ 0 1 2 �ֱ��Ƿ���ϱ�׼A.2.1 A2.2 A2.3Լ������
  Boolean   constrained_set0_flag;                            // u(1)
  Boolean   constrained_set1_flag;                            // u(1)
  Boolean   constrained_set2_flag;                            // u(1)
  //���뼶����Ҫ��ͼ��ֱ����ڴ��С�й�
  unsigned  level_idc;                                        // u(8)
  //ָ�������в�������ID�� ��ֵ��0-31 ����ֵ�ǲ���ָ�����ײ�����
  unsigned  seq_parameter_set_id;                             // ue(v)
  //������������Ƶ������֡��
  unsigned  log2_max_frame_num_minus4;                        // ue(v)
  //ָ��ͼ��Ĳ���˳������ͣ�������Ҫѧϰ=0 ����֡����                                                                                                       
  unsigned pic_order_cnt_type;
  // if( pic_order_cnt_type == 0 ) //Ĭ���ߵ������·��
    unsigned log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
  // else if( pic_order_cnt_type == 1 )//����֧·��ʱ������
    Boolean delta_pic_order_always_zero_flag;               // u(1)
    int     offset_for_non_ref_pic;                         // se(v) se�����з���ָ�����ײ�����
    int     offset_for_top_to_bottom_field;                 // se(v)
    unsigned  num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
    // for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
      int   offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   // se(v)
	
  //�ƶ��ο�֡����󳤶� �������������������ռ䣬��Ž���ĵ�����֡�����16��
  unsigned  num_ref_frames;                                   // ue(v)
  //=1ʱ��ʾ����fram_num������ =0��������
  Boolean   gaps_in_frame_num_value_allowed_flag;             // u(1)
  //ͼ��Ŀ��-1���Ժ��Ϊ��λ
  unsigned  pic_width_in_mbs_minus1;                          // ue(v)
  //ͼ��ĸ߶ȣ�ͼ��ĸ߶���ͼ����֡���й�(frame_mbs_only_flag)
  //��ģʽ���Ժ��Գ��ֵģ�֡ʽ���Ժ����ֵ�
  //����framheightInMbs(�Ժ��Ϊ��λͼ��ĸ߶�)=(2-frame_mbs_only_flag)*pic_height_in_map_units_minus1
  unsigned  pic_height_in_map_units_minus1;                   // ue(v)
  //=1ʱ����ʾͼ���еı���ģʽ������֡��û�г�ģʽ
  Boolean   frame_mbs_only_flag;               //Ĭ�������=1               // u(1)
  // if( !frame_mbs_only_flag ) ��������﷨�жϣ�������﷨��Ҫ����һ���ַ�
    Boolean   mb_adaptive_frame_field_flag;		// u(1) �Ƿ����֡������Ӧ
  //ָ��B֡��ֱ��Ԥ���  SKIPģʽ ����ģʽ��ʱ���� ��ʱ����B֡
  Boolean   direct_8x8_inference_flag;                        // u(1)
  //�Ƿ��ͼ���ͼ����м���
  Boolean   frame_cropping_flag;                              // u(1)
  //�������ָ�����������ҵ�ƫ��
    unsigned  frame_cropping_rect_left_offset;                // ue(v)
    unsigned  frame_cropping_rect_right_offset;               // ue(v)
    unsigned  frame_cropping_rect_top_offset;                 // ue(v)
    unsigned  frame_cropping_rect_bottom_offset;              // ue(v)
  Boolean   vui_parameters_present_flag;                      // u(1)
	//ָ��VUI�ӽṹ�Ƿ����������������
	vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
} seq_parameter_set_rbsp_t;

pic_parameter_set_rbsp_t *AllocPPS ();
seq_parameter_set_rbsp_t *AllocSPS ();
void FreePPS (pic_parameter_set_rbsp_t *pps);
void FreeSPS (seq_parameter_set_rbsp_t *sps);

#endif
