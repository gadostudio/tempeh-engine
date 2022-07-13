#include <tempeh/telemetry.hpp>

#include <sentry.h>

namespace Tempeh
{

	Telemetry::Telemetry()
	{
        sentry_options_t *options = sentry_options_new();
        sentry_options_set_dsn(options, SENTRY_DSN);
        sentry_init(options);
	}

    Telemetry::~Telemetry() {
        sentry_close();
    }
}
