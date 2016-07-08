#include "HrsCLBuffer.h"

namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace HorusCLType {

	HrsCLBuffer::HrsCLBuffer(std::shared_ptr<cl::Context> clContext, size_t bufferSize, cl_mem_flags allocFlag) :
	_clContext(clContext),
	_bufferSize(bufferSize),
	_allocFlag(allocFlag)						
	{
		_clBuffer = std::shared_ptr<cl::Buffer>(new cl::Buffer(*_clContext, _allocFlag, _bufferSize, 0));
	}

	void HrsCLBuffer::write(std::shared_ptr<cl::CommandQueue> clCmdQueue, const std::vector<unsigned char> &data)
	{
		cl_int err = 0;
		err = clCmdQueue->enqueueWriteBuffer(*_clBuffer, CL_TRUE, 0, _bufferSize, data.data());

		if (err != 0)
			std::cout << "Writing to the buffer failed with error code: [ " << err << " ] \n";
	}

	void HrsCLBuffer::read(std::shared_ptr<cl::CommandQueue> clCmdQueue, std::vector<unsigned char> &bufferOut)
	{
		cl_int err = 0;
		err = clCmdQueue->enqueueReadBuffer(*_clBuffer, CL_TRUE, 0, _bufferSize, bufferOut.data());

		if (err != 0)
			std::cout << "Reading the buffer failed with error code: [ " << err << " ] \n";
	}


}}}}}}}