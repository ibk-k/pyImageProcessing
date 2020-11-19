#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "DcimgReader.h"

namespace py = pybind11;

class pyDcimgReader
{
private:
	ImageProcessing::DcimgReader reader;

public:
	pyDcimgReader(const std::wstring& filename);
	pyDcimgReader(const std::wstring& filename, int32 sessionidx);
	void SetCurrentSession(int32 sessionidx);
	int32 GetCurrentSession() const;
	int32 GetNumberOfSessions() const;
	int32 GetHeight() const;
	int32 GetWidth() const;
	int32 GetRowBytes() const;
	int32 GetBitsPerPixel() const;
	int32 GetNumberOfFrames() const;
	std::vector<double> GetTimeStamp() const;
	py::array ReadFrame(size_t index);
	py::array ReadFrames(size_t start, size_t stop);
};
