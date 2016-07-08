#pragma once
#ifndef _HORUS_RECORDER_BACKEND_PIPELINE_COMPONENTS_OPENCL_RENDERCOMPONENT_H_
#define _HORUS_RECORDER_BACKEND_PIPELINE_COMPONENTS_OPENCL_RENDERCOMPONENT_H_

#include <boost/foreach.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp> 
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/container/generation/make_set.hpp>
#include <boost/fusion/include/make_set.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/scoped_ptr.hpp>

#include <root/BaseType.pb.h>
#include <root/Data.pb.h>
#include <root/Metadata.pb.h>
#include <root/Codec.pb.h>
#include <root/CodecProperties.pb.h>
#include <root/MediaCodecProperties.pb.h>
#include <root/VideoCodecProperties.pb.h>

#include <map>
#include <set>
#include <algorithm>
#include <vector>
#include <string>

#include "entity/include/horus/setup/Common.h"
#include "horus/setup/util/PanoramaMeshCreator.h"
#include "horus/setup/util/PanoramaImageCreator.h"
#include "horus/setup/util/PanoramaPtoParser.h"
#include "horus/setup/serialization/SensorSetupBuilder.h"

#include "../Horus.Recorder.Backend.Pipeline/ReceivingPipelineComponent.h"
#include "../Horus.Recorder.Backend.Pipeline/SendingPipelineComponent.h"
#include "../Horus.Recorder.Backend.Pipeline/BidirectionalPipelineComponent.h"

#include <osgViewer/Viewer>
#include <osg/GraphicsContext>

#include <atomic>

//OpenCL stuff
#if _DEBUG
#define __CL_ENABLE_EXCEPTIONS 
#endif
#include <CL/cl.hpp>
#include <memory>


namespace Horus {
	namespace Recorder {
		namespace Backend {
			namespace Pipeline {
				namespace Components {
					namespace OpenCL {

						struct OpenCLKernelSource{
							std::string source;
							std::string buildoptions;
						};

						typedef std::map<std::string, std::pair<int, int>> SizeMap;
						typedef std::map<std::string, std::unique_ptr<cl::Device>> DeviceMap;
						typedef std::map<cl::Device*, cl::Platform> PlatformMap;
						typedef Horus::SDK::ProtocolBuffers::Data::VideoCodecProperties::PixelFormatType PixelFormat;
						typedef boost::shared_ptr<Horus::Recorder::Backend::Pipeline::ProtobufMessage> ProtobufMessagePtr;


						typedef std::map<std::string, cl::Kernel> KernelMap;
						typedef std::map<std::string, OpenCLKernelSource> SourceMap;

						class RenderComponent : public virtual BidirectionalPipelineComponent < RenderComponent > {

							//size map
							static const SizeMap sc_sizeMap;
							static PlatformMap sc_platformMap;
							static DeviceMap sc_deviceMap;


							std::shared_ptr<horus::setup::Panorama> panoramapto;
							std::unique_ptr<horus::setup::util::PanoramaImageCreator> panoramaImageCreator;
							std::vector<Horus::SDK::Configurations::PropertyBlobValue> d_ptoFiles;
							std::vector<std::string> inputChannelsIds;
							void updatePixelFormat(boost::shared_ptr<Horus::Recorder::Backend::Pipeline::ProtobufMessage> message);

							std::vector<std::tuple<std::string, PixelFormat, std::pair<size_t, size_t>>> pixelFormats;


							std::vector<size_t> maxWorkGroupSize;
							std::map<int, std::pair<boost::shared_mutex, std::map<std::string, ProtobufMessagePtr>>> messages;


							std::vector<unsigned char> getMasks(std::shared_ptr<horus::setup::Panorama> panoramapto);
							std::map<std::string, std::vector<unsigned short>> getMaskIndexes(std::shared_ptr<horus::setup::Panorama> panoramapto);
							std::vector<unsigned short> getPanoramaIndexes(std::shared_ptr<horus::setup::Panorama> panoramapto);
							

							size_t d_local_x, d_local_y, d_local_z;
							int d_fps, d_flip, d_streamIndex;
							std::atomic<int> b_flip;
							std::atomic<bool>  stop;
							bool d_horizontalflip, d_initialized;


							std::string d_sizeString, d_deviceString;
							std::pair<int, int> d_size, d_input_size;


							cl::Device *d_device = nullptr;
							cl::Context d_clContext;
							cl::CommandQueue d_clCommandQueue[2];
							cl::Buffer d_clOutputs[2];
							cl::Image2D d_clOutputImage;
							cl::Image2DArray image2DArray[2];
							cl::Image2DArray image2DArrayOut[2];
							cl::Image2DArray masks2DArray;
							cl::Image2DArray indexes2DArray;
							//cl::Buffer d_clMasks;
							//cl::Buffer d_clMaskIndexes;

							std::shared_ptr<cl::Context> _ctxPtr;



							SourceMap d_clKernelSource;
							KernelMap d_clKernels;

							boost::thread d_thread;
							boost::shared_mutex b_lock;

							std::map<std::string, std::string> d_kernelMap;

						public:
							static const Horus::SDK::Configurations::PropertyBase &sc_sizeProperty;
							static const Horus::SDK::Configurations::PropertyBase &sc_ptoFileProperty;
							static const Horus::SDK::Configurations::PropertyBase &sc_streamIndexProperty;
							static const Horus::SDK::Configurations::PropertyBase &sc_framePerSecondProperty;
							static const Horus::SDK::Configurations::PropertyBase &sc_horizontalFlipProperty;

							static const Horus::SDK::Configurations::PropertyBase &sc_Device;

							RenderComponent(Pipeline &pipeline, int id, Horus::Grabbers::Core::Logger &logger, std::map<std::string, std::string> &kernelMap);
							~RenderComponent(void);
							virtual void operator()(boost::any const &sender, boost::shared_ptr<ProtobufMessage> data);


							virtual void Start();
							virtual void Stop();
							virtual bool Initialize();

							void Run();

						private:


							std::vector<std::string> kernelMap2;

							void size(const std::pair<int, int>& value);
							const std::pair<int, int>& size();

							size_t const &getLocalX() const;
							void setLocalX(size_t const &value);

							size_t const &getLocalY() const;
							void setLocalY(size_t const &value);

							size_t const &getLocalZ() const;
							void setLocalZ(size_t const &value);


							std::string const &getSize() const;
							void setSize(std::string const &value);

							std::vector<Horus::SDK::Configurations::PropertyBlobValue> const &getPtoFiles() const;
							void setPtoFiles(std::vector<Horus::SDK::Configurations::PropertyBlobValue> const &value);

							int getStreamIndex() const;
							void setStreamIndex(int value);

							int getFramePerSecond() const;
							void setFramePerSecond(int value);

							bool getHorizontalFlip() const;
							void setHorizontalFlip(bool value);

							std::string const &getDevice() const;
							void setDevice(std::string const &value);

							static std::vector<std::string> createDevicePlatformMap(DeviceMap *deviceMap, PlatformMap *platformMap);


						};
					}
				}
			}
		}
	}
}

#endif

