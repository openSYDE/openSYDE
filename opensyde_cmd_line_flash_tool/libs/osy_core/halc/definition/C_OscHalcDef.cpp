//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscHalcDef.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

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
C_OscHalcDef::C_OscHalcDef(void) :
   C_OscHalcDefBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain size

   \return
   Current domain count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscHalcDef::GetDomainSize(void) const
{
   return static_cast<uint32_t>(this->mc_Domains.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain

   \param[in]  ou32_Index  Index

   \return
   NULL Invalid index
   Else Valid domain
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscHalcDefDomain * C_OscHalcDef::GetDomainDefDataConst(const uint32_t ou32_Index) const
{
   const C_OscHalcDefDomain * pc_Retval = NULL;

   if (ou32_Index < this->mc_Domains.size())
   {
      pc_Retval = &this->mc_Domains[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add domain

   \param[in]  orc_Domain  Domain
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDef::AddDomain(const C_OscHalcDefDomain & orc_Domain)
{
   this->mc_Domains.push_back(orc_Domain);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle file load post processing

    Note: not necessary here
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in this implementation
void C_OscHalcDef::HandleFileLoadPostProcessing(void)
{
}
