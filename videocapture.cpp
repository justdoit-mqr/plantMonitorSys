/*视频采集捕获处理类*/
#include "videocapture.h"
VideoCapture::VideoCapture():QObject()
{
#ifdef VIDEO_ENCODE_SAVE
    /*视频编码保存相关的库初始化,放在初始化函数中，如果放在类外调用函数中，
    那么该对象就不在次线程中运行了*/
   videoEncode = new VideoEncode(this);
   timer = new QTimer(this);
   connect(timer,SIGNAL(timeout()),this,SLOT(getAndSaveFrame()));
#endif
   yuv422Frame = NULL;
   fd = -1;
   unMmap = false;
   strcpy(videoEqmName,"/dev/video0");
   hotplugTimer = new QTimer(this);
   connect(hotplugTimer,SIGNAL(timeout()),this,SLOT(hotplugSlot()));
   hotplugTimer->start(200);
}

VideoCapture::~VideoCapture()
{
    unInitMmap();
    closeDevice();
}

bool VideoCapture::initGlobal()
{
    if(!openDevice())
    {
        return false;
    }
    initDevice();
    initMmap();
    //start();
    //closeDevice();
   return true;
}
/*打开设备----对应usb摄像头设备的文件,该设备文件路径与名
有内核驱动V4l2决定*/
bool VideoCapture::openDevice()
{
    /*通过下面这个系统调用ls命令，并使用awk 命令匹配指定列，这里是第五列。因为在/dev下面的都是
   设备文件，第五列表示主设备号，第六列表示次设备号，而v4l2内核驱动指定的主设备号是81，由于只使用
    一个视频采集设备，所以如果连入视频设备的话，dev下有且只有一个主设备号为81的设备文件。获取
    该设备文件名及路径，重定向写入video_name文件中，重定向每次写入都会先清空文件，如果指定列没有
    匹配项就不会写入文件*/
    system("ls -l /dev | awk '$5==\"81,\" {print \"/dev/\"$10 > \"./config/video_name.txt\"}'");
    QFile file("./config/video_name");
    if(file.exists())
    {
        file.open(QIODevice::ReadOnly);
        memset(videoEqmName,0,sizeof(videoEqmName));
        int size = file.readLine(videoEqmName,30);
        videoEqmName[size-1]='\0';//因为readline会读入\n换行符，将\n换成字符串结束标志
        //qDebug()<<size<<"videoEqmName:"<<videoEqmName<<strlen(videoEqmName);
        file.close();
    }
    //qDebug()<<"videoEqmName:"<<videoEqmName;
    fd = open(videoEqmName,O_RDWR);
    //fd = open("/dev/video0",O_RDWR)
	if(fd == -1)
	{
		printf("Cann't not open video device file.\n");
		qDebug()<<"Please insert camera device.";
        //exit(1);
        return false;
	}
	printf("fd=%d\n",fd);
    return true;
}
/*关闭设备*/
void VideoCapture::closeDevice()
{
    int f = close(fd);
	if(-1 == f)
	{
		printf("close device failed.");
		exit(-1);
    }
}
/*
 *@brief:   获取视频采集设备状态（存在or不存在）
 *@date:    2017.5.19
 */
bool VideoCapture::getVideoEqmExistStatus()
{
    return videoEqmExist;
}
/*
 *@brief:   将yuv帧格式数据转换为rgb
 *@date:    2017.5.10
 *@param:   y:亮度分量值
 *@param:   u:R-Y色差分量
 *@param:   v:B-Y色差分量
 */
int VideoCapture::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    uint pixel_32 = 0;
    uchar *pixel = (uchar *) & pixel_32;
    int r,g,b;
    r = y + (1.370705 * (v - 128));
    g = y - (0.698001 * (v - 128)) - (0.337633 * (u - 128));
    b = y + (1.732446 * (u - 128));
    if(r > 255)
    {
        r = 255;
    }
    if(g > 255)
    {
        g = 255;
    }
    if(b > 255)
    {
        b = 255;
    }
    if(r < 0)
    {
        r = 0;
    }
    if(g < 0)
    {
        g = 0;
    }
    if(b < 0)
    {
        b = 0;
    }
    pixel[0] = r * 220/255;
    pixel[1] = g * 220/255;
    pixel[2] = b * 220/255;
    return pixel_32;
}
/*
 *@brief:   将yuv帧格式数据转换成rgb，由参数rgb指向数据地址
 *@date:    2017.5.10
 *@param:   yuv:yuv帧格式数据地址
 *@param:   rgb:rgb帧格式地址
 *@param:   width:宽度
 *@param:   height:高度
 */
