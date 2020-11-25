#pragma once

#include <optional>
#include <string>
#include <memory>
#include <tuple>
#include <vector>
#include <iostream>
#include "console_dcimg.h"
#include "common_dcimg.h"
#include "dcimgapi.h"

#ifdef OPENCV_SUPPORT
#include <opencv2/core.hpp>
#endif // OPENCV_SUPPORT


namespace ImageProcessing
{
	class DcimgReader
	{
	private:
		//----------------------------------------------------------------------------------------------------
		//											private fields
		std::shared_ptr<tag_DCIMG> hdcimg;
		int32 current_session;
		int32 height;
		int32 width;
		int32 rowBytes;
		int32 bits_per_pixel;
		int32 nframes_in_currentsession;
		int32 nsessions;

		//----------------------------------------------------------------------------------------------------
		//										private methods
	private:
		std::tuple<int32, int32, int32, int32, int32> get_image_information();
		int32 get_sessioncount();
		bool set_session(int32 sessionidx);


		//----------------------------------------------------------------------------------------------------
		//											public methods
	public:
		DcimgReader(const std::wstring& filename);
		DcimgReader(const std::wstring& filename, int32 sessionidx);
		void SetCurrentSession(int32 sessionidx);
		int32 GetNumberOfSessions() const;
		int32 GetCurrentSession() const;
		int32 GetHeight() const;
		int32 GetWidth() const;
		int32 GetRowBytes() const;
		int32 GetBitsPerPixel() const;
		int32 GetNumberOfFrames() const;
		std::optional<std::vector<double>> GetTimeStamp() const;
		std::unique_ptr<unsigned char[]> ReadFrame(size_t index);
		std::unique_ptr<unsigned char[]> ReadFrames(size_t start, size_t stop);
		std::unique_ptr<unsigned short[]> ReadFrameMONO16(size_t index);
		std::unique_ptr<unsigned short[]> ReadFramesMONO16(size_t start, size_t stop);

#ifdef OPENCV_SUPPORT
		std::optional<cv::Mat> ReadCvFrame(size_t index);
		std::optional<std::vector<cv::Mat>> ReadCvFrames(size_t start, size_t stop);
#endif // OPENCV_SUPPORT

	};	//class DcimgReader
}	//namespace ImageProcessing


//戻り値の順番: height, width, rowbytes, pixeltype, nframes
//取得に失敗すると、0が入る。
inline std::tuple<int32, int32, int32, int32, int32> ImageProcessing::DcimgReader::get_image_information()
{
	DCIMG_ERR err;

	int32 nWidth;
	// get width
	err = dcimg_getparaml(hdcimg.get(), DCIMG_IDPARAML_IMAGE_WIDTH, &nWidth);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_getparaml(DCIMG_IDPARAML_IMAGE_WIDTH)");
		nWidth = 0;
	}

	int32 nHeight;
	// get height
	err = dcimg_getparaml(hdcimg.get(), DCIMG_IDPARAML_IMAGE_HEIGHT, &nHeight);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_getparaml(DCIMG_IDPARAML_IMAGE_HEIGHT)");
		nHeight = 0;
	}

	int32 nRowbytes;
	// get row bytes
	err = dcimg_getparaml(hdcimg.get(), DCIMG_IDPARAML_IMAGE_ROWBYTES, &nRowbytes);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_getparaml(DCIMG_IDPARAML_IMAGE_ROWBYTES)");
		nRowbytes = 0;
	}

	int32 nPixeltype;
	// get pixel type
	err = dcimg_getparaml(hdcimg.get(), DCIMG_IDPARAML_IMAGE_PIXELTYPE, &nPixeltype);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_getparaml(DCIMG_IDPARAML_IMAGE_PIXELTYPE)");
		nPixeltype = 0;
	}

	int32 bits = nPixeltype << 3;	//nPixeltype * 8

	int32 nFrames;
	err = dcimg_getparaml(hdcimg.get(), DCIMG_IDPARAML_NUMBEROF_FRAME, &nFrames);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_getparaml(DCIMG_IDPARAML_NUMBEROF_FRAME)");
		nFrames = 0;
	}

	return { nHeight, nWidth, nRowbytes, bits, nFrames };
}



