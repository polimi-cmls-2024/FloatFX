/*
  ==============================================================================

    SerialDevice.cpp
    Created: 22 May 2024 10:26:43am
    Author:  Davide

  ==============================================================================
*/

#include "SerialDevice.h"

#define kBPS 9600
const auto kNumberOfDecimalPlaces { 4 };


// NOTE: This is a very basic protocol without any error checking. To add error checking, you would want to calculate an error check (checksum, crc, etc)
//       and add that to the end of the packet. On the receiving end you would calculate the error check and compare it to the one sent

enum ParseState
{
    waitingForStartByte1,
    waitingForStartByte2,
    waitingForCommand,
    waitingForCommandDataSize,
    waitingForCommandData
};
ParseState parseState = waitingForStartByte1;
// NOTE: the start bytes are used to indicate the start of a packet.
//       they are arbitrary values, and when you choose them it is better the less likely they will appear in your data together
const int kStartByte1 = '*';
const int kStartByte2 = '~';

// NOTE: this is an example of list of command that can be sent and received
enum Command
{
    none,
    lightColor,
    tempo,
    chargingAlarmLevel,
    endOfList,
    read
};
const int kMaxPayloadSize = 20;
Command gTestCommandToExecute {Command::lightColor};

SerialDevice::SerialDevice ()
    : Thread (juce::String ("SerialDevice"))
{
    // start the serial thread reading data
    startThread ();

    // NOTE: this timer is used to send commands for testing purposes
    startTimer (500);
}

SerialDevice::~SerialDevice ()
{
    stopThread (500);
    closeSerialPort ();
}

void SerialDevice::init (juce::String newSerialPortName)
{
    serialPortName = newSerialPortName;
}

void SerialDevice::setLightColor (uint16_t color)
{
    if (serialPortOutput.get () == nullptr)
        return;

    // TODO: make helper function to assemble packets
    // TODO: use helper functions to break larger data into bytes

    const std::vector<uint8_t> data { kStartByte1, kStartByte2, Command::lightColor, 2,
                                      static_cast<uint8_t>(color & 0xff), static_cast<uint8_t>((color >> 8) & 0xff) };
    serialPortOutput->write (data.data (), data.size ());
}

void SerialDevice::setTempo (float tempoToSend)
{
    if (serialPortOutput.get () == nullptr)
        return;

    // NOTE: by sending an int instead of a float we don't have to worry about the receiving end storing floats in the same format as the send
    const auto tempo_as_int { static_cast<uint32_t>(tempoToSend * std::pow (10, kNumberOfDecimalPlaces)) };
    const std::vector<uint8_t> data { kStartByte1, kStartByte2, Command::tempo, 4,
                                      static_cast<uint8_t>(tempo_as_int & 0xff), static_cast<uint8_t>((tempo_as_int >> 8) & 0xff),
                                      static_cast<uint8_t>((tempo_as_int >> 16) & 0xff), static_cast<uint8_t>((tempo_as_int >> 24) & 0xff) };
    serialPortOutput->write (data.data (), data.size ());
}

void SerialDevice::setChargingAlarmLevel (uint8_t alarmType, uint8_t chargeLevel)
{
    if (serialPortOutput.get () == nullptr)
        return;

    const std::vector<uint8_t> data { kStartByte1, kStartByte2, Command::chargingAlarmLevel, 2,
                                      alarmType, chargeLevel };
    serialPortOutput->write (data.data (), data.size ());
}

void SerialDevice::read_data()
{
    char buffer[1000];
    juce::Logger::outputDebugString("ACQUIRING DATA \n");
    int bytes_obtained = serialPortInput->read(&buffer, 1000);
    std::string data(buffer, bytes_obtained);
    juce::Logger::outputDebugString(std::to_string(bytes_obtained));
    juce::Logger::outputDebugString(data);
    juce::Logger::outputDebugString("DATA ACQUIRED \n");
}

void SerialDevice::open (void)
{
    if (serialPortName.length () > 0)
        threadTask = ThreadTask::openSerialPort;
}

