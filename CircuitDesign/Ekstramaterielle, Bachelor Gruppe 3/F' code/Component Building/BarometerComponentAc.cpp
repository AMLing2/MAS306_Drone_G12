// ======================================================================
// \title  barometerComponentAc.cpp
// \author Auto-generated
// \brief  cpp file for barometer component base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <cstdio>
#include <FpConfig.hpp>
#include <RPI/Barometer/BarometerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_ENABLE_TEXT_LOGGING
#include <Fw/Types/String.hpp>
#endif


namespace BarometerApp {

  // ----------------------------------------------------------------------
  // Anonymous namespace to prevent name collisions
  // ----------------------------------------------------------------------

  namespace {

    typedef enum {
      BAROMETER_COMPONENT_EXIT = Fw::ActiveComponentBase::ACTIVE_COMPONENT_EXIT,
      SERIALRECV_SERIALREAD,
      CMD_BAROMETER_LOCK
    } MsgTypeEnum;

    // Get the max size by doing a union of the input port serialization sizes.

    typedef union {
      BYTE port1[Drv::InputSerialReadPort::SERIALIZED_SIZE];
      BYTE cmdPort[Fw::InputCmdPort::SERIALIZED_SIZE];
    } BuffUnion;

    // Define a message buffer class large enough to handle all the
    // asynchronous inputs to the component

    class ComponentIpcSerializableBuffer :
      public Fw::SerializeBufferBase
    {

      public:

        enum {
          // Max. message size = size of data + message id + port
          SERIALIZATION_SIZE =
            sizeof(BuffUnion) +
            sizeof(NATIVE_INT_TYPE) +
            sizeof(NATIVE_INT_TYPE)
        };

        NATIVE_UINT_TYPE getBuffCapacity() const {
          return sizeof(m_buff);
        }

        U8* getBuffAddr() {
          return m_buff;
        }

        const U8* getBuffAddr() const {
          return m_buff;
        }

      private:
        // Should be the max of all the input ports serialized sizes...
        U8 m_buff[SERIALIZATION_SIZE];

    };

  }

  // ----------------------------------------------------------------------
  // Getters for numbers of input ports
  // ----------------------------------------------------------------------

  Fw::InputCmdPort *barometerComponentBase ::
    get_cmdIn_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_cmdIn_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_cmdIn_InputPort[portNum];
  }

  Drv::InputSerialReadPort *barometerComponentBase ::
    get_serialRecv_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_serialRecv_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_serialRecv_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Typed connectors for output ports
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    set_cmdRegOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputCmdRegPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_cmdRegOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_cmdRegOut_OutputPort[portNum].addCallPort(port);
  }

  void barometerComponentBase ::
    set_cmdResponseOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputCmdResponsePort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_cmdResponseOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_cmdResponseOut_OutputPort[portNum].addCallPort(port);
  }

  void barometerComponentBase ::
    set_eventOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputLogPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_eventOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_eventOut_OutputPort[portNum].addCallPort(port);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void barometerComponentBase ::
    set_textEventOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputLogTextPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_textEventOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_textEventOut_OutputPort[portNum].addCallPort(port);
  }
#endif

  void barometerComponentBase ::
    set_tlmOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputTlmPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_tlmOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_tlmOut_OutputPort[portNum].addCallPort(port);
  }

  void barometerComponentBase ::
    set_serialBufferOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputBufferSendPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_serialBufferOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_serialBufferOut_OutputPort[portNum].addCallPort(port);
  }

  void barometerComponentBase ::
    set_I2cWrite_OutputPort(
        NATIVE_INT_TYPE portNum,
        Drv::InputI2cPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_I2cWrite_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_I2cWrite_OutputPort[portNum].addCallPort(port);
  }

  void barometerComponentBase ::
    set_I2cRead_OutputPort(
        NATIVE_INT_TYPE portNum,
        Drv::InputI2cPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_I2cRead_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_I2cRead_OutputPort[portNum].addCallPort(port);
  }

  void barometerComponentBase ::
    set_Time_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputTimePort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_Time_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_Time_OutputPort[portNum].addCallPort(port);
  }

  // ----------------------------------------------------------------------
  // Serialization connectors for output ports
  // ----------------------------------------------------------------------

#if FW_PORT_SERIALIZATION

  void barometerComponentBase ::
    set_cmdRegOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_cmdRegOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_cmdRegOut_OutputPort[portNum].registerSerialPort(port);
  }

  void barometerComponentBase ::
    set_cmdResponseOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_cmdResponseOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_cmdResponseOut_OutputPort[portNum].registerSerialPort(port);
  }

  void barometerComponentBase ::
    set_eventOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_eventOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_eventOut_OutputPort[portNum].registerSerialPort(port);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void barometerComponentBase ::
    set_textEventOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_textEventOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_textEventOut_OutputPort[portNum].registerSerialPort(port);
  }
