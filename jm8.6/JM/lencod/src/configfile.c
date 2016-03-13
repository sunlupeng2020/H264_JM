
/*!
 ***********************************************************************
 * \file
 *    configfile.c
 * \brief
 *    Configuration handling.
 * \author
 *  Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Stephan Wenger           <stewe@cs.tu-berlin.de>
 * \note
 *    In the future this module should hide the Parameters and offer only
 *    Functions for their access.  Modules which make frequent use of some parameters
 *    (e.g. picture size in macroblocks) are free to buffer them on local variables.
 *    This will not only avoid global variable and make the code more readable, but also
 *    speed it up.  It will also greatly facilitate future enhancements such as the
 *    handling of different picture sizes in the same sequence.                         \n
 *                                                                                      \n
 *    For now, everything is just copied to the inp_par structure (gulp)
 *
 **************************************************************************************
 * \par Configuration File Format
 **************************************************************************************
 * Format is line oriented, maximum of one parameter per line                           \n
 *                                                                                      \n
 * Lines have the following format:                                                     \n
 * \<ParameterName\> = \<ParameterValue\> # Comments \\n                                    \n
 * Whitespace is space and \\t
 * \par
 * \<ParameterName\> are the predefined names for Parameters and are case sensitive.
 *   See configfile.h for the definition of those names and their mapping to
 *   configinput->values.
 * \par
 * \<ParameterValue\> are either integers [0..9]* or strings.
 *   Integers must fit into the wordlengths, signed values are generally assumed.
 *   Strings containing no whitespace characters can be used directly.  Strings containing
 *   whitespace characters are to be inclosed in double quotes ("string with whitespace")
 *   The double quote character is forbidden (may want to implement something smarter here).
 * \par
 * Any Parameters whose ParameterName is undefined lead to the termination of the program
 * with an error message.
 *
 * \par Known bug/Shortcoming:
 *    zero-length strings (i.e. to signal an non-existing file
 *    have to be coded as "".
 *
 * \par Rules for using command files
 *                                                                                      \n
 * All Parameters are initially taken from DEFAULTCONFIGFILENAME, defined in configfile.h.
 * If an -f \<config\> parameter is present in the command line then this file is used to
 * update the defaults of DEFAULTCONFIGFILENAME.  There can be more than one -f parameters
 * present.  If -p <ParameterName = ParameterValue> parameters are present then these
 * overide the default and the additional config file's settings, and are themselfes
 * overridden by future -p parameters.  There must be whitespace between -f and -p commands
 * and their respecitive parameters
 ***********************************************************************
 *在这个文件中是有关配置项的读取和解析的函数
 */

#define INCLUDED_BY_CONFIGFILE_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "global.h"
#include "configfile.h"
#include "fast_me.h"


#include "fmo.h"
/************************************************************************/
/* 
*功能获取配置文件
*/
/************************************************************************/
static char *GetConfigFileContent (char *Filename);
static void ParseContent (char *buf, int bufsize);
static int ParameterNameToMapIndex (char *s);
static void PatchInp ();
static void ProfileCheck();
static void LevelCheck();

//支持的最大配置条数,所有的配置条数在.cfg文件中配置
#define MAX_ITEMS_TO_PARSE  10000

void JMHelpExit()
{
  fprintf( stderr, "\n   lencod [-h] [-p defenc.cfg] {[-f curenc1.cfg]...[-f curencN.cfg]}"
    " {[-p EncParam1=EncValue1]..[-p EncParamM=EncValueM]}\n\n"    
    "## Parameters\n\n"

    "## Options\n"
    "   -h :  prints function usage\n"
    "   -d :  use <defenc.cfg> as default file for parameter initializations.\n"
    "         If not used then file defaults to encoder.cfg in local directory.\n"
    "   -f :  read <curencM.cfg> for reseting selected encoder parameters.\n"
    "         Multiple files could be used that set different parameters\n"
    "   -p :  Set parameter <EncParamM> to <EncValueM>.\n"
    "         See default encoder.cfg file for description of all parameters.\n\n"
    
    "## Supported video file formats\n"
    "   RAW:  .yuv -> YUV 4:2:0\n\n"
    
    "## Examples of usage:\n"
    "   lencod\n"
    "   lencod  -h\n"
    "   lencod  -d default.cfg\n"
    "   lencod  -f curenc1.cfg\n"
    "   lencod  -f curenc1.cfg -p InputFile=\"e:\\data\\container_qcif_30.yuv\" -p SourceWidth=176 -p SourceHeight=144\n"  
    "   lencod  -f curenc1.cfg -p FramesToBeEncoded=30 -p QPFirstFrame=28 -p QPRemainingFrame=28 -p QPBPicture=30\n");

  exit(-1);
}

/*!
 ***********************************************************************
 * \brief
 *    Parse the command line parameters and read the config files.
 * \param ac
 *    number of command line parameters
 * \param av
 *    command line parameters
 ***********************************************************************
 */
