//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for file generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"

#include "C_GtGetText.h"
#include "C_SdCodeGenerationModel.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up model

   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdCodeGenerationModel::C_SdCodeGenerationModel(QObject * const opc_Parent) :
   C_TblTreeModelCheckable(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdCodeGenerationModel::~C_SdCodeGenerationModel(void)
{
   this->m_CleanUpLastModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the model.
            Due to inheritance the Init function has a return value. But in this specific derivation the return value is
            not needed, hence it was made const to make PC-Lint happy.

   \param[in]  orc_ElementIndices   Node indices

   \return
   C_NO_ERR    no problems (see above)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdCodeGenerationModel::Init(const std::vector<uint32> & orc_ElementIndices)
{
   const sint32 s32_RETURN = C_NO_ERR;
   C_TblTreeModelCheckableItem * const pc_VisibleRootItem = new C_TblTreeModelCheckableItem();

   const std::vector<C_OSCNode> & rc_Nodes = C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().c_Nodes;

   this->beginResetModel();

   // Clear
   this->m_CleanUpLastModel();

   this->mpc_InvisibleRootItem = new C_TblTreeModelCheckableItem();

   pc_VisibleRootItem->c_Name = C_GtGetText::h_GetText("Network Topology");

   for (uint32 u32_NodeCounter = 0; u32_NodeCounter < rc_Nodes.size(); u32_NodeCounter++)
   {
      bool q_HasProgApp = false;
      const C_OSCNode & rc_Node = rc_Nodes.at(u32_NodeCounter);
      C_TblTreeModelCheckableItem * const pc_NodeItem = new C_TblTreeModelCheckableItem();

      pc_NodeItem->c_Name = rc_Node.c_Properties.c_Name.c_str();
      pc_NodeItem->u32_Index = u32_NodeCounter;

      std::vector<C_OSCNodeApplication> c_Applications = rc_Node.c_Applications;

      if (c_Applications.empty() == false)
      {
         for (uint32 u32_ApplicationCounter = 0; u32_ApplicationCounter < c_Applications.size();
              u32_ApplicationCounter++)
         {
            const C_OSCNodeApplication & rc_Application = c_Applications.at(u32_ApplicationCounter);

            if (rc_Application.e_Type != C_OSCNodeApplication::eBINARY)
            {
               C_TblTreeModelCheckableItem * const pc_ApplicationItem = new C_TblTreeModelCheckableItem();
               const stw_scl::C_SCLString c_Indentation = "\n   ";
               stw_scl::C_SCLString c_ToolTip;
               if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
               {
                  pc_ApplicationItem->c_Name = (rc_Application.c_Name + " (Source Code; Structure Version: " +
                                                rc_Application.u16_GenCodeVersion + ")").c_str();
               }
               else if (rc_Application.e_Type == C_OSCNodeApplication::ePARAMETER_SET_HALC)
               {
                  pc_ApplicationItem->c_Name = (rc_Application.c_Name + " (Parameter Set Image)").c_str();
               }
               else
               {
                  // nothing to do
               }
               c_ToolTip += rc_Application.c_Comment;
               c_ToolTip += "\n\n";
               c_ToolTip += C_GtGetText::h_GetText("Data Block Properties:");
               c_ToolTip += c_Indentation + C_GtGetText::h_GetText("File Generation: ");
               if (rc_Application.q_Active == true)
               {
                  c_ToolTip += C_GtGetText::h_GetText("active");
               }
               else
               {
                  c_ToolTip += C_GtGetText::h_GetText("inactive");
               }
               c_ToolTip += c_Indentation + C_GtGetText::h_GetText("Project Path: ");
               c_ToolTip += rc_Application.c_ProjectPath;
               c_ToolTip += c_Indentation + C_GtGetText::h_GetText("Generation Directory: ");
               c_ToolTip += rc_Application.c_GeneratePath;
               c_ToolTip += c_Indentation + C_GtGetText::h_GetText("Structure Version: ");
               c_ToolTip += QString::number(rc_Application.u16_GenCodeVersion).toStdString().c_str();
               c_ToolTip += c_Indentation + C_GtGetText::h_GetText("Output File: ");
               if (rc_Application.c_ResultPaths.size() == 2)
               {
                  c_ToolTip += rc_Application.c_ResultPaths[0];
                  c_ToolTip += "\n                        ";
                  c_ToolTip += rc_Application.c_ResultPaths[1];
               }
               else
               {
                  c_ToolTip += rc_Application.c_ResultPaths[0];
               }
               c_ToolTip += c_Indentation + C_GtGetText::h_GetText("File Generator: ");
               c_ToolTip += rc_Application.c_CodeGeneratorPath;
               c_ToolTip += c_Indentation + C_GtGetText::h_GetText("IDE Call: ");
               c_ToolTip += rc_Application.c_IDECall;

               pc_ApplicationItem->c_ToolTipHeading = rc_Application.c_Name.c_str();
               pc_ApplicationItem->c_ToolTipContent = c_ToolTip.c_str();

               pc_ApplicationItem->u32_Index = u32_ApplicationCounter;
               pc_ApplicationItem->q_Enabled = true;
               pc_ApplicationItem->q_Checkable = true;
               pc_ApplicationItem->q_Selectable = true;

               pc_NodeItem->AddChild(pc_ApplicationItem);

               q_HasProgApp = true;
            }
         }
      }

      pc_NodeItem->q_Enabled = q_HasProgApp;
      pc_NodeItem->q_Checkable = q_HasProgApp;
      pc_NodeItem->q_Selectable = q_HasProgApp;

      if (q_HasProgApp == true)
      {
         pc_VisibleRootItem->AddChild(pc_NodeItem);
      }
   } //lint !e593  //no memory leak because of the parent of pc_NodeItem and the Qt memory management

   pc_VisibleRootItem->q_Enabled = true;
   pc_VisibleRootItem->q_Checkable = true;
   pc_VisibleRootItem->q_Selectable = false;

   this->m_CheckInitItems(*pc_VisibleRootItem, orc_ElementIndices);

   this->mpc_InvisibleRootItem->AddChild(pc_VisibleRootItem);
   this->endResetModel();

   return s32_RETURN;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the count of checked items for file generation

   \return
   Checked Application Counter
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdCodeGenerationModel::GetCheckedItemCount(void) const
{
   uint32 u32_CheckedItems = 0;

   std::vector<uint32> c_NodeIndices;
   std::vector<std::vector<uint32> > c_AppIndicesPerNode;

   this->GetCheckedItems(c_NodeIndices, c_AppIndicesPerNode);

   for (uint32 u32_ItItems = 0; u32_ItItems < c_AppIndicesPerNode.size(); ++u32_ItItems)
   {
      u32_CheckedItems += c_AppIndicesPerNode[u32_ItItems].size();
   }

   return u32_CheckedItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the checked items for file generation

   \param[out]  orc_ElementIndices         Node indices (ID)
   \param[out]  orc_ChildIndicesPerElement   Vector of vectors of application indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationModel::GetCheckedItems(std::vector<uint32> & orc_ElementIndices,
                                              std::vector<std::vector<uint32> > & orc_ChildIndicesPerElement) const
{
   std::vector<C_TblTreSimpleItem *> c_InvisibleRootChildren = this->mpc_InvisibleRootItem->c_Children;
   orc_ElementIndices.clear();
   orc_ChildIndicesPerElement.clear();
   for (uint32 u32_ItInvisibleRootChild = 0;
        u32_ItInvisibleRootChild < c_InvisibleRootChildren.size(); u32_ItInvisibleRootChild++)
   {
      C_TblTreeModelCheckableItem * const pc_VisibleRootItem =
         dynamic_cast<C_TblTreeModelCheckableItem *>(c_InvisibleRootChildren.at(u32_ItInvisibleRootChild));
      if (pc_VisibleRootItem != NULL)
      {
         std::vector<C_TblTreSimpleItem *> c_VisibleRootChildren = pc_VisibleRootItem->c_Children;
         for (uint32 u32_ItVisibleRootChild = 0;
              u32_ItVisibleRootChild < c_VisibleRootChildren.size(); u32_ItVisibleRootChild++)
         {
            C_TblTreeModelCheckableItem * const pc_NodeItem =
               dynamic_cast<C_TblTreeModelCheckableItem *>(c_VisibleRootChildren.at(u32_ItVisibleRootChild));
            if (pc_NodeItem != NULL)
            {
               std::vector<C_TblTreSimpleItem *> c_NodeChildren = pc_NodeItem->c_Children;
               std::vector<uint32> c_AppIndices;
               for (uint32 u32_ItNodeChild = 0; u32_ItNodeChild < c_NodeChildren.size(); u32_ItNodeChild++)
               {
                  C_TblTreeModelCheckableItem * const pc_AppItem =
                     dynamic_cast<C_TblTreeModelCheckableItem *>(c_NodeChildren.at(u32_ItNodeChild));
                  if ((pc_AppItem != NULL) && (pc_AppItem->e_CheckState == Qt::Checked))
                  {
                     c_AppIndices.push_back(pc_AppItem->u32_Index);
                  }
               }

               if (c_AppIndices.empty() == false)
               {
                  orc_ElementIndices.push_back(pc_NodeItem->u32_Index);
                  orc_ChildIndicesPerElement.push_back(c_AppIndices);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all items which are used on level and adapts the check state to the state of the parent

   \param[in,out]  opc_VisibleRootItem  Parent to check its children
   \param[in]      orc_NodeIndices      Node indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationModel::m_CheckInitItems(C_TblTreeModelCheckableItem & orc_VisibleRootItem,
                                               const std::vector<uint32> & orc_NodeIndices) const
{
   uint32 u32_CheckedNodeCounter = 0;
   uint32 u32_NotEnabledNodeCounter = 0;

   for (uint32 u32_ItRootChildren = 0; u32_ItRootChildren < orc_VisibleRootItem.c_Children.size();
        u32_ItRootChildren++)
   {
      C_TblTreeModelCheckableItem * const pc_NodeItem =
         dynamic_cast<C_TblTreeModelCheckableItem *>(orc_VisibleRootItem.c_Children.at(u32_ItRootChildren));
      if (pc_NodeItem != NULL)
      {
         for (uint32 u32_ItNodeIndices = 0; u32_ItNodeIndices < orc_NodeIndices.size(); u32_ItNodeIndices++)
         {
            if (pc_NodeItem->u32_Index == orc_NodeIndices.at(u32_ItNodeIndices))
            {
               for (uint32 u32_ItNodeChildren = 0; u32_ItNodeChildren < pc_NodeItem->c_Children.size();
                    u32_ItNodeChildren++)
               {
                  C_TblTreeModelCheckableItem * const pc_AppItem =
                     dynamic_cast<C_TblTreeModelCheckableItem *>(pc_NodeItem->c_Children.at(u32_ItNodeChildren));
                  if (pc_AppItem != NULL)
                  {
                     pc_AppItem->e_CheckState = Qt::Checked;
                  }
               }
               if (pc_NodeItem->q_Enabled == false)
               {
                  u32_NotEnabledNodeCounter++;
               }
               else
               {
                  u32_CheckedNodeCounter++;
                  pc_NodeItem->e_CheckState = Qt::Checked;
               }
            }
         }
      }
   }

   if (u32_CheckedNodeCounter == static_cast<uint32>(orc_VisibleRootItem.c_Children.size() - u32_NotEnabledNodeCounter))
   {
      orc_VisibleRootItem.e_CheckState = Qt::Checked;
   }
   else if ((u32_CheckedNodeCounter > 0) &&
            (u32_CheckedNodeCounter <
             static_cast<uint32>(orc_VisibleRootItem.c_Children.size() - u32_NotEnabledNodeCounter)))
   {
      orc_VisibleRootItem.e_CheckState = Qt::PartiallyChecked;
   }
   else
   {
      orc_VisibleRootItem.e_CheckState = Qt::Unchecked;
   }
}