#endif

  void barometerComponentBase ::
    set_tlmOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_tlmOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_tlmOut_OutputPort[portNum].registerSerialPort(port);
  }

  void barometerComponentBase ::
    set_serialBufferOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_serialBufferOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_serialBufferOut_OutputPort[portNum].registerSerialPort(port);
  }

  void barometerComponentBase ::
    set_I2cWrite_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_I2cWrite_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_I2cWrite_OutputPort[portNum].registerSerialPort(port);
  }

  void barometerComponentBase ::
    set_I2cRead_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_I2cRead_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_I2cRead_OutputPort[portNum].registerSerialPort(port);
  }

  void barometerComponentBase ::
    set_Time_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_Time_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_Time_OutputPort[portNum].registerSerialPort(port);
  }

#endif

  // ----------------------------------------------------------------------
  // Command registration
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    regCommands()
  {
    FW_ASSERT(this->m_cmdRegOut_OutputPort[0].isConnected());
    this->m_cmdRegOut_OutputPort[0].invoke(
        this->getIdBase() + OPCODE_BAROMETER_LOCK
    );
  }

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

    barometerComponentBase :: barometerComponentBase(const char* compName) :
        Fw::ActiveComponentBase(compName) {



  }

  void barometerComponentBase ::
    init(
        NATIVE_INT_TYPE queueDepth,
        NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class
    Fw::ActiveComponentBase::init(instance);

    // Connect input port cmdIn
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_cmdIn_InputPorts();
        port++
    ) {

      this->m_cmdIn_InputPort[port].init();
      this->m_cmdIn_InputPort[port].addCallComp(
          this,
          m_p_cmdIn_in
      );
      this->m_cmdIn_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_cmdIn_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_cmdIn_InputPort[port].setObjName(portName);
#endif

    }

    // Connect input port serialRecv
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_serialRecv_InputPorts();
        port++
    ) {

      this->m_serialRecv_InputPort[port].init();
      this->m_serialRecv_InputPort[port].addCallComp(
          this,
          m_p_serialRecv_in
      );
      this->m_serialRecv_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_serialRecv_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_serialRecv_InputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port cmdRegOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_cmdRegOut_OutputPorts();
        port++
    ) {
      this->m_cmdRegOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_cmdRegOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_cmdRegOut_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port cmdResponseOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_cmdResponseOut_OutputPorts();
        port++
    ) {
      this->m_cmdResponseOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_cmdResponseOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_cmdResponseOut_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port eventOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_eventOut_OutputPorts();
        port++
    ) {
      this->m_eventOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_eventOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_eventOut_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port textEventOut
#if FW_ENABLE_TEXT_LOGGING == 1
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_textEventOut_OutputPorts();
        port++
    ) {
      this->m_textEventOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_textEventOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_textEventOut_OutputPort[port].setObjName(portName);
#endif

    }
#endif

    // Initialize output port tlmOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_tlmOut_OutputPorts();
        port++
    ) {
      this->m_tlmOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_tlmOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_tlmOut_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port serialBufferOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_serialBufferOut_OutputPorts();
        port++
    ) {
      this->m_serialBufferOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_serialBufferOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_serialBufferOut_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port I2cWrite
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_I2cWrite_OutputPorts();
        port++
    ) {
      this->m_I2cWrite_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_I2cWrite_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_I2cWrite_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port I2cRead
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_I2cRead_OutputPorts();
        port++
    ) {
      this->m_I2cRead_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_I2cRead_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_I2cRead_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port Time
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_Time_OutputPorts();
        port++
    ) {
      this->m_Time_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[120];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_Time_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_Time_OutputPort[port].setObjName(portName);
#endif

    }

    Os::Queue::QueueStatus qStat =
    this->createQueue(
        queueDepth,
        ComponentIpcSerializableBuffer::SERIALIZATION_SIZE
    );
    FW_ASSERT(
        Os::Queue::QUEUE_OK == qStat,
        static_cast<AssertArg>(qStat)
    );

  }

  barometerComponentBase::
    ~barometerComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    serialBufferOut_out(
        NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_serialBufferOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_serialBufferOut_OutputPort[portNum].invoke(fwBuffer);
  }

  Drv::I2cStatus barometerComponentBase ::
    I2cWrite_out(
        NATIVE_INT_TYPE portNum,
        U32 addr, Fw::Buffer &serBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_I2cWrite_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_I2cWrite_OutputPort[portNum].invoke(addr, serBuffer);
  }

  Drv::I2cStatus barometerComponentBase ::
    I2cRead_out(
        NATIVE_INT_TYPE portNum,
        U32 addr, Fw::Buffer &serBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_I2cRead_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_I2cRead_OutputPort[portNum].invoke(addr, serBuffer);
  }

  // ----------------------------------------------------------------------
  // Getters for numbers of ports
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_cmdIn_InputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_cmdIn_InputPort));
  }

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_cmdRegOut_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_cmdRegOut_OutputPort));
  }

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_cmdResponseOut_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_cmdResponseOut_OutputPort));
  }

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_eventOut_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_eventOut_OutputPort));
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_textEventOut_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_textEventOut_OutputPort));
  }
