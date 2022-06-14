//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "C_PopServiceProjSettingsModel.h"
#include "C_GtGetText.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"

#include "C_UsHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *
 *  \param[in]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopServiceProjSettingsModel::C_PopServiceProjSettingsModel(QObject * const opc_Parent) :
   C_TblTreeModelCheckable(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopServiceProjSettingsModel::~C_PopServiceProjSettingsModel(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the model

   \param[in]       orc_ElementIndices     View indices

   \retval  C_NO_ERR    Model init without error
   \retval  C_RANGE     View is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PopServiceProjSettingsModel::Init(const std::vector<stw_types::uint32> & orc_ElementIndices)
{
   sint32 s32_Return = C_NO_ERR;

   C_TblTreeModelCheckableItem * const pc_VisibleRootItem = new C_TblTreeModelCheckableItem();

   this->beginResetModel();

   this->m_CleanUpLastModel();

   this->mpc_InvisibleRootItem = new C_TblTreeModelCheckableItem();

   pc_VisibleRootItem->c_Name = C_GtGetText::h_GetText("System Commissioning");

   for (uint32 u32_ViewCnt = 0; u32_ViewCnt < orc_ElementIndices.size(); ++u32_ViewCnt)
   {
      C_TblTreeModelCheckableItem * const pc_TreeNode = new C_TblTreeModelCheckableItem();
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(u32_ViewCnt);
      if (pc_View != NULL)
      {
         pc_TreeNode->c_Name = "VIEW #" + QString::number(u32_ViewCnt + 1UL) + " - " + pc_View->GetName();
         pc_TreeNode->q_Enabled = true;
         pc_TreeNode->q_CheckBoxVisible = true;
         pc_TreeNode->q_Selectable = true;
         pc_VisibleRootItem->AddChild(pc_TreeNode);

         C_TblTreeModelCheckableItem * const pc_TreeNodeSetup = new C_TblTreeModelCheckableItem();
         pc_TreeNodeSetup->c_Name = C_GtGetText::h_GetText("Setup");
         pc_TreeNodeSetup->q_Enabled = true;
         pc_TreeNodeSetup->q_CheckBoxVisible = true;
         pc_TreeNodeSetup->q_Selectable = true;

         C_TblTreeModelCheckableItem * const pc_TreeNodeUpdate = new C_TblTreeModelCheckableItem();
         pc_TreeNodeUpdate->c_Name = C_GtGetText::h_GetText("Update");
         pc_TreeNodeUpdate->q_Enabled = true;
         pc_TreeNodeUpdate->q_CheckBoxVisible = true;
         pc_TreeNodeUpdate->q_Selectable = true;

         C_TblTreeModelCheckableItem * const pc_TreeNodeDashb = new C_TblTreeModelCheckableItem();
         pc_TreeNodeDashb->c_Name = C_GtGetText::h_GetText("Dashboard");
         pc_TreeNodeDashb->q_Enabled = true;
         pc_TreeNodeDashb->q_CheckBoxVisible = true;
         pc_TreeNodeDashb->q_Selectable = true;

         pc_TreeNode->AddChild(pc_TreeNodeSetup);
         pc_TreeNode->AddChild(pc_TreeNodeUpdate);
         pc_TreeNode->AddChild(pc_TreeNodeDashb);
      }
      else
      {
         s32_Return = C_RANGE;
         delete pc_TreeNode;
      }
   }

   pc_VisibleRootItem->q_Enabled = true;
   pc_VisibleRootItem->q_CheckBoxVisible = true;
   pc_VisibleRootItem->q_Selectable = true;

   this->m_CheckInitItems(*pc_VisibleRootItem, orc_ElementIndices);

   this->mpc_InvisibleRootItem->AddChild(pc_VisibleRootItem);
   this->endResetModel();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the checked items for service mode permissions

   \param[out]       orc_ElementIndices         View indices
   \param[out]      orc_ChildIndicesPerElement   Vector of vectors for child indices per view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopServiceProjSettingsModel::GetCheckedItems(std::vector<uint32> & orc_ElementIndices,
                                                    std::vector<std::vector<uint32> > & orc_ChildIndicesPerElement)
const
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
            C_TblTreeModelCheckableItem * const pc_ViewItem =
               dynamic_cast<C_TblTreeModelCheckableItem *>(c_VisibleRootChildren.at(u32_ItVisibleRootChild));
            if (pc_ViewItem != NULL)
            {
               std::vector<C_TblTreSimpleItem *> c_ViewChildren = pc_ViewItem->c_Children;
               std::vector<uint32> c_ChildIndices;
               for (uint32 u32_ItNodeChild = 0; u32_ItNodeChild < c_ViewChildren.size(); u32_ItNodeChild++)
               {
                  C_TblTreeModelCheckableItem * const pc_ChildItem =
                     dynamic_cast<C_TblTreeModelCheckableItem *>(c_ViewChildren.at(u32_ItNodeChild));
                  if ((pc_ChildItem != NULL) && (pc_ChildItem->e_CheckState == Qt::Checked))
                  {
                     // we need to distinguish checked items from unchecked (so 1 = checked, 0 = not checked)
                     c_ChildIndices.push_back(1);
                  }
                  else
                  {
                     c_ChildIndices.push_back(0);
                  }
               }

               if (c_ChildIndices.empty() == false)
               {
                  orc_ElementIndices.push_back(pc_ViewItem->u32_Index);
                  orc_ChildIndicesPerElement.push_back(c_ChildIndices);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the count of checked items for service mode permissions
 *
 *  \return
 *
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PopServiceProjSettingsModel::GetCheckedItemCount(void) const
{
   uint32 u32_CheckedItems = 0;

   std::vector<uint32> c_ViewIndices;
   std::vector<std::vector<uint32> > c_ChildIndicesPerView;

   this->GetCheckedItems(c_ViewIndices, c_ChildIndicesPerView);

   for (uint32 u32_ItItem = 0; u32_ItItem < c_ChildIndicesPerView.size(); ++u32_ItItem)
   {
      for (uint32 u32_ItChild = 0; u32_ItChild < c_ChildIndicesPerView[u32_ItItem].size(); ++u32_ItChild)
      {
         if (c_ChildIndicesPerView[u32_ItItem][u32_ItChild] == 1)
         {
            u32_CheckedItems++;
         }
      }
   }
   return u32_CheckedItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks all items that need to be checked based on user settings

   \param[in,out]  opc_VisibleRootItem  Parent to check its children
   \param[in]      orc_NodeIndices      Node indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopServiceProjSettingsModel::m_CheckInitItems(C_TblTreeModelCheckableItem & orc_VisibleRootItem,
                                                     const std::vector<uint32> & orc_ViewIndices) const
{
   uint32 u32_CheckedChildCounter;
   uint32 u32_CheckedViewCounter = 0;
   uint32 u32_CheckedPartViewCounter = 0;

   for (uint32 u32_ItRootChildren = 0; u32_ItRootChildren < orc_VisibleRootItem.c_Children.size();
        u32_ItRootChildren++)
   {
      C_TblTreeModelCheckableItem * const pc_ViewItem =
         dynamic_cast<C_TblTreeModelCheckableItem *>(orc_VisibleRootItem.c_Children.at(u32_ItRootChildren));
      if (pc_ViewItem != NULL)
      {
         // load user setting for this view
         std::array<bool, 3> c_ViewConfig;
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(u32_ItRootChildren);

         // when no service project was created based on the current project until now, the tree shall be completely
         // unchecked
         // otherwise we load the last saved permissions from user_settings.ini
         if (C_UsHandler::h_GetInstance()->GetLastKnownServiceProjectPath() == "")
         {
            c_ViewConfig.fill(false);
         }
         else
         {
            c_ViewConfig = C_UsHandler::h_GetInstance()->GetViewPermissions(pc_View->GetName());
         }

         for (uint32 u32_ItNodeIndices = 0; u32_ItNodeIndices < orc_ViewIndices.size(); u32_ItNodeIndices++)
         {
            if (pc_ViewItem->u32_Index == orc_ViewIndices.at(u32_ItNodeIndices))
            {
               u32_CheckedChildCounter = 0;
               for (uint32 u32_ItNodeChildren = 0; u32_ItNodeChildren < pc_ViewItem->c_Children.size();
                    u32_ItNodeChildren++)
               {
                  C_TblTreeModelCheckableItem * const pc_ChildItem =
                     dynamic_cast<C_TblTreeModelCheckableItem *>(pc_ViewItem->c_Children.at(u32_ItNodeChildren));
                  if (pc_ChildItem != NULL)
                  {
                     if (c_ViewConfig[u32_ItNodeChildren] == true)
                     {
                        pc_ChildItem->e_CheckState = Qt::Checked;
                        u32_CheckedChildCounter++;
                     }
                     else
                     {
                        pc_ChildItem->e_CheckState = Qt::Unchecked;
                     }
                  }
               }
               if ((u32_CheckedChildCounter > 0) && (u32_CheckedChildCounter < pc_ViewItem->c_Children.size()))
               {
                  pc_ViewItem->e_CheckState = Qt::PartiallyChecked;
                  u32_CheckedPartViewCounter++;
               }
               else if (u32_CheckedChildCounter == pc_ViewItem->c_Children.size())
               {
                  pc_ViewItem->e_CheckState = Qt::Checked;
                  u32_CheckedViewCounter++;
               }
               else
               {
                  pc_ViewItem->e_CheckState = Qt::Unchecked;
               }
            }
         }
      }
   }

   if (u32_CheckedPartViewCounter > 0)
   {
      orc_VisibleRootItem.e_CheckState = Qt::PartiallyChecked;
   }
   else if (u32_CheckedViewCounter == orc_VisibleRootItem.c_Children.size())
   {
      orc_VisibleRootItem.e_CheckState = Qt::Checked;
   }
   else
   {
      orc_VisibleRootItem.e_CheckState = Qt::Unchecked;
   }
}
