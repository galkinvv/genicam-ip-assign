#include <exception>
#include <iostream>
#include <typeinfo>

#include "Arena/ArenaApi.h"

struct ArenaSystemDeleter {
    void operator()(Arena::ISystem *asystem) {
        // docs doesn't say if nullptr ISystem* value should be freed, so free it without check according to example
        Arena::CloseSystem(asystem);
    }
};
typedef std::unique_ptr<Arena::ISystem, ArenaSystemDeleter> ArenaSystemUniquePtr;

void configure_cameras(std::ostream &log, ArenaSystemUniquePtr &asystem) {
    log << "All done\n";
}

int main(int argc, char *argv[]) {
    try {
        ArenaSystemUniquePtr asystem(Arena::OpenSystem());
        configure_cameras(std::cout, asystem);
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Unexpected exception of type " << typeid(e).name() << "   " << e.what() << std::endl;
        return 1;
    }
}