void Configure(int ac, char *av[])
{
  char *content;
  int CLcount, ContentLen, NumberParams;
  char *filename=DEFAULTCONFIGFILENAME;//默认的配置文件 

  printf("configure system\n");
//清0数据结构存储
  memset (&configinput, 0, sizeof (InputParameters));
/*
  //Set some initial parameters.
  //此部分是默认的配置
  //编码等级，在配置文件中可以定义
  //与解码器等级有关，根据内存来进行相应的配置
*/
  configinput.LevelIDC   = LEVEL_IDC;//=21级别enables interlace support
  configinput.ProfileIDC = PROFILE_IDC;//=88采用的是扩展档次66基本档次 77主要档次
  // Process default config file 进程配置文件
  CLcount = 1;
/*
输入参数是两个
第一个是默认的是可执行程序的名字
第二个是传输的命令
此处不走
*/
  if (ac==2)
  {
    if (0 == strncmp (av[1], "-h", 2))
    {
      JMHelpExit();
    }
  }
 /*
参数是三个
以一个是命令 -d
第二个参数数是配置文件位置
-d :  运用下一个命令的内容作为配置文件encoder_baseline.cfg
 */
  if (ac>=3)//函数名 -d 配置文件 -->所以是三个字符串。
  {//走这条支路
	if (0 == strncmp (av[1], "-d", 2))//说明一个配置文件
	{//走这条路
		  filename=av[2];
		  CLcount = 3;
    }
    if (0 == strncmp (av[1], "-h", 2))
    {
      JMHelpExit();
    }
  }
  //输出解析文件的名字
  printf ("Parsing Configfile %s", filename);
  //获取配置文件内容
  content = GetConfigFileContent (filename);
  //解析命令 文件中配置
  ParseContent (content, strlen(content));
  printf ("\n");
  free (content);

  // Parse the command line

  while (CLcount < ac)
  {//CLcount = 3所以不走
    if (0 == strncmp (av[CLcount], "-h", 2))
    {
      JMHelpExit();
    }
    
    if (0 == strncmp (av[CLcount], "-f", 2))  // A file parameter?
    {
      content = GetConfigFileContent (av[CLcount+1]);
      printf ("Parsing Configfile %s", av[CLcount+1]);
      ParseContent (content, strlen (content));
      printf ("\n");
      free (content);
      CLcount += 2;
    } else
    {
      if (0 == strncmp (av[CLcount], "-p", 2))  // A config change?
      {
        // Collect all data until next parameter (starting with -<x> (x is any character)),
        // put it into content, and parse content.

        CLcount++;
        ContentLen = 0;
        NumberParams = CLcount;

        // determine the necessary size for content
        while (NumberParams < ac && av[NumberParams][0] != '-')
          ContentLen += strlen (av[NumberParams++]);        // Space for all the strings
        ContentLen += 1000;                     // Additional 1000 bytes for spaces and \0s


        if ((content = malloc (ContentLen))==NULL) no_mem_exit("Configure: content");;
        content[0] = '\0';

        // concatenate all parameters identified before

        while (CLcount < NumberParams)
        {
          char *source = &av[CLcount][0];
          char *destin = &content[strlen (content)];

          while (*source != '\0')
          {
            if (*source == '=')  // The Parser expects whitespace before and after '='
            {
              *destin++=' '; *destin++='='; *destin++=' ';  // Hence make sure we add it
            } else
              *destin++=*source;
            source++;
          }
          *destin = '\0';
          CLcount++;
        }
        printf ("Parsing command line string '%s'", content);
        ParseContent (content, strlen(content));
        free (content);
        printf ("\n");
      }
      else
      {
        snprintf (errortext, ET_SIZE, "Error in command line, ac %d, around string '%s', missing -f or -p parameters?", CLcount, av[CLcount]);
        error (errortext, 300);
      }
    }
  }
	//直到此位置
  printf ("\n");
  PatchInp();//检查
}

/*!
 ***********************************************************************
 * \brief
 *    allocates memory buf, opens file Filename in f, reads contents into
 *    buf and returns buf
 * \param Filename
 *    name of config file
 ***********************************************************************
 */
char *GetConfigFileContent (char *Filename)
{
  long FileSize;
  FILE *f;
  char *buf;
/*
	打开对应文件，通过Filename指定的字符串
*/
  if (NULL == (f = fopen (Filename, "r")))
  {
    snprintf (errortext, ET_SIZE, "Cannot open configuration file %s.\n", Filename);
    error (errortext, 300);
  }
  //将文件指针移动到文件的最后结尾
  if (0 != fseek (f, 0, SEEK_END))
  {
    snprintf (errortext, ET_SIZE, "Cannot fseek in configuration file %s.\n", Filename);
    error (errortext, 300);
  }
  //判断配置文件的大小
  FileSize = ftell (f);
  if (FileSize < 0 || FileSize > 60000)
  {
    snprintf (errortext, ET_SIZE, "Unreasonable Filesize %ld reported by ftell for configuration file %s.\n", FileSize, Filename);
    error (errortext, 300);
  }
  //重新指向文件的开头位置
  if (0 != fseek (f, 0, SEEK_SET))
  {
    snprintf (errortext, ET_SIZE, "Cannot fseek in configuration file %s.\n", Filename);
    error (errortext, 300);
  }

  if ((buf = malloc (FileSize + 1))==NULL) no_mem_exit("GetConfigFileContent: buf");//为配置文件分配内存

  // Note that ftell() gives us the file size as the file system sees it.  The actual file size,
  // as reported by fread() below will be often smaller due to CR/LF to CR conversion and/or
  // control characters after the dos EOF marker in the file.

  //读取文件到缓存buf中
  FileSize = fread (buf, 1, FileSize, f);
  buf[FileSize] = '\0';//添加结束符号


  fclose (f);
  return buf;
}


