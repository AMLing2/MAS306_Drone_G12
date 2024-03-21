// ======================================================================
// \title  barometerComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for barometer component base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef BAROMETER_COMP_HPP_
#define BAROMETER_COMP_HPP_

#include <FpConfig.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Tlm/TlmString.hpp>
#include <Fw/Time/TimePortAc.hpp>
#include <Fw/Log/LogString.hpp>
#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Cmd/CmdPortAc.hpp>
#include <Fw/Cmd/CmdRegPortAc.hpp>
#include <Fw/Cmd/CmdResponsePortAc.hpp>
#include <Fw/Log/LogPortAc.hpp>
#if FW_ENABLE_TEXT_LOGGING == 1
#include <Fw/Log/LogTextPortAc.hpp>
#endif
#include <Fw/Tlm/TlmPortAc.hpp>
#include <Drv/SerialDriverPorts/SerialReadPortAc.hpp>
#include <Fw/Buffer/BufferSendPortAc.hpp>
#include <Drv/I2cDriverPorts/I2cPortAc.hpp>
#include <Fw/Time/TimePortAc.hpp>

namespace BarometerApp {

  //! \class barometerComponentBase
  //! \brief Auto-generated base for barometer component
  //!
  class barometerComponentBase :
    public Fw::ActiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class barometerComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return cmdIn[portNum]
    //!
    Fw::InputCmdPort* get_cmdIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return serialRecv[portNum]
    //!
    Drv::InputSerialReadPort* get_serialRecv_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  public:

    // ----------------------------------------------------------------------
    // Connect typed input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to cmdRegOut[portNum]
    //!
    void set_cmdRegOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdRegPort *port /*!< The port*/
    );

    //! Connect port to cmdResponseOut[portNum]
    //!
    void set_cmdResponseOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputCmdResponsePort *port /*!< The port*/
    );

    //! Connect port to eventOut[portNum]
    //!
    void set_eventOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputLogPort *port /*!< The port*/
    );

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Connect port to textEventOut[portNum]
    //!
    void set_textEventOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputLogTextPort *port /*!< The port*/
    );
#endif

    //! Connect port to tlmOut[portNum]
    //!
    void set_tlmOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputTlmPort *port /*!< The port*/
    );

    //! Connect port to serialBufferOut[portNum]
    //!
    void set_serialBufferOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputBufferSendPort *port /*!< The port*/
    );

    //! Connect port to I2cWrite[portNum]
    //!
    void set_I2cWrite_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Drv::InputI2cPort *port /*!< The port*/
    );

    //! Connect port to I2cRead[portNum]
    //!
    void set_I2cRead_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Drv::InputI2cPort *port /*!< The port*/
    );

    //! Connect port to Time[portNum]
    //!
    void set_Time_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputTimePort *port /*!< The port*/
    );

#if FW_PORT_SERIALIZATION

  public:

    // ----------------------------------------------------------------------
    // Connect serialization input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to cmdRegOut[portNum]
    //!
    void set_cmdRegOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to cmdResponseOut[portNum]
    //!
    void set_cmdResponseOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to eventOut[portNum]
    //!
    void set_eventOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Connect port to textEventOut[portNum]
    //!
    void set_textEventOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );
#endif

    //! Connect port to tlmOut[portNum]
    //!
    void set_tlmOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to serialBufferOut[portNum]
    //!
    void set_serialBufferOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to I2cWrite[portNum]
    //!
    void set_I2cWrite_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to I2cRead[portNum]
    //!
    void set_I2cRead_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to Time[portNum]
    //!
    void set_Time_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

