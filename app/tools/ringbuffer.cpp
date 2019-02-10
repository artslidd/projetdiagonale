#include "ringbuffer.h"
#include <QDebug>

RingBuffer::RingBuffer(unsigned int size) : m_index(0), m_time(0)
{
    m_size = size * 2;
    m_half = size;

    m_data = new float[m_size];

    for (unsigned int i = 0 ; i < m_size ; ++i)
        m_data[i] = 0.0f;
}

RingBuffer::~RingBuffer()
{
    delete[] m_data;
}
/*
void RingBuffer::addBuffer(float *data, unsigned int frames)
{
    if(data == NULL)
        return;

    int index;

    m_mutex.lock();

        memcpy(&(m_data[m_index]), data, frames * sizeof(float));

        index = m_index;

        if (m_index + m_half + frames <= m_size)
            m_index += frames;
        else
            m_index = frames - (m_half - m_index);

        m_time += frames;

    m_mutex.unlock();

    if (index + m_half + frames <= m_size) {
        memcpy(&(m_data[index + m_half]), data, frames * sizeof(float));
    } else {
        if (m_size != index + m_half)
            memcpy(&(m_data[index + m_half]), data, (m_half - index) * sizeof(float));

        const unsigned int remaining = frames - (m_half - index);
        memcpy(&(m_data[0]), &(data[m_half - index]), remaining * sizeof(float));
    }
}*/

void RingBuffer::addBuffer(float *data, unsigned int frames)
{
    if(data == NULL)
        return;

    int index;

    m_mutex.lock();

        memcpy(&(m_data[m_index]), data, frames * sizeof(float));

        index = m_index;

        if (m_index + m_half + frames <= m_size)
            m_index += frames;
        else
            m_index = frames - (m_half - m_index);

        m_time += frames;

    m_mutex.unlock();

    if (index + m_half + frames <= m_size) {
        memcpy(&(m_data[index + m_half]), data, frames * sizeof(float));
    } else {
        if (m_size != index + m_half)
            memcpy(&(m_data[index + m_half]), data, (m_half - index) * sizeof(float));

        const unsigned int remaining = frames - (m_half - index);
        memcpy(&(m_data[0]), &(data[m_half - index]), remaining * sizeof(float));
    }
}

const float * RingBuffer::rawData() {
    return &(m_data[m_index]);
}
