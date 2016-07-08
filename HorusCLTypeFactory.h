#ifndef HORUS_REC_BACKEND_PIP_COM_OPENCL_CL_TYPE_FACTORY__
#define HORUS_REC_BACKEND_PIP_COM_OPENCL_CL_TYPE_FACTORY__


#include <memory>
#include "IHorusCLType.h"
//#include <root/VideoCodecProperties.pb.h>

namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace HorusCLType {


class HorusCLTypeFactory
{	

public:
	static std::shared_ptr<IHorusCLType> create(std::shared_ptr<cl::Context> ctx);
	static std::shared_ptr<IHorusCLType> create(std::shared_ptr<cl::Context> ctx, size_t bufferSize, cl_mem_flags allocFlag);
	static std::shared_ptr<IHorusCLType> create(const Horus::SDK::ProtocolBuffers::Data::VideoCodecProperties& properties, std::shared_ptr<cl::Context> ctx);
	static std::shared_ptr<IHorusCLType> create(const Horus::SDK::ProtocolBuffers::Data::Data& data,  std::shared_ptr<cl::Context> ctx);
};

}}}}}}}
#endif