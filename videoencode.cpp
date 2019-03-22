/*
 *@file:   videoencode.cpp
 *@author: 缪庆瑞
 *@date:   2017.5.12
 *@brief:  通过ffmpeg相关视频编解码库实现对原始数据帧的编码，输出视频文件
 */
#include "videoencode.h"

VideoEncode::VideoEncode(QObject *parent) : QObject(parent)
{

}

VideoEncode::~VideoEncode()
{

}
/*
 *@brief:   转换yuyv422格式帧为yuv420p　　因为保存到视频的原始帧格式为yuv420p
 *@author:  网络资源
 *@date:    2017.5.12
 *@param:   YUV422:原始数据帧
 *@param:   YUV420:转换后的数据帧
 *@param:   width:分辨率宽度
 *@param:   height: 分辨率高度
 */
void VideoEncode::YUYV422ToYUV420P(uchar *YUV422, uchar *YUV420, int width, int height)
{
    /*下面注释的部分因为动态的申请内存空间而没有释放，加上该函数执行频率比较快(在保存
    视频帧时执行，有采集的帧速率决定),执行一段时间后会出现内存不足，程序退出。经过测试发现
    在每秒25帧也就是每秒执行25次的情况下，大概40秒左右就会出现内存不足现象，所以这里将动态
    申请空间的操作移动到初始化函数(initVideoProcess())中*/
    /*uint8_t *data_srcBuf,*data_dstBuf;
    AVFrame *srcBuf = NULL;
    AVFrame *dstBuf = NULL;
    srcBuf = avcodec_alloc_frame();
    dstBuf = avcodec_alloc_frame();
    data_srcBuf = (uint8_t *)malloc(320*240*2);
    srcBuf->data[0] = data_srcBuf;*/
    memcpy(srcBuf->data[0],YUV422,320*240*2);
    srcBuf->linesize[0] = width*2;

    /*data_dstBuf = (uint8_t *)malloc((320*240*3)/2);
    dstBuf->data[0] = data_dstBuf;
    dstBuf->data[1] = dstBuf->data[0]+320*240;
    dstBuf->data[2] = dstBuf->data[1]+320*240/4;*/
    dstBuf->linesize[0] = width;
    dstBuf->linesize[1] = width/2;
    dstBuf->linesize[2] = width/2;

    yuyv422_to_yuv420p(dstBuf,srcBuf,width,height);//完成原始帧到目标格式帧的转换
    YUV420 = (uchar *)dstBuf->data[0];
}
/*
 *@brief:   初始化保存视频前的相关处理
 *@author:  缪庆瑞
 *@date:    2017.5.12
 *@param:   filename:保存的视频路径及名字
 */
