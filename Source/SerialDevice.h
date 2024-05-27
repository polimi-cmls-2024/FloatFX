/*
* Definition of the class that implements the interconnection between JUCE and Arduino. 
* It inherits the juce::Thread class as it works as a separate background thread.
*/


#pragma once

#include <JuceHeader.h>
#include <stack>
#include "Message.h"

// This class implements the interconnection between JUCE and Arduino. 
// It inherits the juce::Thread class as it works as a separate background thread
class SerialDevice : private juce::Thread, private juce::Timer
{
public:
    SerialDevice ();
    ~SerialDevice ();
    void open (void);
    void close (void);
    void init (juce::String newSerialPortName);

    //Data structure used to store all the data coming from Arduino, in form of objects of type Message. See Message.h.
    std::stack<Message> messages;
    bool isConnected = false;
private:
    enum class ThreadTask
    {
        idle,
        delayBeforeOpening,
        openSerialPort,
        closeSerialPort,
        processSerialPort,
    };

    bool openedAtLeastOnce{ false };

    juce::String serialPortName;
    std::unique_ptr<SerialPort> serialPort;
    std::unique_ptr<SerialPortInputStream> serialPortInput;
    std::unique_ptr<SerialPortOutputStream> serialPortOutput;
    ThreadTask threadTask { ThreadTask::idle };
    uint64_t delayStartTime { 0 };

    bool openSerialPort (void);
    void closeSerialPort (void);

    //This is where the magic happens. This function is responsible for acquiring data bytes
    //from Arduino serial port, and parsing them to create the Message objects used by the JUCE processing
    void run () override;
    void timerCallback () override;
};