void VideoCapture::convert_yuv_to_rgb_buffer(uchar *yuv, uchar *rgb, uint width, uint height)
{
    unsigned in,out = 0;
    uint pixel_16;
    uchar pixel_24[3];
    uint pixel_32;
    int y0,u,y1,v;
    for(in = 0;in<width*height*2;in += 4)
    {
        pixel_16 =
        yuv[in + 3] << 24 |
        yuv[in + 2] << 16 |
        yuv[in + 1] <<  8 |
        yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >> 8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;

        pixel_32 = convert_yuv_to_rgb_pixel(y0,u,v);
        pixel_24[0] = (pixel_32 & 0x000000ff);
        pixel_24[1] = (pixel_32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel_32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];

        pixel_32 = convert_yuv_to_rgb_pixel(y1,u,v);
        pixel_24[0] = (pixel_32 & 0x000000ff);
        pixel_24[1] = (pixel_32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel_32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
}
#ifdef VIDEO_ENCODE_SAVE
/*
 *@brief:   获取并编码保存视频帧
 *@author:  缪庆瑞
 *@date:    2017.5.12
 */
void VideoCapture::getAndSaveFrame()
{
    qDebug()<<"start:"<<QTime::currentTime().toString("hh:mm:ss:zzz");
    //先获取
    v4l2_buffer buf;
    memset(&buf,0,sizeof(buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    int dqbuf = ioctl(fd,VIDIOC_DQBUF,&buf);//从视频输出队列取出一个缓冲帧
    if(dqbuf == -1)
    {
        printf("dqbuf failed.\n");
        //exit(-1);
        return ;//当视频输出队列暂没有缓冲帧时，直接返回函数等待下一次，而不是直接退出
    }
    {
        QMutexLocker locker(&mutex);
        yuv422Frame = bufPtr[buf.index].addr;//取出的yuv422格式帧的地址
    }
    index = buf.index;
    uchar *yuv420;
    videoEncode->YUYV422ToYUV420P((uchar *)yuv422Frame,yuv420,IMAGEWIDTH,IMAGEHEIGHT);
    videoEncode->videoProcess(yuv420);
    {
        //对下面的操作加锁，避免在主线程操作yuv422Frame时，次线程对该地址写入新数据造成屏幕混乱
        QMutexLocker locker(&mutex);
        unGetFrame();/*将一个空的缓冲帧重新放入队列*/
    }
    //qDebug()<<"stop:"<<QTime::currentTime().toString("hh:mm:ss:zzz");
}
/*
 *@brief:   开始录制视频，保存到文件
 *@author:  缪庆瑞
 *@date:    2017.5.12
 */
void VideoCapture::startRecord()
{
    //根据当前时间保存视频文件名
    QString filename(QDateTime::currentDateTime().toString("yyyyMMddhhmm"));
    QString pathAndName("/mnt/mtd/video/"+filename+".avi");
    //QString pathAndName("/mnt/mtd/"+filename+".avi");
    QByteArray array = pathAndName.toUtf8();
    videoEncode->initVideoProcess(array);
    /*因为经过测试发现在视频界面刷新过程中,保存视频的线程在系统调度上会慢一些，
    每秒最多大约能处理11帧的数据，而视频界面不刷新时，最快大约每秒能处理24帧，所以为了能
    让保存的视频时间速度一致，这里设置每秒保存10帧，并设置保存视频文件的帧速率为每秒10帧，
    否则可能造成视频文件的时间、速度与实际不符*/
    //每秒保存10帧,此时如果视频界面刷新会占用一些cpu时间片,导致保存视频的线程会比预期慢一些
    timer->start(100);
}
/*
 *@brief:   停止录制视频
 *@author:  缪庆瑞
 *@date:    2017.5.12
 */
void VideoCapture::stopRecord()
{
    yuv422Frame = NULL;
    timer->stop();
    videoEncode->finishVideoProcess();
}
#endif
/*
 *@brief:   定时查询视频采集设备的状态
 *@author:  缪庆瑞
 *@date:    2017.5.12
 */
void VideoCapture::hotplugSlot()
{
    if(access(videoEqmName,0) == -1)//判断设备文件是否存在
    {
        videoEqmExist = false;//不存在
        /*如果之前成功打开过该设备，此处需要取消内存映射，并关闭设备*/
        if(fd != -1)
        {
            unMmap = true;
            //unInitMmap();在该线程关闭内存映射，如果主线程正在操作内存地址会段错误
            closeDevice();
            fd = -1;
        }
    }
    else
    {
        videoEqmExist = true;
    }
}
/*初始化设备*/
void VideoCapture::initDevice()
{
    /*获取设备的基本信息 驱动能力
    接口v4l2_capability
     struct v4l2_capability
    {
        u8 driver[16]; // 驱动名字
        u8 card[32]; // 设备名字
        u8 bus_info[32]; // 设备在系统中的位置
        u32 version; // 驱动版本号
        u32 capabilities; // 设备支持的操作  视频采集(图像获取)|IO数据流控制等
        u32 reserved[4]; // 保留字段
    };*/
	v4l2_capability capa;
    /*用户空间的ioctl(int fd,unsigned long cmd,...)函数实现对设备的读取与控制(第三个参数一般涉及数据
     * 传输时使用)，在用户空间使用ioctl系统调用控制设备，用户程序只需要通过命令码cmd告诉
     * 驱动程序它想做什么，具体命令怎么解释和实现由驱动程序(这里就是v4l2驱动程序)的ioctl()
     * 函数来实现*/
	ioctl(fd,VIDIOC_QUERYCAP,&capa);
    printf("\nBasic Information:\ndriver=%s\tcard=%s\tbus_info=%s\tversion=%d\ncapa=%x\n",capa.driver,capa.card,capa.bus_info,capa.version,capa.capabilities);

    /*获取设备支持的帧格式描述
    接口v4l2_fmtdesc
    struct v4l2_fmtdesc
    {
        u32 index; // 要查询的格式序号，应用程序设置
        enum v4l2_buf_type type; // 帧类型，应用程序设置
        u32 flags; // 是否为压缩格式
        u8 description[32]; // 格式名称
        u32 pixelformat; // 格式
        u32 reserved[4]; // 保留
    }*/
	v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//表示 采集帧
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc) != -1)//显示所有支持的视频输出格式
	{
        printf("\nSupport Format:\nflags=%d\tdescription=%s\tpixelformat=%c%c%c%c\n",
               fmtdesc.flags,fmtdesc.description,fmtdesc.pixelformat&0xFF,(fmtdesc.pixelformat>>8)&0xFF,
               (fmtdesc.pixelformat>>16)&0xFF,(fmtdesc.pixelformat>>24)&0xFF);
        fmtdesc.index++;
    }

    /*获取视频采集帧速率  帧/秒
    struct v4l2_streamparm
    {
        __u32	 type;			//帧类型，应用程序设置
        union
        {
            struct v4l2_captureparm	capture;
            struct v4l2_outputparm	output;
            __u8	raw_data[200];  //user-defined
        } parm;
    };*/
    v4l2_streamparm streamparm;
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_G_PARM, &streamparm);
    printf("\nFrame Rate:\n%d/%d\n",streamparm.parm.capture.timeperframe.numerator,
           streamparm.parm.capture.timeperframe.denominator);

    /*获取默认视频采集窗口
    struct v4l2_crop {
        __u32			type;	// 类型
        struct v4l2_rect        c;//窗口
    };*/
	v4l2_crop crop;
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//采集类型
	ioctl(fd,VIDIOC_G_CROP,&crop);
    printf("\nDefault window:\nleft=%d\ttop=%d\twidth=%d\theight=%d\n",crop.c.left,crop.c.top,crop.c.width,crop.c.height);

    /*获取设备捕获图像能力的参数
    struct v4l2_cropcap
    {
        enum v4l2_buf_type type; // 数据流的类型，应用程序设置
        struct v4l2_rect bounds; // 这是 camera 的镜头能捕捉到的窗口大小的局限
        struct v4l2_rect defrect; // 定义默认窗口大小，包括起点位置及长,宽的大小，大小以像素为单位
        struct v4l2_fract pixelaspect; // 定义了图片的宽高比
    };*/
	v4l2_cropcap cropcap;
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd,VIDIOC_CROPCAP,&cropcap);
    //这里获取的捕获能力参数是设备上一次被设置的参数
    printf("\nCapture capability:\nmaxleft=%d\tmaxtop=%d\tmaxwidth=%d\tmaxheight=%d\n",cropcap.bounds.left,cropcap.bounds.top,cropcap.bounds.width,cropcap.bounds.height);
	printf("defleft=%d\tdeftop=%d\tdefwidth=%d\tdefheight=%d\n",cropcap.defrect.left,cropcap.defrect.top,cropcap.defrect.width,cropcap.defrect.height);
    printf("aspect ratio=%d:%d\n",cropcap.pixelaspect.numerator,cropcap.pixelaspect.denominator);
	
    /*设置捕获视频帧格式参数
    struct v4l2_format
    {
        enum v4l2_buf_type type; // 帧类型，应用程序设置
        union fmt
        {
            struct v4l2_pix_format pix; // 视频设备使用
            struct v4l2_window win;
            struct v4l2_vbi_format vbi;
            struct v4l2_sliced_vbi_format sliced;
            u8 raw_data[200];
        };
    };
    struct v4l2_pix_format
    {
        u32 width; // 帧宽，单位像素
        u32 height; // 帧高，单位像素
        u32 pixelformat; // 帧格式
        enum v4l2_field field;
        u32 bytesperline;
        u32 sizeimage;
        enum v4l2_colorspace colorspace;
        u32 priv;
    };*/
	v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.width = IMAGEWIDTH;
    format.fmt.pix.height = IMAGEHEIGHT;
    format.fmt.pix.field = V4L2_FIELD_INTERLACED;
    ioctl(fd,VIDIOC_S_FMT,&format);//设置帧格式
//    ioctl(fd,VIDIOC_G_FMT,&format);//获取帧格式
//    printf("\nFrame Format:\npixelformat=%x\twidth=%d\theight=%d\n",format.fmt.pix.pixelformat
//           ,format.fmt.pix.width,format.fmt.pix.height);
}
/*将设备映射到内存空间
    因为v4l2是视频设备的驱动层，位于内核空间，向设备申请的缓冲区也在内核空间
    用户程序不能直接访问，所以使用mmap()内存映射函数把内核空间映射到用户空间*/