/*!
 ***********************************************************************
 * \brief
 *    Parses the character array buf and writes global variable input, which is defined in
 *    configfile.h.  This hack will continue to be necessary to facilitate the addition of
 *    new parameters through the Map[] mechanism (Need compiler-generated addresses in map[]).
 * \param buf
 *    buffer to be parsed
 *   装有配置参数的缓存区
* \param bufsize
 *    buffer size of buffer
 *缓存区的大小
 ***********************************************************************
 */
void ParseContent (char *buf, int bufsize)
{
  //支持的最大配置条数,所有的配置条数在.cfg文件中配置
  char *items[MAX_ITEMS_TO_PARSE];//最大的指向对应的指针
  int MapIdx;
  int item = 0;
  //InString 标识是一个""号的字符串，当前指针p在这个""字符串中
  //InItem  标识一个配置条目的起始位置也就是一个字符
  int InString = 0, InItem = 0;
  char *p = buf;//装有配置文件的缓存区
  char *bufend = &buf[bufsize];//配置命令缓存区的结束位置
  int IntContent;
  double DoubleContent;
  int i;

// Stage one: Generate an argc/argv-type list in items[], without comments and whitespace.
// This is context insensitive and could be done most easily with lex(1).

  while (p < bufend)//当前命令是不是最后一字符，也就是'\n'
  {
   /*8、9、10 和 13 可以分别转换为*/
   /*退格符、制表符、换行符'\n'和回车符'/r'*/
    switch (*p)
    {
      case 13://回车键 指明一条指令结束
        p++;
        break;
      case '#':                 // Found comment 说明是注释开示
      	//如果是注释，则将NULL替换# ,说明一个字符串结束了，也可以说将命令配置和注释分离出来这样P指针只是指向一条没有注释的命令
        *p = '\0';              // Replace '#' with '\0' in case of comment immediately following integer or string
		//P一直查找字符串，知道将相应#后面的注释全部跳过。一直到此命令行的结束或文件结束。
        while (*p != '\n' && p < bufend)  // Skip till EOL or EOF, whichever comes first
          p++;
        InString = 0;
        InItem = 0;
        break;
		//如果是命令结束符 在字符串中不允许出现"\n"
      case '\n':
        InItem = 0;//说明是新的配置条目的开始
        InString = 0;//每个配置条目的第几个字符串
        *p++='\0';//指向了下一行配置符。并将'\n'清零
        break;
		//如果是空格或tab键，
      case ' ':
      case '\t':              // Skip whitespace, leave state unchanged
		//字符串中""
		if (InString)//如果当前指针不在""字符串之间
          p++;//指此条目的向下一个字符
        else
        {                     // Terminate non-strings once whitespace is found
          *p++ = '\0';//如果配置条目的第一个字符是空格或者是tab，把条目的开头配置下一个字符
          InItem = 0;//说明不是一个开始一个条目
        }
        break;

      case '"':               // Begin/End of String 说明是一个字符串的开始或结束
        *p++ = '\0';//"被'\n'代替
        if (!InString)//字符串是起始还是结束位置
        {//起始位置
          items[item++] = p;//将字符串首地址指针负值给条目，每个条目代表一个配置项的一行"xxx = yyy"
          InItem = ~InItem;//开始下一个条目
        }
        else
          InItem = 0;
		//如果是在字符串中则Instring 置位
		//也就是说在第一个"的时候置位
		//在第二个"会被清零
        InString = ~InString; // Toggle 
        break;

      default:
	  	//将每个条目负值给指针
        if (!InItem)
        {
          items[item++] = p;//将这个条目付给相应的指针数组
          InItem = ~InItem;//不在是条目的首字符了，要一直等待'\n'时才会被清零
        }
        p++;//指向下一个字符
    }
  }

  item--;//上面的最后一次执行了item++ 所以在这里--

  for (i=0; i<item; i+= 3)//每个条目含有 三个元素，等号左面"配置变量"  "等号" 等号右面的"配置值"
  {
  //这是第一个元素，也就是等号左面的配置字符串
    if (0 > (MapIdx = ParameterNameToMapIndex (items[i])))
    {
      snprintf (errortext, ET_SIZE, " Parsing error in config file: Parameter Name '%s' not recognized.", items[i]);
      error (errortext, 300);
    }
	//第二个元素，也就是等号，注意等号左右两边必须要有空格
    if (strcmp ("=", items[i+1]))
    {
      snprintf (errortext, ET_SIZE, " Parsing error in config file: '=' expected as the second token in each line.");
      error (errortext, 300);
    }

    // Now interprete the Value, context sensitive...
	//这是第三个元素，将配置文件右侧的配置值付给map 第二个元素
    switch (Map[MapIdx].Type)
    {
    	//如果是数字
      case 0:           // Numerical
        if (1 != sscanf (items[i+2], "%d", &IntContent))//将数字的值
        {
          snprintf (errortext, ET_SIZE, " Parsing error: Expected numerical value for Parameter of %s, found '%s'.", items[i], items[i+2]);
          error (errortext, 300);
        }
        * (int *) (Map[MapIdx].Place) = IntContent;//将整形数据付给map的第二个数据中，此指针是无符号的
        printf (".");//
        break;
		//说明是一个字符串
      case 1:
        strcpy ((char *) Map[MapIdx].Place, items [i+2]);//
        printf (".");
        break;
		//说明是一个浮点类型
      case 2:           // Numerical double
        if (1 != sscanf (items[i+2], "%lf", &DoubleContent))
        {
          snprintf (errortext, ET_SIZE, " Parsing error: Expected numerical value for Parameter of %s, found '%s'.", items[i], items[i+2]);
          error (errortext, 300);
        }
        * (double *) (Map[MapIdx].Place) = DoubleContent;
        printf (".");
        break;
      default:
        assert ("Unknown value type in the map definition of configfile.h");
    }
  }
  //config在map中被配置了，将配置过的configinput复制给input，他们都是InputParameters类型
  memcpy (input, &configinput, sizeof (InputParameters));//
}

