
/*!
 ***********************************************************************
 *  \file
 *     configfile.h
 *  \brief
 *     Prototypes for configfile.c and definitions of used structures.
 ***********************************************************************
 */

#ifndef _CONFIGFILE_H_
#define _CONFIGFILE_H_


#define DEFAULTCONFIGFILENAME "encoder.cfg"

#define PROFILE_IDC     66
#define LEVEL_IDC       21


typedef struct {
  char *TokenName;
  void *Place;
  int Type;
} Mapping;



InputParameters configinput;


#ifdef INCLUDED_BY_CONFIGFILE_C
/*
输入配置映射向量
第一列是配置文件相同相应的配置项，并且一一对应。
第二列是配置文件的配置值，将配置文件的配置项的配置值付给map对应的configinput结构体中
第三列是类型 0 代表是数字 1代表是字符窜 2代表的是浮点
*/
Mapping Map[] = {
	//编码器的档次(baseline)主要档次(main)扩展档次(extended)
    {"ProfileIDC",               &configinput.ProfileIDC,              0},
    //编码级别，大部分级别是与内存大小来分配的。解码器必须支持 
    {"LevelIDC",                 &configinput.LevelIDC,                0},
    {"FrameRate",                &configinput.FrameRate,               0},
    //IDR类型帧使能
    {"IDRIntraEnable",           &configinput.idr_enable,              0},
    //第一帧开始编码的帧号，YUV图片含有多个图像
    {"StartFrame",               &configinput.start_frame,             0},
    //放置I帧的周期 多少个参考帧不包含B帧会添加一个I帧，=0只会有一个I帧
    {"IntraPeriod",              &configinput.intra_period,            0},
    //将要被编码的帧数
    {"FramesToBeEncoded",        &configinput.no_frames,               0},
    
    {"QPFirstFrame",             &configinput.qp0,                     0},
    {"QPRemainingFrame",         &configinput.qpN,                     0},
    {"FrameSkip",                &configinput.jumpd,                   0},
    {"UseHadamard",              &configinput.hadamard,                0},
    {"SearchRange",              &configinput.search_range,            0},
    {"NumberReferenceFrames",    &configinput.num_reference_frames,    0},
    {"PList0References",         &configinput.P_List0_refs,            0},
    {"BList0References",         &configinput.B_List0_refs,            0},
    {"BList1References",         &configinput.B_List1_refs,            0},
    //被编码的图像宽
    {"SourceWidth",              &configinput.img_width,               0},
    //被编码的图像高
    {"SourceHeight",             &configinput.img_height,              0},
	//通过增加帧内编码的宏块个数来增加容错能力
	{"MbLineIntraUpdate",        &configinput.intra_upd,               0},
    {"SliceMode",                &configinput.slice_mode,              0},
    {"SliceArgument",            &configinput.slice_argument,          0},
    //帧间宏块 不允许采用帧内预测方式得到
    {"UseConstrainedIntraPred",  &configinput.UseConstrainedIntraPred, 0},
    {"InputFile",                &configinput.infile,                  1},
    {"InputHeaderLength",        &configinput.infile_header,           0},
    {"OutputFile",               &configinput.outfile,                 1},
    {"ReconFile",                &configinput.ReconFile,               1},
    {"TraceFile",                &configinput.TraceFile,               1},
    {"NumberBFrames",            &configinput.successive_Bframe,       0},
    {"QPBPicture",               &configinput.qpB,                     0},
    {"DirectModeType",           &configinput.direct_type,             0},
    {"DirectInferenceFlag",      &configinput.directInferenceFlag,     0},
    {"SPPicturePeriodicity",     &configinput.sp_periodicity,          0},
    {"QPSPPicture",              &configinput.qpsp,                    0},
    {"QPSP2Picture",             &configinput.qpsp_pred,               0},
    {"SymbolMode",               &configinput.symbol_mode,             0},
    {"OutFileMode",              &configinput.of_mode,                 0},
    {"PartitionMode",            &configinput.partition_mode,          0},
    {"PictureTypeSequence",      &configinput.PictureTypeSequence,     1},
    //预测模式
    {"InterSearch16x16",         &configinput.InterSearch16x16,        0},
    {"InterSearch16x8",          &configinput.InterSearch16x8 ,        0},
    {"InterSearch8x16",          &configinput.InterSearch8x16,         0},
    {"InterSearch8x8",           &configinput.InterSearch8x8 ,         0},
    {"InterSearch8x4",           &configinput.InterSearch8x4,          0},
    {"InterSearch4x8",           &configinput.InterSearch4x8,          0},
    {"InterSearch4x4",           &configinput.InterSearch4x4,          0},

#ifdef _FULL_SEARCH_RANGE_
    {"RestrictSearchRange",      &configinput.full_search,             0},
#endif
#ifdef _ADAPT_LAST_GROUP_
    {"LastFrameNumber",          &configinput.last_frame,              0},
#endif
	//在超过ChangeQPStart帧，量化更改对应的量化值
#ifdef _CHANGE_QP_
    {"ChangeQPI",                &configinput.qp02,                    0},
    {"ChangeQPP",                &configinput.qpN2,                    0},
    {"ChangeQPB",                &configinput.qpB2,                    0},
    {"ChangeQPStart",            &configinput.qp2start,                0},
#endif
	//是否采用拉格朗日失真优化某型
    {"RDOptimization",           &configinput.rdopt,                   0},
    {"LossRateA",                &configinput.LossRateA,               0},
    {"LossRateB",                &configinput.LossRateB,               0},
    {"LossRateC",                &configinput.LossRateC,               0},
    {"NumberOfDecoders",         &configinput.NoOfDecoders,            0},
    //根据参考帧和分块类型的不同设置search range
    {"RestrictRefFrames",        &configinput.RestrictRef ,            0},
#ifdef _LEAKYBUCKET_
    {"NumberofLeakyBuckets",     &configinput.NumberLeakyBuckets,      0},
    {"LeakyBucketRateFile",      &configinput.LeakyBucketRateFile,     1},
    {"LeakyBucketParamFile",     &configinput.LeakyBucketParamFile,    1},
#endif
    {"PicInterlace",             &configinput.PicInterlace,            0},
    //是否采用帧场自适应方式
    {"MbInterlace",              &configinput.MbInterlace,             0},

    {"IntraBottom",              &configinput.IntraBottom,             0},

    {"NumberFramesInEnhancementLayerSubSequence", &configinput.NumFramesInELSubSeq, 0},
    {"NumberOfFrameInSecondIGOP",&configinput.NumFrameIn2ndIGOP, 0},
    {"RandomIntraMBRefresh",     &configinput.RandomIntraMBRefresh,    0},
		
	//是否打得开加权预测 P帧	
    {"WeightedPrediction",       &configinput.WeightedPrediction,      0},
    //是否打得开加权预测 B帧
    {"WeightedBiprediction",     &configinput.WeightedBiprediction,    0},
    {"StoredBPictures",          &configinput.StoredBPictures,         0},
    {"LoopFilterParametersFlag", &configinput.LFSendParameters,        0},
    {"LoopFilterDisable",        &configinput.LFDisableIdc,            0},
    {"LoopFilterAlphaC0Offset",  &configinput.LFAlphaC0Offset,         0},
    {"LoopFilterBetaOffset",     &configinput.LFBetaOffset,            0},
    {"SparePictureOption",       &configinput.SparePictureOption,      0},
    {"SparePictureDetectionThr", &configinput.SPDetectionThreshold,    0},
    {"SparePicturePercentageThr",&configinput.SPPercentageThreshold,   0},
	// 表示是否使用条带组，0表示不使用条带组，也就是说一帧图像只有一个条带组
	// 1表示有2个条带组，2表示3个条带组
    {"num_slice_groups_minus1",           &configinput.num_slice_groups_minus1,           0},
    {"slice_group_map_type",              &configinput.slice_group_map_type,              0},
    {"slice_group_change_direction_flag", &configinput.slice_group_change_direction_flag, 0},
    {"slice_group_change_rate_minus1",    &configinput.slice_group_change_rate_minus1,    0},
    //设置文件名sg0conf.cfg sg2conf.cfg sg6conf.cfg 这些文件分别对应FMO0 FMO2 FMO6
    {"SliceGroupConfigFileName",          &configinput.SliceGroupConfigFileName,          1},
		

    {"UseRedundantSlice",        &configinput.redundant_slice_flag,    0},
    //根据图像类型来显示图像顺序
    {"PicOrderCntType",          &configinput.pic_order_cnt_type,      0},

    {"ContextInitMethod",        &configinput.context_init_method,     0},
    {"FixedModelNumber",         &configinput.model_number,            0},

    // Rate Control
    {"RateControlEnable",        &configinput.RCEnable,                0},
    {"Bitrate",                  &configinput.bit_rate,                0},
    {"InitialQP",                &configinput.SeinitialQP,             0},
    {"BasicUnit",                &configinput.basicunit,               0},
    {"ChannelType",              &configinput.channel_type,            0},

    // Fast ME enable快速移动预测使能
    {"UseFME",                   &configinput.FMEnable,                0},
    
    {"ChromaQPOffset",           &configinput.chroma_qp_index_offset,  0},    
    {NULL,                       NULL,                                -1}
};

#endif

#ifndef INCLUDED_BY_CONFIGFILE_C
extern Mapping Map[];
#endif


void Configure (int ac, char *av[]);
void PatchInputNoFrames();

#endif

