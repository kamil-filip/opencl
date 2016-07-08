#include <iostream>
#include <memory>
#include <HorusCLTypeFactory.h>

#if defined (__APPLE__) || defined (MACOSX)
	#include <OpenCL/cl.hpp>
#else
	#include <CL/cl.hpp>
#endif




int main()
{
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;

	cl::Platform::get(&platforms);
	std::cout<<"Available amount of platforms is " << platforms.size() << "\n";

	for(auto platform : platforms)
	{
		std::string platformName;
		platform.getInfo((cl_platform_info)CL_PLATFORM_NAME,&platformName);
		std::cout<<"Platform : " << platformName << "\n";
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
		break;
	}

	auto d = devices[0];
	std::string deviceName;
	d.getInfo(CL_DEVICE_NAME,&deviceName);
	std::cout<<deviceName<<"\n";

	cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),0};
	cl::Context context (CL_DEVICE_TYPE_GPU, properties);

	std::vector<cl::ImageFormat> image_formats;
	context.getSupportedImageFormats(CL_MEM_READ_ONLY, CL_MEM_OBJECT_IMAGE2D,&image_formats);

	std::shared_ptr<cl::Context> p_ctx = std::shared_ptr<cl::Context>(new cl::Context(CL_DEVICE_TYPE_GPU, properties));

	std::shared_ptr<cl::CommandQueue> p_cmq = std::shared_ptr<cl::CommandQueue>(new cl::CommandQueue(*p_ctx, d)); // cl::CommandQueue queue(*p_ctx, d);

	typedef Horus::Recorder::Backend::Pipeline::Components::OpenCL::HorusCLType::IHorusCLType IHorusCLType;
	typedef Horus::Recorder::Backend::Pipeline::Components::OpenCL::HorusCLType::HorusCLTypeFactory HorusCLTypeFactory;


	std::vector<unsigned char> fakeData(1000);
	std::fill(fakeData.begin(), fakeData.end(),12);

	std::vector<unsigned char> output(1000);

	std::shared_ptr<IHorusCLType> cltype = HorusCLTypeFactory::create(p_ctx,fakeData.size(), CL_MEM_READ_WRITE);

	cltype->write(p_cmq,fakeData);
	cltype->read(p_cmq, output);

	
	

	system("PAUSE");
	return 0;
}