#endif

  public:

    // ----------------------------------------------------------------------
    // Command registration
    // ----------------------------------------------------------------------

    //! \brief Register commands with the Command Dispatcher
    //!
    //! Connect the dispatcher first
    //!
    void regCommands();

  PROTECTED:

    // ----------------------------------------------------------------------
    // Component construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct a barometerComponentBase object
    //!
    barometerComponentBase(
        const char* compName = "" /*!< Component name*/
    );

    //! Initialize a barometerComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a barometerComponentBase object
    //!
    virtual ~barometerComponentBase();

  PROTECTED:

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port serialRecv
    //
    virtual void serialRecv_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &serBuffer, /*!< 
      Buffer containing data
      */
        Drv::SerialReadStatus &status /*!< 
      Status of read
      */
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports.
    // ----------------------------------------------------------------------
    // Call these functions directly to bypass the corresponding ports.
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port serialRecv
    //!
    void serialRecv_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &serBuffer, /*!< 
      Buffer containing data
      */
        Drv::SerialReadStatus &status /*!< 
      Status of read
      */
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Pre-message hooks for typed async input ports.
    // ----------------------------------------------------------------------
    // Each of these functions is invoked just before processing a message
    // on the corresponding port. By default they do nothing. You can
    // override them to provide specific pre-message behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for async input port serialRecv
    //!
    virtual void serialRecv_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &serBuffer, /*!< 
      Buffer containing data
      */
        Drv::SerialReadStatus &status /*!< 
      Status of read
      */
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port serialBufferOut
    //!
    void serialBufferOut_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer 
    );

    //! Invoke output port I2cWrite
    //!
    Drv::I2cStatus I2cWrite_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 addr, /*!< 
      I2C slave device address
      */
        Fw::Buffer &serBuffer /*!< 
      Buffer with data to read/write to/from
      */
    );

    //! Invoke output port I2cRead
    //!
    Drv::I2cStatus I2cRead_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 addr, /*!< 
      I2C slave device address
      */
        Fw::Buffer &serBuffer /*!< 
      Buffer with data to read/write to/from
      */
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of input ports
    // ----------------------------------------------------------------------

    //! Get the number of cmdIn input ports
    //!
    //! \return The number of cmdIn input ports
    //!
    NATIVE_INT_TYPE getNum_cmdIn_InputPorts();

    //! Get the number of serialRecv input ports
    //!
    //! \return The number of serialRecv input ports
    //!
    NATIVE_INT_TYPE getNum_serialRecv_InputPorts();


    // ----------------------------------------------------------------------
    // Enumerations for number of ports
    // ----------------------------------------------------------------------

    enum {
       NUM_CMDIN_INPUT_PORTS = 1,
       NUM_SERIALRECV_INPUT_PORTS = 1,
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

    //! Get the number of cmdRegOut output ports
    //!
    //! \return The number of cmdRegOut output ports
    //!
    NATIVE_INT_TYPE getNum_cmdRegOut_OutputPorts();

    //! Get the number of cmdResponseOut output ports
    //!
    //! \return The number of cmdResponseOut output ports
    //!
    NATIVE_INT_TYPE getNum_cmdResponseOut_OutputPorts();

    //! Get the number of eventOut output ports
    //!
    //! \return The number of eventOut output ports
    //!
    NATIVE_INT_TYPE getNum_eventOut_OutputPorts();

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Get the number of textEventOut output ports
    //!
    //! \return The number of textEventOut output ports
    //!
    NATIVE_INT_TYPE getNum_textEventOut_OutputPorts();
#endif

    //! Get the number of tlmOut output ports
    //!
    //! \return The number of tlmOut output ports
    //!
    NATIVE_INT_TYPE getNum_tlmOut_OutputPorts();

    //! Get the number of serialBufferOut output ports
    //!
    //! \return The number of serialBufferOut output ports
    //!
    NATIVE_INT_TYPE getNum_serialBufferOut_OutputPorts();

    //! Get the number of I2cWrite output ports
    //!
    //! \return The number of I2cWrite output ports
    //!
    NATIVE_INT_TYPE getNum_I2cWrite_OutputPorts();

    //! Get the number of I2cRead output ports
    //!
    //! \return The number of I2cRead output ports
    //!
    NATIVE_INT_TYPE getNum_I2cRead_OutputPorts();

    //! Get the number of Time output ports
    //!
    //! \return The number of Time output ports
    //!
    NATIVE_INT_TYPE getNum_Time_OutputPorts();


    enum {
       NUM_CMDREGOUT_OUTPUT_PORTS = 1,
       NUM_CMDRESPONSEOUT_OUTPUT_PORTS = 1,
       NUM_EVENTOUT_OUTPUT_PORTS = 1,
       NUM_TEXTEVENTOUT_OUTPUT_PORTS = 1,
       NUM_TLMOUT_OUTPUT_PORTS = 1,
       NUM_SERIALBUFFEROUT_OUTPUT_PORTS = 1,
       NUM_I2CWRITE_OUTPUT_PORTS = 1,
       NUM_I2CREAD_OUTPUT_PORTS = 1,
       NUM_TIME_OUTPUT_PORTS = 1,
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Connection status queries for output ports
    // ----------------------------------------------------------------------

    //! Check whether port cmdRegOut is connected
    //!
    //! \return Whether port cmdRegOut is connected
    //!
    bool isConnected_cmdRegOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port cmdResponseOut is connected
    //!
    //! \return Whether port cmdResponseOut is connected
    //!
    bool isConnected_cmdResponseOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port eventOut is connected
    //!
    //! \return Whether port eventOut is connected
    //!
    bool isConnected_eventOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Check whether port textEventOut is connected
    //!
    //! \return Whether port textEventOut is connected
    //!
    bool isConnected_textEventOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );
#endif

    //! Check whether port tlmOut is connected
    //!
    //! \return Whether port tlmOut is connected
    //!
    bool isConnected_tlmOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port serialBufferOut is connected
    //!
    //! \return Whether port serialBufferOut is connected
    //!
    bool isConnected_serialBufferOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port I2cWrite is connected
    //!
    //! \return Whether port I2cWrite is connected
    //!
    bool isConnected_I2cWrite_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port I2cRead is connected
    //!
    //! \return Whether port I2cRead is connected
    //!
    bool isConnected_I2cRead_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port Time is connected
    //!
    //! \return Whether port Time is connected
    //!
    bool isConnected_Time_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command opcodes
    // ----------------------------------------------------------------------

    enum {
      OPCODE_BAROMETER_LOCK = 0, /* A command to force an EVR reporting lock status. */
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handlers to implement
    // ----------------------------------------------------------------------

    //! Handler for command Barometer_lock
    /* A command to force an EVR reporting lock status. */
    virtual void Barometer_lock_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Pre-message hooks for async commands.
    // ----------------------------------------------------------------------
    // Each of these functions is invoked just before processing the
    // corresponding command. By default they do nothing. You can
    // override them to provide specific pre-command behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for command Barometer_lock
    //!
    virtual void Barometer_lock_preMsgHook(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command handler base-class functions.
    // Call these functions directly to bypass the command input port.
    // ----------------------------------------------------------------------

    //! Base-class handler function for command Barometer_lock
    //!
    void Barometer_lock_cmdHandlerBase(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdArgBuffer &args /*!< The command argument buffer*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Command response
    // ----------------------------------------------------------------------

    //! Emit command response
    //!
    void cmdResponse_out(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        Fw::CmdResponse response /*!< The command response*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event IDs
    // ----------------------------------------------------------------------

    enum {
      EVENTID_BAROMETER_REDY = 0, /* A notification on GPS lock acquired */
      EVENTID_BAROMETER_OFFLINE = 1, /* A warning on GPS lock lost */
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------

    //! Log event Barometer_redy
    //!
    /* A notification on GPS lock acquired */
    void log_ACTIVITY_HI_Barometer_redy(

    );


    //! Log event Barometer_offline
    //!
    /* A warning on GPS lock lost */
    void log_WARNING_HI_Barometer_offline(

    );


  PROTECTED:

    // ----------------------------------------------------------------------
    // Channel IDs
    // ----------------------------------------------------------------------

    enum {
      CHANNELID_PRESSURE = 0, //!< Channel ID for Pressure
      CHANNELID_CELCIUS = 1, //!< Channel ID for Celcius
      CHANNELID_FAHRENHEIT = 2, //!< Channel ID for Fahrenheit
    };

  PROTECTED:

    // ----------------------------------------------------------------------
    // Telemetry write functions
    // ----------------------------------------------------------------------

    //! Write telemetry channel Pressure
    //!
    /* The current latitude */
    void tlmWrite_Pressure(
        F32 arg /*!< The telemetry value*/,
        Fw::Time _tlmTime=Fw::Time() /*!< Timestamp. Default: unspecified, request from getTime port*/
    );

    //! Write telemetry channel Celcius
    //!
    /* The current longitude */
    void tlmWrite_Celcius(
        F32 arg /*!< The telemetry value*/,
        Fw::Time _tlmTime=Fw::Time() /*!< Timestamp. Default: unspecified, request from getTime port*/
    );

    //! Write telemetry channel Fahrenheit
    //!
    /* The current altitude */
    void tlmWrite_Fahrenheit(
        F32 arg /*!< The telemetry value*/,
        Fw::Time _tlmTime=Fw::Time() /*!< Timestamp. Default: unspecified, request from getTime port*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Time
    // ----------------------------------------------------------------------

    //! Get the time
    //!
    //! \return The current time
    //!
    Fw::Time getTime();



  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed input ports
    // ----------------------------------------------------------------------

    //! Input port cmdIn
    //!
    Fw::InputCmdPort m_cmdIn_InputPort[NUM_CMDIN_INPUT_PORTS];

    //! Input port serialRecv
    //!
    Drv::InputSerialReadPort m_serialRecv_InputPort[NUM_SERIALRECV_INPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port cmdRegOut
    //!
    Fw::OutputCmdRegPort m_cmdRegOut_OutputPort[NUM_CMDREGOUT_OUTPUT_PORTS];

    //! Output port cmdResponseOut
    //!
    Fw::OutputCmdResponsePort m_cmdResponseOut_OutputPort[NUM_CMDRESPONSEOUT_OUTPUT_PORTS];

    //! Output port eventOut
    //!
    Fw::OutputLogPort m_eventOut_OutputPort[NUM_EVENTOUT_OUTPUT_PORTS];

#if FW_ENABLE_TEXT_LOGGING == 1
    //! Output port textEventOut
    //!
    Fw::OutputLogTextPort m_textEventOut_OutputPort[NUM_TEXTEVENTOUT_OUTPUT_PORTS];
#endif

    //! Output port tlmOut
    //!
    Fw::OutputTlmPort m_tlmOut_OutputPort[NUM_TLMOUT_OUTPUT_PORTS];

    //! Output port serialBufferOut
    //!
    Fw::OutputBufferSendPort m_serialBufferOut_OutputPort[NUM_SERIALBUFFEROUT_OUTPUT_PORTS];

    //! Output port I2cWrite
    //!
    Drv::OutputI2cPort m_I2cWrite_OutputPort[NUM_I2CWRITE_OUTPUT_PORTS];

    //! Output port I2cRead
    //!
    Drv::OutputI2cPort m_I2cRead_OutputPort[NUM_I2CREAD_OUTPUT_PORTS];

    //! Output port Time
    //!
    Fw::OutputTimePort m_Time_OutputPort[NUM_TIME_OUTPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Calls for messages received on typed input ports
    // ----------------------------------------------------------------------

    //! Callback for port cmdIn
    //!
    static void m_p_cmdIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwOpcodeType opCode, /*!< 
      Command Op Code
      */
        U32 cmdSeq, /*!< 
      Command Sequence
      */
        Fw::CmdArgBuffer &args /*!< 
      Buffer containing arguments
      */
    );

    //! Callback for port serialRecv
    //!
    static void m_p_serialRecv_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &serBuffer, /*!< 
      Buffer containing data
      */
        Drv::SerialReadStatus &status /*!< 
      Status of read
      */
    );

  PRIVATE:

    // ----------------------------------------------------------------------
    // Message dispatch functions
    // ----------------------------------------------------------------------

    //! Called in the message loop to dispatch a message from the queue
    //!
    virtual MsgDispatchStatus doDispatch();


  PRIVATE:
    // ----------------------------------------------------------------------
    // Counter values for event throttling
    // ----------------------------------------------------------------------

  };

} // end namespace BarometerApp
#endif