#endif

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_tlmOut_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_tlmOut_OutputPort));
  }

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_serialRecv_InputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_serialRecv_InputPort));
  }

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_serialBufferOut_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_serialBufferOut_OutputPort));
  }

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_I2cWrite_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_I2cWrite_OutputPort));
  }

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_I2cRead_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_I2cRead_OutputPort));
  }

  NATIVE_INT_TYPE barometerComponentBase ::
    getNum_Time_OutputPorts()
  {
    return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_Time_OutputPort));
  }

  // ----------------------------------------------------------------------
  // Port connection status queries
  // ----------------------------------------------------------------------

  bool barometerComponentBase ::
    isConnected_cmdRegOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_cmdRegOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_cmdRegOut_OutputPort[portNum].isConnected();
  }

  bool barometerComponentBase ::
    isConnected_cmdResponseOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_cmdResponseOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_cmdResponseOut_OutputPort[portNum].isConnected();
  }

  bool barometerComponentBase ::
    isConnected_eventOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_eventOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_eventOut_OutputPort[portNum].isConnected();
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  bool barometerComponentBase ::
    isConnected_textEventOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_textEventOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_textEventOut_OutputPort[portNum].isConnected();
  }
#endif

  bool barometerComponentBase ::
    isConnected_tlmOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_tlmOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_tlmOut_OutputPort[portNum].isConnected();
  }

  bool barometerComponentBase ::
    isConnected_serialBufferOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_serialBufferOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_serialBufferOut_OutputPort[portNum].isConnected();
  }

  bool barometerComponentBase ::
    isConnected_I2cWrite_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_I2cWrite_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_I2cWrite_OutputPort[portNum].isConnected();
  }

  bool barometerComponentBase ::
    isConnected_I2cRead_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_I2cRead_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_I2cRead_OutputPort[portNum].isConnected();
  }

  bool barometerComponentBase ::
    isConnected_Time_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_Time_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_Time_OutputPort[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Calls for messages received on command input ports
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    m_p_cmdIn_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {

    // Select base class function based on opcode
    FW_ASSERT(callComp);
    barometerComponentBase* compPtr = static_cast<barometerComponentBase*>(callComp);

    const U32 idBase = callComp->getIdBase();
    FW_ASSERT(opCode >= idBase, opCode, idBase);
    switch (opCode - idBase) {

      case OPCODE_BAROMETER_LOCK: /*!< A command to force an EVR reporting lock status. */
        compPtr->Barometer_lock_cmdHandlerBase(
            opCode,
            cmdSeq,
            args
        );
        break;

      default: {
        compPtr->cmdResponse_out(
            opCode,cmdSeq,
            Fw::CmdResponse::INVALID_OPCODE
        );
        break;
      }

    }

  }

  void barometerComponentBase ::
    cmdResponse_out(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdResponse response
    )
  {
    FW_ASSERT(this->m_cmdResponseOut_OutputPort[0].isConnected());
    this->m_cmdResponseOut_OutputPort[0].invoke(opCode,cmdSeq,response);
  }

  // ----------------------------------------------------------------------
  // Base class command functions
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    Barometer_lock_cmdHandlerBase(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {

    // Call pre-message hook
    this->Barometer_lock_preMsgHook(opCode,cmdSeq);

    // Defer deserializing arguments to the message dispatcher
    // to avoid deserializing and reserializing just for IPC
    ComponentIpcSerializableBuffer msg;
    Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

    // Serialize for IPC
    _status = msg.serialize(static_cast<NATIVE_INT_TYPE>(CMD_BAROMETER_LOCK));
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
        );

    // Fake port number to make message dequeue work
    NATIVE_INT_TYPE port = 0;
    _status = msg.serialize(port);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(opCode);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(cmdSeq);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(args);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    // send message
    Os::Queue::QueueBlocking _block =
      Os::Queue::QUEUE_NONBLOCKING;
    Os::Queue::QueueStatus qStatus =
      this->m_queue.send(msg, 0,_block);
    FW_ASSERT(
        qStatus == Os::Queue::QUEUE_OK,
        static_cast<AssertArg>(qStatus)
    );

  }

  void barometerComponentBase::
    Barometer_lock_preMsgHook(
        FwOpcodeType opCode,
        U32 cmdSeq
    )
  {
    // Defaults to no-op; can be overridden
  }

  // ----------------------------------------------------------------------
  // Telemetry write functions
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    tlmWrite_Pressure(F32 arg, Fw::Time _tlmTime)
  {
    if (this->m_tlmOut_OutputPort[0].isConnected()) {
      if (this->m_Time_OutputPort[0].isConnected() && _tlmTime ==  Fw::ZERO_TIME) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );

      FwChanIdType _id;

      _id = this->getIdBase() + CHANNELID_PRESSURE;

      this->m_tlmOut_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  void barometerComponentBase ::
    tlmWrite_Celcius(F32 arg, Fw::Time _tlmTime)
  {
    if (this->m_tlmOut_OutputPort[0].isConnected()) {
      if (this->m_Time_OutputPort[0].isConnected() && _tlmTime ==  Fw::ZERO_TIME) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );

      FwChanIdType _id;

      _id = this->getIdBase() + CHANNELID_CELCIUS;

      this->m_tlmOut_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  void barometerComponentBase ::
    tlmWrite_Fahrenheit(F32 arg, Fw::Time _tlmTime)
  {
    if (this->m_tlmOut_OutputPort[0].isConnected()) {
      if (this->m_Time_OutputPort[0].isConnected() && _tlmTime ==  Fw::ZERO_TIME) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );

      FwChanIdType _id;

      _id = this->getIdBase() + CHANNELID_FAHRENHEIT;

      this->m_tlmOut_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  Fw::Time barometerComponentBase ::
    getTime()
  {
    if (this->m_Time_OutputPort[0].isConnected()) {
      Fw::Time _time;
      this->m_Time_OutputPort[0].invoke(_time);
      return _time;
    } else {
      return Fw::Time(TB_NONE,0,0);
    }
  }

  // ----------------------------------------------------------------------
  // Event handling functions
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    log_ACTIVITY_HI_Barometer_redy(
        
    )
  {


    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }

    FwEventIdType _id = static_cast<FwEventIdType>(0);

    _id = this->getIdBase() + EVENTID_BAROMETER_REDY;

    // Emit the event on the log port
    if (this->m_eventOut_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
#if FW_AMPCS_COMPATIBLE
      // for AMPCS, need to encode zero arguments
      Fw::SerializeStatus _zero_status = Fw::FW_SERIALIZE_OK;
      _zero_status = _logBuff.serialize(static_cast<U8>(0));
      FW_ASSERT(
          _zero_status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_zero_status)
      );
#endif


      this->m_eventOut_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LogSeverity::ACTIVITY_HI,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_textEventOut_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Baro OK";
#else
      const char* _formatString =
        "%s: Baro OK";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "Barometer_redy "
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_textEventOut_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LogSeverity::ACTIVITY_HI,
          _logString
      );

    }
#endif

  }


  void barometerComponentBase ::
    log_WARNING_HI_Barometer_offline(
        
    )
  {


    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }

    FwEventIdType _id = static_cast<FwEventIdType>(0);

    _id = this->getIdBase() + EVENTID_BAROMETER_OFFLINE;

    // Emit the event on the log port
    if (this->m_eventOut_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
#if FW_AMPCS_COMPATIBLE
      // for AMPCS, need to encode zero arguments
      Fw::SerializeStatus _zero_status = Fw::FW_SERIALIZE_OK;
      _zero_status = _logBuff.serialize(static_cast<U8>(0));
      FW_ASSERT(
          _zero_status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_zero_status)
      );
#endif


      this->m_eventOut_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LogSeverity::WARNING_HI,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_textEventOut_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Baro Off";
#else
      const char* _formatString =
        "%s: Baro Off";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "Barometer_offline "
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_textEventOut_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LogSeverity::WARNING_HI,
          _logString
      );

    }
