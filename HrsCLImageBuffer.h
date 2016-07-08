#ifndef HORUS_REC_BACKEND_PIP_COM_OPENCL_HRS_CL_BUFFER__
#define HORUS_REC_BACKEND_PIP_COM_OPENCL_HRS_CL_BUFFER__

#include "IHorusCLType.h"

namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace HorusCLType {



class HrsCLImageBuffer : public IHorusCLType
{
private:
	typedef FakeProperties TestProperties;
	cl_mem_flags _allocFlag;
	size_t _numChannels;
	size_t _bufferSize;	
	std::shared_ptr<cl::Context> _clContext;
	std::shared_ptr<cl::Buffer> _clBuffer;
	TestProperties _properties;

	//Horus::SDK::ProtocolBuffers::Data::VideoCodecProperties _properties;

public:	

	HrsCLImageBuffer(std::shared_ptr<cl::Context> ctx, TestProperties properties, size_t numChannels, size_t bufferSize);

	virtual NativeType Type() { return NativeType::CL_IMG_BUF; }

	virtual void read( std::shared_ptr<cl::CommandQueue> clCmdQueue, std::vector<unsigned char> &bufferOut);
	virtual void write(std::shared_ptr<cl::CommandQueue> clCmdQueue, const std::vector<unsigned char> &data);

	virtual State initialize()
	{
		if (_clContext)
		{		
		}
		else
		{
			return State::NO_CONTEXT;
		}
		return State::OKE;
	}
};

}}}}}}}
#endif