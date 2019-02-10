#ifndef SAMPLERMODULE_H
#define SAMPLERMODULE_H

#include "enginemodule.h"
#include "../filter/filter.h"
#include <QPushButton>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QString>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>

int inoutSampler(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *streamData);

namespace Ui {
class SamplerModule;
}

/*!
 * \brief Le séquenceur. Il récupère les fichiers audios des samples et gère le stream. Il gère l'interface avec l'utilisateur (édition, lecture, enregistrement).
 */
class SamplerModule : public EngineModule
{
    Q_OBJECT

public:

    /*!
     * \brief Structure contenant l’ensemble des données nécessaires au stream
     */
    struct StreamData
    {
        Filter * filter;
        RingBuffer *ringBuffer;
        float **samples;
        int buttonsState[4][16];
        int loopCount;
        int loopStates[2];
        bool pianoTone;
        int pianoFreq;
        void *samplerModule;
        bool timeUpdated;
        int sliderValue[2];
    };

    // On associe les instruments et accords à chaque style

    /*!
     * \brief Structure contenant l’ensemble des données d'un style
     */
    struct Style
    {
        QString name;

        QVector<QString> chords;
        QVector<QString> instruments;
        QVector<QVector<int>> nb_samples;
        int nb_instruments = 0;
        int nb_chords = 0;
        int total_samples = 0;

    };

    explicit SamplerModule(QWidget *parent = 0);
    ~SamplerModule();

    void link(RingBuffer *ringBuffer, Filter * filter, RtAudio *audio);
    void activateAndPlay();
    void deactivate();

    /*!
     * \brief Getter de m_styles, le style actuellement sélectionné.
     * \return m_styles
     */
    QVector<Style> getStyle();

    /*!
     * \brief Getter du nom de l’accord (string) associé à la colonne sélectionnée.
     * Appelée par chacun des boutons, accords pour la couleur du bouton et le texte, instruments pour la couleur du bouton.
     * \param chordSelect Colonne d'accords choisie
     * \return Nom de l'accord en toutes lettres
     */
    QString getChordTitle(int chordSelect);

    /*!
     * \brief Setter de m_streamData.buttonsState[i][j]
     * \param i Numéro de l'instrument (0/1/2) et 3 pour les boutons des accords
     * \param j Index de la colonne, de 0 à 15
     * \param state Etat du bouton, 0 étant l'état inactif
     */
    void setButtonState(int i, int j, int state);

    /*!
     * \brief Setter de m_streamData.loopStates, les deux points de la boucle du morceau. Un clique sur une checkbox l'active et desactive celle cochée en n-2.
     * Les valeurs correspondent à l'index de la checkbox (de 1 à 17)
     * \param btnIndex 0 ou 1, les duex points de la boucle
     */
    void setLoopStates(int btnIndex);

    /*!
     * \brief Getter de m_streamData.buttonsState.
     * \param i Numéro de l'instrument (0/1/2) et 3 pour les boutons des accords
     * \param j Index de la colonne, de 0 à 15
     * \return Etat du bouton, 0 étant l'état inactif
     */
    int getButtonState(int i, int j);

    /*!
     * \brief Getter de m_streamData.loopStates, retourne l'état de la i-ème checkbox.
     * \param i Index de la checkbox (0 ou 1)
     * \return Index (état) de la checkbox
     */
    int getLoopStates(int i);

    /*!
     * \brief Getter de m_styles[m_style].nb_samples[inst][chord], c'est à dire le nombre de samples disponibles pour un instrument sous un certain accord.
     * \param inst Index de l'instrument
     * \param chord Index de l'accord
     * \return Nombre de samples disponibles
     */
    int getInstNbSamples(int inst, int chord);

    /*!
     * \brief Déplace le slider, change la valeurs de l'horloge et modifie la valeur de m_line_pos (position de la ligne temporelle, tracée avec paintEvent). Méthode utilisée pour suivre l'avancement du morceau.
     * \param time Position dans la colonne de lecture
     * \param column Numéro de la colonne de sample
     */
    void moveLine(int time, int column);


    /*!
     * \brief Setter de timeUpdated (le passe à true)
     */
    void timeUpdateDone();

    /*!
     * \brief Change la valeur de l'horloge et modifie la valeur de m_line_pos (position de la ligne temporelle, tracée avec paintEvent). Méthode utilisée quand le Slider est bougé.
     * \param time Position dans la colonne de lecture
     * \param column Numéro de la colonne de sample
     */
    void followSlider(int time, int column);

signals:
    /*!
     * \brief Signal qui déclenche le slot reset de chacun des boutons du séquenceur
     */
    void resetState();

    /*!
     * \brief Signal qui indique le chagement de l'instrument 1 (lancé au changement de style par setStyle)
     */
    void instrumentChanged(QString inst);

    /*!
     * \brief Signal qui indique le chagement de l'instrument 2 (lancé au changement de style par setStyle)
     */
    void instrumentChanged_2(QString inst);

    /*!
     * \brief Signal qui indique le chagement de l'instrument 3 (lancé au changement de style par setStyle)
     */
    void instrumentChanged_3(QString inst);

