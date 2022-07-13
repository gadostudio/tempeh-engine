#ifndef _TEMPEH_SCRIPTING_MONO_RUNTIME_HPP
#define _TEMPEH_SCRIPTING_MONO_RUNTIME_HPP

//#include <mono/metadata/object.h>
//#include <mono/metadata/appdomain.h>
//#include <mono/metadata/assembly.h>
#include <memory>

namespace Tempeh::Scripting
{

	class MonoRuntime
	{
	private:
//		MonoDomain* domain;
//		MonoAssembly* game_assembly;
	public:
		MonoRuntime();
		~MonoRuntime();
	};

}

#endif
