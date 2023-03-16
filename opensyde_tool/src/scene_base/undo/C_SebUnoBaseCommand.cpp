//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all topology undo commands (implementation)

   Base class for all topology undo commands

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SebUnoBaseCommand.hpp"
#include "C_SebScene.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace std;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene  Pointer to currently active scene
   \param[in]     orc_Ids    Affected unique IDs
   \param[in]     orc_Text   Command description
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBaseCommand::C_SebUnoBaseCommand(QGraphicsScene * const opc_Scene, const vector<uint64_t> & orc_Ids,
                                         const QString & orc_Text, QUndoCommand * const opc_Parent) :
   QUndoCommand(orc_Text, opc_Parent),
   mpc_Scene(opc_Scene),
   mc_Ids(orc_Ids)
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
   mc_Ids(opc_Prev->mc_Ids)
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
   c_Retval.resize(this->mc_Ids.size(), NULL);
   for (uint32_t u32_ItId = 0; u32_ItId < this->mc_Ids.size(); ++u32_ItId)
   {
      c_Retval[u32_ItId] = m_GetSceneItem(this->mc_Ids[u32_ItId]);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pointer for connected item

   \param[in] oru64_Id Unique ID

   \return
   NULL: not found
   else: pointer to item
*/
//----------------------------------------------------------------------------------------------------------------------
QGraphicsItem * C_SebUnoBaseCommand::m_GetSceneItem(const uint64_t & oru64_Id) const
{
   QGraphicsItem * pc_Retval;

   const stw::opensyde_gui::C_SebScene * const pc_DetailedScene =
      dynamic_cast<const stw::opensyde_gui::C_SebScene * const>(mpc_Scene);

   if (pc_DetailedScene != NULL)
   {
      pc_Retval = pc_DetailedScene->GetItemById(oru64_Id);
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

   if (this->mc_Ids.size() == opc_Command->mc_Ids.size())
   {
      vector<uint64_t>::const_iterator c_ItId2 = opc_Command->mc_Ids.begin();
      for (vector<uint64_t>::const_iterator c_ItId1 = this->mc_Ids.begin(); c_ItId1 != this->mc_Ids.end();
           ++c_ItId1)
      {
         if (*c_ItId1 != *c_ItId2)
         {
            q_Retval = false;
         }
         ++c_ItId2;
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
const std::vector<uint64_t> C_SebUnoBaseCommand::m_GetIds(void) const
{
   return this->mc_Ids;
}
