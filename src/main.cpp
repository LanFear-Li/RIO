#include "renderer.hpp"

int main(int argc, char* argv[])
{
    auto scene_name = "default-scene";

    if (argc == 2) {
        scene_name = argv[1];
    }

    auto renderer = std::make_unique<Renderer>(scene_name);
    renderer->run();
    return 0;
}
