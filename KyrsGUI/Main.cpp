#include "MyForm.h"
using namespace Kyrs;

[STAThreadAttribute]
void main() {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	MyForm forma;
	//forma.start();
	Application::Run(%forma);
}

