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
#include "precomp_headers.hpp"

#include "C_OscBinaryHash.hpp"
#include "TglFile.hpp"
#include "C_Md5Checksum.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::tgl;
using namespace stw::md5;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

bool C_OscBinaryHash::mhq_HashCompleted = false;
stw::scl::C_SclString C_OscBinaryHash::mhc_BinaryHash = "";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor. Set to private
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscBinaryHash::C_OscBinaryHash(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Creates MD5-Checksum of executable running in current process

   \return  void
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscBinaryHash::h_CreateBinaryHash(void)
{
   const stw::scl::C_SclString c_ExePath = stw::tgl::TglGetExePath();

   if (h_GetHashBool() == false)
   {
      mhc_BinaryHash = stw::md5::C_Md5Checksum::GetMD5(c_ExePath).UpperCase();
      h_SetHashBool(true);
   }
   return mhc_BinaryHash;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set, whether Checksum has already been created or not

   \param[in]       oq_State   represents if checksum has already been created.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscBinaryHash::h_SetHashBool(const bool oq_State)
{
   mhq_HashCompleted = oq_State;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get, whether Checksum has already been created or not

   \return  q_HashCompleted

*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscBinaryHash::h_GetHashBool(void)
{
   return mhq_HashCompleted;
}
