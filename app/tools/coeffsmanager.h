#ifndef COEFFSMANAGER_H
#define COEFFSMANAGER_H

#include <QVector>
#include <QtMath>

#define AMP_STEPS 12
#define FREQ_STEPS 18
#define ORDER 4
#define NB_BIQUADS qCeil(ORDER/2.0)

#define FMIN 87
#define FMAX 9334

#define AMAX 40

/*!
 * \brief Classe permettant d'obtenir les coefficients d'une fonction de transfert spécifiée.
 * A la construction d'une instance de CoeffsManager, celui-ci charge dans la mémoire vive
 * l'ensemble des coefficients des fonctions de transfert rendues disponibles par le module
 * de filtrage. Il les obtient à partir des fichiers générés par MATLAB (inclus dans les
 * ressources du programme, fichiers .dat) et les stocke dans \a m_coeffs().
 * Lorsqu'on désire obtenir des coefficients, il suffit de se munir du type du filtre, de ses
 * fréquences caractéristiques et de l'amplitude dans la bande coupée. On appelle alors
 * \a getBiquad1() et \a getBiquad2() pour obtenir respectivement les coefficients du premier
 * et du second biquad réalisant ce filtre.
 */
class CoeffsManager
{
public:

    //! L'énumération décrivant un type donné de filtre
    enum {LOWPASS, HIGHPASS, BANDPASS, BANDSTOP};

    //! Ouvre les fichiers .dat contenant les coefficients, charge ces derniers et referme les fichiers
    CoeffsManager();
    //! Se contente de détruire les coefficients enregistrés à la construction
    ~CoeffsManager();

    /*!
     * \brief Permet d'obtenir les coefficients du premier biquad réalisant les spécifications données, n'a pas de sens sans \a getBiquad2()
     * \param type Le type de filtre à réaliser
     * \param a L'atténuation de la bande coupée entre 0 et AMP_STEPS
     * \param f1 La première fréquence de coupure du filtre
     * \param f2 La seconde fréquence de coupure du filtre, non lue pour un passe-bas ou un passe-haut
     * \param attenuation Permet de diviser la sortie par 2 pour éviter une saturation du filtre
     * \return Le tableau des coefficients, ceux du numérateur d'abord, puis ceux du dénominateur
     */
    QVector<float> getBiquad1(int type, unsigned int a, float f1, float f2, bool attenuation);
    /*!
     * \brief Permet d'obtenir les coefficients du second biquad réalisant les spécifications données, n'a pas de sens sans \a getBiquad1()
     * \param type Le type de filtre à réaliser
     * \param a L'atténuation de la bande coupée entre 0 et AMP_STEPS
     * \param f1 La première fréquence de coupure du filtre
     * \param f2 La seconde fréquence de coupure du filtre, non lue pour un passe-bas ou un passe-haut
     * \param attenuation Permet de diviser la sortie par 2 pour éviter une saturation du filtre
     * \return Le tableau des coefficients, ceux du numérateur d'abord, puis ceux du dénominateur
     */
    QVector<float> getBiquad2(int type, unsigned int a, float f1, float f2, bool attenuation);

private:
    //! Fonction utilisée en interne pour retrouver plus facilement les coefficients dans la mémoire
    unsigned int getIndex(int type, unsigned int a, float f1, float f2);
    //! Booléen valant false si une erreur s'est produite à la création de l'instance (typiquement : fichiers non présents ou de la mauvaise taille)
    bool m_coeffsLoadingOK;
    //! Tableau de quatre pointeurs. Chacun pointe sur une liste des coefficients d'un type de filtre
    float *m_coeffs[4];
};

#endif // COEFFSMANAGER_H
