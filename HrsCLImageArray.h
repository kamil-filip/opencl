#ifndef HORUS_REC_BACKEND_PIP_COM_OPENCL_HRS_CL_IMAGEARRAY__
#define HORUS_REC_BACKEND_PIP_COM_OPENCL_HRS_CL_IMAGEARRAY__

#include "IHorusCLType.h"


namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace HorusCLType {

	class HrsCLImageArray : public IHorusCLType
	{
		private:
			size_t _arraySize;
			cl::size_t<3> origin;
			cl::size_t<3> regionIn;

			std::shared_ptr<cl::CommandQueue> _clCmdQueue;
			std::shared_ptr<cl::Context> _clContext;
			std::shared_ptr<cl::Image2DArray> _clImage2DArray;
			Horus::SDK::ProtocolBuffers::Data::VideoCodecProperties _properties;

		public:

			HrsCLImageArray::HrsCLImageArray(std::shared_ptr<cl::Context> clContex, Horus::SDK::ProtocolBuffers::Data::VideoCodecProperties properties, size_t arraySize) :
				_clContext(clContex),
				_properties(properties),
				_arraySize(arraySize)
			{
				_clImage2DArray(std::shared_ptr<cl::Image2D>(new cl::Image2D(*_clContext, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RGBA, CL_UNSIGNED_INT8))))
				_arraySize, _properties.width(), _properties.height(), 0, 0));
				regionIn[0] = _properties.width();
				regionIn[1] = _properties.height();
			}

			virtual NativeType Type() { return CL_IMG_ARRAY_2D; }

			virtual void write(std::vector<const Horus::SDK::ProtocolBuffers::Data::Data>& dataVec, std::shared_ptr<cl::CommandQueue> clCmdQueue)
			{
				size_t arrayIndex = 0;

				for (auto data : dataVec)
				{
					origin[2] = arrayIndex;
					clCmdQueue->enqueueWriteImage(*_clImage2DArray, CL_TRUE, origin, regionIn, 0, 0, static_cast<void*>(const_cast<char*>(data.buffer().c_str())));
				}
				origin[2] = 0;
			}

			virtual void read(std::shared_ptr<cl::CommandQueue> clCmdQueue, std::vector<unsigned char> &bufferOut){
				cl_int err = 0;
				
				origin[2] = 0;
				err = clCmdQueue->enqueueReadImage(*_clImage2DArray,CL_TRUE,origin,regionIn,0,0,bufferOut.data());
				
				if(err!=0)
					std::cout << "Unable to read the buffer, error code: [ " <<err << " ] \n";
			}


			virtual State initialize()
			{
				if (_clContext)
				{
					return State::OKE;
				}
				else
				{
					return State::NO_CONTEXT;
				}
			}

	};
}}}}}}}
#endif