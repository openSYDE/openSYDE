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
#include "C_OscDataLoggerJobAdditionalTriggerProperties.hpp"

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

   enum E_UseCase ///< Use cases
   {
      eUC_MANUAL,        ///< use case manual
      eUC_AWS,           ///< use case AWS
      eUC_MACHINES_CLOUD ///< use case machines.cloud
   };

   C_OscDataLoggerJobProperties();

   void CalcHash(uint32_t & oru32_HashValue) const;

   stw::scl::C_SclString c_Name;                                                ///< data logger job name
   stw::scl::C_SclString c_Comment;                                             ///< data logger job comment
   E_UseCase e_UseCase;                                                         ///< selected local log trigger
   E_LogFileFormat e_LogFileFormat;                                             ///< selected log file format
   uint32_t u32_MaxLogEntries;                                                  ///< max log entries
   uint32_t u32_MaxLogDurationSec;                                              ///< max log duration in seconds
   uint32_t u32_LogIntervalMs;                                                  ///< logging interval in milliseconds
   E_LocalLogTrigger e_LocalLogTrigger;                                         ///< selected local log trigger
   stw::scl::C_SclString c_LogDestinationDirectory;                             ///< log destination directory
   C_OscDataLoggerJobAdditionalTriggerProperties c_AdditionalTriggerProperties; ///< additional trigger config
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
