//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view data element (implementation)

   System view data element

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscViewData.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscViewData::C_OscViewData(void) :
   mc_Name("NewView")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   clean up ...
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscViewData::~C_OscViewData(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewData::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->mc_Name.c_str(), this->mc_Name.Length(), oru32_HashValue);
   this->mc_PcData.CalcHash(oru32_HashValue);
   for (uint32_t u32_ItUpdate = 0; u32_ItUpdate < this->mc_NodeUpdateInformation.size(); ++u32_ItUpdate)
   {
      const C_OscViewNodeUpdate & rc_Update = this->mc_NodeUpdateInformation[u32_ItUpdate];
      rc_Update.CalcHash(oru32_HashValue);
   }
   for (uint32_t u32_ItNode = 0; u32_ItNode < this->mc_NodeActiveFlags.size(); ++u32_ItNode)
   {
      const uint8_t u8_Value = this->mc_NodeActiveFlags[u32_ItNode];
      stw::scl::C_SclChecksums::CalcCRC32(&u8_Value, sizeof(u8_Value), oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get PC data

   \return
   Current PC data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscViewPc & C_OscViewData::GetOscPcData(void) const
{
   return this->mc_PcData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set PC data

   \param[in]  orc_Value   New PC data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewData::SetOscPcData(const C_OscViewPc & orc_Value)
{
   this->mc_PcData = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node active

   \param[in]  ou32_NodeIndex    Node index

   \return
   Status if node is active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscViewData::GetNodeActive(const uint32_t ou32_NodeIndex) const
{
   bool q_Retval = false;

   if (ou32_NodeIndex < this->mc_NodeActiveFlags.size())
   {
      if (this->mc_NodeActiveFlags[ou32_NodeIndex] == 0U)
      {
         q_Retval = false;
      }
      else
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node active flags

   \return
   Current node active flags
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8_t> & C_OscViewData::GetNodeActiveFlags(void) const
{
   return this->mc_NodeActiveFlags;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node active flags

   \param[in]  orc_Value   New node active flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewData::SetNodeActiveFlags(const std::vector<uint8_t> & orc_Value)
{
   this->mc_NodeActiveFlags = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node update information

   \return
   Current node update information
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OscViewNodeUpdate> & C_OscViewData::GetAllNodeUpdateInformation(void) const
{
   return this->mc_NodeUpdateInformation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information

   \param[in]  orc_NodeUpdateInformation  New node update information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewData::SetNodeUpdateInformation(const std::vector<C_OscViewNodeUpdate> & orc_NodeUpdateInformation)
{
   this->mc_NodeUpdateInformation = orc_NodeUpdateInformation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node update information

   \param[in]  ou32_NodeIndex    Node index

   \return
   NULL Update information not found
   Else Valid update information
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscViewNodeUpdate * C_OscViewData::GetNodeUpdateInformation(const uint32_t ou32_NodeIndex) const
{
   const C_OscViewNodeUpdate * pc_Retval = NULL;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      pc_Retval = &this->mc_NodeUpdateInformation[ou32_NodeIndex];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  orc_NodeUpdateInformation  New node update information

   \return
   C_NO_ERR    No error
   C_RANGE     Node index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformation(const uint32_t ou32_NodeIndex,
                                                const C_OscViewNodeUpdate & orc_NodeUpdateInformation)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      this->mc_NodeUpdateInformation[ou32_NodeIndex] = orc_NodeUpdateInformation;
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information path

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  orc_Value         New path
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformationPath(const uint32_t ou32_NodeIndex, const uint32_t ou32_Index,
                                                    const C_SclString & orc_Value,
                                                    const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.SetPath(ou32_Index, orc_Value, oe_Type);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformationParamInfo(const uint32_t ou32_NodeIndex, const uint32_t ou32_Index,
                                                         const C_OscViewNodeUpdateParamInfo & orc_Value)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.SetParamInfo(ou32_Index, orc_Value);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information PEM file path

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformationPemFilePath(const uint32_t ou32_NodeIndex, const C_SclString & orc_Value)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.SetPemFilePath(orc_Value);
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information skip flag

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  oq_SkipFile       New skip flag
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformationSkipUpdateOfPath(const uint32_t ou32_NodeIndex,
                                                                const uint32_t ou32_Index, const bool oq_SkipFile,
                                                                const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.SetSkipUpdateOfPath(ou32_Index, oq_SkipFile, oe_Type);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set skip flag

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  oq_SkipFile       New skip flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformationSkipUpdateOfParamInfo(const uint32_t ou32_NodeIndex,
                                                                     const uint32_t ou32_Index, const bool oq_SkipFile)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.SetSkipUpdateOfParamInfo(ou32_Index, oq_SkipFile);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information PEM file skip flag

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oq_SkipFile       New skip flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformationSkipUpdateOfPemFile(const uint32_t ou32_NodeIndex,
                                                                   const bool oq_SkipFile)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.SetSkipUpdateOfPemFile(oq_SkipFile);
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information states

   \param[in]      ou32_NodeIndex    Node index
   \param[in]      oe_StateSecurity  Security state of node
   \param[in]      oe_StateDebugger  Debugger state of node

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformationStates(const uint32_t ou32_NodeIndex,
                                                      const C_OscViewNodeUpdate::E_StateSecurity oe_StateSecurity,
                                                      const C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.SetStates(oe_StateSecurity, oe_StateDebugger);
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_Index           Index to access
   \param[in]  orc_FilePath         New path
   \param[in]  ou32_LastKnownCrc    Last known CRC for this file

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::SetNodeUpdateInformationParamInfoContent(const uint32_t ou32_NodeIndex,
                                                                const uint32_t ou32_Index,
                                                                const C_SclString & orc_FilePath,
                                                                const uint32_t ou32_LastKnownCrc)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.SetParamInfoContent(ou32_Index, orc_FilePath, ou32_LastKnownCrc);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information path

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::AddNodeUpdateInformationPath(const uint32_t ou32_NodeIndex, const C_SclString & orc_Value,
                                                    const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.AddPath(orc_Value, oe_Type);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information for a parameter set

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::AddNodeUpdateInformationParamInfo(const uint32_t ou32_NodeIndex,
                                                         const C_OscViewNodeUpdateParamInfo & orc_Value)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.AddParamInfo(orc_Value);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name

   \return
   Current name
*/
//----------------------------------------------------------------------------------------------------------------------
const C_SclString & C_OscViewData::GetName(void) const
{
   return this->mc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set name

   \param[in]  orc_Value   New name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewData::SetName(const C_SclString & orc_Value)
{
   this->mc_Name = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connected state of pc in view

   \param[in]  oq_Connected   Flag if pc is connected
   \param[in]  ou32_BusIndex  Bus index PC is connected to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewData::SetPcConnected(const bool oq_Connected, const uint32_t ou32_BusIndex)
{
   this->mc_PcData.SetConnected(oq_Connected, ou32_BusIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to added bus index

   \param[in]  ou32_Index  Added bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewData::OnSyncBusAdded(const uint32_t ou32_Index)
{
   this->mc_PcData.OnSyncBusAdded(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to deleted bus index

   \param[in]  ou32_Index  Deleted bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewData::OnSyncBusDeleted(const uint32_t ou32_Index)
{
   this->mc_PcData.OnSyncBusDeleted(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information path

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to remove
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::RemoveNodeUpdateInformationPath(const uint32_t ou32_NodeIndex, const uint32_t ou32_Index,
                                                       const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.RemovePath(ou32_Index, oe_Type);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information parameter set information

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to remove

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::RemoveNodeUpdateInformationParamInfo(const uint32_t ou32_NodeIndex, const uint32_t ou32_Index)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.RemoveParamInfo(ou32_Index);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information PEM file path

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::RemoveNodeUpdateInformationPemFilePath(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.RemovePemFilePath();
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all node update information paths as appropriate for the type

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::ClearNodeUpdateInformationAsAppropriate(const uint32_t ou32_NodeIndex,
                                                               const C_OscViewNodeUpdate::E_GenericFileType oe_Type)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.ClearPathsAsAppropriate(oe_Type);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all node update information parameter set paths for this node

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscViewData::ClearNodeUpdateInformationParamPaths(const uint32_t ou32_NodeIndex)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_OscViewNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.ClearParamPaths();

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}
