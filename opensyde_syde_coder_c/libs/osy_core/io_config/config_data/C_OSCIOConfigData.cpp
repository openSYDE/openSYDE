//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group config data for IO config

   Group config data for IO config

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCIOConfigData.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
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
C_OSCIOConfigData::C_OSCIOConfigData(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain size

   \return
   Current domain count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCIOConfigData::GetDomainSize() const
{
   return this->c_Domains.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain config data const

   \param[in] ou32_Index Index

   \return
   NULL Invalid index
   Else Valid domain
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCIOConfigDataDomain * C_OSCIOConfigData::GetDomainConfigDataConst(const uint32 ou32_Index) const
{
   const C_OSCIOConfigDataDomain * pc_Retval = NULL;

   if (ou32_Index < this->c_Domains.size())
   {
      pc_Retval = &this->c_Domains[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain file data const

   \param[in] ou32_Index Index

   \return
   NULL Invalid index
   Else Valid domain
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCIOFileDataDomain * C_OSCIOConfigData::GetDomainFileDataConst(const uint32 ou32_Index) const
{
   const C_OSCIOFileDataDomain * pc_Retval = NULL;

   if (ou32_Index < this->c_Domains.size())
   {
      pc_Retval = &this->c_Domains[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add domain

   \param[in] orc_Domain Domain
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCIOConfigData::AddDomain(const C_OSCIOFileDataDomain & orc_Domain)
{
   this->c_Domains.push_back(orc_Domain);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set domain config

   \param[in] ou32_Index Index
   \param[in] orc_Domain Domain

   \return
   C_NO_ERR Value set
   C_RANGE  Invalid index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigData::SetDomainConfig(const uint32 ou32_Index, const C_OSCIOConfigDataDomain & orc_Domain)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->c_Domains.size())
   {
      this->c_Domains[ou32_Index] = orc_Domain;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
