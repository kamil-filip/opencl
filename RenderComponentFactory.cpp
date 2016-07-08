#include "RenderComponentFactory.h"
#undef SendMessage 

using namespace Horus::Recorder::Backend::Pipeline;
using namespace Horus::Recorder::Backend::Pipeline::Components::OpenCL;

std::map<std::string, std::string> RenderComponentFactory::kernelData;

PipelineComponentBase* RenderComponentFactory::CreatePipelineComponent(Pipeline &pipeline, int id, PipelineComponentPropertyCollection const &properties, Horus::Grabbers::Core::Logger &logger)
{	
	return new RenderComponent(pipeline, id, logger, kernelData);
}
