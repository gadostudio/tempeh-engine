#include <tempeh/scripting/mono.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <cassert>

namespace Tempeh::Scripting
{

	MonoRuntime::MonoRuntime()
	{
		mono_set_dirs(".", ".");
		// TODO change file string value
		domain = mono_jit_init("TempehEngine");
		assert(domain && "Mono domain can't be started");

		game_assembly = mono_domain_assembly_open(domain, "Game.dll");


	}

	MonoRuntime::~MonoRuntime()
	{
		if (game_assembly)
		{
			mono_assembly_close(game_assembly);
		}

		if (domain)
		{
			mono_jit_cleanup(domain);
		}
	}
}
