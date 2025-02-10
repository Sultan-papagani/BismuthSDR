#include "src/backend.h"
#include "src/logger.h"

int main(int, char**)
{
    Logger::log(LogL::DEBUG, "BismuthSDR starting");
    backend_init(1280, 720, "BismuthSDR");
    int error_code = backend_ui_loop();
    Logger::log(LogL::DEBUG, "Backend UI Loop exit with:", error_code);
    backend_destroy();
    Logger::log(LogL::DEBUG, "Backend destroyed! quitting...");
}