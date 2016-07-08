#ifndef _HORUS_RECORDER_BACKEND_PIPELINE_COMPONENTS_OPENCL_RENDERCOMPONENTFACTORY_H_
#define _HORUS_RECORDER_BACKEND_PIPELINE_COMPONENTS_OPENCL_RENDERCOMPONENTFACTORY_H_

#include "../Horus.Recorder.Backend.Pipeline/PipelineComponentFactory.h"
#include "RenderComponent.h"

#include <boost/ptr_container/ptr_map.hpp>
#include <string>

namespace Horus {
	namespace Recorder {
		namespace Backend {
			namespace Pipeline {
				namespace Components {
					namespace OpenCL {

						class RenderComponentFactory :public PipelineComponentFactory<RenderComponent>
						{
						private:
					

						public:
							virtual PipelineComponentBase *CreatePipelineComponent(Pipeline &pipeline, int id, PipelineComponentPropertyCollection const &properties, Horus::Grabbers::Core::Logger &logger);
							virtual std::string Name() const { return "OpenCL Renderer Factory"; };
							virtual std::string DisplayName() const { return "OpenCL Renderer"; };
							virtual PipelineComponentType ComponentType() const { return PipelineComponentType::BOTH; }

							static std::map<std::string, std::string> kernelData;
						};
					}
				}
			}
		}
	}
}
#endif

