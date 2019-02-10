#ifndef WAVEFORMPIXMAP_H
#define WAVEFORMPIXMAP_H

#include <QPixmap>
#include <QPainter>

/*!
 * \brief Classe permettant d'afficher une waveform à l'écran.
 * Cette classe reçoit des informations sur l'affichage (synchro, période)
 * ainsi que des trames successives. A partir de là, elle crée en interne
 * une pixmap qui peut être dessinée à l'aide de la fonction \a draw().
 */
class WaveformPixmap
{

public:

    // Construction
    /*!
     * \brief Instancie une WaveformPixmap dont la longueur maximale est fournie en paramètre
     * \param width La longueur maximale de la pixmap
     */
    WaveformPixmap(int width);
    ~WaveformPixmap();

    // Contrôle
    /*!
     * \brief Règle la dilatation de la forme d'onde
     * \param frames Le nombre de frames que l'on souhaite afficher sur la longueur maximale de la WaveformPixmap
     * \return Le nombre de frames qui seront effectivement affichés sur la longueur maximale
     */
    int setDisplayedFrames(int frames);
    /*!
     * \brief Active ou désactive la synchronisation de la forme d'onde sur une période
     * \param enable Booléen à vrai si l'on souhaite synchroniser la forme d'onde
     */
    void enableSynchronization(bool enable);

    // Réception des données à afficher
    /*!
     * \brief Fonction permettant d'ajouter une trame audio à la WaveformPixmap
     * \param data Pointeur sur le début de la trame
     * \param frames Le nombre de samples à ajouter depuis l'argument data
     * \param time La date de la trame, exprimée en nombre de samples
     */
    void update(const float * data, unsigned int frames, unsigned int time);

    // Dessin
    /*!
     * \brief Fonction permettant de dessiner la WaveformPixmap dans un QPainter
     * \param painter Le painter sur lequel dessiner
     * \param allowedWidth La largeur que peut prendre au maximum la pixmap
     * \param allowedHeight La hauteur que peut prendre au maximum la pixmap
     */
    void draw(QPainter & painter, int allowedWidth, int allowedHeight);

private:

    // Pour la synchronisation
    /*!
     * \brief Fonction retrouvant l'indice du début estimé d'une période sur une trame donnée
     * \param data Le début de la trame
     * \param frames Le nombre de frames la composant
     * \return L'indice du début d'une période
     */
    unsigned int findPeriodStart(const float * data, unsigned int frames);

    // Actualisation en interne
    //! \brief Fonction interne d'affichage lorsqu'il y a peu de samples à afficher
    void updateFewPoints(const float * data, unsigned int frames, unsigned int time);
    //! \brief Fonction interne d'affichage lorsqu'il y a beaucoup de samples à afficher
    void updateSeveralPoints(const float * data, unsigned int frames, unsigned int time);

    //! Espace sur l'écran séparant deux frames successives (nombre flottant)
    float m_pixelsPerFrame;

    // Dimensions
    //! Largeur maximale de la pixmap
    int m_width;
    //! Hauteur maximale de la pixmap
    int m_height;

    // Etat
    //! Le curseur sur le buffer de données permettant d'obtenir l'effet de défilement
    float        m_currentPixel;
    //! La date de la dernière trame ajoutée
    unsigned int m_currentTime;
    //! Vaut vrai si il est nécessaire de redessiner toute la pixmap au prochain appel de \a draw()
    bool         m_invalidated;
    //! Vaut vrai s'il faut synchroniser l'affichage sur une période
    bool         m_synchro;
    //! Vaut vrai lors du dessin effectif. Utile dans le cas critique où on demande un nouveau rafraichissement avant la fin du dernier
    bool         m_painting;

    // Le trigger
    //! Variable permettant de déterminer le début d'une période, il s'agit d'un seuil calculé à partir des trames récentes
    float m_trigger;

    // Le pen
    //! Gradient permettant l'affichage esthétique de la waveform
    QLinearGradient * m_gradient;
    //! Brush contenant le gradient
    QBrush          * m_brush;
    //! Pen contenant la brush
    QPen            * m_pen;

    // Données
    //! La pixmap contenant les données de l'instance de WaveformPixmap
    QPixmap m_pixmap;
};

#endif // WAVEFORMPIXMAP_H
