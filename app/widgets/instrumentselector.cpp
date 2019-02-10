#include "instrumentselector.h"
#include <QString>

#include <QMessageBox>

InstrumentSelector::InstrumentSelector(QWidget *parent) : QPushButton(parent)
{
    module = dynamic_cast <SamplerModule *> (parent);
    setText("");
    setFont(QFont("Arial", 30, QFont::Bold, true));

    this->setStyleSheet("QPushButton {"
                        "    font-size:30;"
                        "    border-color: grey;"
                        "    background-color: rgba(255,255,255,0.1);"
                        "    background-image: url(\":/button_bckg.png\");"
                        "}"
                       );

    connect(this, SIGNAL(clicked()), this, SLOT(setState()));
    connect(module, SIGNAL(resetState()), this, SLOT(reset()));
    connect(module, SIGNAL(updateAllButtons()), this, SLOT(updateState()));
    connect(module, SIGNAL(updateAllButtons()), this, SLOT(setChord()));
}

void InstrumentSelector::setState() {
    m_chord_state = module->getButtonState(3,this->objectName().right(2).toInt()-1); // Etat de l'accord associé
    m_state = module->getButtonState(this->objectName().mid(8,1).toInt()-1, this->objectName().right(2).toInt()-1);
    if(m_chord_state==0){
        m_state = (m_state+1)%(module->getInstNbSamples(this->objectName().mid(8,1).toInt()-1, m_chord_state)+1); // Etat du bouton
    }else{
        m_state = (m_state+1)%(module->getInstNbSamples(this->objectName().mid(8,1).toInt()-1, m_chord_state-1)+1); // Etat du bouton
    }

    if(m_state!=0){
        if(m_chord_state==0){
            // Active le chord
            module->setButtonState(3, this->objectName().right(2).toInt()-1, 1);
        }
        setText(QString::number(m_state));
    }else{
        setText("");
    }
    module->setButtonState(this->objectName().mid(8,1).toInt()-1, this->objectName().right(2).toInt()-1,m_state);
}

void InstrumentSelector::updateState() {
    m_chord_state = module->getButtonState(3,this->objectName().right(2).toInt()-1); // Etat de l'accord associé
    m_state = module->getButtonState(this->objectName().mid(8,1).toInt()-1, this->objectName().right(2).toInt()-1);

    if(m_state!=0){
        if(m_chord_state==0){
            // Active le chord
            module->setButtonState(3, this->objectName().right(2).toInt()-1, 1);
        }
        setText(QString::number(m_state));
    }else{
        setText("");
    }
    module->setButtonState(this->objectName().mid(8,1).toInt()-1, this->objectName().right(2).toInt()-1,m_state);
}


void InstrumentSelector::setChord(){
    m_chord = module->getChordTitle(this->objectName().right(2).toInt()-1);
    m_chord_state = module->getButtonState(3,this->objectName().right(2).toInt()-1);
    if(m_state!=0 and m_chord_state!=0){
        if(m_chord=="Do"){
            this->setStyleSheet("QPushButton { background-color: rgba(230,0,58, 0.8); }");
        }else if(m_chord=="Dom"){
            this->setStyleSheet("QPushButton { background-color: rgba(244,110,143, 0.8); }");
        }else if(m_chord=="Do7"){
            this->setStyleSheet("QPushButton { background-color: rgba(174,44,76, 0.8); }");
        }

        else if(m_chord=="Re"){
            this->setStyleSheet("QPushButton { background-color: rgba(0,204,0, 0.8); }");
        }else if(m_chord=="Rem"){
            this->setStyleSheet("QPushButton { background-color: rgba(103,230,103, 0.8); }");
        }else if(m_chord=="Re7"){
            this->setStyleSheet("QPushButton { background-color: rgba(38,153,38, 0.8); }");
        }

        else if(m_chord=="Mi"){
            this->setStyleSheet("QPushButton { background-color: rgba(255,146,0, 0.8); }");
        }else if(m_chord=="Mim"){
            this->setStyleSheet("QPushButton { background-color: rgba(255,195,115, 0.8); }");
        }else if(m_chord=="Mi7"){
            this->setStyleSheet("QPushButton { background-color: rgba(191,130,48, 0.8); }");
        }

        else if(m_chord=="Fa"){
            this->setStyleSheet("QPushButton { background-color: rgba(11,97,164, 0.8); }");
        }else if(m_chord=="Fam"){
            this->setStyleSheet("QPushButton { background-color: rgba(102,163,210, 0.8); }");
        }else if(m_chord=="Fa7"){
            this->setStyleSheet("QPushButton { background-color: rgba(37,86,123, 0.8); }");
        }

        else if(m_chord=="Sol"){
            this->setStyleSheet("QPushButton { background-color: rgba(255,211,0, 0.8); }");
        }else if(m_chord=="Solm"){
            this->setStyleSheet("QPushButton { background-color: rgba(255,231,115, 0.8); }");
        }else if(m_chord=="Sol7"){
            this->setStyleSheet("QPushButton { background-color: rgba(191,167,48, 0.8); }");
        }

        else if(m_chord=="La"){
            this->setStyleSheet("QPushButton { background-color: rgba(57,20,175, 0.8); }");
        }else if(m_chord=="Lam"){
            this->setStyleSheet("QPushButton { background-color: rgba(135,110,215, 0.8); }");
        }else if(m_chord=="La7"){
            this->setStyleSheet("QPushButton { background-color: rgba(65,44,132, 0.8); }");
        }

        else if(m_chord=="Si"){
            this->setStyleSheet("QPushButton { background-color: rgba(166,0,166, 0.8); }");
        }else if(m_chord=="Sim"){
            this->setStyleSheet("QPushButton { background-color: rgba(210,95,210, 0.8); }");
        }else if(m_chord=="Si7"){
            this->setStyleSheet("QPushButton { background-color: rgba(124,31,124, 0.8); }");
        }

        else{
            this->setStyleSheet("QPushButton { background-color: rgba(255,255,255,0.2); }");
        }
    }else{
        this->setStyleSheet("QPushButton { background-color: rgba(255,255,255,0.1); }");
    }
}

void InstrumentSelector::reset(){
    m_state = 0,
    module->setButtonState(this->objectName().mid(8,1).toInt()-1, this->objectName().right(2).toInt()-1,m_state);
    this->setStyleSheet("QPushButton { background-color: rgba(255,255,255,0.1); }");
    this->setText("");
}
