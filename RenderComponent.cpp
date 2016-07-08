#pragma once



#include "RenderComponent.h"

#include "../Horus.Recorder.Backend.Pipeline/Pipeline.h"
#include "../Horus.Recorder.Backend.Pipeline/PipelineComponentPropertyMetadata.h"
#include "../Horus.Recorder.Backend.Pipeline/PipelineInitializationException.h"
#include <boost/foreach.hpp>

#include <root/BaseType.pb.h>
#include <root/Data.pb.h>
#include <root/Metadata.pb.h>
#include <root/Codec.pb.h>
#include <root/CodecProperties.pb.h>
#include <root/MediaCodecProperties.pb.h>
#include <root/VideoCodecProperties.pb.h>

#include "HorusCLTypes\HorusCLTypeFactory.h"


#define INTMAX_MAX   _I64_MAX
#include <mutex>
#include <regex>
#include <limits.h>
#include <stdio.h>
#include <iomanip>
#include <ctime>
#include <chrono>

#undef SendMessage // windows.h defines SendMessage as either SendMessageA or SendMessageW, messing up our SendMessage() call below

using namespace Horus::SDK::ProtocolBuffers::Data;
using namespace Horus::Recorder::Backend::Pipeline;
using namespace Horus::Recorder::Backend::Pipeline::Components::OpenCL;
using namespace boost::assign;
using namespace Horus::SDK::ProtocolBuffers::Coordinates;

typedef Horus::SDK::ProtocolBuffers::Data::Data DataType;

#define OPENCL_MINIMUM_VERSION 1.2

namespace
{
	template<typename T>
	std::vector<std::string> MakeVector(const T &map)
	{
		std::vector<std::string> result;
		auto iterator = map.cbegin();
		while (iterator != map.cend()) {
			result.push_back(iterator->first);
			iterator++;
		}
		return result;
	}

}

const SizeMap RenderComponent::sc_sizeMap = map_list_of
("hd4320 (7680x4320)", std::make_pair(7680, 4320))
("hd2160 (3840x2160)", std::make_pair(3840, 2160))
("hd1080 (1920x1080)", std::make_pair(1920, 1080))
("hd720 (1280x720)", std::make_pair(1280, 720))
("hd480 (852x480)", std::make_pair(852, 480))
("wuxga (1920x1200)", std::make_pair(1920, 1200))
("sxga (1280x1024)", std::make_pair(1280, 1024))
("uxga (1600x1200)", std::make_pair(1600, 1200))
("xga (1024x768)", std::make_pair(1024, 768))
("svga (800x600)", std::make_pair(800, 600))
("panorama uxhd (8192x4096)", std::make_pair(8192, 4096))
("panorama uhd (4096x2048)", std::make_pair(4096, 2048))
("panorama hd (2048x1024)", std::make_pair(2048, 1024))
("panorama hdr (1024x512)", std::make_pair(1024, 512));


std::vector<std::string>  RenderComponent::createDevicePlatformMap(DeviceMap* deviceMap, PlatformMap* platformMap)
{
	std::vector<cl::Platform> platformList;
	std::string version;
	cl::Platform::get(&platformList);
	std::regex regOpenCLVersion("[Oo][Pp][Ee][Nn][Cc][Ll][[:blank:]]*[[:digit:]]+((\\.)[[:digit:]]+)?");
	for (auto platform : platformList)
	{
		platform.getInfo(CL_PLATFORM_VERSION, &version);
		auto matches_begin = std::sregex_iterator(version.begin(), version.end(), regOpenCLVersion);
		if (matches_begin != std::sregex_iterator())
		{
			std::regex regVersion("[[:digit:]]+((\\.)[[:digit:]]+)?");
			std::string opencl = matches_begin->str();
			matches_begin = std::sregex_iterator(opencl.begin(), opencl.end(), regVersion);
			if (matches_begin != std::sregex_iterator())
			{
				if (std::stod(matches_begin->str()) >= OPENCL_MINIMUM_VERSION)
				{
					std::vector<cl::Device> devices;
					platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

					std::string name;
					for (auto device : devices)
					{
						device.getInfo(CL_DEVICE_NAME, &name);
						//name.erase(std::remove_if(name.begin(), name.end(), std::isblank), name.end());
						name.pop_back();
						name = std::regex_replace(name, std::regex("^ +| +$|( ) +"), "$1");
						std::unique_ptr<cl::Device> device_ptr(new cl::Device(device));
						platformMap->insert(std::make_pair(device_ptr.get(), platform));
						deviceMap->insert(std::make_pair(name, std::move(device_ptr)));
					}
				}
			}
		}
	}
	return MakeVector(*deviceMap);
}

