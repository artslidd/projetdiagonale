#include "midiengine.h"
#include <QMessageBox>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QList>
#include "../constants.h"

#define MIDI_CONTROLLER_ID_MSB_MASK 0x0F

#define MIDI_COMMAND_MASK     0xF0
#define MIDI_COMMAND_CONTROL  0xB0
#define MIDI_COMMAND_NOTE_ON  0x90
#define MIDI_COMMAND_NOTE_OFF 0x80

void midiEngineCallback(double deltaTime, std::vector< unsigned char > *message, void *engine) {
    Q_UNUSED(deltaTime);

    unsigned char command = (*message)[0];

    if ((command & MIDI_COMMAND_MASK) == MIDI_COMMAND_CONTROL) {
        if (message->size() != 3)
            return;

        unsigned short msb     = ((*message)[0]) & MIDI_CONTROLLER_ID_MSB_MASK;
        unsigned short lsb     =  (*message)[1];
        unsigned short control =  (msb << 8) | (lsb & 0x00FF);
        unsigned char  value   =  (*message)[2];

        ((MidiEngine *)engine)->processControlInput((int) control, value);
    } else if ((command & MIDI_COMMAND_MASK) == MIDI_COMMAND_NOTE_ON) {
        if (message->size() < 2)
            return;

        ((MidiEngine *)engine)->processNoteInput((*message)[1], true);
    } else if ((command & MIDI_COMMAND_MASK) == MIDI_COMMAND_NOTE_OFF) {
        if (message->size() < 2)
            return;

        ((MidiEngine *)engine)->processNoteInput((*message)[1], false);
    } else {
        return;
    }
}

MidiEngine::MidiEngine() : m_rtMidi(NULL), m_learningControl(false), m_inputsBlocked(false), m_lowerNote(-1), m_upperNote(-1)
{
    qRegisterMetaType< MidiEngine::MidiSoftwareInput >("MidiEngine::MidiSoftwareInput");

    try {
        m_rtMidi = new RtMidiIn();
    } catch (RtMidiError & error) {
        QMessageBox msgBox;
        msgBox.setText("L'interface MIDI n'a pas pu démarrer :\n" + QString(error.getMessage().c_str()));
        msgBox.exec();
    }

    const QStringList inputs = getAvailableInputs();
    if (!inputs.isEmpty())
        selectSource(inputs.first());
}

MidiEngine::~MidiEngine() {
    delete m_rtMidi;
}

QStringList MidiEngine::getAvailableInputs() {
    if (m_rtMidi == NULL)
        return QStringList();

    unsigned int nPorts = m_rtMidi->getPortCount();
    QStringList result;
    for ( unsigned int i=0; i<nPorts; i++ ) {
        try {
            result << QString(m_rtMidi->getPortName(i).c_str());
        } catch ( RtMidiError &error ) {
            QMessageBox msgBox; msgBox.setText("L'interface MIDI a rencontré un problème inattendu :\n" + QString(error.getMessage().c_str()));
            msgBox.exec();
        }
    }
    return result;
}

void MidiEngine::selectSource(const QString &source) {
    if (m_rtMidi != NULL)
        delete m_rtMidi;

    if (m_learningControl) {
        m_learningControl = false;
        emit midiInputLearningFailed(m_inputBeingLearned);
    }

    try {
        m_rtMidi = new RtMidiIn();

        unsigned int nPorts = m_rtMidi->getPortCount(), result = 0;
        for ( unsigned int i=0; i<nPorts; i++ )
            if (QString(m_rtMidi->getPortName(i).c_str()) == source)
                result = i;

        m_rtMidi->openPort(result);
        m_rtMidi->setCallback(&midiEngineCallback, (void *) this);
        m_rtMidi->ignoreTypes(true);
    } catch ( RtMidiError &error ) {
        QMessageBox msgBox; msgBox.setText("L'interface MIDI a rencontré un problème inattendu :\n" + QString(error.getMessage().c_str()));
        msgBox.exec();
        return;
    }
}

void MidiEngine::clearAssociations() {
    m_inputUsingControl.clear();
    m_controlAssociatedToInput.clear();
}

void MidiEngine::learnInput(MidiSoftwareInput input) {
    if (m_learningControl)
        emit midiInputLearningFailed(m_inputBeingLearned);

    m_learningControl = true;
    m_inputBeingLearned = input;
}

