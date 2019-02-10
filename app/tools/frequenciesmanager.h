#ifndef FREQUENCIESMANAGER_H
#define FREQUENCIESMANAGER_H

#include <QVector>
#include <QPointF>

/*!
 * \brief Cette classe rassemble les fonctions utiles permettant de travailler avec la dualité fréquence/notes de musique.
 * La classe FrequenciesManager fournit des outils statiques et n'a pas à être
 * instanciée. Elle génère des tableaux de données lorsqu'ils sont demandés
 * pour la première fois et les enregistre pour ne plus avoir à les recalculer
 * par la suite.
 */
class FrequenciesManager
{

public:
    /*!
     * \brief Renvoie les fréquences en Hz des touches successives d'un piano.
     * La constante NOTES_START_FREQUENCY donne le dernier 'la' qui n'est pas dans
     * le piano, et le nombre de touches est calculé à partir de SPECTRUM_NUMBER_OF_OCTAVES.
     * La note la plus basse (la première du tableau) n'est pas un 'do' comme sur le piano,
     * mais la touche précédente, de sorte que les intervalles de fréquences associés aux
     * touches puissent être calculés convenablementet que l'afficheur de spectre puisse
     * ignorer la première barre (qui 'aspire' toutes les basses fréquences).
     * \sa getPianoBounds()
     * \return Le tableau des fréquences, de taille SPECTRUM_NUMBER_OF_OCTAVES * 12 - 2
     */
    static QVector<float> getPianoFrequencies();
    /*!
     * \brief Renvoie les fréquences successives encadrant les notes du piano.
     * \return Le tableau, de taille SPECTRUM_NUMBER_OF_OCTAVES * 12 - 1
     */
    static QVector<float> getPianoBounds();

    /*!
     * \brief Renvoie le numéro de la touche de piano associée à la fréquence fournie en paramètre.
     * \param f La fréquence dont on veut connaître la note associée.
     * \return L'indice de la fréquence, par rapport au tableau renvoyé par \a getPianoFrequencies().
     */
    static int getIndexInPianoFrequenciesFromFreq(float f);
    /*!
     * \brief Renvoie l'index de la fréquence sur une échelle linéaire prenant en compte les constantes de l'afficheur de spectre.
     * On notera que les fréquences trop hautes ou trop basses sont 'accumulées' aux extrémités
     * \param f La fréquence dont on veut connaître l'index
     * \return L'indice de la fréquence, entre 0 et SPECTRUM_NUMBER_OF_BARS - 1
     */
    static int getIndexInLinearFrequenciesFromFreq(float f);
    /*!
     * \brief Renvoie les largeurs successives des touches d'un piano.
     * \return Le tableau en question
     */
    static QVector<double> getPianoWidths();
    /*!
     * \brief A partir de la position de la souris sur le piano, renvoie l'indice de la touche survolée.
     * \param x La coordonnée x de la souris
     * \param y La coordonnée y de la souris
     * \return L'index de la touche sous la souris
     */
    static int getIndexInPianoFrequenciesFromPosition(double x, double y = 0.0);
    /*!
     * \brief A partir de l'index d'une touche de piano, renvoie sa position à l'écran sur l'image du piano
     * \param index L'index de la touche
     * \return La position de la touche, normalisée entre 0 et 1 (à multiplier par la taille de l'image)
     */
    static QPointF getPositionInPianoFromIndex(int index);

private:

    FrequenciesManager() {}
};

#endif // FREQUENCIESMANAGER_H
