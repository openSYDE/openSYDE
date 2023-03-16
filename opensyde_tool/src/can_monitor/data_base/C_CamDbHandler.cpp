//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for data base storage (non permanent) and access (implementation)

   Class for data base storage (non permanent) and access

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CamDbHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
C_CamDbHandler C_CamDbHandler::mhc_Instance;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove all known files from storage (e.g. in case of new project)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbHandler::RemoveAllFiles(void)
{
   this->mc_DbcFiles.clear();
   this->mc_OsyFiles.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove DBC file

   \param[in]  orc_File    File to remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbHandler::RemoveDbcFile(const QString & orc_File)
{
   this->mc_DbcFiles.remove(orc_File);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove OSY file

   \param[in]  orc_File    File to remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbHandler::RemoveOsyFile(const QString & orc_File)
{
   this->mc_OsyFiles.remove(orc_File);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append data of DBC file

   \param[in]  orc_File    File to add
   \param[in]  orc_Data    Data extracted from this file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbHandler::AddDbcFile(const QString & orc_File, const C_CieConverter::C_CieCommDefinition & orc_Data)
{
   C_CamDbDbc c_New;

   c_New.SetData(orc_Data);
   this->mc_DbcFiles.insert(orc_File, c_New);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append data of OSY file

   \param[in]  orc_File    File to add
   \param[in]  orc_Data    Data extracted from this file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbHandler::AddOsyFile(const QString & orc_File,
                                const stw::opensyde_core::C_OscComMessageLoggerOsySysDefConfig & orc_Data)
{
   C_CamDbOsy c_New;

   c_New.SetData(orc_Data);
   this->mc_OsyFiles.insert(orc_File, c_New);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if file already loaded

   \param[in]  orc_File    File to search for

   \return
   True  Already loaded
   False Not yet known
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamDbHandler::GetContainsDbc(const QString & orc_File) const
{
   return this->mc_DbcFiles.contains(orc_File);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if file already loaded

   \param[in]  orc_File    File to search for

   \return
   True  Already loaded
   False Not yet known
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamDbHandler::GetContainsOsy(const QString & orc_File) const
{
   return this->mc_OsyFiles.contains(orc_File);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Parse all messages to store the fast access index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamDbHandler::FindAllMessages(void)
{
   for (QMap<QString, C_CamDbOsy>::iterator c_It = this->mc_OsyFiles.begin(); c_It != this->mc_OsyFiles.end(); ++c_It)
   {
      c_It->FindAllMessages();
   }
   for (QMap<QString, C_CamDbDbc>::iterator c_It = this->mc_DbcFiles.begin(); c_It != this->mc_DbcFiles.end(); ++c_It)
   {
      c_It->FindAllMessages();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search all databases for a message with this ID, return the first one found

   First searching all OSY files, then all DBC files

   \param[in]   ou32_Id          CAN ID to search for
   \param[in]   oq_IsExtended    Is extended
   \param[out]  orc_File         Found file path (only valid if C_NO_ERR)
   \param[out]  orc_Message      Found message name (only valid if C_NO_ERR)

   \return
   C_NO_ERR Found at least one matching message
   C_NOACT  No matching message found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamDbHandler::FindMessageById(const uint32_t ou32_Id, const bool oq_IsExtended, QString & orc_File,
                                        QString & orc_Message) const
{
   int32_t s32_Retval = C_NOACT;

   for (QMap<QString, C_CamDbOsy>::const_iterator c_It = this->mc_OsyFiles.begin();
        (c_It != this->mc_OsyFiles.end()) && (s32_Retval == C_NOACT); ++c_It)
   {
      s32_Retval = c_It->FindMessageById(ou32_Id, oq_IsExtended, orc_Message);
      if (s32_Retval == C_NO_ERR)
      {
         //Found match
         orc_File = c_It.key();
      }
   }
   for (QMap<QString, C_CamDbDbc>::const_iterator c_It = this->mc_DbcFiles.begin();
        (c_It != this->mc_DbcFiles.end()) && (s32_Retval == C_NOACT); ++c_It)
   {
      s32_Retval = c_It->FindMessageById(ou32_Id, oq_IsExtended, orc_Message);
      if (s32_Retval == C_NO_ERR)
      {
         //Found match
         orc_File = c_It.key();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find OSY message in data base

   \param[in]  orc_File       File path of file to search in
   \param[in]  orc_Message    Message name to search for

   \return
   C_NO_ERR Found message
   C_RANGE  File not loaded
   C_NOACT  Message not found in file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamDbHandler::FindOsyMessage(const QString & orc_File, const QString & orc_Message)
{
   int32_t s32_Retval = C_NO_ERR;
   const QMap<QString, C_CamDbOsy>::iterator c_It = this->mc_OsyFiles.find(orc_File);

   if (c_It != this->mc_OsyFiles.end())
   {
      s32_Retval = c_It->FindMessage(orc_Message);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find DBC message in data base

   \param[in]  orc_File       File path of file to search in
   \param[in]  orc_Message    Message name to search for

   \return
   C_NO_ERR Found message
   C_RANGE  File not loaded
   C_NOACT  Message not found in file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamDbHandler::FindDbcMessage(const QString & orc_File, const QString & orc_Message)
{
   int32_t s32_Retval = C_NO_ERR;
   const QMap<QString, C_CamDbDbc>::iterator c_It = this->mc_DbcFiles.find(orc_File);

   if (c_It != this->mc_DbcFiles.end())
   {
      s32_Retval = c_It->FindMessage(orc_Message);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC CAN message

   Requirement: this function can only return a valid index if the function FindOsyMessage was at least called once

   \param[in]   orc_File            File name to look in
   \param[in]   orc_Message         Message name to look for
   \param[in]   oq_UseHash          Use hash
   \param[in]   ou32_Hash           Hash
   \param[out]  ope_ProtocolType    Protocol type

   \return
   NULL OSC CAN message not found
   Else Valid OSC CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
const stw::opensyde_core::C_OscCanMessage * C_CamDbHandler::GetOscMessage(const QString & orc_File,
                                                                          const QString & orc_Message,
                                                                          const bool oq_UseHash,
                                                                          const uint32_t ou32_Hash,
                                                                          C_OscCanProtocol::E_Type * const ope_ProtocolType)
const
{
   const stw::opensyde_core::C_OscCanMessage * pc_Retval = NULL;
   const QMap<QString, C_CamDbOsy>::const_iterator c_It = this->mc_OsyFiles.find(orc_File);

   if (c_It != this->mc_OsyFiles.end())
   {
      pc_Retval = c_It->GetOscMessage(orc_Message, oq_UseHash, ou32_Hash, ope_ProtocolType);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC list

   Requirement: this function can only return a valid index if the function FindOsyMessage was at least called once

   \param[in]  orc_File       File name to look in
   \param[in]  orc_Message    Message name to look for
   \param[in]  oq_UseHash     Use hash
   \param[in]  ou32_Hash      Hash

   \return
   NULL OSC list not found
   Else Valid OSC list
*/
//----------------------------------------------------------------------------------------------------------------------
const stw::opensyde_core::C_OscNodeDataPoolList * C_CamDbHandler::GetOscList(const QString & orc_File,
                                                                             const QString & orc_Message,
                                                                             const bool oq_UseHash,
                                                                             const uint32_t ou32_Hash) const
{
   const stw::opensyde_core::C_OscNodeDataPoolList * pc_Retval = NULL;
   const QMap<QString, C_CamDbOsy>::const_iterator c_It = this->mc_OsyFiles.find(orc_File);

   if (c_It != this->mc_OsyFiles.end())
   {
      pc_Retval = c_It->GetOscList(orc_Message, oq_UseHash, ou32_Hash);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get DBC message

   Requirement: this function can only return a valid index if the function FindDbcMessage was at least called once

   \param[in]  orc_File       File name to look in
   \param[in]  orc_Message    Message name to look for
   \param[in]  oq_UseHash     Use hash
   \param[in]  ou32_Hash      Hash

   \return
   NULL ODBC message not found
   Else Valid DBC message
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CieConverter::C_CieCanMessage * C_CamDbHandler::GetDbcMessage(const QString & orc_File,
                                                                      const QString & orc_Message,
                                                                      const bool oq_UseHash,
                                                                      const uint32_t ou32_Hash) const
{
   const C_CieConverter::C_CieCanMessage * pc_Retval = NULL;
   const QMap<QString, C_CamDbDbc>::const_iterator c_It = this->mc_DbcFiles.find(orc_File);

   if (c_It != this->mc_DbcFiles.end())
   {
      pc_Retval = c_It->GetDbcMessage(orc_Message, oq_UseHash, ou32_Hash);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all DBC files

   \return
   All DBC files
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<QString, C_CamDbDbc> & C_CamDbHandler::GetDbcFiles(void) const
{
   return this->mc_DbcFiles;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all OSY files

   \return
   All OSY files
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<QString, C_CamDbOsy> & C_CamDbHandler::GetOsyFiles(void) const
{
   return this->mc_OsyFiles;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active flag

   \param[in]  orc_File    File to change active state for
   \param[in]  oq_Active   New active state

   \return
   C_NO_ERR Changed flag
   C_RANGE  File not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamDbHandler::SetDbcActive(const QString & orc_File, const bool oq_Active)
{
   int32_t s32_Retval = C_RANGE;
   const QMap<QString, C_CamDbDbc>::iterator c_It = this->mc_DbcFiles.find(orc_File);

   if (c_It != this->mc_DbcFiles.end())
   {
      c_It->SetActive(oq_Active);
      s32_Retval = C_NO_ERR;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active flag

   \param[in]  orc_File    File to change active state for
   \param[in]  oq_Active   New active state

   \return
   C_NO_ERR Changed flag
   C_RANGE  File not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamDbHandler::SetOsyActive(const QString & orc_File, const bool oq_Active)
{
   int32_t s32_Retval = C_RANGE;
   const QMap<QString, C_CamDbOsy>::iterator c_It = this->mc_OsyFiles.find(orc_File);

   if (c_It != this->mc_OsyFiles.end())
   {
      c_It->SetActive(oq_Active);
      s32_Retval = C_NO_ERR;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Replace the openSYDE bus index

   Warning: this clears the found indices

   \param[in]   orc_File         File to change bus index for
   \param[in]   ou32_BusIndex    New bus index
   \param[out]  orq_Change       Flag if bus index was changed

   \return
   All OSY files
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamDbHandler::ReplaceOsyBusIndex(const QString & orc_File, const uint32_t ou32_BusIndex, bool & orq_Change)
{
   int32_t s32_Retval = C_NOACT;
   const QMap<QString, C_CamDbOsy>::iterator c_It = this->mc_OsyFiles.find(orc_File);

   if (c_It != this->mc_OsyFiles.end())
   {
      orq_Change = c_It->ReplaceOsyBusIndex(ou32_BusIndex);
      s32_Retval = C_NO_ERR;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check hash for message

   \param[in]  orc_File       File
   \param[in]  orc_Message    Message
   \param[in]  ou32_Hash      Hash

   \retval   true   Valid
   \retval   false  Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamDbHandler::CheckHashForMessage(const QString & orc_File, const QString & orc_Message,
                                         const uint32_t ou32_Hash) const
{
   bool q_Retval = false;
   const QMap<QString, C_CamDbOsy>::const_iterator c_ItOsy = this->mc_OsyFiles.find(orc_File);
   const QMap<QString, C_CamDbDbc>::const_iterator c_ItDbc = this->mc_DbcFiles.find(orc_File);

   if (c_ItOsy != this->mc_OsyFiles.end())
   {
      q_Retval = c_ItOsy->CheckHashForMessage(orc_Message, ou32_Hash);
   }

   if (c_ItDbc != this->mc_DbcFiles.end())
   {
      q_Retval = c_ItDbc->CheckHashForMessage(orc_Message, ou32_Hash);
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamDbHandler * C_CamDbHandler::h_GetInstance(void)
{
   return &C_CamDbHandler::mhc_Instance;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamDbHandler::C_CamDbHandler(void)
{
}
