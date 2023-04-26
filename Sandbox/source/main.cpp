#include <Engine Core.h>
#include <GLSandbox.h>
#include <GLSandbox.h>
#include <Windows.h>

int main() 
{
	EngineCore core = EngineCore(800, 600, "Kai", 60, new GLSandbox());

	core.Run();

	return 0;
}