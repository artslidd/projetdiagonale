#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QVector>
#include <QMutex>

// Attention : ne pas fournir un buffer plus long que la taille du RingBuffer
/*!
 * \brief Classe s'occupant de stocker les trames audio au fur et à mesure qu'elle arrivent.
 * L'objectif de cette classe est de fournir des temps d'écriture acceptables, pour des
 * lectures instantanées et sur une plage de mémoire contiguë.
 */
class RingBuffer
{

public:

    // Construction
    /*!
     * \brief Construit un RingBuffer vide.
     * \param size La taille du RingBufer point de vue utilisateur (ne pas se soucier de la duplication)
     */
    RingBuffer(unsigned int size);
    ~RingBuffer();

    // Ajout et récupération
    /*!
     * \brief Ajoute une trame de longueur \a frames() au buffer.
     * \param data Une trame, dont la longueur ne doit pas dépasser la taille du buffer
     * \param frames Le nombre de samples contenus dans la trame
     */
    void addBuffer(float * data, unsigned int frames);
    /*!
     * \brief Fournit un accès direct aux données stockées dans le RingBuffer.
     * \return Un pointeur sur les données stockées en contiguë. En 0, les samples sont les plus anciens.
     */
    const float * rawData();

    // Accesseurs
    /*!
     * \brief Renvoie la taille du buffer du point de vue utilisateur
     * \return La taille du buffer
     */
    unsigned int getSize() {return m_half;}
    /*!
     * \brief Renvoie le nombre de samples total ajoutées depuis la création du buffer
     * \return Le nombre de samples
     */
    unsigned int getTime() {return m_time;}

    // Pour les autres threads
    /*!
     * \brief Avertit le RingBuffer que l'on va lire ses données (nécessaire uniquement depuis un thread séparé du thread d'écriture)
     */
    void willReadInSeparateProcess() {m_mutex.lock();}
    /*!
     * \brief Prévient le RingBuffer que les données ont été lues
     */
    void finishedReadingInSeparateProcess() {m_mutex.unlock();}

private:

    // Caractéristiques
    //! La taille réelle du buffer (deux fois la taille maximale stockable
    unsigned int m_size;
    //! La taille du buffer du point de vue de l'utilisateur
    unsigned int m_half;

    // Données
    //! Le pointeur vers les données
    float *      m_data;

    // Etat
    //! Indice du curseur du RingBuffer, marque le début de la zone de stockage contiguë
    unsigned int m_index;
    //! Le nombre total de frames ajoutées
    unsigned int m_time;

    // Sécurisation
    //! Le mutex d'écriture
    QMutex m_mutex;
};

#endif // RINGBUFFER_H
