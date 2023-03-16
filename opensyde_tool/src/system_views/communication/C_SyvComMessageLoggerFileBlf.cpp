//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class with concrete implementation for BLF log files (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_SyvComMessageLoggerFileBlf.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace Vector;
using namespace BLF;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_FilePath                 Path for file
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComMessageLoggerFileBlf::C_SyvComMessageLoggerFileBlf(const stw::scl::C_SclString & orc_FilePath) :
   C_OscComMessageLoggerFileBase(orc_FilePath, "")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Writes the end line and closes the open file
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComMessageLoggerFileBlf::~C_SyvComMessageLoggerFileBlf(void) noexcept
{
   try
   {
      if (this->mc_File.is_open() == true)
      {
         this->mc_File.close();
      }
   }
   catch (...)
   {
      //not much we can do here ...
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates, if necessary, and opens file and adds the default header of the file.

   An already opened file will be closed and deleted.

   \return
   C_NO_ERR    File successfully opened and created
   C_RD_WR     Error on creating file, folders or deleting old file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComMessageLoggerFileBlf::OpenFile(void)
{
   int32_t s32_Return;

   if (this->mc_File.is_open() == true)
   {
      // Close the file if it is open. The previous file will be deleted
      this->mc_File.close();
   }

   if (this->mc_FilePath.SubString(this->mc_FilePath.Length() - 3U, 4U).LowerCase() != ".blf")
   {
      // Missing file extension
      this->mc_FilePath += ".blf";
   }

   s32_Return = C_OscComMessageLoggerFileBase::OpenFile();

   if (s32_Return == C_NO_ERR)
   {
      this->mc_File.open(this->mc_FilePath.c_str(), File::OpenMode::Write);

      if (this->mc_File.is_open() == false)
      {
         // Error on opening the BLF file
         s32_Return = C_RD_WR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adding of a concrete CAN message to the log file

   \param[in]     orc_MessageData      Current CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComMessageLoggerFileBlf::AddMessageToFile(const C_OscComMessageLoggerData & orc_MessageData)
{
   if (this->mc_File.is_open() == true)
   {
      Vector::BLF::CanMessage c_CanObj;

      c_CanObj.channel = 1U;
      c_CanObj.dlc = orc_MessageData.c_CanMsg.u8_DLC;

      // Tx and RTR information
      c_CanObj.flags = 0U;
      if (orc_MessageData.q_IsTx == true)
      {
         c_CanObj.flags |= static_cast<uint8_t>(0x01);
      }
      if (orc_MessageData.c_CanMsg.u8_RTR > 0U)
      {
         c_CanObj.flags |= static_cast<uint8_t>(0x80);
      }

      // CAN Id and extended flag
      c_CanObj.id = orc_MessageData.c_CanMsg.u32_ID;
      if (orc_MessageData.c_CanMsg.u8_XTD > 0U)
      {
         // Vector magic for extended identifier
         c_CanObj.id |= 0x80000000U;
      }

      // CAN data
      std::memcpy(&c_CanObj.data[0], orc_MessageData.c_CanMsg.au8_Data, 8);

      // Timestamp in ns
      c_CanObj.objectFlags = ObjectHeader::TimeOneNans;
      // us into ns
      c_CanObj.objectTimeStamp = orc_MessageData.u64_TimeStampAbsoluteStart * 1000U;

      this->mc_File.write(&c_CanObj);
   }
}
