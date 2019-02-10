#include "loopcheckbox.h"
#include <QDebug>

LoopCheckBox::LoopCheckBox(QWidget *parent) : QCheckBox(parent)
{
    module = dynamic_cast <SamplerModule *> (parent);
    connect(this, SIGNAL(clicked()), this, SLOT(btnStateChanged()));
    connect(module, SIGNAL(UpdateAllLoopStates()), this, SLOT(updateState()));

}

void LoopCheckBox::btnStateChanged(){
    module->setLoopStates(this->objectName().right(2).toInt());
}

void LoopCheckBox::updateState(){
    if(this->objectName().right(2).toInt() == module->getLoopStates(0) || this->objectName().right(2).toInt() == module->getLoopStates(1)){
       this->setChecked(true);
    }else{
       this->setChecked(false);
    }
}
