//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for add and remove bend point actions (implementation)

   Base class for add and remove bend point actions

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SebUnoBendPointBaseCommand.h"
#include "C_SebScene.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace std;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene         Pointer to currently active scene
   \param[in]     orc_IDs           Affected unique IDs
   \param[in]     orc_Text          Command description
   \param[in]     orc_ScenePosition Scene position to bend or remove bend point at
   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBendPointBaseCommand::C_SebUnoBendPointBaseCommand(QGraphicsScene * const opc_Scene,
                                                           const std::vector<uint64> & orc_IDs,
                                                           const QString & orc_Text, const QPointF & orc_ScenePosition,
                                                           QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, orc_Text, opc_Parent),
   mc_ScenePosition(orc_ScenePosition),
   ms32_Index(-1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoBendPointBaseCommand::~C_SebUnoBendPointBaseCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete bend point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBendPointBaseCommand::m_DeleteBend(void)
{
   const C_SebScene * const pc_Scene = dynamic_cast<const C_SebScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      vector<QGraphicsItem *> c_Items = this->m_GetSceneItems();
      for (uint32 u32_ItItem = 0; u32_ItItem < c_Items.size(); ++u32_ItItem)
      {
         if (this->ms32_Index < 0)
         {
            this->ms32_Index = pc_Scene->RemoveBendLine(c_Items[u32_ItItem], this->mc_ScenePosition);
         }
         else
         {
            (void)pc_Scene->RemoveBendLine(c_Items[u32_ItItem], this->mc_ScenePosition, &(this->ms32_Index));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add bend point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoBendPointBaseCommand::m_AddBend(void)
{
   C_SebScene * const pc_Scene = dynamic_cast<C_SebScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      vector<QGraphicsItem *> c_Items = this->m_GetSceneItems();
      for (uint32 u32_ItItem = 0; u32_ItItem < c_Items.size(); ++u32_ItItem)
      {
         if (this->ms32_Index < 0)
         {
            this->ms32_Index = pc_Scene->BendLine(c_Items[u32_ItItem], this->mc_ScenePosition);
         }
         else
         {
            (void)pc_Scene->BendLine(c_Items[u32_ItItem], this->mc_ScenePosition, &(this->ms32_Index));
         }
      }
   }
}