/**
@brief get number of session in opened file.
@return	result to get number of session
		if failed, return -1
*/
inline int32 ImageProcessing::DcimgReader::get_sessioncount()
{
	DCIMG_ERR err;

	int32 paraml = 0;

	// get session count
	err = dcimg_getparaml(hdcimg.get(), DCIMG_IDPARAML_NUMBEROF_SESSION, &paraml);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_getparaml(DCIMG_IDPARAML_NUMBEROF_SESSION)");
		return -1;
	}

	return paraml;
}




/*
@brief change session.
@return if succeeded, return true.*/
inline bool ImageProcessing::DcimgReader::set_session(int32 sessionidx)
{
	DCIMG_ERR err;

	err = dcimg_setsessionindex(hdcimg.get(), sessionidx);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_setsessionindex");
		return false;
	}

	return true;
}



//-----------------------------------------------------------------------------------------------------
//											Constructor
//@param	filename
inline ImageProcessing::DcimgReader::DcimgReader(const std::wstring& filename) : DcimgReader(filename, 0) {}


//-----------------------------------------------------------------------------------------------------
//											Constructor
//@param	filename
//@param	sessionidx
inline ImageProcessing::DcimgReader::DcimgReader(const std::wstring& filename, int32 sessionidx)
{
	hdcimg = std::shared_ptr<tag_DCIMG>(dcimgcon_init_open(filename.c_str()), dcimg_close);

	int32 sessions = get_sessioncount();
	if (sessions != -1)
		nsessions = sessions;
	else
		nsessions = 0;
	SetCurrentSession(sessionidx);
}


//-----------------------------------------------------------------------------------------------------
//										Set session index
//@param	sessionidx
inline void ImageProcessing::DcimgReader::SetCurrentSession(int32 sessionidx)
{
	current_session = sessionidx;
	set_session(sessionidx);

	//新しいセッションの情報に更新
	auto inf = get_image_information();

	height = std::get<0>(inf);
	width = std::get<1>(inf);
	rowBytes = std::get<2>(inf);
	bits_per_pixel = std::get<3>(inf);
	nframes_in_currentsession = std::get<4>(inf);
}


//----------------------------------------------------------------------------------------------------
//							returns number of sessions in this dcimg file
//@return	int32
inline int32 ImageProcessing::DcimgReader::GetNumberOfSessions() const
{
	return nsessions;
}


//----------------------------------------------------------------------------------------------------
//									returns current session index
//@return	int32
inline int32 ImageProcessing::DcimgReader::GetCurrentSession() const
{
	return current_session;
}


//----------------------------------------------------------------------------------------------------
//									returns height of the images
//@return	int32
inline int32 ImageProcessing::DcimgReader::GetHeight() const
{
	return height;
}


//----------------------------------------------------------------------------------------------------
//									returns width of the images
//@return	int32
inline int32 ImageProcessing::DcimgReader::GetWidth() const
{
	return width;
}


//----------------------------------------------------------------------------------------------------
//									returns the byte size of a row
//@return	int32
inline int32 ImageProcessing::DcimgReader::GetRowBytes() const
{
	return rowBytes;
}


//----------------------------------------------------------------------------------------------------
//									returns the bit size per pixel
//@return	int32
inline int32 ImageProcessing::DcimgReader::GetBitsPerPixel() const
{
	return bits_per_pixel;
}


//----------------------------------------------------------------------------------------------------
//						returns the number of frames in the current session
//@return	int32
inline int32 ImageProcessing::DcimgReader::GetNumberOfFrames() const
{
	return nframes_in_currentsession;
}


