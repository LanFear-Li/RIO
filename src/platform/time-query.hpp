#pragma once

#include <glad/glad.h>

class Time_Query final
{
public:
    Time_Query();
    void query_start();
    float query_end();

private:
    GLuint query_start_id;
    GLuint query_end_id;
};
