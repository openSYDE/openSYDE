//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCBinaryHash.h"
#include "TGLFile.h"
#include "CMD5Checksum.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;
using namespace stw_tgl;
using namespace stw_md5;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

bool C_OSCBinaryHash::hq_HashCompleted = false;
stw_scl::C_SCLString C_OSCBinaryHash::hc_BinaryHash = "";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor. Set to private
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCBinaryHash::C_OSCBinaryHash()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creates MD5-Checksum of executable running in current process

   \return  void
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCBinaryHash::h_CreateBinaryHash(void)
{
   stw_scl::C_SCLString c_ExePath = stw_tgl::TGL_GetExePath();
   if (h_GetHashBool() == true)
   {
      return hc_BinaryHash;
   }
   else
   {
      hc_BinaryHash = stw_md5::CMD5Checksum::GetMD5(c_ExePath).UpperCase();
      h_SetHashBool(true);
      return hc_BinaryHash;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set, whether Checksum has already been created or not

   \param[in]       q_State   represents if checksum has already been created.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCBinaryHash::h_SetHashBool(bool const q_State)
{
   hq_HashCompleted = q_State;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get, whether Checksum has already been created or not

   \return  q_HashCompleted

*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCBinaryHash::h_GetHashBool()
{
   return hq_HashCompleted;
}
