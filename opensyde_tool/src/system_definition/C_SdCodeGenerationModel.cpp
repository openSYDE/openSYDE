//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for code generation items

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
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdCodeGenerationItem::C_SdCodeGenerationItem(void) :
   C_TblTreItem(),
   e_CheckState(Qt::Unchecked),
   q_Checkable(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up model

   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdCodeGenerationModel::C_SdCodeGenerationModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent)
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
/*! \brief  Initialization of the model

   \param[in]  orc_NodeIndices   Node indices

   \return
   Checked Application Counter
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdCodeGenerationModel::Init(const std::vector<uint32> & orc_NodeIndices)
{
   uint32 u32_CheckedAppsCounter;
   C_SdCodeGenerationItem * const pc_VisibleRootItem = new C_SdCodeGenerationItem();

   const std::vector<C_OSCNode> & rc_Nodes = C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().c_Nodes;

   this->beginResetModel();

   // Clear
   this->m_CleanUpLastModel();

   this->mpc_InvisibleRootItem = new C_SdCodeGenerationItem();

   pc_VisibleRootItem->c_Name = C_GtGetText::h_GetText("Network Topology");

   for (uint32 u32_NodeCounter = 0; u32_NodeCounter < rc_Nodes.size(); u32_NodeCounter++)
   {
      bool q_HasProgApp = false;
      const C_OSCNode & rc_Node = rc_Nodes.at(u32_NodeCounter);
      C_SdCodeGenerationItem * const pc_NodeItem = new C_SdCodeGenerationItem();

      pc_NodeItem->c_Name = rc_Node.c_Properties.c_Name.c_str();
      pc_NodeItem->u32_Index = u32_NodeCounter;

      std::vector<C_OSCNodeApplication> c_Applications = rc_Node.c_Applications;

      if (c_Applications.empty() == false)
      {
         for (uint32 u32_ApplicationCounter = 0; u32_ApplicationCounter < c_Applications.size();
              u32_ApplicationCounter++)
         {
            const C_OSCNodeApplication & rc_Application = c_Applications.at(u32_ApplicationCounter);

            if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               C_SdCodeGenerationItem * const pc_ApplicationItem = new C_SdCodeGenerationItem();
               pc_ApplicationItem->c_Name = (rc_Application.c_Name + " (Code structure version: " +
                                             rc_Application.u16_GenCodeVersion + ")").c_str();
               pc_ApplicationItem->c_ToolTipHeading = rc_Application.c_Name.c_str();
               pc_ApplicationItem->c_ToolTipContent = (rc_Application.c_Comment + "\n\nData Block Pproperties:\n" +
                                                       "  Project Path: " + rc_Application.c_ProjectPath + "\n" +
                                                       "  Output File: " + rc_Application.c_GeneratePath + "\n" +
                                                       "  Gen. Code Directory: " + rc_Application.c_ResultPath + "\n" +
                                                       "  Code Generator: " + rc_Application.c_CodeGeneratorPath +
                                                       "\n" +
                                                       "  IDE Call: " + rc_Application.c_IDECall).c_str();

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

   u32_CheckedAppsCounter = this->m_CheckInitItems(*pc_VisibleRootItem, orc_NodeIndices);

   this->mpc_InvisibleRootItem->AddChild(pc_VisibleRootItem);
   this->endResetModel();

   return u32_CheckedAppsCounter;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the count of checked items to code generation

   \return
   Checked Application Counter
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdCodeGenerationModel::GetCheckedItemCount(void) const
{
   std::vector<C_TblTreSimpleItem *> c_InvisibleRootChildren = this->mpc_InvisibleRootItem->c_Children;
   std::vector<uint32> c_CheckedApps;
   for (uint32 u32_ItInvisibleRootChild = 0;
        u32_ItInvisibleRootChild < c_InvisibleRootChildren.size(); u32_ItInvisibleRootChild++)
   {
      C_SdCodeGenerationItem * const pc_VisibleRootItem =
         dynamic_cast<C_SdCodeGenerationItem *>(c_InvisibleRootChildren.at(u32_ItInvisibleRootChild));
      if (pc_VisibleRootItem != NULL)
      {
         std::vector<C_TblTreSimpleItem *> c_VisibleRootChildren = pc_VisibleRootItem->c_Children;
         for (uint32 u32_ItVisibleRootChild = 0;
              u32_ItVisibleRootChild < c_VisibleRootChildren.size(); u32_ItVisibleRootChild++)
         {
            C_SdCodeGenerationItem * const pc_NodeItem =
               dynamic_cast<C_SdCodeGenerationItem *>(c_VisibleRootChildren.at(u32_ItVisibleRootChild));
            if (pc_NodeItem != NULL)
            {
               std::vector<C_TblTreSimpleItem *> c_NodeChildren = pc_NodeItem->c_Children;
               std::vector<uint32> c_AppIndices;
               for (uint32 u32_ItNodeChild = 0; u32_ItNodeChild < c_NodeChildren.size(); u32_ItNodeChild++)
               {
                  C_SdCodeGenerationItem * const pc_AppItem =
                     dynamic_cast<C_SdCodeGenerationItem *>(c_NodeChildren.at(u32_ItNodeChild));
                  if ((pc_AppItem != NULL) && (pc_AppItem->e_CheckState == Qt::Checked))
                  {
                     c_AppIndices.push_back(pc_AppItem->u32_Index);
                  }
               }
               for (uint32 u32_Counter = 0; u32_Counter < c_AppIndices.size(); u32_Counter++)
               {
                  c_CheckedApps.push_back(c_AppIndices.at(u32_Counter));
               }
            }
         }
      }
   }
   return c_CheckedApps.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the checked items to code generation

   \param[out]  orc_NodeIndices         Node indices (ID)
   \param[out]  orc_AppIndicesPerNode   Vector of vectors of application indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationModel::GetCheckedItems(std::vector<uint32> & orc_NodeIndices,
                                              std::vector<std::vector<uint32> > & orc_AppIndicesPerNode) const
{
   std::vector<C_TblTreSimpleItem *> c_InvisibleRootChildren = this->mpc_InvisibleRootItem->c_Children;
   orc_NodeIndices.clear();
   orc_AppIndicesPerNode.clear();
   for (uint32 u32_ItInvisibleRootChild = 0;
        u32_ItInvisibleRootChild < c_InvisibleRootChildren.size(); u32_ItInvisibleRootChild++)
   {
      C_SdCodeGenerationItem * const pc_VisibleRootItem =
         dynamic_cast<C_SdCodeGenerationItem *>(c_InvisibleRootChildren.at(u32_ItInvisibleRootChild));
      if (pc_VisibleRootItem != NULL)
      {
         std::vector<C_TblTreSimpleItem *> c_VisibleRootChildren = pc_VisibleRootItem->c_Children;
         for (uint32 u32_ItVisibleRootChild = 0;
              u32_ItVisibleRootChild < c_VisibleRootChildren.size(); u32_ItVisibleRootChild++)
         {
            C_SdCodeGenerationItem * const pc_NodeItem =
               dynamic_cast<C_SdCodeGenerationItem *>(c_VisibleRootChildren.at(u32_ItVisibleRootChild));
            if (pc_NodeItem != NULL)
            {
               std::vector<C_TblTreSimpleItem *> c_NodeChildren = pc_NodeItem->c_Children;
               std::vector<uint32> c_AppIndices;
               for (uint32 u32_ItNodeChild = 0; u32_ItNodeChild < c_NodeChildren.size(); u32_ItNodeChild++)
               {
                  C_SdCodeGenerationItem * const pc_AppItem =
                     dynamic_cast<C_SdCodeGenerationItem *>(c_NodeChildren.at(u32_ItNodeChild));
                  if ((pc_AppItem != NULL) && (pc_AppItem->e_CheckState == Qt::Checked))
                  {
                     c_AppIndices.push_back(pc_AppItem->u32_Index);
                  }
               }

               if (c_AppIndices.empty() == false)
               {
                  orc_NodeIndices.push_back(pc_NodeItem->u32_Index);
                  orc_AppIndicesPerNode.push_back(c_AppIndices);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree column count

   \param[in] orc_Parent Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdCodeGenerationModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdCodeGenerationModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Return;

   if ((osn_Role == static_cast<sintn>(Qt::CheckStateRole)) &&
       (orc_Index.isValid() == true))
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      C_SdCodeGenerationItem * const pc_TreeItem =
         static_cast<C_SdCodeGenerationItem * const>(orc_Index.internalPointer());

      if (pc_TreeItem != NULL)
      {
         c_Return = static_cast<sintn>(pc_TreeItem->e_CheckState);
      }
   }
   else
   {
      c_Return = C_TblTreModel::data(orc_Index, osn_Role);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  osn_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdCodeGenerationModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const sintn osn_Role)
{
   bool q_Return = false;

   if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
   {
      if (orc_Index.isValid() == true)
      {
         //lint -e{9079}  Result of Qt interface restrictions, set by index function
         C_SdCodeGenerationItem * const pc_TreeItem =
            static_cast<C_SdCodeGenerationItem * const>(orc_Index.internalPointer());

         if (pc_TreeItem != NULL)
         {
            QModelIndex c_StartIndex = orc_Index;
            QModelIndex c_EndIndex = orc_Index;

            // Switching from unchecked and partial to checked
            pc_TreeItem->e_CheckState = (pc_TreeItem->e_CheckState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;

            // Check if the item has children
            this->m_CheckChildren(pc_TreeItem, c_StartIndex, c_EndIndex);

            // or has a parent
            this->m_CheckParent(pc_TreeItem, orc_Index, c_StartIndex);

            Q_EMIT (this->dataChanged(c_StartIndex, c_EndIndex, QVector<stw_types::sintn>() << osn_Role));
            q_Return = true;
         }
      }
   }
   else
   {
      q_Return = C_TblTreModel::setData(orc_Index, orc_Value, osn_Role);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get item flags

   \param[in] orc_Index  Index

   \return
   Item flags
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdCodeGenerationModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = C_TblTreModel::flags(orc_Index);

   if (orc_Index.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_SdCodeGenerationItem * const pc_TreeItem =
         static_cast<const C_SdCodeGenerationItem * const>(orc_Index.internalPointer());
      if (pc_TreeItem != NULL)
      {
         if (pc_TreeItem->q_Checkable == true)
         {
            c_Retval = c_Retval | Qt::ItemIsUserCheckable;
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all children (recursive) and adapts the check state to the state of the parent

   \param[in,out]   opc_TreeItem   Parent to check its children
   \param[in]       orc_ItemIndex  Index of the parent
   \param[out]      orc_EndIndex   Last index of the last child
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationModel::m_CheckChildren(C_SdCodeGenerationItem * const opc_TreeItem,
                                              const QModelIndex & orc_ItemIndex, QModelIndex & orc_EndIndex)
{
   if (opc_TreeItem->c_Children.size() > 0)
   {
      uint32 u32_ChildCounter;
      const uint32 u32_IndexLastChild = static_cast<uint32>(opc_TreeItem->c_Children.size() - 1UL);

      orc_EndIndex = this->index(u32_IndexLastChild, 0, orc_ItemIndex);

      for (u32_ChildCounter = 0U; u32_ChildCounter < opc_TreeItem->c_Children.size(); ++u32_ChildCounter)
      {
         C_SdCodeGenerationItem * const pc_ChildItem =
            dynamic_cast<C_SdCodeGenerationItem * const>(opc_TreeItem->c_Children[u32_ChildCounter]);
         const QModelIndex c_ChildIndex = this->index(u32_ChildCounter, 0, orc_ItemIndex);

         // Set all children to the same checked state
         if (pc_ChildItem != NULL)
         {
            if (pc_ChildItem->q_Enabled == true)
            {
               pc_ChildItem->e_CheckState = opc_TreeItem->e_CheckState;

               // Check the children for their children
               this->m_CheckChildren(pc_ChildItem, c_ChildIndex, orc_EndIndex);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the parent (recursive) and adapts the check state to the state of the child

   \param[in]       opc_TreeItem   Parent to check its children
   \param[in]       orc_ItemIndex  Index of the child
   \param[out]      orc_StartIndex First index of the first parent
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationModel::m_CheckParent(const C_SdCodeGenerationItem * const opc_TreeItem,
                                            const QModelIndex & orc_ItemIndex, QModelIndex & orc_StartIndex)
{
   if (opc_TreeItem->pc_Parent != NULL)
   {
      C_SdCodeGenerationItem * const pc_ParentItem =
         dynamic_cast<C_SdCodeGenerationItem * const>(opc_TreeItem->pc_Parent);

      if (pc_ParentItem != NULL)
      {
         bool q_AllAreChecked = true;
         bool q_AtLeastOneIsChecked = false;
         uint32 u32_ChildCounter;
         const QModelIndex c_ParentIndex = this->parent(orc_ItemIndex);
         orc_StartIndex = c_ParentIndex;

         // Check the other children to get the necessary state of the parent
         for (u32_ChildCounter = 0U; u32_ChildCounter < pc_ParentItem->c_Children.size(); ++u32_ChildCounter)
         {
            C_SdCodeGenerationItem * const pc_ChildItem =
               dynamic_cast<C_SdCodeGenerationItem * const>(pc_ParentItem->c_Children[u32_ChildCounter]);

            if (pc_ChildItem != NULL)
            {
               if (pc_ChildItem->e_CheckState == Qt::Unchecked)
               {
                  q_AllAreChecked = false;
               }
               else
               {
                  q_AtLeastOneIsChecked = true;
               }
            }
         }

         if (q_AllAreChecked == true)
         {
            pc_ParentItem->e_CheckState = Qt::Checked;
         }
         else if (q_AtLeastOneIsChecked == true)
         {
            pc_ParentItem->e_CheckState = Qt::PartiallyChecked;
         }
         else
         {
            pc_ParentItem->e_CheckState = Qt::Unchecked;
         }

         // Check the parent of the parent
         this->m_CheckParent(pc_ParentItem, c_ParentIndex, orc_StartIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clean up current model

   Warning: pointer might be invalid after call of this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationModel::m_CleanUpLastModel(void)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      delete (this->mpc_InvisibleRootItem);
      this->mpc_InvisibleRootItem = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all items which are used on level and adapts the check state to the state of the parent

   \param[in,out]  opc_VisibleRootItem  Parent to check its children
   \param[in]      orc_NodeIndices      Node indices
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdCodeGenerationModel::m_CheckInitItems(C_SdCodeGenerationItem & orc_VisibleRootItem,
                                                 const std::vector<uint32> & orc_NodeIndices) const
{
   uint32 u32_CheckedAppsCounter = 0;
   uint32 u32_CheckedNodeCounter = 0;
   uint32 u32_NotEnabledNodeCounter = 0;

   for (uint32 u32_ItRootChildren = 0; u32_ItRootChildren < orc_VisibleRootItem.c_Children.size();
        u32_ItRootChildren++)
   {
      C_SdCodeGenerationItem * const pc_NodeItem =
         dynamic_cast<C_SdCodeGenerationItem *>(orc_VisibleRootItem.c_Children.at(u32_ItRootChildren));
      if (pc_NodeItem != NULL)
      {
         for (uint32 u32_ItNodeIndices = 0; u32_ItNodeIndices < orc_NodeIndices.size(); u32_ItNodeIndices++)
         {
            if (pc_NodeItem->u32_Index == orc_NodeIndices.at(u32_ItNodeIndices))
            {
               for (uint32 u32_ItNodeChildren = 0; u32_ItNodeChildren < pc_NodeItem->c_Children.size();
                    u32_ItNodeChildren++)
               {
                  C_SdCodeGenerationItem * const pc_AppItem =
                     dynamic_cast<C_SdCodeGenerationItem *>(pc_NodeItem->c_Children.at(u32_ItNodeChildren));
                  if (pc_AppItem != NULL)
                  {
                     u32_CheckedAppsCounter++;
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

   return u32_CheckedAppsCounter;
}
