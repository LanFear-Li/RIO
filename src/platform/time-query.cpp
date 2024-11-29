#include "time-query.hpp"

Time_Query::Time_Query()
{
    glGenQueries(1, &query_start_id);
    glGenQueries(1, &query_end_id);
}

void Time_Query::query_start()
{
    glQueryCounter(query_start_id, GL_TIMESTAMP);
}

float Time_Query::query_end()
{
    glQueryCounter(query_end_id, GL_TIMESTAMP);

    GLuint64 time_start, time_end;
    glGetQueryObjectui64v(query_start_id, GL_QUERY_RESULT, &time_start);
    glGetQueryObjectui64v(query_end_id, GL_QUERY_RESULT, &time_end);

    return (time_end - time_start) / 1000000.0f;
}
