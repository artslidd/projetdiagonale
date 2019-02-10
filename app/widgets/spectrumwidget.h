#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QColor>
#include <QElapsedTimer>

#include "../constants.h"

/*!
 * \brief Le widget permettant l'affichage du spectre du signal audio en train d'être lu.
 * Ce widget reçoit régulièrement des données à partir du slot \a spectrumChanged(), depuis
 * le SpectrumAnalyser. Il s'agit d'une classe purement graphique, elle n'effectue aucun calcul
 * sur le signal audio. Elle reçoit donc directement un tableau contenant les hauteurs de ses barres
 * et se contente de les afficher à l'écran (en gérant toutefois la mise à l'échelle).
 * Il est possible de demander à ce que l'affichage se fasse en linéaire ou au dessus d'un
 * piano, au choix.
 */
class SpectrumWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumWidget(QWidget *parent = 0);

    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *event);

    void mouseReleaseEvent(QMouseEvent * e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent * e) Q_DECL_OVERRIDE;

    //! \brief Fonction demandant à afficher les barres plus grandes (ne marche pas si l'autoscale est actif)
    void zoomIn();
    //! \brief Fonction demandant à afficher les barres plus petites (ne marche pas si l'autoscale est actif)
    void zoomOut();

signals:

public slots:
    /*!
     * \brief La fonction d'approvisionnement en données, cela doit correspondre aux barres à afficher en linéaire ou en logarithmique selon le mode de l'instance
     * \param newSpectrum Le tableau des hauteurs de barres
     */
    void spectrumChanged(QVector<float> newSpectrum);
    /*!
     * \brief Réglage de l'autoscale
     * \param autoScale Booléen sur true si on souhaite activer l'autoscale pour des raisons de compatibilité, cet argument est un int (0 pour faux, 1 pour vrai)
     */
    void setAutoscale(int autoScale);
    //! Le toggle permettant de passer du mode linéaire au mode piano et réciproquement
    void switchMode();

private:
    /*!
     * \brief Fonction graphique permettant de dessiner une barre à l'écran
     * \param x L'abscisse du début de la barre
     * \param width La largeur de la barre
     * \param height La hauteur de la barre
     * \param painter Le QPainter utilisé
     */
    void drawBar(int x, int width, int height, QPainter *painter);

    //! Gradient servant à dessiner chaque barre
    QLinearGradient m_gradient;

    //! Le spectre actuel à dessiner
    QVector<float> m_spectrum;
    //QVector<qint64> m_barList;

    //! Horloge interne à chaque instance permettant de lisser l'affichage
    QElapsedTimer m_clock;

    //! Amplitude demandée par le système de calcul de l'autoscale, varie brusquement
    float m_amplitude;
    //! Amplitude demandée par l'utilisateur, varie brusquement
    float m_userAmplitude;
    //! L'amplitude actuelle (facteur de zoom), cette valeur évolue doucement vers \a m_amplitude() si l'autoscale est actif, vers \a m_userAmplitude() sinon
    float m_smoothAmplitude;

    //! Booléen valant true si l'autoscale est inactif, l'échelle est alors fixée et ne change pas sans intervention de l'utilisateur
    bool m_fixedScale;
    //! Booléen valant true si le widget est en mode piano, false si on affiche les barres en linéaire
    bool m_pianoMode;

    //! Booléen permettant de mettre en surbrillance le bouton de zoom
    bool m_buttonPlus;
    //! Booléen permettant de mettre en surbrillance le bouton de dézoom
    bool m_buttonMinus;

    //! Valeur utilisée pour l'autoscale, temps écoulé depuis l'observation du maxima actuel
    int m_maximumTimer;
    //! Valeur utilisée pour l'autoscale, index du maximum actuel
    int m_currentMax;
    //! Tableau utilisé pour améliorer l'autoscale, il enregistre les derniers maxima observés (circulaire)
    QVector<float> m_maxima;
};

#endif // SPECTRUMWIDGET_H
