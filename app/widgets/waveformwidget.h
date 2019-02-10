#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <QPainter>
#include <QWidget>
#include <QVector>
#include <QSize>
#include <QPixmap>
#include <QTimer>

#include "constants.h"
#include "../tools/ringbuffer.h"
#include "../tools/waveformpixmap.h"

/*!
 * \brief Classe d'interface utilisateur s'occupant de gérer la waveform en haut à droite de l'écran.
 * Cette classe ne dessine pas la waveform, c'est le travail de WaveformPixmap. En revanche, elle
 * gère les interactions avec les widgets de la partie affichage de waveform du logiciel. Elle
 * répercute les demandes de l'utilisateur sur la WaveformPixmap.
 */
class WaveformWidget : public QWidget
{
    Q_OBJECT

public:

    explicit WaveformWidget(QWidget *parent = 0);

    // Initialisation
    /*!
     * \brief Permet au widget d'accéder au buffer de travail contenant les samples audio à représenter
     * \param buffer Pointeur vers le RingBuffer à analyser en continu
     */
    void setBuffer(RingBuffer * buffer);

    // QWidget
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

signals:

    // Pour la période affichée
    /*!
     * \brief Signale un changement de la période à afficher à l'écran
     * \param interval La période affichée après changement
     */
    void displayedIntervalChanged(int interval);
    /*!
     * \brief Signale qu'il faut actualiser le texte à droite du slider indiquant la période visualisée
     * \param string Le nouveau texte à afficher
     */
    void displayedStringChanged(QString string);

public slots:

    // Contrôles sous le widget
    //! \brief Slot demandant un ajustement automatique de la période affichée
    void automaticAdjust();
    //! \brief Demande à changer la période affichée
    void setDisplayedInterval(int interval);
    //! \brief Slot permettant d'activer ou désactiver la synchronisation de la waveform
    void enableTrigger(bool enable);
    //! \brief Cache le widget pour laisser la place au module de samples
    void hide();
    //! \brief Affiche le widget
    void show();

private:

    // Pour l'auto-adjust
    //! \brief Fonction utilitaire interne trouvant le premier endroit du buffer correspondant à un passage d'une valeur négative à une valeur positive, à paritr du paramètre
    int findFirstPositiveZeroCrossing(int start);

    // Pour le refresh à 25 FPS
    //! Timer permettant le rafraîchissement constant de la pixmap
    QTimer m_timer;

    // Les données et la pixmap
    //! Pixmap contenant le dessin de la waveform
    WaveformPixmap m_pixmap;
    //! Pointeur vers les données à afficher
    RingBuffer * m_data;
};

#endif // WAVEFORMWIDGET_H
