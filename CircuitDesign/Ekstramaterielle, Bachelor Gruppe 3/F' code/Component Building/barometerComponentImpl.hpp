// ======================================================================
// \title  barometer.hpp
// \author jakob
// \brief  hpp file for barometer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef barometer_HPP
#define barometer_HPP

#include "RPI/Barometer/BarometerComponentAc.hpp"

#define NUM_I2C_BUFFERS 20
#define I2C_READ_BUFF_SIZE 1024
#define NUM_UART_BUFFERS 20
#define UART_READ_BUFF_SIZE 1024
namespace BarometerApp {

  class barometer :
    public barometerComponentBase
  {
      struct MPpacket {
          float pressure;
      };

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object barometer
      //!
      barometer(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object barometer
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object barometer
      //!
      ~barometer();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for serialRecv
      //!
      void serialRecv_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer, /*!< 
      Buffer containing data
      */
          Drv::SerialReadStatus &status /*!< 
      Status of read
      */
      );
      void I2cRead(
          const NATIVE_INT_TYPE portNum,
          Fw::Buffer &serBuffer,
          Drv::I2cStatus &status
      );
    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for Barometer_lock command handler
      //! A command to force an EVR reporting lock status.
      void Barometer_lock_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );


    };

} // end namespace BarometerApp

#endif