void MidiEngine::processControlInput(int control, unsigned char value) {
    if (!m_learningControl) {
        if (m_inputsBlocked)
            return;

        if (m_inputUsingControl.contains(control))
            emit midiInputEvent(m_inputUsingControl[control], (int) value);
    } else {
        if (m_inputBeingLearned <= MidiPianoOff)
            return;

        if (m_controlAssociatedToInput.contains(m_inputBeingLearned)) { // Si l'input était déjà sur "appris"
            m_inputUsingControl.remove(m_controlAssociatedToInput[m_inputBeingLearned]);
            m_controlAssociatedToInput.remove(m_inputBeingLearned);
        }

        if (m_inputUsingControl.contains(control)) { // Si cette touche MIDI servait déjà pour un bouton de l'application
            emit midiInputLearningFailed(m_inputUsingControl[control]);

            m_controlAssociatedToInput.remove(m_inputUsingControl[control]);
            m_inputUsingControl.remove(control);
        }

        m_learningControl = false;
        m_inputUsingControl[control] = m_inputBeingLearned;
        m_controlAssociatedToInput[m_inputBeingLearned] = control;
        emit midiInputLearned(m_inputBeingLearned, m_controlAssociatedToInput[m_inputBeingLearned]);
    }
}

void MidiEngine::processNoteInput(int note, bool state) {
    if (!m_learningControl) {
        if (m_inputsBlocked)
            return;

        if (note >= m_lowerNote && note <= m_upperNote) {
            int value = note - 12 * ((m_lowerNote >= 0 ? m_lowerNote : 0) / 12);

            if (m_upperNote != -1 && m_upperNote - m_lowerNote <= 24)
                value += 12;

            value++;

            if (value > SPECTRUM_NUMBER_OF_OCTAVES * 12 - 3)
                value = SPECTRUM_NUMBER_OF_OCTAVES * 12 - 3;

            emit midiInputEvent(state ? MidiPianoOn : MidiPianoOff, value);
        }
    } else {
        if (m_inputBeingLearned >= MidiPianoOn)
            return;

        if (m_inputBeingLearned == MidiPianoMin) {
            m_lowerNote = note;

            m_learningControl = false;
            emit midiInputLearned(MidiPianoMin, note);
        } else {
            m_upperNote = note;

            m_learningControl = false;
            emit midiInputLearned(MidiPianoMax, note);
        }
    }
}

void MidiEngine::blockInputs() {
    m_inputsBlocked = true;
}

void MidiEngine::unblockInputs() {
    if (m_learningControl)
        emit midiInputLearningFailed(m_inputBeingLearned);

    m_learningControl = false;
    m_inputsBlocked = false;
}

void MidiEngine::saveConfig(){

    QFile file(m_currentConfigFileName);

    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox msgBox;
        msgBox.setText("Impossible d'écrire dans le fichier de configuration. Peut-être n'avez-vous pas les droits d'écriture dans le dossier spécifié.");
        msgBox.exec();

        return;
    }

    QDataStream out(&file);
    QList<int> keyList = m_inputUsingControl.keys();
    QList<MidiSoftwareInput> valueList =  m_inputUsingControl.values();

    out << keyList;
    out << valueList;
    out << m_lowerNote;
    out << m_upperNote;

    file.close();
}



void MidiEngine::openFile(QString filename){
    m_currentConfigFileName = filename;
    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox msgBox;
        msgBox.setText("Impossible d'ouvrir le fichier de configuration spécifié.");
        msgBox.exec();

        return;
    }


    QDataStream in(&file);

    m_inputUsingControl.clear();
    m_controlAssociatedToInput.clear();


    QList<int> keyList;
    QList<int> valueList;

    try {
        in >> keyList;
        in >> valueList;
        in >> m_lowerNote;
        in >> m_upperNote;

        if(in.status() != QDataStream::Ok){
            file.close();

            QMessageBox msgBox;
            msgBox.setText("Erreur lors de la lecture du fichier de configuration.");
            msgBox.exec();

            return;
        }
    } catch (std::bad_alloc& e) {
        QMessageBox msgBox;
        msgBox.setText("Erreur lors de la lecture du fichier de configuration :\n" + QString(e.what()));
        msgBox.exec();

        return;
    }

    for (int i = 0 ; i < keyList.length() ; ++i){
        m_inputUsingControl[keyList[i]] = (MidiSoftwareInput)valueList[i];
        m_controlAssociatedToInput[(MidiSoftwareInput)valueList[i]] = keyList[i];
        emit midiInputLearned((MidiSoftwareInput)valueList[i], keyList[i]);
    }

    if (m_lowerNote >= 0)
        emit midiInputLearned(MidiPianoMin, m_lowerNote);
    if (m_upperNote >= 0)
        emit midiInputLearned(MidiPianoMax, m_upperNote);

    file.close();
}

void MidiEngine::createConfigFile(QString filename) {
    m_currentConfigFileName = filename;
    QFile file(m_currentConfigFileName);

    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox msgBox;
        msgBox.setText("Impossible d'écrire dans le fichier de configuration. Peut-être n'avez-vous pas les droits d'écriture dans le dossier spécifié.");
        msgBox.exec();

        return;
    }

    file.close();

    saveConfig();
}
