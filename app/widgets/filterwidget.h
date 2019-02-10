#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>
#include "../filter/filter.h"

/*!
 * \brief La classe FilterWidget permet à l'utilisateur de configurer le filtre quand il le souhaite.
 * Visuellement, cette classe est le rectangle dans lequel est dessiné grossièrement le gabarit du filtre.
 * En interne, c'est elle qui centralise tous les clics sur les widgets associés au filtrage (boutons
 * radio, slider, dial...) et qui configure le filtre \a m_filter() en conséquence.
 */
class FilterWidget : public QWidget
{
    Q_OBJECT

public:
    FilterWidget(QWidget * parent = 0);

    // QWidget
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

    // Accès
    //! Accesseur pour connecter le filtre en temps réel
    Filter * getFilter() { return &m_filter; }

public slots:

    // Bouton de reset
    //! Remet à zéro le filtre, inutilisé dans la release 1.0
    void reset();

    // Contrôles en dessous
    //! Slot de contrôle directement relié aux widgets
    void amplitudeChanged(int value);
    //! Slot de contrôle directement relié aux widgets
    void frequencyChangedSingle(int value);
    //! Slot de contrôle directement relié aux widgets
    void frequencyChangedDouble(int lowerValue, int upperValue);
    //! Slot de contrôle directement relié aux widgets
    void attenuationSet(bool attenuation);

    //! Slot de changement de mode directement relié aux boutons radio
    void setNone(bool);
    //! Slot de changement de mode directement relié aux boutons radio
    void setLowPass(bool checked);
    //! Slot de changement de mode directement relié aux boutons radio
    void setHighPass(bool checked);
    //! Slot de changement de mode directement relié aux boutons radio
    void setBandPass(bool checked);
    //! Slot de changement de mode directement relié aux boutons radio
    void setBandCut(bool checked);

signals:

    //! Signale que l'on est passé sur un filtre simple (passe-bas, passe-haut) et permet de change en conséquence l'apparence du DoubleSlider
    void singleMode();
    //! Signale que l'on est passé sur un filtre double (passe-bande, coupe-bande) et permet de change en conséquence l'apparence du DoubleSlider
    void doubleMode();

private:

    //! Le filtre actif dans tout le programme
    Filter m_filter;

    // Dessin
    //! La pixmap de l'arrière-plan du rectangle de visualisation du gabarit du filtre
    QPixmap m_background;
    //! La pixmap de l'avant-plan du rectangle de visualisation du gabarit du filtre
    QPixmap m_foreground;
    //! La pixmap de la bordure du rectangle de visualisation du gabarit du filtre
    QPixmap m_border;
};

#endif // FILTERWIDGET_H
