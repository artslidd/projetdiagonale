#ifndef SOURCESELECTOR_H
#define SOURCESELECTOR_H

#include <QWidget>
#include <QGroupBox>

/*!
 * \brief Ce widget est une QGroupBox disposant d'un menu à gauche permettant d'afficher plusieurs onglets.
 * Il est utilisé pour sélectionner une source. On place alors à l'intérieur un stack widget pour simuler
 * l'effet d'onglets. En bas à gauche du widget se trouve un petit bouton MIDI relié à un signal.
 */
class SourceSelector : public QGroupBox
{
    Q_OBJECT

public:
    SourceSelector(QWidget * parent);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * e) Q_DECL_OVERRIDE;

    /*!
     * \brief Accesseur
     * \return L'onglet actuellement sélectionné
     */
    int currentIndex();

public slots:

    //! Slot permettant de passer à l'onglet suivant (circulaire)
    void nextSource();
    //! Slot permettant de passer à l'onglet précédent (circulaire)
    void previousSource();

signals:

    //! Signale l'appui sur le petit bouton MIDI en bas à gauche
    void midiConfigPanelRequested();
    //! Signale un changement d'onglet
    void newTabSelected(int);
    //! Signale qu'il faut cacher le module waveform
    void hideWaveform();
    //! Signale qu'il faut afficher le module waveform
    void showWaveform();

private:
    //! La pixmap d'arrière-plan commune à toutes les QGroupBox
    QPixmap m_background;
    //! La pixmap d'arrière-plan pour le fond large (sampler)
    QPixmap m_background_large;
    //! La pixmap d'arrière-plan pour le menu gauche
    QPixmap m_background_left;
    //! La pixmap de titre commune à toutes les QGroupBox
    QPixmap m_title;
    //! La pixmap des quatre sources disponibles
    QPixmap m_buttons;
    //! La pixmap du dégradé blanc circulaire, placée au dessus du bouton actif
    QPixmap m_glow;

    //! L'index de l'onglet actuel
    int m_index;
};

#endif // SOURCESELECTOR_H
