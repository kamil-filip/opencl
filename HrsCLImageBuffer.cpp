#include "HrsCLImageBuffer.h"

namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace HorusCLType {

	HrsCLImageBuffer::HrsCLImageBuffer(std::shared_ptr<cl::Context> ctx, TestProperties properties, size_t numChannels, size_t bufferSize):
		_clContext(ctx), 
		_properties(properties),
		_numChannels(numChannels),
		_bufferSize(bufferSize)
	{
		_clBuffer = std::shared_ptr<cl::Buffer>(new cl::Buffer(*_clContext, _allocFlag, _bufferSize, 0));
	}

	void HrsCLImageBuffer::read( std::shared_ptr<cl::CommandQueue> clCmdQueue, std::vector<unsigned char> &bufferOut)
	{	
		cl_int err = 0;

		err = clCmdQueue->enqueueReadBuffer(*_clBuffer, CL_TRUE, 0, _properties.width() * _properties.height() * _numChannels, bufferOut.data());
	
		if (err != 0)
			std::cout << "Unable to read to the buffer, error code: [ " << err << " ] \n";
	}

	void HrsCLImageBuffer::write(std::shared_ptr<cl::CommandQueue> clCmdQueue, const std::vector<unsigned char> &data)
	{
		/*auto sptr = reinterpret_cast<char*>(&(const_cast<std::string&>(data.)[0]));		
		err = clCmdQueue->enqueueWriteBuffer(*_clBuffer, CL_FALSE, 0, _properties.width() * _properties.height(), static_cast<void*>(sptr));*/
		cl_int err = 0;	
		err = clCmdQueue->enqueueWriteBuffer(*_clBuffer, CL_TRUE, 0,  _properties.width() * _properties.height() * _numChannels, data.data());

		if (err != 0)
			std::cout << "Unable to write to the buffer, error code: [ " <<err << " ] \n";
	}


}}}}}}}