/*!
 ***********************************************************************
 * \brief
 *    Returns the index number from Map[] for a given parameter name.
 *功能:将参数的字符串映射到代码的map配置索引中,map中包含了所有的JM配置元素字符串
 * \param s
 *    parameter name string
 * \return
 *    the index number if the string is a valid parameter name,         \n
 *    -1 for error
 ***********************************************************************
 */
static int ParameterNameToMapIndex (char *s)
{
  int i = 0;

  while (Map[i].TokenName != NULL)
    if (0==strcmp (Map[i].TokenName, s))//比较s和map中TokenName 配置条目是否同名
      return i;//返回s字符串对应的索引号
    else
      i++;
  return -1;
};


/*!
 ************************************************************************
 * \brief
 *    calculate Ceil(Log2(uiVal))
 * log2求值 数学意义是uival的值是可以被2除几次。
 * 例如9 
 ************************************************************************
 */
unsigned CeilLog2( unsigned uiVal)
{
  unsigned uiTmp = uiVal-1;
  unsigned uiRet = 0;

  while( uiTmp != 0 )
  {
    uiTmp >>= 1;
    uiRet++;
  }
  return uiRet;
}


/*!
 ***********************************************************************
 * \brief
 *    Checks the input parameters for consistency.
 ***********************************************************************
 */