    void updateChord_01();
    void updateChord_02();
    void updateChord_03();
    void updateChord_04();
    void updateChord_05();
    void updateChord_06();
    void updateChord_07();
    void updateChord_08();
    void updateChord_09();
    void updateChord_10();
    void updateChord_11();
    void updateChord_12();
    void updateChord_13();
    void updateChord_14();
    void updateChord_15();
    void updateChord_16();

    /*!
     * \brief Signal appelé dans setLoopStates, c'est à dire à chaque fois qu'une checkbox est cliquée.
     * Lance le signal updateState dans des boutons de la classe loopCheckbox
     */
    void UpdateAllLoopStates();

    /*!
     * \brief Signal appelé dans la méthode random pour mettre à jour le visuel de chacun des boutons.
     */
    void updateAllButtons();

public slots:
    bool midiInputEvent(MidiEngine::MidiSoftwareInput input, int value);

    void enablePianoTone(int freq);
    void disablePianoTone();
    /*!
     * \brief Charge les nouveaux samples et reset la grille quand le style est changé.
     * \param style Indice du style
     */
    void setStyle(int style);

    /*!
     * \brief Lance le stream
     */
    void play();
    /*!
     * \brief Vide la grille
     */
    void reset();

    void changeChord_01();
    void changeChord_02();
    void changeChord_03();
    void changeChord_04();
    void changeChord_05();
    void changeChord_06();
    void changeChord_07();
    void changeChord_08();
    void changeChord_09();
    void changeChord_10();
    void changeChord_11();
    void changeChord_12();
    void changeChord_13();
    void changeChord_14();
    void changeChord_15();
    void changeChord_16();

    /*!
     * \brief Slot qui change la valeur de m_streamData.loopCount, le nombre de boucles voulu.
     * Appelé par le signal currentIndexChanged(int) de loopCountBox
     * \param loopCount Entier donné par ui->loopCountBox
     */
    void changeLoop(int loopCount);

    /*!
     * \brief Remplit la grille aléatoirement
     */
    void random();

    /*!
     * \brief Coupe le stream lors d'un déplacement manuel du Slider et le relance après le déplacement.
     */
    void checkStreamStatus();

    /*!
     * \brief Change les valeurs de temps a transmettre au stream quand le slider est déplacé manuellement et lance followslider.
     * \param value Emplacement du Slider par rapport à sa taille et à son découpage.
     */
    void updateTime(int value);


    /*!
     * \brief Permet de choisir l'emplacement du fichier et lance l'enregistrement.
     */
    void exportMusic();

    /*!
     * \brief Enregistre le fichier : écriture du Header, calcul et écriture des données.
     */
    void saveWavFile();

private:

    // UI
    Ui::SamplerModule *ui;
    /*!
     * \brief Données envoyées au stream.
     */
    StreamData m_streamData;
    /*!
     * \brief L'indice du style en cours
     */
    int m_style;   
    //QString m_instruments[][3];
    /*!
     * \brief Vecteur contenant les infos sur les styles.
     */
    QVector<struct Style> m_styles;
    /*!
     * \brief Vecteur contenant les adresses de tout les samples.
     */
    QVector< QVector< QVector< QVector<QString> > > > m_samples;   
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent * event);

    /*!
     * \brief Nombre de styles, calculé à partir du fichier samples.txt
     */
    int nb_styles;

    /*!
     * \brief index maximal d'un sample
     */
    int nbmax_samples;

    /*!
     * \brief nombre max d'accords parmi les styles.
     */
    int nbmax_chords;

    /*!
     * \brief Id de l'accord en cours, utilisé uniquement pour getChordTitle.
     */
    int chordStateId;

    /*!
     * \brief Borne inférieure (en pixels) de la plage que peut occuper la timeline.
     */
    int m_line_pos_min;

    /*!
     * \brief Borne supérieure (en pixels) de la plage que peut occuper la timeline.
     */
    int m_line_pos_max;

    /*!
     * \brief Position de la barre pour lors du repaint.
     */
    int m_line_pos;

    /*!
     * \brief Entier utilisé dans setLoopStates, toujours entre 1 et 0, permet de créer le système de coche des checkboxes.
     */
    int last_clicked;


    //Les variables suivantes sont utilisées en vue de l'exportation du fichier wav

    /*!
     * \brief Fréquence d'échantillonnage
     */
    int fe = 44100;

    /*!
     * \brief Nombre de canaux, 1 = Mono, 2 = Stéréo
     */
    int nChannels = 2;

    /*!
     * \brief Nombre de bits par sample
     */
    int nBitsPerSample = 64;

    /*!
     * \brief Nombre d'octets par block (avec le nombre de canaux)
     */
    int nBlockAlign = nChannels*nBitsPerSample/8;

    /*!
     * \brief Taille d'un sample de 4 secondes
     */
    int sampleSize = 4*fe;
    bool wasPlaying=false;

    /*!
     * \brief Adresse du fichier d'enregistrement.
     */
    QString m_currentWavFileName;
};

#endif // BASICSIGNALMODULE_H
