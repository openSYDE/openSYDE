//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO configuration data base

   IO configuration data base

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "C_OSCIOFileDataBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCIOFileDataBase::C_OSCIOFileDataBase(void) :
   u32_ContentVersion(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCIOFileDataBase::~C_OSCIOFileDataBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if all IDs are unique

   \param[in,out] orc_DuplicateIds Duplicate ids

   \retval true  All unique
   \retval false Duplicate Ids found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCIOFileDataBase::CheckIdsUnique(std::vector<stw_scl::C_SCLString> & orc_DuplicateIds) const
{
   bool q_Retval = true;

   std::vector<stw_scl::C_SCLString> c_AllIds;

   //Find all valid IDs
   for (uint32 u32_ItDomain = 0UL; u32_ItDomain < this->GetDomainSize(); ++u32_ItDomain)
   {
      const C_OSCIOFileDataDomain * const pc_Domain = this->GetDomainFileDataConst(u32_ItDomain);
      tgl_assert(pc_Domain != NULL);
      if (pc_Domain != NULL)
      {
         c_AllIds.push_back(pc_Domain->c_Id);
         for (uint32 u32_ItChannel = 0UL; u32_ItChannel < pc_Domain->c_ChannelUseCases.size(); ++u32_ItChannel)
         {
            const C_OSCIOFileDataChannelUseCase & rc_Channel = pc_Domain->c_ChannelUseCases[u32_ItChannel];
            c_AllIds.push_back(rc_Channel.c_Id);
         }
         for (uint32 u32_ItParam = 0UL; u32_ItParam < pc_Domain->c_Parameters.size(); ++u32_ItParam)
         {
            const C_OSCIOFileDataStruct & rc_Param = pc_Domain->c_Parameters[u32_ItParam];
            c_AllIds.push_back(rc_Param.c_Id);
            for (uint32 u32_ItParamElem = 0UL; u32_ItParamElem < rc_Param.c_StructElements.size(); ++u32_ItParamElem)
            {
               const C_OSCIOFileDataElement & rc_ParamElem = rc_Param.c_StructElements[u32_ItParamElem];
               c_AllIds.push_back(rc_ParamElem.c_Id);
            }
         }
      }
   }

   //Check all IDs
   for (uint32 u32_ItCompare1 = 0UL; u32_ItCompare1 < c_AllIds.size(); ++u32_ItCompare1)
   {
      for (uint32 u32_ItCompare2 = u32_ItCompare1; u32_ItCompare2 < c_AllIds.size(); ++u32_ItCompare2)
      {
         //Check every string, but don't compare with itself
         const stw_scl::C_SCLString & rc_Comp1 = c_AllIds[u32_ItCompare1];
         if ((u32_ItCompare1 != u32_ItCompare2) && (rc_Comp1 == c_AllIds[u32_ItCompare2]))
         {
            bool q_AlreadyExists = false;
            for (uint32 u32_ItOut = 0UL; u32_ItOut < orc_DuplicateIds.size(); ++u32_ItOut)
            {
               if (rc_Comp1 == orc_DuplicateIds[u32_ItOut])
               {
                  q_AlreadyExists = true;
                  break;
               }
            }
            if (q_AlreadyExists == false)
            {
               orc_DuplicateIds.push_back(rc_Comp1);
            }
            q_Retval = false;
         }
      }
   }

   return q_Retval;
}