PlatformMap RenderComponent::sc_platformMap = PlatformMap{};
DeviceMap RenderComponent::sc_deviceMap = DeviceMap{};

const Horus::SDK::Configurations::PropertyBase &RenderComponent::sc_sizeProperty =
RenderComponent::RegisterProperty<std::string>("Size", &RenderComponent::getSize, &RenderComponent::setSize,
PipelineComponentPropertyMetadata(PipelineComponentPropertyValueType::STRING, true, "Select the size of the resulting image", MakeVector<SizeMap>(sc_sizeMap)));

const Horus::SDK::Configurations::PropertyBase &RenderComponent::sc_ptoFileProperty =
RenderComponent::RegisterCollectionProperty<std::vector<Horus::SDK::Configurations::PropertyBlobValue>>("Pto File(s)", &RenderComponent::getPtoFiles, &RenderComponent::setPtoFiles,
PipelineComponentPropertyMetadata(PipelineComponentPropertyValueType::BLOB, true, "Select the pto file"));

const Horus::SDK::Configurations::PropertyBase &RenderComponent::sc_streamIndexProperty =
RenderComponent::RegisterProperty<int>("Stream Index", &RenderComponent::getStreamIndex, &RenderComponent::setStreamIndex,
PipelineComponentPropertyMetadata(PipelineComponentPropertyValueType::INTEGER, true, "Set the stream index"));

const Horus::SDK::Configurations::PropertyBase &RenderComponent::sc_framePerSecondProperty =
RenderComponent::RegisterProperty<int>("Frame(s) per second", &RenderComponent::getFramePerSecond, &RenderComponent::setFramePerSecond,
PipelineComponentPropertyMetadata(PipelineComponentPropertyValueType::INTEGER, true, "Set the frame(s) per second"));

const Horus::SDK::Configurations::PropertyBase &RenderComponent::sc_horizontalFlipProperty =
RenderComponent::RegisterProperty<bool>("Horizontal flip", &RenderComponent::getHorizontalFlip, &RenderComponent::setHorizontalFlip,
PipelineComponentPropertyMetadata(PipelineComponentPropertyValueType::BOOLEAN, true, "Set if image needs to be flipped"));

const Horus::SDK::Configurations::PropertyBase &RenderComponent::sc_Device =
RenderComponent::RegisterProperty<std::string>("OpenCL device", &RenderComponent::getDevice, &RenderComponent::setDevice,
PipelineComponentPropertyMetadata(PipelineComponentPropertyValueType::STRING, true, "Select the device", RenderComponent::createDevicePlatformMap(&sc_deviceMap, &sc_platformMap)));



RenderComponent::RenderComponent(Horus::Recorder::Backend::Pipeline::Pipeline &pipeline, int id, Horus::Grabbers::Core::Logger &logger, std::map<std::string, std::string> &kernelMap)
	:
	SendingPipelineComponent(pipeline, id, logger),
	SendingPipelineComponentBase(pipeline, id, logger),
	ReceivingPipelineComponentBase(pipeline, id, logger),
	ReceivingPipelineComponent(pipeline, id, logger),
	PipelineComponentBase(pipeline, id, logger),
	PipelineComponent(pipeline, id, logger),
	BidirectionalPipelineComponent(pipeline, id, logger),
	d_device(sc_deviceMap.size() ? nullptr : sc_deviceMap.begin()->second.get()),
	d_size(sc_sizeMap.begin()->second),
	d_flip(0),
	d_kernelMap(kernelMap)
{

}