//----------------------------------------------------------------------------------------------------
//							returns timestamps in the current session
//@return	std::optional<std::vector<double>>
inline std::optional<std::vector<double>> ImageProcessing::DcimgReader::GetTimeStamp() const
{
	std::unique_ptr<DCIMG_TIMESTAMP[]> timestamps = std::make_unique<DCIMG_TIMESTAMP[]>(nframes_in_currentsession);
	if (!timestamps)
	{
		printf("Error: fail to allocate %d TIMESTAMP.\n", nframes_in_currentsession);
		return std::nullopt;
	}

	DCIMG_TIMESTAMPBLOCK block;
	memset(&block, 0, sizeof(block));

	block.hdr.size = sizeof(block);
	block.hdr.iKind = DCIMG_METADATAKIND_TIMESTAMPS;
	block.timestamps = timestamps.get();
	block.timestampmax = nframes_in_currentsession;
	block.timestampsize = sizeof(timestamps[0]);

	DCIMG_ERR err;
	std::vector<double> ret(nframes_in_currentsession);

	err = dcimg_copymetadatablock(hdcimg.get(), &block.hdr);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_copymetadatablock(DCIMG_TIMESTAMPBLOCK)");
		return std::nullopt;
	}
	else if (block.timestampvalidsize < sizeof(timestamps[0]))
	{
		printf("time stamp size is unknown (%d bytes). This is smaller than expected.\n", block.timestampvalidsize);
		return std::nullopt;
	}
	else
	{
		int32 iFrame;
		DCIMG_TIMESTAMP& ts = timestamps[0];
		double fStart = double(ts.sec) + double(ts.microsec) / 1000000;

		for (iFrame = 0; iFrame < block.timestampcount; ++iFrame)
		{
			ts = timestamps[iFrame];
			double fNow = double(ts.sec) + double(ts.microsec) / 1000000;
			ret[iFrame] = fNow - fStart;
		}
	}

	return ret;
}


//----------------------------------------------------------------------------------------------------
//							returns an 8bit image pointer of the index
//return	std::unique_ptr<unsigned char[]>
inline std::unique_ptr<unsigned char[]> ImageProcessing::DcimgReader::ReadFrame(size_t index)
{
	return ReadFrames(index, index + 1);
}


//----------------------------------------------------------------------------------------------------
//						returns 8bit images pointer between start and stop
//return	std::unique_ptr<unsigned char[]>
inline std::unique_ptr<unsigned char[]> ImageProcessing::DcimgReader::ReadFrames(size_t start, size_t stop)
{
	DCIMG_ERR err;

	if (stop > nframes_in_currentsession)
	{
		printf("Number of frame in this session is %d.\n", nframes_in_currentsession);
		printf("%zd is out of range!\n", stop);
		return nullptr;
	}

	if (stop - start < 0)
	{
		std::cerr << "Stop index must be greater than start index.";
		return nullptr;
	}

	// access to frame data
	DCIMG_FRAME	frame;

	memset(&frame, 0, sizeof(frame));
	frame.size = sizeof(frame);
	frame.width = this->width;
	frame.height = this->height;
	frame.rowbytes = this->rowBytes;
	frame.type = static_cast<DCIMG_PIXELTYPE>(this->bits_per_pixel >> 3);

	std::unique_ptr<unsigned char[]> buffer = std::make_unique<unsigned char[]>(static_cast<size_t>(rowBytes) * static_cast<size_t>(height) * (stop - start));
	unsigned char* it = buffer.get();

	for (size_t i = 0; i < stop - start; ++i)
	{
		frame.buf = it;
		frame.iFrame = static_cast<int32>(start + i);


		err = dcimg_copyframe(hdcimg.get(), &frame);
		if (failed(err))
		{
			dcimgcon_show_dcimgerr(err, "dcimg_copyframe()", "#%d frame", frame.iFrame);
			return nullptr;
		}

		it += static_cast<size_t>(rowBytes) * static_cast<size_t>(height);
	}

	return buffer;
}


//----------------------------------------------------------------------------------------------------
//								returns an 16bit image of the index
//@return	std::unique_ptr<unsigned short[]>
inline std::unique_ptr<unsigned short[]> ImageProcessing::DcimgReader::ReadFrameMONO16(size_t index)
{
	return ReadFramesMONO16(index, index + 1);
}