static void PatchInp ()
{


  // These variables are added for FMO
  FILE * sgfile=NULL;
  int i;
  int frame_mb_only;
  int mb_width, mb_height, mapunit_height;

	
  // consistency check of QPs
  //第一帧的量化级别的范围0-51
  if (input->qp0 > MAX_QP || input->qp0 < MIN_QP)
  {
    snprintf(errortext, ET_SIZE, "Error input parameter quant_0,check configuration file");
    error (errortext, 400);
  }
  //剩余帧的量化级别的范围0-51
  if (input->qpN > MAX_QP || input->qpN < MIN_QP)
  {
    snprintf(errortext, ET_SIZE, "Error input parameter quant_n,check configuration file");
    error (errortext, 400);
  }

  if (input->qpB > MAX_QP || input->qpB < MIN_QP)
  {
    snprintf(errortext, ET_SIZE, "Error input parameter quant_B,check configuration file");
    error (errortext, 400);
  }
  //采用可变QP编码是量化范围
#ifdef _CHANGE_QP_
  if (input->qp2start && (input->qpB2 > MAX_QP || input->qpB2 < MIN_QP))
  {
    snprintf(errortext, ET_SIZE, "Error input parameter ChangeQPB,check configuration file");
    error (errortext, 400);
  }

  if (input->qp2start && (input->qp02 > MAX_QP || input->qp02 < MIN_QP))
  {
    snprintf(errortext, ET_SIZE, "Error input parameter ChangeQIB,check configuration file");
    error (errortext, 400);
  }

  if (input->qp2start && (input->qpN2 > MAX_QP || input->qpN2 < MIN_QP))
  {
    snprintf(errortext, ET_SIZE, "Error input parameter ChangeQPP,check configuration file");
    error (errortext, 400);
  }


#endif
  //SP帧的QP量化范围
  if (input->qpsp > MAX_QP || input->qpsp < MIN_QP)
  {
    snprintf(errortext, ET_SIZE, "Error input parameter quant_sp,check configuration file");
    error (errortext, 400);
  }
  if (input->qpsp_pred > MAX_QP || input->qpsp_pred < MIN_QP)
  {
    snprintf(errortext, ET_SIZE, "Error input parameter quant_sp_pred,check configuration file");
    error (errortext, 400);
  }
  if (input->sp_periodicity <0)
  {
    snprintf(errortext, ET_SIZE, "Error input parameter sp_periodicity,check configuration file");
    error (errortext, 400);
  }
  if (input->FrameRate < 0 || input->FrameRate>100)   
  {
    snprintf(errortext, ET_SIZE, "Error in input parameter FrameRate, check configuration file");
    error (errortext, 400);
  }
  if (input->FrameRate == 0)
    input->FrameRate = INIT_FRAME_RATE;

  if (input->idr_enable < 0 || input->idr_enable > 1)   
  {
    snprintf(errortext, ET_SIZE, "Error in input parameter IDRIntraEnable, check configuration file");
    error (errortext, 400);
  }

  if (input->start_frame < 0 )   
  {
    snprintf(errortext, ET_SIZE, "Error in input parameter StartFrane, Check configuration file.");
    error (errortext, 400);
  }

  // consistency check num_reference_frames
  if (input->num_reference_frames<1) input->num_reference_frames=1;


  // consistency check size information

  if (input->img_height % 16 != 0 || input->img_width % 16 != 0)
  {
    snprintf(errortext, ET_SIZE, "Unsupported image format x=%d,y=%d, must be a multiple of 16",input->img_width,input->img_height);
    error (errortext, 400);
  }

#ifdef _LEAKYBUCKET_
  // consistency check for Number of Leaky Bucket parameters
  if(input->NumberLeakyBuckets < 2 || input->NumberLeakyBuckets > 255) 
  {
    snprintf(errortext, ET_SIZE, "Number of Leaky Buckets should be between 2 and 255 but is %d \n", input->NumberLeakyBuckets);
    error(errortext, 400);
  }
#endif

  // check range of filter offsets
  if (input->LFAlphaC0Offset > 6 || input->LFAlphaC0Offset < -6)
  {
    snprintf(errortext, ET_SIZE, "Error input parameter LFAlphaC0Offset, check configuration file");
    error (errortext, 400);
  }

  if (input->LFBetaOffset > 6 || input->LFBetaOffset < -6)
  {
    snprintf(errortext, ET_SIZE, "Error input parameter LFBetaOffset, check configuration file");
    error (errortext, 400);
  }

  // Set block sizes
  /*
	设置宏块的大小 二维数组中每个元素是一个宏块类型的大小
  */
  input->blc_size[0][0]=16;
  input->blc_size[0][1]=16;
  
  input->blc_size[1][0]=16;
  input->blc_size[1][1]=16;
  
  input->blc_size[2][0]=16;
  input->blc_size[2][1]= 8;
  
  input->blc_size[3][0]= 8;
  input->blc_size[3][1]=16;
  
  input->blc_size[4][0]= 8;
  input->blc_size[4][1]= 8;
  
  input->blc_size[5][0]= 8;
  input->blc_size[5][1]= 4;
  
  input->blc_size[6][0]= 4;
  input->blc_size[6][1]= 8;
  
  input->blc_size[7][0]= 4;
  input->blc_size[7][1]= 4;
  
  // set proper log2_max_frame_num_minus4. 
  //StoredBPictures 是否将B帧存储起来作为参考帧
  //storedBplus1存储B帧的总数+1,successive_Bframe  P帧之间的B帧数量
  {
    int storedBplus1 = (input->StoredBPictures ) ? input->successive_Bframe + 1: 1;
  //no_frames除去B帧 之外被编码的帧数
  //计算帧的最大数 log2_max_frame_num_minus4=相当于将一个视频分成no_frames份，每份视频中只含有一个P(I)和successive_Bframe个B帧。
  //所以帧的总数是 no_frames理解为帧号
    log2_max_frame_num_minus4 = max( (int)(CeilLog2(1+ input->no_frames *storedBplus1 ))-4, 0);//最大帧数减4
  
//    log2_max_frame_num_minus4 = 0;
  }
  // 最多的帧数是2倍的参考帧数 循环数
  log2_max_pic_order_cnt_lsb_minus4 = max( (int)(CeilLog2(1+2*input->no_frames * (input->successive_Bframe + 1))) -4, 0);

  if (input->partition_mode < 0 || input->partition_mode > 1)
  {
    snprintf(errortext, ET_SIZE, "Unsupported Partition mode, must be between 0 and 1");
    error (errortext, 400);
  }
  //0 表示字节流 1表示RTP流
  if (input->of_mode < 0 || input->of_mode > 2)
  {
    snprintf(errortext, ET_SIZE, "Unsupported Output file mode, must be between 0 and 1");
    error (errortext, 400);
  }

  // B picture consistency check
  //编码基本层时跳过的帧数，B帧编码也需要设置此参数是，代表插入B帧的个数
  //也就是说jumpd包含B帧和空帧
  if(input->successive_Bframe > input->jumpd)
  {
    snprintf(errortext, ET_SIZE, "Number of B-frames %d can not exceed the number of frames skipped", input->successive_Bframe);
    error (errortext, 400);
  }

  // Direct Mode consistency check
  if(input->successive_Bframe && input->direct_type != DIR_SPATIAL && input->direct_type != DIR_TEMPORAL)
  {
    snprintf(errortext, ET_SIZE, "Unsupported direct mode=%d, use TEMPORAL=0 or SPATIAL=1", input->direct_type);
    error (errortext, 400);
  }

  if (input->PicInterlace>0 || input->MbInterlace>0)
  {
    if (input->directInferenceFlag==0)
      printf("\nDirectInferenceFlag set to 1 due to interlace coding.");
    input->directInferenceFlag=1;
  }

  if (input->PicInterlace>0)
  {
    if (input->IntraBottom!=0 && input->IntraBottom!=1)
    {
      snprintf(errortext, ET_SIZE, "Incorrect value %d for IntraBottom. Use 0 (disable) or 1 (enable).", input->IntraBottom);
      error (errortext, 400);
    }
  } 
  // Cabac/UVLC consistency check
  if (input->symbol_mode != UVLC && input->symbol_mode != CABAC)
  {
    snprintf (errortext, ET_SIZE, "Unsupported symbol mode=%d, use UVLC=0 or CABAC=1",input->symbol_mode);
    error (errortext, 400);
  }

  // Open Files 文件检查，查看输入的配置文件是否是可读的
  if ((p_in=fopen(input->infile,"rb"))==NULL)
  {
    snprintf(errortext, ET_SIZE, "Input file %s does not exist",input->infile);
    error (errortext, 500);
  }
  //查看重构文件是否命名，并且创建可写的重构文件
  if (strlen (input->ReconFile) > 0 && (p_dec=fopen(input->ReconFile, "wb"))==NULL)
  {
    snprintf(errortext, ET_SIZE, "Error open file %s", input->ReconFile);
    error (errortext, 500);
  }

  if (strlen (input->TraceFile) > 0 && (p_trace=fopen(input->TraceFile,"w"))==NULL)
  {
    snprintf(errortext, ET_SIZE, "Error open file %s", input->TraceFile);
    error (errortext, 500);
  }

	
  // add check for MAXSLICEGROUPIDS 片组数检查
  //
  if(input->num_slice_groups_minus1>=MAXSLICEGROUPIDS)
  {
    snprintf(errortext, ET_SIZE, "num_slice_groups_minus1 exceeds MAXSLICEGROUPIDS");
    error (errortext, 500);
  }
  
  
  
  // Following codes are to read slice group configuration from SliceGroupConfigFileName for slice group type 0,2 or 6
  //
  if( (input->num_slice_groups_minus1!=0)&&
    ((input->slice_group_map_type == 0) || (input->slice_group_map_type == 2) || (input->slice_group_map_type == 6)) )
  { 
    if (strlen (input->SliceGroupConfigFileName) > 0 && (sgfile=fopen(input->SliceGroupConfigFileName,"r"))==NULL)
    {
      snprintf(errortext, ET_SIZE, "Error open file %s", input->SliceGroupConfigFileName);
      error (errortext, 500);
    }
    else
    {
      if (input->slice_group_map_type == 0) 
      {
        input->run_length_minus1=(int *)malloc(sizeof(int)*(input->num_slice_groups_minus1+1));
        
        
        // each line contains one 'run_length_minus1' value
        for(i=0;i<=input->num_slice_groups_minus1;i++)
        {
          fscanf(sgfile,"%d",(input->run_length_minus1+i));
          fscanf(sgfile,"%*[^\n]");
          
        }
      }
      else if (input->slice_group_map_type == 2)
      {
        input->top_left=(int *)malloc(sizeof(int)*input->num_slice_groups_minus1);
        input->bottom_right=(int *)malloc(sizeof(int)*input->num_slice_groups_minus1);
        
        // every two lines contain 'top_left' and 'bottom_right' value
        for(i=0;i<input->num_slice_groups_minus1;i++)
        {
          fscanf(sgfile,"%d",(input->top_left+i));
          fscanf(sgfile,"%*[^\n]");
          fscanf(sgfile,"%d",(input->bottom_right+i));
          fscanf(sgfile,"%*[^\n]");
        }
        
      }
      else if (input->slice_group_map_type == 6)
      {
        frame_mb_only = !(input->PicInterlace || input->MbInterlace);
        mb_width= input->img_width/16;
        mb_height= input->img_height/16;
        mapunit_height=mb_height/(2-frame_mb_only);
        
        input->slice_group_id=(int * ) malloc(sizeof(int)*mapunit_height*mb_width);
        
        // each line contains slice_group_id for one Macroblock
        for (i=0;i<mapunit_height*mb_width;i++)
        {
          fscanf(sgfile,"%d",(input->slice_group_id+i));
          if ( *(input->slice_group_id+i) > input->num_slice_groups_minus1 )
          {
            snprintf(errortext, ET_SIZE, "Error read slice group information from file %s", input->SliceGroupConfigFileName);
            error (errortext, 500);
          }
          fscanf(sgfile,"%*[^\n]");
        }
      }
      fclose(sgfile);
      
    }
  }
  
  // frame/field consistency check 帧场的一致性检查 必须支持一个 帧模式 帧场自适应 场模式
  if (input->PicInterlace != FRAME_CODING && input->PicInterlace != ADAPTIVE_CODING && input->PicInterlace != FIELD_CODING)
  {
    snprintf (errortext, ET_SIZE, "Unsupported PicInterlace=%d, use frame based coding=0 or field based coding=1 or adaptive=2",input->PicInterlace);
    error (errortext, 400);
  }

  // frame/field consistency check
  //
  if (input->MbInterlace != FRAME_CODING && input->MbInterlace != ADAPTIVE_CODING && input->MbInterlace != FIELD_CODING)
  {
    snprintf (errortext, ET_SIZE, "Unsupported MbInterlace=%d, use frame based coding=0 or field based coding=1 or adaptive=2",input->MbInterlace);
    error (errortext, 400);
  }
  //加权方式范围，只能是0或者1，代表是否采用加权模式，加权模式在场景的淡入淡出有用，场景不是运动的只是亮度发生变化
  if (input->WeightedPrediction < 0 || input->WeightedPrediction > 1 )
  {
    snprintf (errortext, ET_SIZE, "\nWeightedPrediction=%d is not allowed.Select 0 (normal) or 1 (explicit)",input->WeightedPrediction);
    error (errortext, 400);
  }
  //B帧的加权方式，P帧支持系那是加权，而B帧支持显式和隐式方式
  //0表示B帧关闭加权方式1表示显式加权方式2表示隐式方式
  if (input->WeightedBiprediction < 0 || input->WeightedBiprediction > 2 )
  {
    snprintf (errortext, ET_SIZE, "\nWeightedBiprediction=%d is not allowed.Select 0 (normal), 1 (explicit), or 2 (implicit)",input->WeightedBiprediction);
    error (errortext, 400);
  }
  //帧场自适应类型
  //PicInterlace表示帧(图像)帧场自适应取值0-2 MbInterlace表示宏块方式的帧场自适应取值0-2
  //0 表示只使用帧编码方式 默认的 1表示只使用场编码 2 表示采用帧场自适应
  //帧场自适应暂时不看
  if (input->PicInterlace || input->MbInterlace)
  {
    //采用的是宏块对，每个宏块是16X16大小的，所以宏块对的高度一定是32。
	//这就是说明图像的高度一定是32的倍数才可以用场方式
    if (input->img_height % 32 != 0 )
    { 
      snprintf(errortext, ET_SIZE, "Unsupported image format y=%d, must be a multiple of 32 for adaptive frame/field",input->img_height);
      error (errortext, 400);
    }
  }

  if ((!input->rdopt)&&(input->MbInterlace))
  {
    snprintf(errortext, ET_SIZE, "MB AFF is not compatible with non-rd-optimized coding.");
    error (errortext, 500);
  }


  // Tian Dong: May 31, 2002
  // The number of frames in one sub-seq in enhanced layer should not exceed
  // the number of reference frame number.
  if ( input->NumFramesInELSubSeq >= input->num_reference_frames || input->NumFramesInELSubSeq < 0 )
  {
    snprintf(errortext, ET_SIZE, "NumFramesInELSubSeq (%d) is out of range [0,%d).", input->NumFramesInELSubSeq, input->num_reference_frames);
    error (errortext, 500);
  }
  // Tian Dong: Enhanced GOP is not supported in bitstream mode. September, 2002
  if ( input->NumFramesInELSubSeq > 0 && input->of_mode == PAR_OF_ANNEXB )
  {
    snprintf(errortext, ET_SIZE, "Enhanced GOP is not supported in bitstream mode and RTP mode yet.");
    error (errortext, 500);
  }
  // Tian Dong (Sept 2002)
  // The AFF is not compatible with spare picture for the time being.
  if ((input->PicInterlace || input->MbInterlace) && input->SparePictureOption == TRUE)
  {
    snprintf(errortext, ET_SIZE, "AFF is not compatible with spare picture.");
    error (errortext, 500);
  }

  // Only the RTP mode is compatible with spare picture for the time being.
  if (input->of_mode != PAR_OF_RTP && input->SparePictureOption == TRUE)
  {
    snprintf(errortext, ET_SIZE, "Only RTP output mode is compatible with spare picture features.");
    error (errortext, 500);
  }

  if( (input->WeightedPrediction > 0 || input->WeightedBiprediction > 0) && (input->MbInterlace))
  {
    printf("Weighted prediction coding is not supported for MB AFF currently.");
    error (errortext, 500);
  }
  if ( input->NumFramesInELSubSeq > 0 && input->WeightedPrediction > 0)
  {
    snprintf(errortext, ET_SIZE, "Enhanced GOP is not supported in weighted prediction coding mode yet.");
    error (errortext, 500);
  }

  //! the number of slice groups is forced to be 1 for slice group type 3-5
  if(input->num_slice_groups_minus1 > 0)
  {
    if( (input->slice_group_map_type >= 3) && (input->slice_group_map_type<=5) ) 
      input->num_slice_groups_minus1 = 1;
  }
	
  // Rate control
  if(input->RCEnable)
  {
    if ( (input->img_height*input->img_width/256)%input->basicunit!=0)
    {
      snprintf(errortext, ET_SIZE, "Basic unit is not defined correctly.");
      error (errortext, 500);
    }
  }

  if ((input->successive_Bframe)&&(input->StoredBPictures)&&(input->idr_enable)&&(input->intra_period)&&(input->pic_order_cnt_type!=0))
  {
    error("Stored B pictures combined with IDR pictures only supported in Picture Order Count type 0\n",-1000);
  }
  
  if( !input->direct_type && input->num_reference_frames<2 && input->successive_Bframe >0)
    error("temporal direct needs at least 2 ref frames\n",-1000);


  ProfileCheck();
  LevelCheck();
}

