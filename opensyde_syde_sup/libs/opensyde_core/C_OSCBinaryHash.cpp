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

bool C_OSCBinaryHash::mhq_HashCompleted = false;
stw_scl::C_SCLString C_OSCBinaryHash::mhc_BinaryHash = "";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor. Set to private
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCBinaryHash::C_OSCBinaryHash(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creates MD5-Checksum of executable running in current process

   \return  void
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCBinaryHash::h_CreateBinaryHash(void)
{
   const stw_scl::C_SCLString c_ExePath = stw_tgl::TGL_GetExePath();

   if (h_GetHashBool() == true)
   {
      return mhc_BinaryHash;
   }
   else
   {
      mhc_BinaryHash = stw_md5::CMD5Checksum::GetMD5(c_ExePath).UpperCase();
      h_SetHashBool(true);
      return mhc_BinaryHash;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set, whether Checksum has already been created or not

   \param[in]       oq_State   represents if checksum has already been created.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCBinaryHash::h_SetHashBool(bool const oq_State)
{
   mhq_HashCompleted = oq_State;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get, whether Checksum has already been created or not

   \return  q_HashCompleted

*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCBinaryHash::h_GetHashBool(void)
{
   return mhq_HashCompleted;
}
