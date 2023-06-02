#include"time.h"
#include "DeferredShadingViewer.h"
//#include "SViewer.h"

int
main(int argc, char** argv)
{
	srand(time(NULL));
	return cg::Application{new DeferredShadingViewer{1280, 720}}.run(argc, argv);
	//return cg::Application{ new SViewer{1280, 720}}.run(argc, argv);
}