void VideoCapture::initMmap()
{
    /*向设备申请缓冲区
     struct v4l2_requestbuffers
    {
        u32 count; // 缓冲区内缓冲帧的数目
        enum v4l2_buf_type type; // 缓冲帧数据格式
        enum v4l2_memory memory; // 区别是内存映射还是用户指针方式
        u32 reserved[2];
    };*/
	v4l2_requestbuffers reqbufs;
	reqbufs.count = BUFFERCOUNT;//缓冲帧数目
    reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//帧采集
	reqbufs.memory = V4L2_MEMORY_MMAP;//用户程序与设备交换数据的方式
    ioctl(fd,VIDIOC_REQBUFS,&reqbufs);//分配内存
	
	/*将设备映射到内存空间*/
	bufPtr = (buffer *)calloc(reqbufs.count,sizeof(*bufPtr));//为缓冲区映射对象开辟内存空间
	if(!bufPtr)
	{
		fprintf(stderr,"Out of memory.\n");
		exit(-1);
	}
	for(numBuf = 0;numBuf<reqbufs.count;numBuf++)
	{
        v4l2_buffer buf;//一个缓冲帧
        buf.index = numBuf;//序号从0开始
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
        /*从内存映射查询序号为numBuf的缓冲帧信息*/
		if(ioctl(fd,VIDIOC_QUERYBUF,&buf) == -1)
		{
			printf("query buffer failed.\n");
			exit(-1);
		}
		//printf("buf.length=%d\n",buf.length);
		bufPtr[numBuf].length = buf.length;
		bufPtr[numBuf].addr = (unsigned char *)mmap(0,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,buf.m.offset);
		if(bufPtr[numBuf].addr == MAP_FAILED)
		{
		printf("map failed.\n");
		perror("mmap");
		exit(-1);
		}
	}
	//unInitMmap();
}
/*卸载内存映射*/
void VideoCapture::unInitMmap()
{
	for(uint i = 0;i<numBuf;i++)
	{	
		munmap(bufPtr[i].addr,bufPtr[i].length);
	}
}
/*开始采集*/
void VideoCapture::start()
{
    /*将缓冲帧放入视频输入队列并启动数据流*/

	for(uint i = 0;i < numBuf;i++)
	{
		v4l2_buffer buf;

		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		
        ioctl(fd,VIDIOC_QBUF,&buf);//缓冲帧放入视频输入队列 FIFO
	}
	v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /*启动数据流，内核驱动开始采集数据帧，放入视频输入队列缓冲帧,填满一帧
    移到视频输出缓冲队列*/
    ioctl(fd,VIDIOC_STREAMON,&type);
}
/*停止采集*/
void VideoCapture::stop()
{
	v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd,VIDIOC_STREAMOFF,&type);//停止数据流
}
/*获取图片帧*/
void VideoCapture::getFrame(void *rgbFrameAddr)
{
    //该函数实际在主线程运行
    if(yuv422Frame)
    {
        QMutexLocker locker(&mutex);//使用加锁操作，避免与次线程同时操作相同内存
        convert_yuv_to_rgb_buffer((uchar *)yuv422Frame,(uchar *)rgbFrameAddr,IMAGEWIDTH,IMAGEHEIGHT);
        return ;
    }
    v4l2_buffer buf;
	memset(&buf,0,sizeof(buf));

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

    int dqbuf = ioctl(fd,VIDIOC_DQBUF,&buf);//从视频输出队列取出一个缓冲帧
	if(dqbuf == -1)
	{
		printf("dqbuf failed.\n");
        //exit(-1);
        return ;
	}
    /*FIFO队列尾放入一帧 放在这里视频采集没问题,但从v4l2使用步骤来看，将空的缓冲帧
    重新放入队列这一操作，需要在对取出的缓冲帧做出处理（转换成对应图片）后在进行*/
    //unGetFrame();
    yuvFrameAddr = bufPtr[buf.index].addr;//取出的yuv422格式帧的地址
    index = buf.index;
    convert_yuv_to_rgb_buffer((uchar *)yuvFrameAddr,(uchar *)rgbFrameAddr,IMAGEWIDTH,IMAGEHEIGHT);//将yuv格式帧转换为rgb格式
    //qDebug()<<*bufFrame<<*len;
    /*FIFO队列尾放入一帧 放这里会闪屏,原因已找到,因为将缓冲帧放入队列时，
    按照先入先出原则，新入的缓冲帧会放在出去的缓冲帧地址上，如果不做延时
    处理的话，可能会导致出去的帧数据和新入的帧数据混合，出现闪屏现象*/
    unGetFrame();
    //usleep(100000);
    /*查询指定序号index的缓冲帧信息，通过flags判断状态
     * flags=1：内存映射状态,不在视频输入和输出队列中
        flags=3：内存映射状态，在视频输入队列但缓冲区帧为空，尚未填满
        flags=5：内存映射状态，驱动采集到一帧数据放入缓冲区，移至视频输出队列*/
//    if(ioctl(fd,VIDIOC_QUERYBUF,&buf) == -1)
//    {
//        printf("query buffer failed.\n");
//        exit(-1);
//    }
//    printf("buf.index=%d\tbuf.flag=%x\n",buf.index,buf.flags);
}
/*将一个空的缓冲帧重新放入队列*/
void VideoCapture::unGetFrame()
{
    if(index != -1)
	{
        v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        //qDebug()<<"index="<<index;
        buf.index = index;

        ioctl(fd,VIDIOC_QBUF,&buf);
	}
}
