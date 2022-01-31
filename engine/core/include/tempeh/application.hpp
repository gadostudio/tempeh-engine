namespace Tempeh::Core
{

	class Application
	{
	private:
		bool is_running = true;
	public:
		using ApplicationReturn = int;

		Application();
		~Application();
		ApplicationReturn run();
	};

}