RenderComponent::~RenderComponent(void){

}

void RenderComponent::operator()(boost::any const &sender, boost::shared_ptr<Horus::Recorder::Backend::Pipeline::ProtobufMessage> message)
{

	int temp_flip = (b_flip + 1) % 2;
	messages[temp_flip].first.lock();

	b_lock.lock();

	messages[temp_flip].second[message.get()->Message().sourceid()] = message;
	updatePixelFormat(message);

	b_lock.unlock();

	int d_next_flip = ((d_flip + 1) % 2);
	d_flip = d_next_flip;

	messages[temp_flip].first.unlock();
}

void RenderComponent::updatePixelFormat(boost::shared_ptr<Horus::Recorder::Backend::Pipeline::ProtobufMessage> message)
{
	CodecProperties properties = message.get()->Message().mutable_data()->begin()->metadata().codec().properties();
	auto mcprop = properties.MutableExtension(MediaCodecProperties::MediaProperties);
	auto curPixelFormat = mcprop->MutableExtension(VideoCodecProperties::VideoProperties)->pixelformat();
	size_t width = mcprop->MutableExtension(VideoCodecProperties::VideoProperties)->width();
	size_t height = mcprop->MutableExtension(VideoCodecProperties::VideoProperties)->height();

	std::tuple<std::string, PixelFormat, std::pair<size_t, size_t>> inputFormat(message.get()->Message().sourceid(), curPixelFormat, std::pair<size_t, size_t>(width, height));

	auto it_cur = std::find(pixelFormats.begin(), pixelFormats.end(), inputFormat);

	if (it_cur == pixelFormats.end())
	{
		auto it_pos = std::find_if(pixelFormats.begin(), pixelFormats.end(), [](std::tuple<std::string, PixelFormat, std::pair<size_t, size_t>>& element)
		{
			return std::get<1>(element) == VideoCodecProperties_PixelFormatType_PIX_FMT_NONE;
		}
		);
		std::swap(*it_pos, inputFormat);
	//	stop = true;
	}
}

void RenderComponent::Start()
{
	stop = false;
	d_initialized = true;
	d_thread.swap(boost::thread(boost::bind(&RenderComponent::Run, this)));
	d_flip = 0;
	b_flip = 0;



}

void RenderComponent::Stop()
{

	if (stop)
	{
		Start();
	}

}