void VideoEncode::initVideoProcess(QByteArray filename)
{
    AVOutputFormat *outputFormat;//输出到视频文件的格式
    AVCodec *avCodec;//存储编解码器信息
    const char * outFilename = filename.constData();//输出文件名及路径
    //const char * outFilename = "./video/test.avi";//输出文件名及路径
    av_register_all();//注册所有的编解码器
    //初始化输出码流的AVFormatContext
    //根据文件名后缀猜应该用什么编码，找不到对应后缀的编码方式返回NULL
    if(!(outputFormat = guess_format(NULL,outFilename,NULL)))
    {
        printf("The video format cannot be encoded.");
        exit(1);
    }
    formatContext = avformat_alloc_context();//申请空间返回指针
    formatContext->oformat = outputFormat;
    snprintf(formatContext->filename,sizeof(formatContext->filename),"%s",outFilename);

    //创建输出码流的AVStream
    stream = NULL;
    if(outputFormat->video_codec != CODEC_ID_NONE)
    {
        stream = av_new_stream(formatContext,0);//申请空间
        codecContext = stream->codec;
        codecContext->codec_id = outputFormat->video_codec;
        codecContext->codec_type = CODEC_TYPE_VIDEO;//视频类型
        codecContext->pix_fmt = PIX_FMT_YUV420P;//视频帧像素格式
        codecContext->bit_rate = 200000;//比特率
        codecContext->width = 320;//分辨率宽度
        codecContext->height = 240;
        codecContext->time_base.num = 1;//帧速率
        codecContext->time_base.den = 10;
        codecContext->gop_size = 12;

        //根据输出文件格式不同，设置对应参数
        if(codecContext->codec_id == CODEC_ID_MPEG2VIDEO)
        {
            codecContext->max_b_frames = 2;
        }
        if(codecContext->codec_id == CODEC_ID_MPEG1VIDEO)
        {
            codecContext->mb_decision = 2;
        }
        if(!strcmp(formatContext->oformat->name,"mp4")||!strcmp(formatContext->oformat->name,"mov")||!strcmp(formatContext->oformat->name,"3gp"))
        {
            codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }
    }
    if(av_set_parameters(formatContext,NULL)<0)
    {
        return;
    }
    dump_format(formatContext,0,outFilename,1);
    if(stream)
    {
        avCodec = avcodec_find_encoder(codecContext->codec_id);//根据输出文件格式得到的id查找编码器
        if(!avCodec)
        {
            printf("Not found the corresponding encoder\n");
            return;
        }
        if(avcodec_open(codecContext,avCodec) < 0)//打开编码器
        {
            printf("Failed to open encoder\n");
            return;
        }
        if(!(formatContext->oformat->flags&AVFMT_RAWPICTURE))//非原始(AVPicture结构)图片帧
        {
            //printf("Not RawPicture\n");
            video_outbuf_size = 200000;
            video_outbuf = (uchar *)av_malloc(video_outbuf_size);
        }
        rawFrame = avcodec_alloc_frame();//原始帧
        //通过帧格式以及宽高，获取原始帧所占空间大小
        int sizes = avpicture_get_size(codecContext->pix_fmt, codecContext->width, codecContext->height);
        rawFrame_buf = (uchar *)av_malloc(sizes);//为原始帧申请空间
        if(!rawFrame_buf)
        {
            printf("request allot rawFrame failed");
            av_free(rawFrame);//释放原始帧内存
            return ;
        }
        //将原始帧数据大小填充到rawFrame
        avpicture_fill((AVPicture *)rawFrame, rawFrame_buf, codecContext->pix_fmt, codecContext->width, codecContext->height);
    }

    if (!(outputFormat->flags & AVFMT_NOFILE))//写方式打开输出文件
    {
        if(url_fopen(&formatContext->pb, outFilename, URL_WRONLY) < 0)
        {
            printf("Failed to open output file\n");
            return;
        }
    }
    av_write_header(formatContext);//写文件头
    if(stream)
    {
        //video_pts = 0;
        //double video_pts = (double)(stream->time_base.num / stream->time_base.den);
        //rawFrame->pts = (double)(25);
    }
    else
    {
        //video_pts = 0;
        //rawFrame->pts = 0.0;
    }
    /*update 2017.5.14  下面是对YUYV422ToYUV420P()转换函数用到的变量的初始化,因为放在转换
    函数中会不断的动态申请内存空间，而没地方手动释放，时间长了就会导致内存不足*/
    srcBuf = avcodec_alloc_frame();//为yuyv422帧AVFrame申请空间
    dstBuf = avcodec_alloc_frame();
    data_srcBuf = (uint8_t *)malloc(320*240*2);//为yuyv422帧数据申请空间
    data_dstBuf = (uint8_t *)malloc((320*240*3)/2);
    srcBuf->data[0] = data_srcBuf;
    dstBuf->data[0] = data_dstBuf;
    dstBuf->data[1] = dstBuf->data[0]+320*240;
    dstBuf->data[2] = dstBuf->data[1]+320*240/4;
}
/*
 *@brief:   编码保存视频帧
 *@author:  缪庆瑞
 *@date:    2017.5.12
 *@param:   YUV420:视频帧的内存地址
 */
