#ifndef PROGRESS_H
#define PROGRESS_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>

/*!
 * \brief La classe ProgressWidget se contente d'ajouter à une QProgressBar les fonctions de base nécessaires pour servir de barre de navigation.
 * En particulier, il est possible de cliquer sur la barre pour changer l'avancement de la lecture d'un son. Il ne s'agit pas
 * d'une simple ProgressBar cliquable, mais bien d'une barre de navigation gérant une longueur en secondes et travaillant sur des temps.
 * Le widget peut optionnellement être associé à un label indiquant la progression actuelle.
 */
class ProgressWidget : public QProgressBar
{
    Q_OBJECT

public:

    // Construction et initialisation
    ProgressWidget();

    /*!
     * \brief Fonction à appeler après l'instanciation dans le cas où l'on souhaite que le widget soit associé à un petit texte de progression du type x:xx / y:yy
     * \param label Adresse du label à maintenir actualisé en même temps que le widget
     */
    void link(QLabel * label);

    // Events
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

    // Settings
    /*!
     * \brief Permet de changer la longueur de la barre
     * \param seconds La nouvelle longueur, en secondes
     */
    void setTotalLength(int seconds);

    // Remise à zéro
    //! Remise à zéro de la progression (typiquement à la fin de la lecture)
    void reset();

signals:

    // On clique
    /*!
     * \brief Signale que l'utilisateur a demandé une nouvelle position
     * \param seconds La nouvelle position en secondes
     */
    void userRequest(float seconds);

public slots:

    // Valeur
    /*!
     * \brief Fonction permettant de changer l'avancement de la barre
     * \param value La nouvelle valeur de la barre entre 0 et 1000
     */
    void setProgress(int value);

    // Actualisation de la progression
    /*!
     * \brief Fonction permettant de changer l'avancement de la barre
     * \param value La nouvelle valeur de la barre en secondes
     */
    bool setProgressInSeconds(int seconds);

private:

    // En interne
    //! Fonction utilisée en interne pour afficher la nouvelle progression
    void updateLabel();

    // Petite vignette
    //! Pointeur vers l'éventuel label affichant l'état d'avancement
    QLabel * m_label;

    // Durée totale
    //! La durée total du morceau en cours de lecture, en secondes
    int m_totalSeconds;

    // Pour savoir si on est en train de cliquer
    //! Booléen d'interface utilisateur valant true si l'on est en train de cliquer-déplacer sur la barre
    bool m_selectionInProgress;
};

#endif // PROGRESS_H
