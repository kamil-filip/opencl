#include "RenderComponent.h"

using namespace Horus::SDK::ProtocolBuffers::Data;
using namespace Horus::Recorder::Backend::Pipeline;
using namespace boost::assign;

namespace Horus {
namespace Recorder {
namespace Backend {
namespace Pipeline {
namespace Components {
namespace OpenCL {

	void RenderComponent::size(const std::pair<int, int>& value)
	{
		d_size = value;
	}

	const std::pair<int, int>& RenderComponent::size()
	{
		return d_size;
	}

	std::string const &RenderComponent::getSize() const
	{
		return d_sizeString;
	}

	void RenderComponent::setSize(std::string const &value)
	{
		SizeMap::const_iterator it = sc_sizeMap.find(value);
		if (it != sc_sizeMap.end()) {
			d_size = it->second;
			d_sizeString = it->first;
		}
	}

	std::vector<Horus::SDK::Configurations::PropertyBlobValue> const &RenderComponent::getPtoFiles() const
	{
		return d_ptoFiles;
	}

	void RenderComponent::setPtoFiles(std::vector<Horus::SDK::Configurations::PropertyBlobValue> const &value)
	{
		d_ptoFiles = value;
	}

	int RenderComponent::getStreamIndex() const
	{
		return d_streamIndex;
	}

	void RenderComponent::setStreamIndex(int value)
	{
		d_streamIndex = value;
	}

	int RenderComponent::getFramePerSecond() const
	{
		return d_fps;
	}

	void RenderComponent::setFramePerSecond(int value)
	{
		d_fps = value;
	}

	bool RenderComponent::getHorizontalFlip() const
	{
		return d_horizontalflip;
	}

	void RenderComponent::setHorizontalFlip(bool value)
	{
		d_horizontalflip = value;
	}

	std::string const &RenderComponent::getDevice() const
	{
		return d_deviceString;
	}

	size_t const &RenderComponent::getLocalX() const
	{
		return d_local_x;
	}

	void  RenderComponent::setLocalX(size_t const &value)
	{
		d_local_x = value;
	}

	size_t const &RenderComponent::getLocalY() const
	{
		return d_local_y;
	}

	void  RenderComponent::setLocalY(size_t const &value)
	{
		d_local_y = value;
	}

	size_t const &RenderComponent::getLocalZ() const
	{
		return d_local_z;
	}

	void  RenderComponent::setLocalZ(size_t const &value)
	{
		d_local_z = value;
	}

	void RenderComponent::setDevice(std::string const &value)
	{
		DeviceMap::const_iterator it = sc_deviceMap.find(value);
		if (it != sc_deviceMap.end()) {
			d_device = it->second.get();
			d_deviceString = it->first;
		}
	}



}}}}}}