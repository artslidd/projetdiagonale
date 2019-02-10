#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "enhancedfft_wrapper.h"
#include "constants.h"
#include "../tools/ringbuffer.h"

/*!
 * \brief La classe SpectrumAnalyser s'occupe de calculer en continu le spectre du signal en cours de lecture.
 * Il pointe directement sur le RingBuffer dans lequel tout le flux
 * audio de l'application circule. Dans un thread séparé, il cherche le buffer de 4096 samples le plus récent
 * possible dans le RingBuffer, calcule son spectre, envoie ses résultats dans le thread principal via un signal, dort 25 millisecondes et recommence.
 * Ainsi, même si son exécution est très ralentie, il ne va pas accumuler des buffers en retard.
 */
class SpectrumAnalyser : public QThread
{
    Q_OBJECT

public:

    // Initialisation
    explicit SpectrumAnalyser(QObject *parent = 0);
    ~SpectrumAnalyser();

    /*!
     * \brief Au démarrage de l'application, il est nécessaire de fournir un buffer au SpectrumAnalyser sur lequel travailler
     * \param buffer Adresse du RingBuffer principal
     */
    void setBuffer(RingBuffer * buffer);

    // Démarrage
    //! Fonction principale de l'analyseur de spectre, à ne pas appeler directement (cf. documentation Qt des QThread)
    void run() Q_DECL_OVERRIDE;

    // Arrêt
    //! Demande à ce que l'analyseur de spectre s'arrête
    void stopAnalyser();

signals:

    // Emission du resultat
    /*!
     * \brief Signale qu'un nouveau spectre plus récent vient d'être calculé
     * \param result Le spectre en question, formaté en linéaire ou en piano selon les besoins
     */
    void calculationComplete(QVector<float> result);

public slots:

    //! Demande au SpectrumAnalyser de formater ses résultats pour être affichés au dessus d'un piano
    void setPiano(bool piano);
    //! Demande au spectre de ne retenir dans ses résultats que les maxima locaux
    void setMaximaDetection(bool maximaDetection);

private:

    // Wrapper
    //! Le wrapper de FFT permettant d'effectuer les traitements
    FFTRealWrapper m_fftWrapper;

    // Fenetre
    //! La fenêtre principale du programme
    float * m_window;

    // Buffer source
    //! Le buffer dans lequel chercher les données
    RingBuffer * m_sourceRingBuffer;

    // Buffer de sortie
    //! Le buffer dans lequel les données sont stockées
    float * m_outputBuffer;

    // Buffer temporaire d'entrée
    //! Buffer temporaire dans lequel sont stockées les données d'entrée
    float * m_inputBuffer;

    // Options
    //! Booléen valant true si les résultats doivent être formatés "en piano"
    bool m_piano;
    //! Booléen valant true si l'on doit ignorer les barres non maximales
    bool m_maximaDetection;

    // Résultat
    //! Le tableau contenant le résultat des calculs
    QVector<float> m_result;
    //! Le tableau indiquant quels indices de \a m_result() sont des maxima locaux
    QVector<bool> m_result_maximum;

    // Life boolean
    //! Booléen d'activité maintenant le run()
    bool m_running;
};

#endif // SPECTRUMANALYSER_H