void RenderComponent::Run()
{
	cl_int err = 0;



	cl::size_t<3> region_input;
	cl::size_t<3> region_output;
	cl::size_t<3> origin;

	std::vector<char> outputVec[2];


	d_input_size.first = 1664;
	d_input_size.second = 1664;


	outputVec[0].resize(d_size.first * d_size.second * 3);
	outputVec[1].resize(d_size.first * d_size.second * 3);

	region_output[0] = d_size.first;
	region_output[1] = d_size.second;
	region_output[2] = 1;

	region_input[0] = d_input_size.first;
	region_input[1] = d_input_size.second;
	region_input[2] = 1;


	//createClBuffers(image2DArray, VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_BGGR8);		
	//createClBuffers(d_clOutputs, VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_BGGR8);

	for (size_t idx = 0; idx != getLocalZ(); ++idx)
	{
	d_clOutputs[idx] = cl::Buffer(d_clContext, CL_MEM_READ_WRITE, d_size.first * d_size.second * 3 * sizeof(unsigned char));
	image2DArray[idx] = cl::Image2DArray(d_clContext, CL_MEM_READ_WRITE, cl::ImageFormat(CL_R, CL_UNSIGNED_INT8), 2, 1664, 1664, 0, 0, 0);
	image2DArrayOut[idx] = cl::Image2DArray(d_clContext, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RGBA, CL_UNSIGNED_INT8), 2, 1664, 1664, 0, 0, 0);
	}


	std::string emptyBayer(d_input_size.first * d_input_size.second, '0');
	std::string emptyRGB(d_input_size.first * d_input_size.second * 3, '0');
	std::string emptyYUV420P(d_input_size.first * d_input_size.second * 1.5, '0');

	std::vector<cl::Buffer> yuvInput(getLocalZ());
	std::vector<cl::Buffer> rgbInput(getLocalZ());

	for (auto it = rgbInput.begin(); it != rgbInput.end(); ++it)
	*it = cl::Buffer(d_clContext, CL_MEM_READ_WRITE, d_input_size.first * d_input_size.second * 3 * sizeof(unsigned char) * getLocalZ());

	for (auto it = yuvInput.begin(); it != yuvInput.end(); ++it)
	*it = cl::Buffer(d_clContext, CL_MEM_READ_WRITE, d_input_size.first * d_input_size.second * 1.5 * sizeof(unsigned char));


	origin[2] = 0;

	std::vector<cl_int> forDebug[2];
	forDebug[0].resize(d_input_size.first * d_input_size.second * 4);
	forDebug[1].resize(d_input_size.first * d_input_size.second * 4);

	int xred = 1;
	int yred = 0;

	err += d_clKernels["MHC_DEMOSAIC"].setArg(0, xred);
	err += d_clKernels["MHC_DEMOSAIC"].setArg(1, yred);
	


	while (!stop)
	{
		messages[b_flip].first.lock();

		size_t rgbCounter = 0;
		size_t yuvCounter = 0;
		size_t bayerCounter = 0;


		auto temp = pixelFormats;

		for (auto it = pixelFormats.begin(); it != pixelFormats.end(); it++)
		{
			switch (std::get<1>(*it)){

			case VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_BGGR8:
			case VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_RGGB8:
			case VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_GBRG8:
			case VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_GRBG8:
			case VideoCodecProperties_PixelFormatType_PIX_FMT_NONE:
			{
				

				char  * source = (messages[b_flip].second[std::get<0>(*it)] == 0) ? (char *)(emptyBayer.c_str()) :
					(char *)(messages[b_flip].second[std::get<0>(*it)].get()->Message().mutable_data()->Get(0).buffer().c_str());

				origin[2] = bayerCounter;
				err += d_clCommandQueue[b_flip].enqueueWriteImage(image2DArray[b_flip], CL_FALSE, origin, region_input, 0, 0, static_cast<void*>(source));
				bayerCounter++;
			}
			break;

			case VideoCodecProperties_PixelFormatType_PIX_FMT_RGB24:
			{
			/*	char  * source = (messages[b_flip].second[std::get<0>(*it)] == 0) ? (char *)(emptyRGB.c_str()) :
					(char *)(messages[b_flip].second[std::get<0>(*it)].get()->Message().mutable_data()->Get(0).buffer().c_str());

				err += d_clCommandQueue[b_flip].enqueueWriteBuffer(rgbInput[b_flip], CL_TRUE, rgbCounter * emptyRGB.size(), emptyRGB.size(), static_cast<void*>(source));
				rgbCounter++;
				*/
			}
			break;

			case VideoCodecProperties_PixelFormatType_PIX_FMT_YUV420P:
			{
				/*char  * source = (messages[b_flip].second[std::get<0>(*it)] == 0) ? (char *)(emptyYUV420P.c_str()) :
					(char *)(messages[b_flip].second[std::get<0>(*it)].get()->Message().mutable_data()->Get(0).buffer().c_str());

				auto tempSize = messages[b_flip].second[std::get<0>(*it)].get()->Message().mutable_data()->Get(0).buffer().size();

				err += d_clCommandQueue[b_flip].enqueueWriteBuffer(yuvInput[yuvCounter], CL_TRUE, 0, emptyRGB.size(), static_cast<void*>(source));
				yuvCounter++;*/

			}
			break;
			}
		}



		//Just for tests
		origin[2] = 1;
		err += d_clCommandQueue[b_flip].enqueueReadImage(image2DArrayOut[b_flip], CL_FALSE, origin, region_input, 0, 0, forDebug[b_flip].data());

		//err += d_clCommandQueue[b_flip].enqueueReadBuffer(d_clOutputs[b_flip], CL_FALSE, 0, outputVec[b_flip].size(), outputVec[b_flip].data(), 0, 0);



		std::vector<std::string> _streamindices;
		_streamindices.push_back(boost::lexical_cast<std::string>(1));
		auto messageBuilder = Horus::Grabbers::Core::MessageBuilder(_streamindices);
		messageBuilder
			.SourceId(this->Name())
			//.AddData(reinterpret_cast<char*>(outputVec[b_flip].data()), outputVec[b_flip].size())
			.AddData(reinterpret_cast<char*>(forDebug[b_flip].data()), forDebug[b_flip].size())
			.AddTime(clock(), Time_TimeSource::Time_TimeSource_Grabber, Time_TimeFormat::Time_TimeFormat_Pts)
			.WithMetadata()
			.AddTimeBase(1, this->getFramePerSecond())
			.WithVideoCodec()
			.SetCodec(MediaCodecProperties_MediaCodecId::MediaCodecProperties_MediaCodecId_CODEC_ID_RAWVIDEO)
			//.SetHeight(d_size.second)
			//.SetWidth(d_size.first)
			.SetHeight(d_input_size.second)
			.SetWidth(d_input_size.first)
			.SetKeyFrame(true)
			.SetPixelFormat(VideoCodecProperties_PixelFormatType::VideoCodecProperties_PixelFormatType_PIX_FMT_BGRA);
		SendMessage(this, boost::shared_ptr<ProtobufMessage>(new ProtobufMessage(messageBuilder())));


		//	BAYER 
		err += d_clKernels["MHC_DEMOSAIC"].setArg(2, image2DArray[(b_flip + 1) % 2]);
		err += d_clKernels["MHC_DEMOSAIC"].setArg(3, image2DArrayOut[(b_flip + 1) % 2]);
		err += d_clCommandQueue[(b_flip + 1) % 2].enqueueNDRangeKernel(d_clKernels["MHC_DEMOSAIC"], cl::NullRange, cl::NDRange(d_input_size.first, d_input_size.second, getLocalZ()), cl::NDRange(32, 32, 1), 0, 0);

		err += d_clKernels["BUILD_IMAGE_2I"].setArg(0, image2DArrayOut[(b_flip + 1) % 2]);
		err += d_clKernels["BUILD_IMAGE_2I"].setArg(3, d_clOutputs[(b_flip + 1) % 2]);
		err += d_clCommandQueue[(b_flip + 1) % 2].enqueueNDRangeKernel(d_clKernels["BUILD_IMAGE_2I"], cl::NullRange, cl::NDRange(d_size.first, d_size.second, getLocalZ()), cl::NDRange(getLocalX(), getLocalY(), getLocalZ()), 0, 0);
		

		messages[b_flip].first.unlock();
		b_flip = (b_flip + 1) % 2;
	}


	Stop();

}


