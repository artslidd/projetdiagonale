#include "waveformpixmap.h"
#include "constants.h"
#include <QtCore>

#define WAVEFORM_PIXMAP_SECURITY_WIDTH 50

WaveformPixmap::WaveformPixmap(int width) : m_currentPixel(0.0f), m_invalidated(false), m_synchro(false), m_painting(false), m_trigger(0.0f), m_pixmap(width + WAVEFORM_PIXMAP_SECURITY_WIDTH, 256) {
    m_width  = width + WAVEFORM_PIXMAP_SECURITY_WIDTH;

    QPainter painter(&m_pixmap);
    painter.fillRect(0, 0, m_width, 256, Qt::transparent);

    m_gradient = new QLinearGradient();
    m_gradient->setStart(0, 256 / 2);
    m_gradient->setFinalStop(0, 0);
    m_gradient->setColorAt(0, "#7F799A");
    m_gradient->setColorAt(1, "#FFFFFF");
    m_gradient->setSpread(QGradient::ReflectSpread);
    m_brush = new QBrush(*m_gradient);
    m_pen = new QPen();
    m_pen->setBrush(*m_brush);
}

WaveformPixmap::~WaveformPixmap() {
    delete m_pen;
    delete m_brush;
    delete m_gradient;
}

int WaveformPixmap::setDisplayedFrames(int frames) {
    if ((float) frames / (float) FREQUENCE_ECHANTILLONNAGE < WAVEFORM_MIN_PERIOD)
        frames = WAVEFORM_MIN_PERIOD * (float) FREQUENCE_ECHANTILLONNAGE;
    else if ((float) frames / (float) FREQUENCE_ECHANTILLONNAGE > WAVEFORM_MAX_PERIOD)
        frames = WAVEFORM_MAX_PERIOD * (float) FREQUENCE_ECHANTILLONNAGE;

    m_pixelsPerFrame = (float) m_width / (float) frames;

    m_invalidated = true;

    return frames;
}

void WaveformPixmap::enableSynchronization(bool enable) {
    m_synchro = enable;
}

void WaveformPixmap::update(const float *data, unsigned int frames, unsigned int time) {
    if (m_pixelsPerFrame >= 0.25f)
        updateFewPoints(data, frames, time);
    else
        updateSeveralPoints(data, frames, time);
}

void WaveformPixmap::updateFewPoints(const float *data, unsigned int frames, unsigned int time) {
    // Ici, il y a tellement peu de points qu'on redessine tout à chaque fois
    QPainter painter(&m_pixmap);
    painter.fillRect(0, 0, m_width, 256, Qt::black);
    painter.setPen(*m_pen);

    // On détermine quelle frame sera la plus à gauche
    unsigned int firstFrame = frames - (m_width / m_pixelsPerFrame);
    if (m_synchro) firstFrame = findPeriodStart(data, firstFrame);

    // Puis quelle frame sera la plus à droite
    const unsigned int lastFrame = firstFrame + (m_width / m_pixelsPerFrame);

    // Si la synchro est activée, un calcul un peu compliqué permet de faire du pixel perfect, sinon, on démarre à zéro
    m_currentPixel = (m_synchro && data[firstFrame] != data[firstFrame + 1]) ?
        qMax(- m_pixelsPerFrame - m_pixelsPerFrame * qAbs(m_trigger - data[firstFrame]) / qAbs(data[firstFrame+1] - data[firstFrame]), - 3.0f * m_pixelsPerFrame) : 0.0f;

    // On dessine une ligne reliant chaque paire de frame
    for (unsigned int frame = firstFrame ; frame < lastFrame ; ++frame) {
        painter.drawLine(qRound(m_currentPixel), 128 + 127 * data[frame-1], qRound(m_currentPixel + m_pixelsPerFrame), 128 + 127 * data[frame]);

        m_currentPixel += m_pixelsPerFrame;
    }

    // On actualise les variables d'état
    m_currentPixel = m_width - WAVEFORM_PIXMAP_SECURITY_WIDTH;
    m_currentTime  = time;

    m_invalidated = false;
}