void SerialDevice::close (void)
{
    threadTask = ThreadTask::closeSerialPort;
}

bool SerialDevice::openSerialPort (void)
{
    serialPort = std::make_unique<SerialPort> ([] (juce::String, juce::String) {});
    bool opened = serialPort->open (serialPortName);

    if (opened)
    {
        SerialPortConfig serialPortConfig;
        serialPort->getConfig (serialPortConfig);
        serialPortConfig.bps = kBPS;
        serialPortConfig.databits = 8;
        serialPortConfig.parity = SerialPortConfig::SERIALPORT_PARITY_NONE;
        serialPortConfig.stopbits = SerialPortConfig::STOPBITS_1;
        serialPort->setConfig (serialPortConfig);

        serialPortInput = std::make_unique<SerialPortInputStream> (serialPort.get());
        serialPortOutput = std::make_unique<SerialPortOutputStream> (serialPort.get ());
        juce::Logger::outputDebugString ("Serial port: " + serialPortName + " opened");
    }
    else
    {
        // report error
        juce::Logger::outputDebugString ("Unable to open serial port:" + serialPortName);
    }

    return opened;
}

void SerialDevice::closeSerialPort (void)
{
    serialPortOutput = nullptr;
    serialPortInput = nullptr;
    if (serialPort != nullptr)
    {
        serialPort->close ();
        serialPort = nullptr;
    }
}

// NOTE: these handleXXXXCommand functions store the received data into the data model, and should also alert listeners of the change
//       I usually use ValueTrees for the data model, and use the property change callbacks to notify listeners
void SerialDevice::handleTempoCommand (uint8_t* data, int dataSize)
{
    if (dataSize != 4)
        return;
    const auto tempoAsInt { static_cast<uint32_t>(data [0] + (data [1] << 8) + (data [2] << 16) + (data [3] << 24)) };
    tempo = static_cast<float>(tempoAsInt / std::pow (10, kNumberOfDecimalPlaces));
}

void SerialDevice::handleLightColorCommand (uint8_t* data, int dataSize)
{
    if (dataSize != 2)
        return;
    lightColor = static_cast<uint16_t>(data [0] + (data [1] << 8));
}

void SerialDevice::handleChargingAlarmLevelCommand (uint8_t* data, int dataSize)
{
    if (dataSize != 2)
        return;

    const auto alarmIndex { data [0] };
    if (alarmIndex >= 2)
        return;

    alarmLevels [alarmIndex] = data [1];
}

void SerialDevice::handleCommand (uint8_t command, uint8_t* data, int dataSize)
{
    switch (command)
    {
        case Command::tempo: handleTempoCommand (data, dataSize); break;
        case Command::lightColor : handleLightColorCommand (data, dataSize); break;
        case Command::chargingAlarmLevel: handleChargingAlarmLevelCommand (data, dataSize); break;
    }
}