std::vector<unsigned char> RenderComponent::getMasks(std::shared_ptr<horus::setup::Panorama> panoramapto)
{
	std::vector<unsigned char> ptoMasks;

	for (auto it = panoramapto->begin(); it != panoramapto->end(); it++)
	{
		std::vector<unsigned char> maskImage = it->mask()->data();
		ptoMasks.insert(ptoMasks.end(), maskImage.begin(), maskImage.end());
	}
	
	return ptoMasks;
}


std::vector<unsigned short>  RenderComponent::getPanoramaIndexes(std::shared_ptr<horus::setup::Panorama> panoramapto)
{
	std::vector<unsigned short> indexesVec;
	std::map<std::string, horus::setup::util::IndexImageData> maskdata;
	std::unique_ptr<horus::setup::util::PanoramaImageCreator> panoramaImageCreator;
	std::shared_ptr<horus::setup::Size> size = std::shared_ptr<horus::setup::Size>(horus::setup::serialization::SensorSetupBuilder::createSize());

	size->width(d_input_size.first);
	size->height(d_input_size.second);

	panoramaImageCreator.reset(new horus::setup::util::PanoramaImageCreator(*panoramapto.get()));

	for (auto it = panoramapto->begin(); it != panoramapto->end(); it++)
	{
		std::cout << "Loading streamIndexes : " + it->streamIndex() + " \n";

		if (it->mask())
			maskdata.insert(std::pair<std::string, horus::setup::util::IndexImageData>(it->streamIndex(), panoramaImageCreator->createPanoramaImage((*it), it->mask()->size())));

		for (auto _it = maskdata[it->streamIndex()].begin(); _it != maskdata[it->streamIndex()].end(); _it++)
		{
			indexesVec.push_back(_it->x);
			indexesVec.push_back(_it->y);
		}
	}

	return indexesVec;

}


