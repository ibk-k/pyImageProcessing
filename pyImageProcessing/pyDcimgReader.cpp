#include "pyDcimgReader.h"

pyDcimgReader::pyDcimgReader(const std::wstring& filename) : reader(filename)
{
}

pyDcimgReader::pyDcimgReader(const std::wstring& filename, int32 sessionidx) : reader(filename, sessionidx)
{
}

void pyDcimgReader::SetCurrentSession(int32 sessionidx)
{
	reader.SetCurrentSession(sessionidx);
}

int32 pyDcimgReader::GetCurrentSession() const
{
	return reader.GetCurrentSession();
}

int32 pyDcimgReader::GetNumberOfSessions() const
{
	return reader.GetNumberOfSessions();
}

int32 pyDcimgReader::GetHeight() const
{
	return reader.GetHeight();
}

int32 pyDcimgReader::GetWidth() const
{
	return reader.GetWidth();
}

int32 pyDcimgReader::GetRowBytes() const
{
	return reader.GetRowBytes();
}

int32 pyDcimgReader::GetBitsPerPixel() const
{
	return reader.GetBitsPerPixel();
}

int32 pyDcimgReader::GetNumberOfFrames() const
{
	return reader.GetNumberOfFrames();
}

std::vector<double> pyDcimgReader::GetTimeStamp() const
{
	auto result = reader.GetTimeStamp();
	if (result)
	{
		return *result;
	}
	else
	{
		throw std::runtime_error("Failed to get timestamps.");
	}
}

py::array pyDcimgReader::ReadFrame(size_t index)
{
	py::array::ShapeContainer shape{ reader.GetHeight(), reader.GetWidth() };
	py::array::StridesContainer strides{ reader.GetRowBytes(), reader.GetBitsPerPixel() >> 3 };

	if (reader.GetBitsPerPixel() == 8)	//8bit Image
	{
		auto cpp_img = reader.ReadFrame(index);
		py::array_t<unsigned char> ret(shape, strides, cpp_img.get());

		return ret;
	}
	else if (reader.GetBitsPerPixel() == 16)	//16bit Image
	{
		auto cpp_img = reader.ReadFrameMONO16(index);
		py::array_t<unsigned short> ret(shape, strides, cpp_img.get());

		return ret;
	}

	py::array ret;
	return ret;
}

py::array pyDcimgReader::ReadFrames(size_t start, size_t stop)
{
	int32 nframes = stop - start;
	py::array::ShapeContainer shape{ nframes, reader.GetHeight(), reader.GetWidth() };
	py::array::StridesContainer strides{ reader.GetRowBytes() * reader.GetHeight(), reader.GetRowBytes(), reader.GetBitsPerPixel() >> 3 };

	if (reader.GetBitsPerPixel() == 8)	//8bit Image
	{
		auto cpp_img = reader.ReadFrames(start, stop);
		py::array_t<unsigned char> ret(shape, strides, cpp_img.get());

		return ret;
	}
	else if (reader.GetBitsPerPixel() == 16)	//16bit Image
	{
		auto cpp_img = reader.ReadFramesMONO16(start, stop);
		py::array_t<unsigned short> ret(shape, strides, cpp_img.get());

		return ret;
	}

	py::array ret;
	return ret;
}

PYBIND11_MODULE(pyImageProcessing, m)
{
	m.doc() = "Python Binding of C++ ImageProcessing Library";
	py::class_<pyDcimgReader>(m, "DcimgReader")
		.def(py::init<const std::wstring&>())
		.def(py::init<const std::wstring&, int32>())
		.def("get_timestamp", &pyDcimgReader::GetTimeStamp)
		.def("read_frame", &pyDcimgReader::ReadFrame)
		.def("read_frames", &pyDcimgReader::ReadFrames)
		.def_property("current_session", &pyDcimgReader::GetCurrentSession, &pyDcimgReader::SetCurrentSession)
		.def_property_readonly("number_of_sessions", &pyDcimgReader::GetNumberOfSessions)
		.def_property_readonly("height", &pyDcimgReader::GetHeight)
		.def_property_readonly("width", &pyDcimgReader::GetWidth)
		.def_property_readonly("row_bytes", &pyDcimgReader::GetRowBytes)
		.def_property_readonly("bits_per_pixel", &pyDcimgReader::GetBitsPerPixel)
		.def_property_readonly("number_of_frames", &pyDcimgReader::GetNumberOfFrames);
}