#define kSerialPortBufferLen 256
void SerialDevice::run ()
{   
    Message m;
    std::string number="";
    const int kMaxCommandDataBytes = 4;
    uint8_t commandData [kMaxCommandDataBytes];
    uint8_t command = Command::none;
    uint8_t commandDataSize = 0;
    uint8_t commandDataCount = 0;
    while (!threadShouldExit ())
    {
        switch (threadTask)
        {
            case ThreadTask::idle:
            {
                wait (1);
                if (serialPortName.isNotEmpty ())
                    threadTask = ThreadTask::openSerialPort;
            }
            break;
            case ThreadTask::openSerialPort:
            {
                if (openSerialPort ())
                {
                    threadTask = ThreadTask::processSerialPort;
                }
                else
                {
                    threadTask = ThreadTask::delayBeforeOpening;
                    delayStartTime = static_cast<uint64_t>(juce::Time::getMillisecondCounterHiRes ());
                }
            }
            break;

            case ThreadTask::delayBeforeOpening:
            {
                wait (1);
                if (juce::Time::getApproximateMillisecondCounter() > delayStartTime + 1000)
                    threadTask = ThreadTask::openSerialPort;
            }
            break;

            case ThreadTask::closeSerialPort:
            {
                closeSerialPort ();
                threadTask = ThreadTask::idle;
            }
            break;

            case ThreadTask::processSerialPort:
            {   
                isConnected = true;
                // handle reading from the serial port
                if ((serialPortInput != nullptr) && (!serialPortInput->isExhausted ()))
                {
                    auto  bytesRead = 0;
                    auto  dataIndex = 0;
                    uint8_t incomingData [kSerialPortBufferLen];

                    bytesRead = serialPortInput->read (incomingData, kSerialPortBufferLen);
                    if (bytesRead < 1)
                    {
                        wait (1);
                        continue;
                    }
                    else
                    {
                        // TODO: extract into function or class
                        // parseIncomingData (incomingData, bytesRead);
                        auto resetParser = [this, &command, &commandDataSize, &commandDataCount] ()
                        {
                            command = Command::none;
                            commandDataSize = 0;
                            commandDataCount = 0;
                            parseState = ParseState::waitingForStartByte1;
                        };
                        // parse incoming data
                        while (dataIndex < bytesRead)
                        {
                            const uint8_t dataByte = incomingData [dataIndex];
                            // NOTE: the following line prints each byte received in the debug output window
                            char character = (char)dataByte;
                            
                            //DBG(character);
                            //juce::Logger::outputDebugString(character);
                            std::string s = std::to_string(character);
                            //juce::Logger::outputDebugString (s);
                       
                            
                            /*if (character == X_AXIS || character == Y_AXIS)
                            {
                                m.direction = character;
                                m.verse = (char) incomingData[dataIndex + 1];
                                dataIndex += 2;
                                std::string number="";
                                do {
                                    character = (char) incomingData[dataIndex];
                                    number.push_back(character);
                                } while (character >= '0' && character <= '9');
                                m.value = std::stoi(number);
                                dataIndex += number.length();

                                messages.push(m);

                                DBG("AXIS:");
                                DBG(m.direction);
                                DBG("VERSE:");
                                DBG(m.verse);
                                DBG("VALUE");
                                DBG(m.value);
                                DBG("\n");
                            }
                            else {
                                ++dataIndex;
                            }*/
                            if (character == X_AXIS || character == Y_AXIS)
                            {
                                m.direction = character;
                            }
                            if (character == PLUS_SIGN || character == MINUS_SIGN)
                            {
                                m.verse = character;
                            }
                            if (character >= '0' && character <= '9')
                                number.push_back(character);
                            if (character == '\r') {
                                m.value = stoi(number);
                                messages.push(m);
                                number = "";
                                /*DBG("AXIS:");
                                DBG(m.direction);
                                DBG("VERSE:");
                                DBG(m.verse);
                                DBG("VALUE");
                                DBG(m.value);
                                DBG("\n");*/
                            }
                            ++dataIndex;
                            
                        }
                    }
                }
            }
            break;
        }
    }
}

void SerialDevice::timerCallback ()
{   /*
    if (serialPortOutput.get () == nullptr)
        return;
    switch (gTestCommandToExecute)
    {
        case Command::lightColor:
        {
            juce::Logger::outputDebugString ("setting lightColor");
            setLightColor (100);
            gTestCommandToExecute = Command::tempo;
        }
        break;
        case Command::tempo:
        {
            juce::Logger::outputDebugString ("setting tempo");
            setTempo (120.f);
            gTestCommandToExecute = Command::chargingAlarmLevel;
        }
        break;
        case Command::chargingAlarmLevel:
        {
            juce::Logger::outputDebugString ("setting ChargingAlarmLevel");
            setChargingAlarmLevel(0, 50);
            gTestCommandToExecute = Command::tempo;
        }
        //case Command::read:
        //{
        //   juce::Logger::outputDebugString("Reading bytes");
        //    read_data();
        //}
        break;
    }*/
}