#ifndef HORUS_REC_BACKEND_PIP_COM_OPENCL_CL_TYPES__
#define HORUS_REC_BACKEND_PIP_COM_OPENCL_CL_TYPES__
#if _DEBUG
#define __CL_ENABLE_EXCEPTIONS 
#endif

#include <iostream>
#include <memory>
#include <CL/cl.hpp>


namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace HorusCLType {



class FakeProperties{

	size_t _width;
	size_t _height;


	public:
		int width() {return _width;}
		int height() {return _height;}
};


class IHorusCLType
{
protected:
	std::shared_ptr<cl::Context> _baseCtx;

public:
	enum NativeType { CL_BUF, CL_IMG_BUF, CL_IMG, CL_IMG_ARRAY_2D, CL_BUF_ARRAY, UNKOWN  };
	enum State {OKE, UNKNOWN_STATE, INVALID_SETTING, NO_CONTEXT};

	virtual void write(std::shared_ptr<cl::CommandQueue> clCmdQueue, const std::vector<unsigned char> &data) = 0;
	virtual void read(std::shared_ptr<cl::CommandQueue> clCmdQueue, std::vector<unsigned char> &bufferOut) = 0;	
	virtual void set(std::shared_ptr<cl::Context> ctx) { _baseCtx = ctx; }
	
	virtual std::shared_ptr<cl::Buffer> getBuffer() = 0;
	virtual State initialize() { return UNKNOWN_STATE; }
	virtual NativeType Type() { return UNKOWN; }
};

}}}}}}}
#endif