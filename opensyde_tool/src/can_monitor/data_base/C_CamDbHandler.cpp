//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class for data base storage (non permanent) and access (implementation)

   Class for data base storage (non permanent) and access

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_CamDbHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
C_CamDbHandler C_CamDbHandler::mhc_Instance;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Remove all known files from storage (e.g. in case of new project)

   \created     23.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbHandler::RemoveAllFiles(void)
{
   this->mc_DBCFiles.clear();
   this->mc_OSYFiles.clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove DBC file

   \param[in] orc_File File to remove

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbHandler::RemoveDbcFile(const QString & orc_File)
{
   this->mc_DBCFiles.remove(orc_File);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove OSY file

   \param[in] orc_File File to remove

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbHandler::RemoveOsyFile(const QString & orc_File)
{
   this->mc_OSYFiles.remove(orc_File);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Append data of DBC file

   \param[in] orc_File File to add
   \param[in] orc_Data Data extracted from this file

   \created     16.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbHandler::AddDbcFile(const QString & orc_File, const C_CieConverter::C_CIECommDefinition & orc_Data)
{
   C_CamDbDbc c_New;

   c_New.SetData(orc_Data);
   this->mc_DBCFiles.insert(orc_File, c_New);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Append data of OSY file

   \param[in] orc_File File to add
   \param[in] orc_Data Data extracted from this file

   \created     16.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbHandler::AddOsyFile(const QString & orc_File,
                                const stw_opensyde_core::C_OSCComMessageLoggerOsySysDefConfig & orc_Data)
{
   C_CamDbOsy c_New;

   c_New.SetData(orc_Data);
   this->mc_OSYFiles.insert(orc_File, c_New);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if file already loaded

   \param[in] orc_File File to search for

   \return
   True  Already loaded
   False Not yet known

   \created     16.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_CamDbHandler::GetContainsDbc(const QString & orc_File) const
{
   return this->mc_DBCFiles.contains(orc_File);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if file already loaded

   \param[in] orc_File File to search for

   \return
   True  Already loaded
   False Not yet known

   \created     16.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_CamDbHandler::GetContainsOsy(const QString & orc_File) const
{
   return this->mc_OSYFiles.contains(orc_File);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Parse all messages to store the fast access index

   \created     21.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamDbHandler::FindAllMessages(void)
{
   for (QMap<QString, C_CamDbOsy>::iterator c_It = this->mc_OSYFiles.begin(); c_It != this->mc_OSYFiles.end(); ++c_It)
   {
      c_It->FindAllMessages();
   }
   for (QMap<QString, C_CamDbDbc>::iterator c_It = this->mc_DBCFiles.begin(); c_It != this->mc_DBCFiles.end(); ++c_It)
   {
      c_It->FindAllMessages();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Search all databases for a message with this ID, return the first one found

   First searching all OSY files, then all DBC files

   \param[in]  ou32_Id     CAN ID to search for
   \param[out] orc_File    Found file path (only valid if C_NO_ERR)
   \param[out] orc_Message Found message name (only valid if C_NO_ERR)

   \return
   C_NO_ERR Found at least one matching message
   C_NOACT  No matching message found

   \created     22.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamDbHandler::FindMessageById(const uint32 ou32_Id, QString & orc_File, QString & orc_Message) const
{
   sint32 s32_Retval = C_NOACT;

   for (QMap<QString, C_CamDbOsy>::const_iterator c_It = this->mc_OSYFiles.begin();
        (c_It != this->mc_OSYFiles.end()) && (s32_Retval == C_NOACT); ++c_It)
   {
      s32_Retval = c_It->FindMessageById(ou32_Id, orc_Message);
      if (s32_Retval == C_NO_ERR)
      {
         //Found match
         orc_File = c_It.key();
      }
   }
   for (QMap<QString, C_CamDbDbc>::const_iterator c_It = this->mc_DBCFiles.begin();
        (c_It != this->mc_DBCFiles.end()) && (s32_Retval == C_NOACT); ++c_It)
   {
      s32_Retval = c_It->FindMessageById(ou32_Id, orc_Message);
      if (s32_Retval == C_NO_ERR)
      {
         //Found match
         orc_File = c_It.key();
      }
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Find OSY message in data base

   \param[in] orc_File    File path of file to search in
   \param[in] orc_Message Message name to search for

   \return
   C_NO_ERR Found message
   C_RANGE  File not loaded
   C_NOACT  Message not found in file

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamDbHandler::FindOsyMessage(const QString & orc_File, const QString & orc_Message)
{
   sint32 s32_Retval = C_NO_ERR;
   const QMap<QString, C_CamDbOsy>::iterator c_It = this->mc_OSYFiles.find(orc_File);

   if (c_It != this->mc_OSYFiles.end())
   {
      s32_Retval = c_It->FindMessage(orc_Message);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Find DBC message in data base

   \param[in] orc_File    File path of file to search in
   \param[in] orc_Message Message name to search for

   \return
   C_NO_ERR Found message
   C_RANGE  File not loaded
   C_NOACT  Message not found in file

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamDbHandler::FindDbcMessage(const QString & orc_File, const QString & orc_Message)
{
   sint32 s32_Retval = C_NO_ERR;
   const QMap<QString, C_CamDbDbc>::iterator c_It = this->mc_DBCFiles.find(orc_File);

   if (c_It != this->mc_DBCFiles.end())
   {
      s32_Retval = c_It->FindMessage(orc_Message);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC CAN message

   Requirement: this function can only return a valid index if the function FindOsyMessage was at least called once

   \param[in] orc_File    File name to look in
   \param[in] orc_Message Message name to look for

   \return
   NULL OSC CAN message not found
   Else Valid OSC CAN message

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const stw_opensyde_core::C_OSCCanMessage * C_CamDbHandler::GetOSCMessage(const QString & orc_File,
                                                                         const QString & orc_Message) const
{
   const stw_opensyde_core::C_OSCCanMessage * pc_Retval = NULL;
   const QMap<QString, C_CamDbOsy>::const_iterator c_It = this->mc_OSYFiles.find(orc_File);

   if (c_It != this->mc_OSYFiles.end())
   {
      pc_Retval = c_It->GetOSCMessage(orc_Message);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get OSC list

   Requirement: this function can only return a valid index if the function FindOsyMessage was at least called once

   \param[in] orc_File    File name to look in
   \param[in] orc_Message Message name to look for

   \return
   NULL OSC list not found
   Else Valid OSC list

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const stw_opensyde_core::C_OSCNodeDataPoolList * C_CamDbHandler::GetOSCList(const QString & orc_File,
                                                                            const QString & orc_Message) const
{
   const stw_opensyde_core::C_OSCNodeDataPoolList * pc_Retval = NULL;
   const QMap<QString, C_CamDbOsy>::const_iterator c_It = this->mc_OSYFiles.find(orc_File);

   if (c_It != this->mc_OSYFiles.end())
   {
      pc_Retval = c_It->GetOSCList(orc_Message);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get DBC message

   Requirement: this function can only return a valid index if the function FindDbcMessage was at least called once

   \param[in] orc_File    File name to look in
   \param[in] orc_Message Message name to look for

   \return
   NULL ODBC message not found
   Else Valid DBC message

   \created     17.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_CieConverter::C_CIECanMessage * C_CamDbHandler::GetDbcMessage(const QString & orc_File,
                                                                      const QString & orc_Message) const
{
   const C_CieConverter::C_CIECanMessage * pc_Retval = NULL;
   const QMap<QString, C_CamDbDbc>::const_iterator c_It = this->mc_DBCFiles.find(orc_File);

   if (c_It != this->mc_DBCFiles.end())
   {
      pc_Retval = c_It->GetDBCMessage(orc_Message);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get all DBC files

   \return
   All DBC files

   \created     21.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const QMap<QString, C_CamDbDbc> & C_CamDbHandler::GetDBCFiles(void)
{
   return this->mc_DBCFiles;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get all OSY files

   \return
   All OSY files

   \created     21.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const QMap<QString, C_CamDbOsy> & C_CamDbHandler::GetOSYFiles(void)
{
   return this->mc_OSYFiles;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set active flag

   \param[in] orc_File  File to change active state for
   \param[in] oq_Active New active state

   \return
   C_NO_ERR Changed flag
   C_RANGE  File not found

   \created     23.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamDbHandler::SetDBCActive(const QString & orc_File, const bool oq_Active)
{
   sint32 s32_Retval = C_RANGE;
   const QMap<QString, C_CamDbDbc>::iterator c_It = this->mc_DBCFiles.find(orc_File);

   if (c_It != this->mc_DBCFiles.end())
   {
      c_It->SetActive(oq_Active);
      s32_Retval = C_NO_ERR;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set active flag

   \param[in] orc_File  File to change active state for
   \param[in] oq_Active New active state

   \return
   C_NO_ERR Changed flag
   C_RANGE  File not found

   \created     23.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamDbHandler::SetOsyActive(const QString & orc_File, const bool oq_Active)
{
   sint32 s32_Retval = C_RANGE;
   const QMap<QString, C_CamDbOsy>::iterator c_It = this->mc_OSYFiles.find(orc_File);

   if (c_It != this->mc_OSYFiles.end())
   {
      c_It->SetActive(oq_Active);
      s32_Retval = C_NO_ERR;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Replace the openSYDE bus index

   Warning: this clears the found indices

   \param[in]  orc_File      File to change bus index for
   \param[in]  ou32_BusIndex New bus index
   \param[out] orq_Change    Flag if bus index was changed

   \return
   All OSY files

   \created     22.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamDbHandler::ReplaceOsyBusIndex(const QString & orc_File, const uint32 ou32_BusIndex, bool & orq_Change)
{
   sint32 s32_Retval = C_NOACT;
   const QMap<QString, C_CamDbOsy>::iterator c_It = this->mc_OSYFiles.find(orc_File);

   if (c_It != this->mc_OSYFiles.end())
   {
      orq_Change = c_It->ReplaceOsyBusIndex(ou32_BusIndex);
      s32_Retval = C_NO_ERR;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton

   \created     16.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamDbHandler * C_CamDbHandler::h_GetInstance(void)
{
   return &C_CamDbHandler::mhc_Instance;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     16.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamDbHandler::C_CamDbHandler(void)
{
}
