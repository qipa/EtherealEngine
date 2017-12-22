#include "device_impl.h"
#include "check.h"
#include <AL/alc.h>
#include <AL/alext.h>
#include <cstdint>
#include <sstream>

namespace audio
{

static bool has_efx(ALCdevice* dev)
{
	return (alcIsExtensionPresent(dev, "ALC_EXT_EFX") == AL_TRUE); // ALC_TRUE
}

static std::string info()
{
	std::stringstream ss;
	if(alGetString(AL_VERSION))
		ss << "OpenAL version: " << alGetString(AL_VERSION) << std::endl;
	if(alGetString(AL_RENDERER))
		ss << "OpenAL renderer: " << alGetString(AL_RENDERER) << std::endl;
	if(alGetString(AL_VENDOR))
		ss << "OpenAL vendor: " << alGetString(AL_VENDOR) << std::endl;
	if(alGetString(AL_EXTENSIONS))
		ss << "OpenAL extensions: " << alGetString(AL_EXTENSIONS) << std::endl;

	return ss.str();
}

device_impl::device_impl(int devnum)
{
	init(devnum);
}

device_impl::~device_impl()
{
	quit();
}

bool device_impl::init(int devnum)
{
	// init
	dev = nullptr;
	ctx = nullptr;
	_device_id = {};

	// device name
	auto list = enumerate();
	if(devnum >= 0 && devnum < int(list.size()))
		_device_id = list[std::size_t(devnum)];

	// select device
	dev = alcOpenDevice(_device_id.empty() ? 0 : _device_id.c_str());

	if(!dev)
		return false;

	bool has_efx_ = has_efx(dev);

	ALint attribs[4] = {0};
	attribs[0] = ALC_MAX_AUXILIARY_SENDS;
	attribs[1] = 4;

	// select device
	ctx = alcCreateContext(dev, has_efx_ ? attribs : nullptr);

	if(!ctx)
		return false;

	enable();

	//$alDistanceModel( AL_INVERSE_DISTANCE );
	alCheck(alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED));

	_info = info();

	return true;
}

void device_impl::enable()
{
	alCheck(alcMakeContextCurrent(ctx));
}

void device_impl::disable()
{
	alCheck(alcMakeContextCurrent(nullptr));
}

void device_impl::quit()
{
	if(ctx)
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(ctx);
		ctx = nullptr;
	}

	if(dev)
	{
		(alcCloseDevice(dev));
		dev = nullptr;
	}
}

bool device_impl::is_valid() const
{
	return dev && ctx;
}

const std::string& device_impl::get_device_id() const
{
    return _device_id;
}

const std::string &device_impl::get_info() const
{
    return _info;
}

std::vector<std::string> device_impl::enumerate()
{
	std::vector<std::string> vs;

	if(alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const char* devices = (const char*)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);

		while(std::string(devices).size())
		{
			vs.push_back(devices);
			devices += vs.back().size() + 1;
		}
	}

	return vs;
}
}