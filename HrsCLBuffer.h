#ifndef HORUS_REC_BACKEND_PIP_COM_OPENCL_HRS_CL_RAW_BUFFER__
#define HORUS_REC_BACKEND_PIP_COM_OPENCL_HRS_CL_RAW_BUFFER__

#include "IHorusCLType.h"

namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace HorusCLType {

	class HrsCLBuffer : public IHorusCLType
	{
		private:
			size_t _bufferSize;		
			cl_mem_flags _allocFlag;
			std::shared_ptr<cl::Context> _clContext;
			std::shared_ptr<cl::Buffer> _clBuffer;

		public:
							
			HrsCLBuffer::HrsCLBuffer(std::shared_ptr<cl::Context> clContext, size_t bufferSize, cl_mem_flags allocFlag);

			virtual void write(std::shared_ptr<cl::CommandQueue> clCmdQueue, const std::vector<unsigned char> &data);
			virtual void read(std::shared_ptr<cl::CommandQueue> clCmdQueue, std::vector<unsigned char> &bufferOut);
			virtual std::shared_ptr<cl::Buffer> getBuffer(){ return _clBuffer;}
			virtual NativeType Type() { return NativeType::CL_BUF; }
		};

}}}}}}}
#endif