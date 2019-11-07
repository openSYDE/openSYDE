//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all topology undo commands (implementation)

   Base class for all topology undo commands

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SebUnoBaseCommand.h"
#include "C_SebScene.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace std;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene  Pointer to currently active scene
   \param[in]     orc_IDs    Affected unique IDs
   \param[in]     orc_Text   Command description
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBaseCommand::C_SebUnoBaseCommand(QGraphicsScene * const opc_Scene, const vector<uint64> & orc_IDs,
                                         const QString & orc_Text, QUndoCommand * const opc_Parent) :
   QUndoCommand(orc_Text, opc_Parent),
   mpc_Scene(opc_Scene),
   mc_IDs(orc_IDs)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy constructor

   \param[in]     opc_Prev   Original command
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBaseCommand::C_SebUnoBaseCommand(const C_SebUnoBaseCommand * const opc_Prev, QUndoCommand * const opc_Parent) :
   QUndoCommand(opc_Prev->text(), opc_Parent),
   mpc_Scene(opc_Prev->mpc_Scene),
   mc_IDs(opc_Prev->mc_IDs)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBaseCommand::~C_SebUnoBaseCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pointers for connected items

   \return
   array with items:
   NULL: not found
   else: pointer to item
*/
//----------------------------------------------------------------------------------------------------------------------
vector<QGraphicsItem *> C_SebUnoBaseCommand::m_GetSceneItems(void) const
{
   vector<QGraphicsItem *> c_Retval;
   c_Retval.resize(this->mc_IDs.size(), NULL);
   for (uint32 u32_ItID = 0; u32_ItID < this->mc_IDs.size(); ++u32_ItID)
   {
      c_Retval[u32_ItID] = m_GetSceneItem(this->mc_IDs[u32_ItID]);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pointer for connected item

   \param[in] oru64_ID Unique ID

   \return
   NULL: not found
   else: pointer to item
*/
//----------------------------------------------------------------------------------------------------------------------
QGraphicsItem * C_SebUnoBaseCommand::m_GetSceneItem(const uint64 & oru64_ID) const
{
   QGraphicsItem * pc_Retval;

   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const stw_opensyde_gui::C_SebScene * const pc_DetailedScene =
      dynamic_cast<const stw_opensyde_gui::C_SebScene * const>(mpc_Scene);

   if (pc_DetailedScene != NULL)
   {
      pc_Retval = pc_DetailedScene->GetItemByID(oru64_ID);
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if merge possible (=same IDs affected)

   \param[in] opc_Command Command to merge with

   \return
   true: merge possible
   false: do not attempt merge
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SebUnoBaseCommand::m_MergePossible(const C_SebUnoBaseCommand * const opc_Command) const
{
   bool q_Retval = true;

   if (this->mc_IDs.size() == opc_Command->mc_IDs.size())
   {
      vector<uint64>::const_iterator c_ItID2 = opc_Command->mc_IDs.begin();
      for (vector<uint64>::const_iterator c_ItID1 = this->mc_IDs.begin(); c_ItID1 != this->mc_IDs.end();
           ++c_ItID1)
      {
         if (*c_ItID1 != *c_ItID2)
         {
            q_Retval = false;
         }
         ++c_ItID2;
      }
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all unique IDs

   \return
   All unique IDs
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint64> C_SebUnoBaseCommand::GetIDs(void) const
{
   return this->mc_IDs;
}
