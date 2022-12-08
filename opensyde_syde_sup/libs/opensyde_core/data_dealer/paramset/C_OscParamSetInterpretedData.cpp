//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage for interpreted param set data (implementation)

   Storage for interpreted param set data

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscParamSetInterpretedData.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;

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
C_OscParamSetInterpretedData::C_OscParamSetInterpretedData(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear internally stored content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedData::Clear()
{
   this->c_FileInfo.Clear();
   this->c_InterpretedNodes.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add some general information for one param set file

   \param[in] orc_FileInfo Optional general file information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedData::AddInterpretedFileData(const C_OscParamSetInterpretedFileInfoData & orc_FileInfo)
{
   this->c_FileInfo = orc_FileInfo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get interpreted node data for specified node

   \param[in] orc_NodeName Node name to look for

   \return
   NULL Interpreted node data not found
   Else Valid interpreted node data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscParamSetInterpretedNode * C_OscParamSetInterpretedData::GetInterpretedDataForNode(
   const C_SclString & orc_NodeName)
const
{
   const C_OscParamSetInterpretedNode * pc_Retval = NULL;

   for (uint32_t u32_ItInterpretedNode = 0; u32_ItInterpretedNode < this->c_InterpretedNodes.size();
        ++u32_ItInterpretedNode)
   {
      const C_OscParamSetInterpretedNode & rc_CurInterpretedNode =
         this->c_InterpretedNodes[u32_ItInterpretedNode];
      if (rc_CurInterpretedNode.c_Name == orc_NodeName)
      {
         pc_Retval = &this->c_InterpretedNodes[u32_ItInterpretedNode];
      }
   }

   return pc_Retval;
}
