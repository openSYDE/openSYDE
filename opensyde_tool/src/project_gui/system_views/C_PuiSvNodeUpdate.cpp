//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Node update data (implementation)

   Node update data

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.12.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "CSCLString.h"
#include "CSCLChecksums.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvNodeUpdate::C_PuiSvNodeUpdate(const bool oq_FileBased) :
   u32_NodeUpdatePosition(0U),
   mq_FileBased(oq_FileBased)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue Hash value with init [in] value and result [out] value

   \created     29.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvNodeUpdate::CalcHash(uint32 & oru32_HashValue) const
{
   for (uint32 u32_ItUpdate = 0; u32_ItUpdate < this->mc_ApplicationPaths.size(); ++u32_ItUpdate)
   {
      const QString & rc_QtData = this->mc_ApplicationPaths[u32_ItUpdate];
      const stw_scl::C_SCLString c_Data = rc_QtData.toStdString().c_str();
      stw_scl::C_SCLChecksums::CalcCRC32(c_Data.c_str(), c_Data.Length(), oru32_HashValue);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get application paths

   \return
   Current application paths

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const std::vector<QString> & C_PuiSvNodeUpdate::GetApplicationPaths(void) const
{
   return this->mc_ApplicationPaths;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set application paths

   \param[in] orc_Value New application paths

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvNodeUpdate::SetApplicationPaths(const std::vector<QString> & orc_Value)
{
   this->mc_ApplicationPaths = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_ApplicationIndex Application index

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationAdded(const uint32 ou32_ApplicationIndex)
{
   // No synchronization for file based devices necessary.
   // These devices have only one application block as file container and has no 1:1 relation to the application paths
   if (this->mq_FileBased == false)
   {
      if (ou32_ApplicationIndex <= this->mc_ApplicationPaths.size())
      {
         this->mc_ApplicationPaths.insert(this->mc_ApplicationPaths.begin() + ou32_ApplicationIndex, QString());
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_ApplicationSourceIndex Application source index
   \param[in] ou32_ApplicationTargetIndex Application target index

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationMoved(const uint32 ou32_ApplicationSourceIndex,
                                                   const uint32 ou32_ApplicationTargetIndex)
{
   // No synchronization for file based devices necessary.
   // These devices have only one application block as file container and has no 1:1 relation to the application paths
   if (this->mq_FileBased == false)
   {
      QString c_Entry;

      if (ou32_ApplicationSourceIndex < this->mc_ApplicationPaths.size())
      {
         c_Entry = this->mc_ApplicationPaths[ou32_ApplicationSourceIndex];
         this->mc_ApplicationPaths.erase(this->mc_ApplicationPaths.begin() + ou32_ApplicationSourceIndex);
      }
      if (ou32_ApplicationTargetIndex <= this->mc_ApplicationPaths.size())
      {
         this->mc_ApplicationPaths.insert(this->mc_ApplicationPaths.begin() + ou32_ApplicationTargetIndex, c_Entry);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_ApplicationIndex Application index

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvNodeUpdate::OnSyncNodeApplicationAboutToBeDeleted(const uint32 ou32_ApplicationIndex)
{
   // No synchronization for file based devices necessary.
   // These devices have only one application block as file container and has no 1:1 relation to the application paths
   if (this->mq_FileBased == false)
   {
      if (ou32_ApplicationIndex < this->mc_ApplicationPaths.size())
      {
         this->mc_ApplicationPaths.erase(this->mc_ApplicationPaths.begin() + ou32_ApplicationIndex);
      }
   }
   else
   {
      // The only one application block (the file container) must be deleted. No application paths can exist anymore.
      this->mc_ApplicationPaths.clear();
   }
}
