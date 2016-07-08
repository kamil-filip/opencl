#include <iostream>
#include "HorusCLTypeFactory.h"
#include "HrsCLImageBuffer.h"
#include "HrsCLBuffer.h"
#include "HrsCLImageArray.h"
#include <root/MediaCodecProperties.pb.h>
#include <root/VideoCodecProperties.pb.h>

using namespace Horus::SDK::ProtocolBuffers::Data;

namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {
namespace HorusCLType {


		/*
			Creates an empty/default IHorusCLType. This is mostely used
			during initialization of the main component. More specific creations can
			be done when more info is known on the type of image.
		*/

		std::shared_ptr<IHorusCLType> HorusCLTypeFactory::create(std::shared_ptr<cl::Context> ctx)
		{
			std::shared_ptr<IHorusCLType> component;
			return component;
		}

		std::shared_ptr<IHorusCLType>  HorusCLTypeFactory::create(std::shared_ptr<cl::Context> ctx, size_t dataSize, cl_mem_flags allocFlag)
		{
			std::shared_ptr<IHorusCLType> component = std::shared_ptr<IHorusCLType>(new HrsCLBuffer(ctx, dataSize, allocFlag));
			return component;
		}


		/*
		Create based on the VideoCodecProperties (mostly use this approach)
		*/
		
		std::shared_ptr<IHorusCLType> create(const Horus::SDK::ProtocolBuffers::Data::VideoCodecProperties& properties, std::shared_ptr<cl::Context> ctx)
		{
			std::shared_ptr<IHorusCLType> component;
			if (!ctx)
			{
				std::cout << " The provided context was NULL" << std::endl;
				return component;
			}

			if (!properties.has_pixelformat() || !properties.has_height() || !properties.has_width())
			{
				std::cout << "The VideoCodecProperties does not have the required information (width/height/pixelformat)" << std::endl;
				return component;
			}

			switch (properties.pixelformat())
			{
			case VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_BGGR8:
			case VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_RGGB8:
			case VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_GBRG8:
			case VideoCodecProperties_PixelFormatType_PIX_FMT_BAYER_GRBG8:
			case VideoCodecProperties_PixelFormatType_PIX_FMT_NONE:
			{

			}
			break;

			case VideoCodecProperties_PixelFormatType_PIX_FMT_RGB24:
			{
				component = std::shared_ptr<IHorusCLType>(new HrsCLBuffer(ctx, properties, 3));
			}
			break;

			case VideoCodecProperties_PixelFormatType_PIX_FMT_YUV420P:
			{

			}
			break;
			}
			// whenever a IHorusCLType is succesfully created provide the context(OpenCL)
			if (component)
			{
				component->set(ctx);
				if (component->initialize() == IHorusCLType::OKE)
					return component;
				else
					component = std::shared_ptr<IHorusCLType>();
			}
			return component;
		}
		

		/*
			Creates a std::shared_ptr<IHorusCLType> based on the Horus::SDK::ProtocolBuffers::Data::Data
			*/
		
		std::shared_ptr<IHorusCLType>  create(const Horus::SDK::ProtocolBuffers::Data::Data& data, std::shared_ptr<cl::Context> ctx)
		{
			std::shared_ptr<IHorusCLType> component;
			if (!ctx)
			{
				std::cout << " The provided context was NULL" << std::endl;
				return component;
			}

			if (data.has_metadata() && data.metadata().has_codec() && data.metadata().codec().has_properties())
			{
				CodecProperties properties = data.metadata().codec().properties();
				if (properties.HasExtension(MediaCodecProperties::MediaProperties))
				{
					auto mcprop = properties.MutableExtension(MediaCodecProperties::MediaProperties);
					if (mcprop->HasExtension(VideoCodecProperties::VideoProperties))
					{
						auto videoProperties = mcprop->GetExtension(VideoCodecProperties::VideoProperties);
						return create(videoProperties, ctx);
					}
					else
						std::cout << "MediaCodec does not contain any VideoCodecProperties" << std::endl;

				}
				else
					std::cout << "Data does not contain any MediaCodecProperties" << std::endl;
			}
			else
				std::cout << "Data does not contain any CodecProperties" << std::endl;
			return component;
		}


}}}}}}}

