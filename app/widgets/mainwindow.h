#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include "../qtwidgets/myqpushbutton.h"
#include "../core/engine.h"
#include "filterwidget.h"
#include "waveformwidget.h"
#include "spectrumwidget.h"
#include "../core/spectrumanalyser.h"
#include "../core/externalsourcemodule.h"
#include "../core/basicsignalmodule.h"
#include "../core/filemodule.h"
#include "../core/samplermodule.h"
#include "../midi/mididialog.h"
#include "../midi/midiengine.h"

namespace Ui {
class MainWindow;
}

/*!
 * \brief La fenêtre principale contenant l'ensemble du programme (sauf panneaux de configuration).
 * Cette classe instancie toutes les classes utiles au bon déroulement du programme. Dans le
 * constructeur, toutes ces classes sont reliées entre elles et le logiciel peut démarrer.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;

    /*!
     * \brief Slot permettat de gérer un appui sur une touche d'un périphérique MIDI
     * \param input L'identifiant de la touche (son rôle)
     * \param value La valeur prise
     * \return True si l'appui sur la touche a été géré
     */
    bool midiInputEvent(MidiEngine::MidiSoftwareInput input, int value);
    //! Demande l'ouverture de la boîte de dialogue About contenant les informations sur les droits d'auteur
    void aboutMessageBox();

private:
    Ui::MainWindow *ui;

    // About
    //! Bouton A propos en haut à droite de l'écran
    MyQPushButton m_pushButton_about;

    // Les dialogs
    //! Panneau de configuration MIDI
    MidiDialog m_midiDialog;

    // Les widgets
    //! Le widget de contrôle du filtrage (en bas à gauche)
    FilterWidget m_filterWidget;
    //! Le widget d'affichage de la waveform (en haut à droite)
    WaveformWidget m_waveformWidget;
    //! Le widget d'affichage du spectre (en bas à droite)
    SpectrumWidget m_spectrumWidget;

    // Core
    //! Module source externe
    ExternalSourceModule m_externalSourceModule;
    //! Module signal de base
    BasicSignalModule m_basicSignalModule;
    //! Module fichier
    FileModule m_fileModule;
    //! Module sample
    SamplerModule m_samplerModule;
    //! Moteur du programme
    Engine m_engine;
    //! Moteur de gestion des entrées MIDI
    MidiEngine m_midiEngine;
};

#endif // MAINWINDOW_H