//----------------------------------------------------------------------------------------------------
//							returns 16bit images between start and stop
//@return	std::unique_ptr<unsigned short[]>
inline std::unique_ptr<unsigned short[]> ImageProcessing::DcimgReader::ReadFramesMONO16(size_t start, size_t stop)
{
	if (bits_per_pixel != 16)
	{
		std::cerr << "This method is for 16bit image only!\n";
		return nullptr;
	}

	std::unique_ptr<unsigned char[]> buf = ReadFrames(start, stop);
	return std::unique_ptr<unsigned short[]>(reinterpret_cast<unsigned short*>(buf.release()));
}




#ifdef OPENCV_SUPPORT
//----------------------------------------------------------------------------------------------------
//								returns cv::Mat image of the index.
//								requires to define OPENCV_SUPPORT.
//@param	index
//
//@return	std::optional<cv::Mat>
inline std::optional<cv::Mat> ImageProcessing::DcimgReader::ReadCvFrame(size_t index)
{
	DCIMG_ERR err;

	if (index > nframes_in_currentsession)
	{
		printf("Number of frame in this session is %d.\n", nframes_in_currentsession);
		printf("%zd is out of range!\n", index);
		return std::nullopt;
	}

	// access to frame data
	DCIMG_FRAME	frame;

	memset(&frame, 0, sizeof(frame));
	frame.size = sizeof(frame);
	frame.width = this->width;
	frame.height = this->height;
	frame.rowbytes = this->rowBytes;
	frame.type = static_cast<DCIMG_PIXELTYPE>(this->bits_per_pixel >> 3);
	frame.iFrame = index;

	int type = 0;
	if (frame.type == DCIMG_PIXELTYPE::DCIMG_PIXELTYPE_MONO8)
		type = CV_8UC1;
	if (frame.type == DCIMG_PIXELTYPE::DCIMG_PIXELTYPE_MONO16)
		type = CV_16UC1;

	cv::Mat img(this->height, this->width, type);
	frame.buf = img.data;

	err = dcimg_copyframe(hdcimg.get(), &frame);
	if (failed(err))
	{
		dcimgcon_show_dcimgerr(err, "dcimg_copyframe()", "#%d frame", frame.iFrame);
		return std::nullopt;
	}

	return img;
}



//----------------------------------------------------------------------------------------------------
//							returns cv::Mat images between start and stop.
//							requires to define OPENCV_SUPPORT.
//@param	start
//@param	stop
//
//@return	std::optional<std::vector<cv::Mat>>
inline std::optional<std::vector<cv::Mat>> ImageProcessing::DcimgReader::ReadCvFrames(size_t start, size_t stop)
{
	DCIMG_ERR err;

	if (stop > nframes_in_currentsession)
	{
		printf("Number of frame in this session is %d.\n", nframes_in_currentsession);
		printf("%zd is out of range!\n", stop);
		return std::nullopt;
	}

	if (stop - start < 0)
	{
		std::cerr << "Stop index must be greater than start index.";
		return std::nullopt;
	}

	// access to frame data
	DCIMG_FRAME	frame;

	memset(&frame, 0, sizeof(frame));
	frame.size = sizeof(frame);
	frame.width = this->width;
	frame.height = this->height;
	frame.rowbytes = this->rowBytes;
	frame.type = static_cast<DCIMG_PIXELTYPE>(this->bits_per_pixel >> 3);

	std::vector<cv::Mat> buffers(stop - start);
	int type = 0;
	if (frame.type == DCIMG_PIXELTYPE::DCIMG_PIXELTYPE_MONO8)
		type = CV_8UC1;
	if (frame.type == DCIMG_PIXELTYPE::DCIMG_PIXELTYPE_MONO16)
		type = CV_16UC1;

	for (size_t i = 0; i < stop - start; ++i)
	{
		buffers[i] = cv::Mat(this->height, this->width, type);
		frame.buf = buffers[i].data;
		frame.iFrame = static_cast<int32>(start + i);


		err = dcimg_copyframe(hdcimg.get(), &frame);
		if (failed(err))
		{
			dcimgcon_show_dcimgerr(err, "dcimg_copyframe()", "#%d frame", frame.iFrame);
			return std::nullopt;
		}
	}
	return buffers;
}
#endif // OPENCV_SUPPORT