void PatchInputNoFrames()
{
  // Tian Dong: May 31, 2002
  // If the frames are grouped into two layers, "FramesToBeEncoded" in the config file
  // will give the number of frames which are in the base layer. Here we let input->no_frames
  // be the total frame numbers.
  input->no_frames = 1+ (input->no_frames-1) * (input->NumFramesInELSubSeq+1);
  if ( input->NumFrameIn2ndIGOP )
    input->NumFrameIn2ndIGOP = 1+(input->NumFrameIn2ndIGOP-1) * (input->NumFramesInELSubSeq+1);
  FirstFrameIn2ndIGOP = input->no_frames;
}

//协议的检查 
static void ProfileCheck()
{
  //必须是所支持的协议 base=66 main=77 ex=88
  if ((input->ProfileIDC != 66 ) && (input->ProfileIDC != 77 ) && (input->ProfileIDC != 88 ))
  {
  	//当前版本的JM支持的profile只有三种:基础, 主要,扩展。
    snprintf(errortext, ET_SIZE, "Profile must be baseline(66)/main(77)/extended(88).");
    error (errortext, 500);
  }
  // baseline 基本
  if (input->ProfileIDC == 66 )
  {
    if (input->successive_Bframe)//基本不支持B帧
    {
      snprintf(errortext, ET_SIZE, "B pictures are not allowed in baseline.");
      error (errortext, 500);
    }
    if (input->sp_periodicity)//是否支持SP帧
    {
      snprintf(errortext, ET_SIZE, "SP pictures are not allowed in baseline.");
      error (errortext, 500);
    }
    if (input->partition_mode)
    {
      snprintf(errortext, ET_SIZE, "Data partitioning is not allowed in baseline.");
      error (errortext, 500);
    }
    if (input->WeightedPrediction)
    {
      snprintf(errortext, ET_SIZE, "Weighted prediction is not allowed in baseline.");
      error (errortext, 500);
    }
    if (input->WeightedBiprediction)
    {
      snprintf(errortext, ET_SIZE, "Weighted prediction is not allowed in baseline.");
      error (errortext, 500);
    }
    if (input->symbol_mode == CABAC)//不支持CABAC编码 只能支持VAVCL
    {
      snprintf(errortext, ET_SIZE, "CABAC is not allowed in baseline.");
      error (errortext, 500);
    }
  }

  // main
  if (input->ProfileIDC == 77 )//主要类型
  {
    if (input->sp_periodicity)//不支持SP帧
    {
      snprintf(errortext, ET_SIZE, "SP pictures are not allowed in main.");
      error (errortext, 500);
    }
    if (input->partition_mode)
    {
      snprintf(errortext, ET_SIZE, "Data partitioning is not allowed in main.");
      error (errortext, 500);
    }
    if (input->num_slice_groups_minus1)//片组 不支持
    {
      snprintf(errortext, ET_SIZE, "num_slice_groups_minus1>0 (FMO) is not allowed in main.");
      error (errortext, 500);
    }
    if (input->redundant_slice_flag)
    {
      snprintf(errortext, ET_SIZE, "Redundant pictures are not allowed in main.");
      error (errortext, 500);
    }
  }

  // extended
  if (input->ProfileIDC == 88 )
  {
    if (!input->directInferenceFlag)//必须支持直接预测模式
    {
      snprintf(errortext, ET_SIZE, "direct_8x8_inference flag must be equal to 1 in extended.");
      error (errortext, 500);
    }

    if (input->symbol_mode == CABAC)
    {
      snprintf(errortext, ET_SIZE, "CABAC is not allowed in extended.");
      error (errortext, 500);
    }
  }
}

static void LevelCheck()
{
  return;
}

