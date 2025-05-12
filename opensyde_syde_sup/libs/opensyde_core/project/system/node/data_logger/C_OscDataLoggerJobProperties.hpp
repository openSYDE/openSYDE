//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger job properties
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDATALOGGERJOBPROPERTIES_HPP
#define C_OSCDATALOGGERJOBPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscDataLoggerJobProperties
{
public:
   enum E_LogFileFormat ///< log file types
   {
      eLFF_CSV,    ///< log file type CSV
      eLFF_PARQUET ///< log file type CSV
   };

   enum E_LocalLogTrigger ///< local log triggers
   {
      eLLT_ON_CHANGE, ///< local log trigger on change
      eLLT_INTERVAL   ///< local log trigger on change
   };

   C_OscDataLoggerJobProperties();

   void CalcHash(uint32_t & oru32_HashValue) const;

   stw::scl::C_SclString c_Name;                    ///< data logger job name
   stw::scl::C_SclString c_Comment;                 ///< data logger job comment
   E_LogFileFormat e_LogFileFormat;                 ///< selected log file format
   uint32_t u32_MaxLogFileSizeMb;                   ///< max log file size in MB
   E_LocalLogTrigger e_LocalLogTrigger;             ///< selected local log trigger
   C_OscSystemBus::E_Type e_ConnectedInterfaceType; ///< e.g. CAN, ethernet
   uint8_t u8_ConnectedInterfaceNumber;             ///< 0 = first interface for this type
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