std::map<std::string, std::vector<unsigned short>> RenderComponent::getMaskIndexes(std::shared_ptr<horus::setup::Panorama> panoramapto)
{

	std::unique_ptr<horus::setup::util::PanoramaImageCreator> panoramaImageCreator;
	std::shared_ptr<horus::setup::Size> size = std::shared_ptr<horus::setup::Size>(horus::setup::serialization::SensorSetupBuilder::createSize());
	std::map<std::string, horus::setup::util::IndexImageData> data;
	std::map<std::string, std::vector<unsigned short>> indexesMask;

	size->width(d_input_size.first);
	size->height(d_input_size.second);

	panoramaImageCreator.reset(new horus::setup::util::PanoramaImageCreator(*panoramapto.get()));

	for (auto it = panoramapto->begin(); it != panoramapto->end(); it++)
	{
		std::cout << "Loading streamIndexes : " + it->streamIndex() + " \n";

		data.insert(std::pair<std::string, horus::setup::util::IndexImageData>(it->streamIndex(), panoramaImageCreator->createPanoramaImage((*it), size.get())));

		for (auto _it = data[it->streamIndex()].begin(); _it != data[it->streamIndex()].end(); _it++)
		{
			indexesMask[it->streamIndex()].push_back(_it->x);
			indexesMask[it->streamIndex()].push_back(_it->y);
		}
	}

	return indexesMask;
}

