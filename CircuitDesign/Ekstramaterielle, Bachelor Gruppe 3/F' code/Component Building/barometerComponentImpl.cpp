// ======================================================================
// \title  barometer.cpp
// \author jakob
// \brief  cpp file for barometer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <RPI/Barometer/barometerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Logger/Logger.hpp

#include <cstring>
namespace BarometerApp {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  barometer ::
    barometer(
        const char *const compName
    ) : barometerComponentBase(compName)
  {

  }

  void barometer ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    barometerComponentBase::init(queueDepth, instance);
  }

  barometer ::
    ~barometer()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void barometer ::
    serialRecv_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer,
        Drv::SerialReadStatus &status
    )
  {

  }

  void BarometerApp::barometer::I2cRead(
          const NATIVE_INT_TYPE portNum,
          0x77,
          Fw::Buffer &serBuffer,
          Drv::I2cStatus &status
          )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void barometer ::
    Barometer_lock_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace BarometerApp
