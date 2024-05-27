/*
* Implementation of SerialDevice.h
*/

#include <chrono>
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
    startTimer(100);
    serialPortName = newSerialPortName;
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
        
        serialPortInput = std::make_unique<SerialPortInputStream>(serialPort.get());
        serialPortOutput = std::make_unique<SerialPortOutputStream>(serialPort.get());
        
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
    bool foundMessage = false;
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
                    foundMessage = false;
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
                using namespace std::chrono;
                openedAtLeastOnce = true;
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
                            //DBG(character);
                            
                            if (character == X_AXIS || character == Y_AXIS)
                            {
                                foundMessage = true;
                                long time_start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                                m.direction = character;
                            }
                            if (character == PLUS_SIGN || character == MINUS_SIGN  && foundMessage)
                            {
                                m.verse = character;
                            }
                            if (character >= '0' && character <= '9' && foundMessage)
                                number.push_back(character);
                            if (character == '\r' && foundMessage) {
                                
                                if (number.empty())
                                    m.value = 0;
                                else {
                                    m.value = stoi(number);
                                }
                                messages.push(m);
                                number = "";
                                /*DBG("AXIS:");
                                DBG(m.direction);
                                DBG("VERSE:");
                                DBG(m.verse);
                                DBG("VALUE");
                                DBG(m.value);
                                DBG("\n");*/
                                long time_end = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                                //DBG("TIME PASSED");
                                //DBG(time_end - time_start);
                                foundMessage = false;
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

// This function allow to restart the port if the Arduino accidentally disconnects from the system.
void SerialDevice::timerCallback ()
{   
    if (serialPort != nullptr) {
        if (serialPort->exists()) {
        }
        else {
            closeSerialPort();
            threadTask = ThreadTask::openSerialPort;
        }
    }
}