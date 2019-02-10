#ifndef FILTER_H
#define FILTER_H

#include <QList>
#include <QPair>
#include <QString>
#include <QVector>

#include "tools/coeffsmanager.h"

/*!
 * \brief Classe s'occupant de filtre le signal audio dans la fonction inOut de l'EngineModule actif
 * Cette classe représente un filtre donné au sens traitement du signal. Elle fournit trois types de fonctions.
 * Le premier groupe de fonctions permet de configurer le filtre (fréquences caractéristiques, amplitude bande coupée, atténuation globale de -6dB).
 * Le second groupe permet d'afficher le filtre en fournissant de quoi savoir comment le tracer. Le dernier groupe permet d'effectivement appliquer
 * le filtre sur une trame.
 */
class Filter
{
public:
    //! Enumération des différents types de filtres utilisée essentiellement en interne
    enum {
        DESACTIVE,
        PASSE_BAS,
        PASSE_HAUT,
        PASSE_BANDE,
        COUPE_BANDE
    };

    // Constructeur
    Filter();

    // Modification
    /*!
     * \brief Paramétrage de l'amplitude de la bande coupée du filtre
     * \param amplitude Amplitude entre 0 et AMP_STEPS
     */
    void setAmplitude(int amplitude);
    /*!
     * \brief Paramétrage de la fréquence de coupure pour un passe-bas ou passe-haut
     * \param f La fréquence en Hz, peut prendre n'importe quelle valeur
     */
    void setFrequencySingle(float f);
    /*!
     * \brief Paramétrage des fréquences caractéristiques d'un passe-bande ou coupe-bande
     * \param f1 La première fréquence en Hz
     * \param f2 La seconde fréquence en Hz
     */
    void setFrequencyDouble(float f1, float f2);

    // Remise à zéro
    //! Remet à zéro le filtre (passe-tout, sans déphasage bien sûr)
    void reset();

    // Lecture
    //! Retourne le nombre de blocs nécessaires au dessin grossier du gabarit du filtre
    int getNumberOfBars();
    //! Retourne la largeur du bloc i entre 0 et 100 pour dessiner un gabarit du filtre
    float getBarLength(int i);
    //! Retourne la hauteur du bloc i entre 0 et 100 pour dessiner un gabarit du filtre
    int getBarValue(int i);

    // Type
    /*!
     * \brief Change le type du filtre
     * \param type Le nouveau type selon l'énumération définie dans la classe Filtre
     */
    void setType(int type);

    // Utilisation
    /*!
     * \brief Fonction permettant d'appliquer le filtre actuel sur un buffer.
     * Elle s'occupe en interne de la continuité du signal buffer après buffer (conserve les dernières valeurs du buffer précédent à chaque fois)
     * \param in Le buffer sur lequel on souhaite appliquer le filtre
     * \param out Le buffer où sera recopié le signal après filtrage (différent de in)
     * \param nBufferFrames Le nombre de frames à traiter
     */
    void applyOnBuffer(float * in, float * out, unsigned int nBufferFrames);
    /*!
     * \brief Fonction permettant d'appliquer un des biquad du filtre actuel sur un buffer, utilisée en interne
     * \param in Le buffer sur lequel on souhaite appliquer le filtre
     * \param out Le buffer où sera recopié le signal après filtrage (différent de in)
     * \param nBufferFrames Le nombre de frames à traiter
     * \param numBiquad Le numéro du biquad à appliquer (1 ou non-1)
     */
    void applyBiquad(float *in, float *out, unsigned int nBufferFrames, int numBiquad);
    /*!
     * \brief Permet d'affectuer au filtre une atténuation globale de 6dB ou non (évite quasiment toute saturation du filtre)
     * \param attenuation Booléen valant vrai si l'on souhaite atténuer la sortie
     */
    void setAttenuation(bool attenuation);

private:
    //! Fonction se contentant de s'assurer du fait que le filtre pourra recevoir correctement des buffers
    void update();

    //! Le CoeffsManager utilisé pour générer les biquads du filtre
    CoeffsManager m_coeffsManager;

    // Application du buffer
    //! Vecteur enregistrant les dernières valeurs du buffer précedent (évite de générer un grésillement)
    QVector< float > m_lastBuffer1;
    //! Vecteur enregistrant les dernières valeurs du buffer précedent (évite de générer un grésillement)
    QVector< float > m_lastValues1;
    //! Vecteur enregistrant les dernières valeurs du buffer précedent (évite de générer un grésillement)
    QVector< float > m_lastBuffer2;
    //! Vecteur enregistrant les dernières valeurs du buffer précedent (évite de générer un grésillement)
    QVector< float > m_lastValues2;


    // Paramètres
    //! Le type de filtre selon l'énumération définie dans la classe
    int m_type;
    //! L'amplitude actuelle du filtre entre 0 et AMP_STEPS (discrète)
    int m_amplitude;
    //! La fréquence caractéristique la plus basse d'un filtre passe-bande ou coupe-bande, ou la fréquence de coupure pour un passe-bas ou passe-haut
    float m_f1;
    //! La fréquence caractéristique la plus haute d'un filtre passe-bande ou coupe-bande, inutilisé pour un passe-bas ou passe-haut
    float m_f2;
    //! Booléen valant true si le filtre doit atténuer la sortie de 6dB
    bool m_applyAttenuation;
};

#endif // FILTER_H