void WaveformPixmap::updateSeveralPoints(const float *data, unsigned int frames, unsigned int time) {
    if (m_painting) // Limite le ralentissement si l'ordi est trop lent
        return;

    // Cette fois, on garde, dans la mesure du possible, ce qui a déjà été dessiné afin de ne pas perdre du temps à tout redessiner à chaque appel
    QPainter painter(&m_pixmap);
    painter.setPen(*m_pen);

    if (time - m_currentTime == 0)
        return;

    // Si la synchro est activée, on détermine où l'on commence (TODO : si on commence au même endroit que la dernière fois, on ne fait rien, sinon on invalide pour afficher une nouvelle période (et encore il est possible de ne même pas invalider mais ça devient compliqué))
    if (m_synchro) {
        const unsigned int synchroFrame = findPeriodStart(data, frames - (m_width - WAVEFORM_PIXMAP_SECURITY_WIDTH) / m_pixelsPerFrame);
        frames = synchroFrame + (m_width - WAVEFORM_PIXMAP_SECURITY_WIDTH) / m_pixelsPerFrame - 1;
        m_invalidated = true;
    }

    // On regarde combien de frames ont été ajoutéees dans le ringbuffer depuis la dernière update
    const int newFrames = m_invalidated || (time - m_currentTime > m_width / m_pixelsPerFrame) ? m_width / m_pixelsPerFrame - 1 : time - m_currentTime;

    // On détermine la plage à actualiser et on la peint en blanc
    const int endPixel  = m_currentPixel + m_pixelsPerFrame * ((float) newFrames);

    m_painting = true;
    if (endPixel < m_width) {
        painter.fillRect((int) m_currentPixel, 0, endPixel - (int) m_currentPixel, 256, Qt::black);
    } else {
        painter.fillRect((int) m_currentPixel, 0, m_width - (int) m_currentPixel, 256, Qt::black);
        painter.fillRect(0, 0, endPixel - m_width, 256, Qt::black);
    }

    // On actualise ce qu'il faut avec les newFrames
    static float min = 10.f, max = -10.f;
    for (int i = 0 ; i < newFrames ; ++i) {
        const int x = m_currentPixel;
        const int f = frames - newFrames + i;

        min = qMin(min, data[f]);
        max = qMax(max, data[f]);

        m_currentPixel += m_pixelsPerFrame;
        if ((int) m_currentPixel >= m_width)
            m_currentPixel -= (float) m_width;

        if ((int) m_currentPixel != x) {
            painter.drawLine(x, 128 + 127 * min, x, 128 + 127 * max);
            min =  10.f;
            max = -10.f;
        }
    }

    // On actualise les variables d'état
    m_currentTime = time;
    m_invalidated = false;
    m_painting = false;
}

void WaveformPixmap::draw(QPainter &painter, int allowedWidth, int allowedHeight) {
    const int rightMostPixel = (int) m_currentPixel;

    if (rightMostPixel >= allowedWidth) {
        painter.drawPixmap(0,(allowedHeight-256)/2,allowedWidth,256,m_pixmap,rightMostPixel-allowedWidth,0,allowedWidth,256);
    } else {
        painter.drawPixmap(allowedWidth-rightMostPixel,(allowedHeight-256)/2,rightMostPixel,256,m_pixmap,0,0,rightMostPixel,256);
        painter.drawPixmap(0,(allowedHeight-256)/2,allowedWidth-rightMostPixel,256,m_pixmap,m_width-(allowedWidth-rightMostPixel),0,allowedWidth-rightMostPixel,256);
    }
}

unsigned int WaveformPixmap::findPeriodStart(const float * data, unsigned int frames) {
    int maxFrame = frames - 1;
    float max = -1.0f;

    for (int frame = frames - 1 ; frame > (int) frames - 1 - qMin((int) (1000.0f / m_pixelsPerFrame), 512) ; --frame) {
        if (data[frame] > max) {
            max = data[frame];
            maxFrame = frame;
        }
    }

    const int framesToProcess = qMin((int)(4.2f * (float) m_width / m_pixelsPerFrame), 8820);

    bool high = false;
    int limitFrame = qMax(1, maxFrame - framesToProcess);
    for (int frame = maxFrame ; frame > limitFrame ; --frame) {
        if (high) {
            if (data[frame] <= m_trigger)
                return frame;
        } else if (data[frame] > m_trigger) {
            high = true;
        }
    }
    if (limitFrame == 1) {
        limitFrame = qMin(maxFrame, (int) frames - (framesToProcess - maxFrame));
        for (int frame = frames - 1 ; frame > limitFrame ; --frame) {
            if (high) {
                if (data[frame] <= m_trigger)
                    return frame;
            } else if (data[frame] > m_trigger) {
                high = true;
            }
        }
    }

    return frames - 1;
}

/*
unsigned int WaveformPixmap::findPeriodStart(const float * data, unsigned int frames) {
    bool high = false;
    for (int frame = frames - 1 ; frame > 1 ; --frame) {
        if (high) {
            if (data[frame] <= m_trigger)
                return frame;
        } else if (data[frame] > m_trigger) {
            high = true;
        }
    }

    return frames - 1;
}
*/