void VideoEncode::videoProcess(uchar *YUV420)
{
    AVPacket pkt;//存储压缩编码数据信息
    int pixelSize = codecContext->width*codecContext->height;
    rawFrame_buf = YUV420;
    rawFrame->data[0] = rawFrame_buf;//y
    rawFrame->data[1] = rawFrame_buf+pixelSize;//u
    rawFrame->data[2] = rawFrame_buf+ pixelSize*5/4;//v

    //原始帧为AVPicture结构，直接编码保存写入文件
    if(formatContext->oformat->flags & AVFMT_RAWPICTURE)
    {
        //printf("rawPicture...................\n");
        av_init_packet(&pkt);
        pkt.flags |= PKT_FLAG_KEY;//标志域
        pkt.stream_index = stream->index;//所属媒体流的索引
        pkt.data = (uchar *)rawFrame;//数据缓冲区指针
        pkt.size = sizeof(AVPicture);//数据缓冲区长度
        av_write_frame(formatContext,&pkt);
    }
    //原始帧非AVPicture结构 按照编码器转换原始帧格式
    else
    {
        //printf("Not RawPicture.................\n");
        //将原始帧按照指定的格式输出到输出缓冲区，返回值为帧的实际大小
        int real_outbuf_size = avcodec_encode_video(codecContext,video_outbuf,video_outbuf_size,rawFrame);
        if(real_outbuf_size > 0)
        {
            av_init_packet(&pkt);
            //时间戳
            pkt.pts = av_rescale_q(codecContext->coded_frame->pts,codecContext->time_base,stream->time_base);
            if (codecContext->coded_frame->key_frame)
            {
                pkt.flags |= PKT_FLAG_KEY;
            }
            pkt.stream_index = stream->index;
            pkt.data = video_outbuf;
            pkt.size = real_outbuf_size;
            av_write_frame(formatContext,&pkt);
            //av_free_packet(&pkt);
        }
    }
}
/*
 *@brief:   结束视频保存处理
 *@author:  缪庆瑞
 *@date:    2017.5.12
 */
void VideoEncode::finishVideoProcess()
{
    //写文件尾，不写会导致文件不完整，可能无法播放且无法获取属性(时长/帧率等)信息
    av_write_trailer(formatContext);
    url_fclose(formatContext->pb);//关闭写方式打开的视频文件
    //释放一些申请的空间
    av_free(formatContext);
    av_free(stream);
    av_free(codecContext);
    av_free(rawFrame);
    av_free(rawFrame_buf);
    av_free(video_outbuf);

    av_free(srcBuf);
    av_free(dstBuf);
}
/*
 *@brief:   转换yuyv422格式帧为yuv420p　　因为保存到视频的原始帧格式为yuv420p
 *@author:  网络资源
 *@date:    2017.5.12
 *@param:   dst:目标格式帧(yuv420p)
 *@param:   src:原始格式帧(yuyv422)
 *@param:   width:分辨率宽度
 *@param:   height: 分辨率高度
 */
void VideoEncode::yuyv422_to_yuv420p(AVFrame *dst, const AVFrame *src, int width, int height)
{
    const uint8_t *p, *p1;
    uint8_t *lum, *cr, *cb, *lum1, *cr1, *cb1;
    int w;

    p1 = src->data[0];
    lum1 = dst->data[0];
    cb1 = dst->data[1];
    cr1 = dst->data[2];

    for(;height >= 1; height -= 2)
    {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for(w = width; w >= 2; w -= 2)
        {
            lum[0] = p[0];
            cb[0] = p[1];
            lum[1] = p[2];
            cr[0] = p[3];
            p += 4;
            lum += 2;
            cb++;
            cr++;
        }
        if(w)
        {
            lum[0] = p[0];
            cb[0] = p[1];
            cr[0] = p[3];
            cb++;
            cr++;
        }
        p1 += src->linesize[0];
        lum1 += dst->linesize[0];
        if(height>1)
        {
            p = p1;
            lum = lum1;
            for(w = width; w >= 2; w -= 2)
            {
                lum[0] = p[0];
                lum[1] = p[2];
                p += 4;
                lum += 2;
            }
            if(w)
            {
                lum[0] = p[0];
            }
            p1 += src->linesize[0];
            lum1 += dst->linesize[0];
        }
        cb1 += dst->linesize[1];
        cr1 += dst->linesize[2];
    }
}