bool RenderComponent::Initialize()
{
	if (!d_device)
		return false;

	cl_context_properties cprops[] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)(sc_platformMap[d_device])(),
		0 };

	d_clContext = std::move(cl::Context(*d_device, cprops));
	d_device->getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroupSize);

	/* TEST CODE*/
	_ctxPtr = std::make_shared<cl::Context>(d_clContext);
	
	

	panoramapto.reset(horus::setup::util::PanoramaPtoParser::parsePtoByFile("E://horus//Recorder//Backend//Source//Horus.Recorder.Backend//Horus.Recorder.Backend.Pipeline.Components.OpenCL//scoop.pto"));
	
	panoramapto->size()->width(d_size.first);
	panoramapto->size()->height(d_size.second);
	
	d_size.first = panoramapto->size()->width();
	d_size.second = panoramapto->size()->height();

	auto temp = panoramapto->size();
	size_t inputsCounter = 2;

	std::vector<unsigned char> masks = getMasks(panoramapto);
	std::vector<unsigned short> indexesVec = getPanoramaIndexes(panoramapto);
	std::map<std::string, std::vector<unsigned short>> indexesMask = getMaskIndexes(panoramapto);

	


	if (maxWorkGroupSize[0] / inputsCounter >= 512){
		setLocalX(32);
		setLocalY(16);
	}
	else{
		setLocalX(16);
		setLocalY(8);
	}

	setLocalZ(inputsCounter);


	/* Compilation Source Code */

	std::string defLocalDimX = "#define LOCALSIZEX " + std::to_string(getLocalX()) + "\n";
	std::string defLocalDimY = "#define LOCALSIZEY " + std::to_string(getLocalY()) + "\n";
	std::string defLocalDimZ = "#define LOCALSIZEZ " + std::to_string(getLocalZ()) + "\n";

	OpenCLKernelSource clsource;
	clsource.buildoptions = "";
	clsource.source = defLocalDimX + defLocalDimY + defLocalDimZ
		+ d_kernelMap["COMMON"]
		+ d_kernelMap["BUILD_MASKS"]
		+ d_kernelMap["BUILD_IMAGE_2I"]
		+ d_kernelMap["MHC_DEMOSAIC"]
		+ d_kernelMap["YUV420P_CONVERSION"]
		+ d_kernelMap["RGB_2_IMAGE"];

	d_clKernelSource["BUILD_MASKS"] = clsource;
	d_clKernelSource["MHC_DEMOSAIC"] = clsource;
	d_clKernelSource["BUILD_IMAGE_2I"] = clsource;
	d_clKernelSource["RGB_TO_IMAGE"] = clsource;

	for (auto it = d_clKernelSource.begin(); it != d_clKernelSource.end(); it++) {
		cl::Program::Sources source(1, std::make_pair(it->second.source.c_str(), it->second.source.length() + 1));
		cl::Program program = cl::Program(d_clContext, source);
		program.build((it->second.buildoptions.length() > 0 ? it->second.buildoptions.c_str() : 0));
		d_clKernels[it->first] = cl::Kernel(program, it->first.c_str());
	}


	for (size_t i = 0; i < getLocalZ(); ++i)
		pixelFormats.push_back(std::tuple<std::string, PixelFormat, std::pair<size_t, size_t>>(std::to_string(i), VideoCodecProperties_PixelFormatType_PIX_FMT_NONE, std::pair<size_t, size_t>(100, 100)));


	/*********************   START FROM HERE   *************/



	//cl::Buffer d_clMask = cl::Buffer(d_clContext, CL_MEM_READ_WRITE, d_size.first * d_size.second * getLocalZ() * sizeof(unsigned char));
	cl::Buffer d_clMasks = cl::Buffer(d_clContext, CL_MEM_READ_WRITE, masks.size());
	cl::Buffer d_clMaskIndexes = cl::Buffer(d_clContext, CL_MEM_READ_WRITE, indexesVec.size() * sizeof(unsigned short));

	cl_int err = 0;
	std::vector<cl::Event> events;
	cl::Event wbIndexes, wbEventMasks, exKern;


	for (size_t idx = 0; idx != getLocalZ(); ++idx)
	{
		d_clCommandQueue[idx] = cl::CommandQueue(d_clContext, *d_device);
	}



	err += d_clCommandQueue[0].enqueueWriteBuffer(d_clMasks, CL_TRUE, 0, masks.size(), static_cast<void*>(masks.data()), &events, &wbEventMasks);
	events.clear();
	events.push_back(wbEventMasks);




	cl::Buffer d_clMask = cl::Buffer(d_clContext, CL_MEM_READ_WRITE, d_size.first * d_size.second * getLocalZ() * sizeof(unsigned char));

	std::shared_ptr<HorusCLType::IHorusCLType> cltype = 
		HorusCLType::HorusCLTypeFactory::create(_ctxPtr, d_size.first * d_size.second * getLocalZ() * sizeof(unsigned char), CL_MEM_READ_WRITE);




	err += d_clCommandQueue[0].enqueueWriteBuffer(d_clMaskIndexes, CL_TRUE, 0, indexesVec.size() * sizeof(unsigned short), static_cast<void*>(indexesVec.data()), &events, &wbIndexes);
	events.clear();
	events.push_back(wbIndexes);


	int maskWidth = panoramapto.get()->begin()->mask()->size()->width();

	


	err += d_clKernels["BUILD_MASKS"].setArg(0, d_clMasks);
	err += d_clKernels["BUILD_MASKS"].setArg(1, d_clMask);
	err += d_clKernels["BUILD_MASKS"].setArg(2, d_clMaskIndexes);
	err += d_clKernels["BUILD_MASKS"].setArg(3, maskWidth);
	err += d_clCommandQueue[0].enqueueNDRangeKernel(d_clKernels["BUILD_MASKS"], cl::NullRange, cl::NDRange(panoramapto->size()->width(), panoramapto->size()->height(), getLocalZ()), cl::NDRange(1, 1, getLocalZ()), NULL, &exKern);
	events.clear();
	events.push_back(exKern);
	cl::WaitForEvents(events);

	std::vector<unsigned char> outMasks(d_size.first * d_size.second * getLocalZ());
	std::fill(outMasks.begin(), outMasks.end(), 0);
	err += d_clCommandQueue[0].enqueueReadBuffer(d_clMask, CL_TRUE, 0, outMasks.size(), outMasks.data(), 0, 0);

	std::vector<unsigned char> mask0(&outMasks[0], &outMasks[outMasks.size() / 2 - 1]);
	std::vector<unsigned char> mask1(&outMasks[outMasks.size() / 2 - 1], &outMasks[outMasks.size() - 1]);

	masks2DArray = cl::Image2DArray(d_clContext, CL_MEM_READ_WRITE, cl::ImageFormat(CL_R, CL_UNSIGNED_INT8), 2, d_size.first, d_size.second, 0, 0, 0);
	indexes2DArray = cl::Image2DArray(d_clContext, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RG, CL_UNSIGNED_INT16), 2, d_size.first, d_size.second, 0, 0, 0);

	std::vector<unsigned char> imageBuffer(d_input_size.first * d_input_size.second);
	std::vector<unsigned char> imageBuffer2(d_input_size.first * d_input_size.second * 3);


	cl::size_t<3> region_input;
	cl::size_t<3> region_output;
	cl::size_t<3> origin;

	std::vector<char> outputVec[2];
	outputVec[0].resize(d_size.first * d_size.second * 3);
	outputVec[1].resize(d_size.first * d_size.second * 3);

	region_output[0] = d_size.first;
	region_output[1] = d_size.second;
	region_output[2] = 1;

	region_input[0] = d_input_size.first;
	region_input[1] = d_input_size.second;
	region_input[2] = 1;





	origin[2] = 0;
	err += d_clCommandQueue[0].enqueueWriteImage(masks2DArray, CL_FALSE, origin, region_output, 0, 0, static_cast<void*>(mask0.data()));
	origin[2] = 1;
	err += d_clCommandQueue[0].enqueueWriteImage(masks2DArray, CL_FALSE, origin, region_output, 0, 0, static_cast<void*>(mask1.data()));
	origin[2] = 0;
	err += d_clCommandQueue[0].enqueueWriteImage(indexes2DArray, CL_FALSE, origin, region_output, 0, 0, static_cast<void*>(indexesMask["0"].data()));
	origin[2] = 1;
	err += d_clCommandQueue[0].enqueueWriteImage(indexes2DArray, CL_FALSE, origin, region_output, 0, 0, static_cast<void*>(indexesMask["1"].data()));


	err += d_clKernels["BUILD_IMAGE_2I"].setArg(1, masks2DArray);
	err += d_clKernels["BUILD_IMAGE_2I"].setArg(2, indexes2DArray);


	d_clCommandQueue[0].flush();
	d_clCommandQueue[1].finish();


	return true;
}