#endif

  }


  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    m_p_serialRecv_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer, Drv::SerialReadStatus &status
    )
  {
    FW_ASSERT(callComp);
    barometerComponentBase* compPtr = static_cast<barometerComponentBase*>(callComp);
    compPtr->serialRecv_handlerBase(portNum, serBuffer, status);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    serialRecv_handlerBase(
        NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer, Drv::SerialReadStatus &status
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_serialRecv_InputPorts(),static_cast<AssertArg>(portNum));

    // Call pre-message hook
    serialRecv_preMsgHook(
        portNum,
        serBuffer, status
    );

    ComponentIpcSerializableBuffer msg;
    Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

    _status = msg.serialize(
        static_cast<NATIVE_INT_TYPE>(SERIALRECV_SERIALREAD)
    );
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(portNum);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    // Serialize argument serBuffer
    _status = msg.serialize(serBuffer);
    FW_ASSERT(
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    // Serialize argument status
    _status = msg.serialize(status);
    FW_ASSERT(
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );


    // send message
    Os::Queue::QueueBlocking _block =
      Os::Queue::QUEUE_NONBLOCKING;
    Os::Queue::QueueStatus qStatus =
      this->m_queue.send(msg, 0,_block);
    FW_ASSERT(
        qStatus == Os::Queue::QUEUE_OK,
        static_cast<AssertArg>(qStatus)
    );

  }

  // ----------------------------------------------------------------------
  // Pre-message hooks for async input ports
  // ----------------------------------------------------------------------

  void barometerComponentBase ::
    serialRecv_preMsgHook(
        NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer, Drv::SerialReadStatus &status
    )
  {
    // Default: no-op
  }

  // ----------------------------------------------------------------------
  // Message dispatch method for active and queued components. Called
  // by active component thread or implementation code for queued components
  // ----------------------------------------------------------------------

  Fw::QueuedComponentBase::MsgDispatchStatus barometerComponentBase ::
    doDispatch()
  {
    ComponentIpcSerializableBuffer msg;
    NATIVE_INT_TYPE priority = 0;

    Os::Queue::QueueStatus msgStatus = this->m_queue.receive(msg,priority,Os::Queue::QUEUE_BLOCKING);
    FW_ASSERT(
        msgStatus == Os::Queue::QUEUE_OK,
        static_cast<AssertArg>(msgStatus)
    );

    // Reset to beginning of buffer
    msg.resetDeser();

    NATIVE_INT_TYPE desMsg = 0;
    Fw::SerializeStatus deserStatus = msg.deserialize(desMsg);
    FW_ASSERT(
        deserStatus == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(deserStatus)
    );

    MsgTypeEnum msgType = static_cast<MsgTypeEnum>(desMsg);

    if (msgType == BAROMETER_COMPONENT_EXIT) {
      return MSG_DISPATCH_EXIT;
    }

    NATIVE_INT_TYPE portNum = 0;
    deserStatus = msg.deserialize(portNum);
    FW_ASSERT(
        deserStatus == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(deserStatus)
    );

    switch (msgType) {

      // Handle async input port serialRecv
      case SERIALRECV_SERIALREAD: {

        // Deserialize argument serBuffer
        Fw::Buffer serBuffer;
        deserStatus = msg.deserialize(serBuffer);
        FW_ASSERT(
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Deserialize argument status
        Drv::SerialReadStatus status;
        deserStatus = msg.deserialize(status);
        FW_ASSERT(
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Call handler function
        this->serialRecv_handler(
            portNum,
            serBuffer, status
        );

        break;

      }

      // Handle command Barometer_lock
      case CMD_BAROMETER_LOCK: {
        // Deserialize opcode
        FwOpcodeType opCode = 0;
        deserStatus = msg.deserialize(opCode);
        FW_ASSERT (
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Deserialize command sequence
        U32 cmdSeq = 0;
        deserStatus = msg.deserialize(cmdSeq);
        FW_ASSERT (
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Deserialize command argument buffer
        Fw::CmdArgBuffer args;
        deserStatus = msg.deserialize(args);
        FW_ASSERT (
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Reset buffer
        args.resetDeser();

        // Make sure there was no data left over.
        // That means the argument buffer size was incorrect.
#if FW_CMD_CHECK_RESIDUAL
        if (args.getBuffLeft() != 0) {
          if (this->m_cmdResponseOut_OutputPort[0].isConnected()) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::FORMAT_ERROR);
          }
          // Don't crash the task if bad arguments were passed from the ground
          break;
        }
#endif
        // Call handler function
        this->Barometer_lock_cmdHandler(opCode, cmdSeq);

        break;

      }

      default:
        return MSG_DISPATCH_ERROR;

    }

    return MSG_DISPATCH_OK;

  }

} // end namespace BarometerApp
