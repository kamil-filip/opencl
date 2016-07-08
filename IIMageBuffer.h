#ifndef HORUS_REC_BACKEND_PIP_COM_OPENCL_IIMG_BUFF__
#define HORUS_REC_BACKEND_PIP_COM_OPENCL_IIMG_BUFF__

#include "IHorusCLType.h"
#include <memory>

namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace ImageBuffers {

class IImageBuffer
{
	virtual void read(std::shared_ptr<HorusCLType::IHorusCLType> type) = 0;
	virtual std::shared_ptr<HorusCLType::IHorusCLType> get() = 0;
};

}}}}}}}
#endif