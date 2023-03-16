//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_SebToolboxUtil.hpp"
#include "C_OgeLabToolboxHeadingGroup.hpp"
#include "C_OgeLabToolboxHeadingGroupBig.hpp"
#include "C_SdUtil.hpp"
#include "C_SdTopologyListWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SebToolboxUtil::hs32_LABEL_SIZE = 33;
const int32_t C_SebToolboxUtil::hs32_HEADING_SPACER_SIZE_TOP = 47;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create new C_SdTopologyListWidget with heading

   \param[in]     orc_Name        Heading for list
   \param[in,out] opc_Layout      Layout to add list to
   \param[in,out] orc_ListWidgets Lists widget to store new pointer to
   \param[in,out] opc_Parent      Parent widget

   \return
   Pointer to created C_SdTopologyListWidget
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdTopologyListWidget * C_SebToolboxUtil::h_AddNewList(const QString & orc_Name, QVBoxLayout * const opc_Layout,
                                                        QVector<C_SdTopologyListWidget *> & orc_ListWidgets,
                                                        QWidget * const opc_Parent)
{
   C_SdTopologyListWidget * pc_Retval = NULL;

   if (opc_Layout != NULL)
   {
      pc_Retval = new C_SdTopologyListWidget(opc_Parent);
      int32_t s32_Index;

      if (orc_Name != "")
      {
         C_OgeLabToolboxHeadingGroup * const pc_Heading = new C_OgeLabToolboxHeadingGroup(opc_Parent);
         pc_Heading->setText(orc_Name);
         pc_Retval->SetGroupName(orc_Name);

         // Configure
         pc_Retval->setDropIndicatorShown(false);

         // Heading
         opc_Layout->addWidget(pc_Heading);
         pc_Heading->setMaximumHeight(C_SebToolboxUtil::hs32_LABEL_SIZE);
         pc_Heading->setMinimumHeight(C_SebToolboxUtil::hs32_LABEL_SIZE);
         s32_Index = opc_Layout->indexOf(pc_Heading);
         opc_Layout->setStretch(s32_Index, 0);
      } //lint !e429  //no memory leak because of the parent of pc_Heading and the Qt memory management
      else
      {
         // Configure
         pc_Retval->setDropIndicatorShown(false);
         pc_Retval->setContextMenuPolicy(Qt::CustomContextMenu);
      }

      //List
      opc_Layout->addWidget(pc_Retval);
      orc_ListWidgets.push_back(pc_Retval);
      s32_Index = opc_Layout->indexOf(pc_Retval);
      opc_Layout->setStretch(s32_Index, 0);
   } //lint !e429  //no memory leak because of the parent of pc_Heading,the call of addWidget and the Qt memory
     // management

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new section heading

   \param[in]     orc_Name           Heading name
   \param[in,out] opc_Layout         Layout to add heading to
   \param[in,out] opc_Parent         Parent widget
   \param[in]     oq_AddSpacerBefore Optional flag to add spacer on top

   \return
   Pointer to frame separator item
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeFraSeparator * C_SebToolboxUtil::h_AddNewHeading(const QString & orc_Name, QVBoxLayout * const opc_Layout,
                                                      QWidget * const opc_Parent, const bool oq_AddSpacerBefore)
{
   C_OgeFraSeparator * pc_FrameSeparator = NULL;

   if (opc_Layout != NULL)
   {
      C_OgeLabToolboxHeadingGroupBig * const pc_Heading = new C_OgeLabToolboxHeadingGroupBig(opc_Parent);
      pc_FrameSeparator = new C_OgeFraSeparator(opc_Parent);
      QVBoxLayout * const pc_FrameLayout = new QVBoxLayout();
      int32_t s32_Index;

      // Optional top spacer
      if (oq_AddSpacerBefore == true)
      {
         QSpacerItem * const pc_SpacerTop = new QSpacerItem(0, C_SebToolboxUtil::hs32_HEADING_SPACER_SIZE_TOP);
         opc_Layout->addSpacerItem(pc_SpacerTop);
      } //lint !e429  //no memory leak because of the parent of pc_Heading,the call of addWidget and the Qt memory
      //management
      // Set name to heading
      pc_Heading->setText(orc_Name);

      // Heading
      opc_Layout->addWidget(pc_Heading);
      pc_Heading->setMaximumHeight(C_SebToolboxUtil::hs32_LABEL_SIZE);
      pc_Heading->setMinimumHeight(C_SebToolboxUtil::hs32_LABEL_SIZE);
      s32_Index = opc_Layout->indexOf(pc_Heading);
      opc_Layout->setStretch(s32_Index, 0);

      // add frame separator between heading and subheading (in dashboard toolbox) to new layout
      pc_FrameLayout->addWidget(pc_FrameSeparator);
      pc_FrameSeparator->setMinimumHeight(1);

      // add new layout with frame separator to existing layout
      opc_Layout->addLayout(pc_FrameLayout);
      pc_FrameLayout->setContentsMargins(13, 0, 0, 0);
   } //lint !e429  //no memory leak because of the parent of the call of addSpacerItem and the Qt
   // memory management
   return pc_FrameSeparator;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new section heading

   \param[in]     orc_Name           Heading name
   \param[in,out] opc_Layout         Layout to add heading to
   \param[in,out] opc_Parent         Parent widget

   \return
   Pointer to svg icon button (add node and clear-all-nodes)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OgePubIconOnly *> C_SebToolboxUtil::h_AddNewUserHeading(const QString & orc_Name,
                                                                      QVBoxLayout * const opc_Layout,
                                                                      QWidget * const opc_Parent)
{
   std::vector<C_OgePubIconOnly *> c_Icons;
   C_OgePubIconOnly * pc_IconButton = NULL;
   C_OgePubIconOnly * pc_ClearAllUserNodesButton = NULL;

   if (opc_Layout != NULL)
   {
      //lint -e429  no memory leak because of the parent of the call of addSpacerItem and the Qt
      // memory management
      C_OgeLabToolboxHeadingGroup * const pc_Heading = new C_OgeLabToolboxHeadingGroup(opc_Parent);
      C_OgeFraSeparator * const pc_FrameSeparator = new C_OgeFraSeparator(opc_Parent);
      QVBoxLayout * const pc_FrameLayout = new QVBoxLayout();
      QHBoxLayout * const pc_HorizontalLayout = new QHBoxLayout();
      pc_IconButton = new C_OgePubIconOnly(opc_Parent);
      pc_ClearAllUserNodesButton = new C_OgePubIconOnly(opc_Parent);

      int32_t s32_Index;

      // Set name to heading
      pc_Heading->setText(orc_Name);

      // Heading
      pc_HorizontalLayout->addWidget(pc_Heading);
      pc_Heading->setMaximumHeight(C_SebToolboxUtil::hs32_LABEL_SIZE);
      pc_Heading->setMinimumHeight(C_SebToolboxUtil::hs32_LABEL_SIZE);
      s32_Index = pc_HorizontalLayout->indexOf(pc_Heading);
      pc_HorizontalLayout->setStretch(s32_Index, 0);

      // Add icon button
      pc_IconButton->setIcon(QIcon("://images/IconAddEnabled.svg"));
      pc_IconButton->setFixedSize(18, 18);
      pc_HorizontalLayout->addWidget(pc_IconButton);

      // Add icon for clear button
      pc_ClearAllUserNodesButton->setIcon(QIcon("://images/system_views/IconClearAllEnabled.svg"));
      pc_ClearAllUserNodesButton->setFixedSize(18, 18);
      pc_HorizontalLayout->addWidget(pc_ClearAllUserNodesButton);

      // Add spacer after button
      pc_HorizontalLayout->addSpacerItem(new QSpacerItem(7, 0, QSizePolicy::Fixed));

      // Add horizontal layout with heading and button to toolbox
      opc_Layout->addLayout(pc_HorizontalLayout);

      // add frame separator between heading and subheading (in dashboard toolbox) to new layout
      pc_FrameLayout->addWidget(pc_FrameSeparator);
      pc_FrameSeparator->setMinimumHeight(1);

      // add new layout with frame separator to existing layout
      opc_Layout->addLayout(pc_FrameLayout);
      pc_FrameLayout->setContentsMargins(13, 0, 0, 0);

      c_Icons.push_back(pc_IconButton);
      c_Icons.push_back(pc_ClearAllUserNodesButton);
   }
   return c_Icons;
} //lint !e593  //no memory leak because of the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add free elements to specified list widget

   \param[in,out] opc_ListWidget List widget to add free elements to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebToolboxUtil::h_InitFreeElements(QListWidget * const opc_ListWidget)
{
   if (opc_ListWidget != NULL)
   {
      C_SebToolboxUtil::h_AddElementToList(opc_ListWidget, C_GtGetText::h_GetText(
                                              "Text element"), ":/images/system_definition/TextElement.png", "",
                                           C_GtGetText::h_GetText("Text element"));
      C_SebToolboxUtil::h_AddElementToList(opc_ListWidget, C_GtGetText::h_GetText(
                                              "Boundary"), ":/images/system_definition/Boundary.png", "",
                                           C_GtGetText::h_GetText("Boundary"));
      C_SebToolboxUtil::h_AddElementToList(opc_ListWidget, C_GtGetText::h_GetText(
                                              "Line/Arrow"), ":/images/system_definition/LineArrow.png", "",
                                           C_GtGetText::h_GetText("Line/Arrow"));
      C_SebToolboxUtil::h_AddElementToList(opc_ListWidget, C_GtGetText::h_GetText(
                                              "Image"), ":/images/system_definition/Image.svg", "",
                                           C_GtGetText::h_GetText("Image"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add specified item to list

   \param[in,out] opc_ListWidget     List
   \param[in]     orc_Text           Item text
   \param[in]     orc_IconPath       Item icon path
   \param[in]     orc_IconPathDark   Item dark icon path
   \param[in]     orc_ToolTipContent Tool tip content text
   \param[in]     orc_ToolTipHeading Tool tip heading text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebToolboxUtil::h_AddElementToList(QListWidget * const opc_ListWidget, const QString & orc_Text,
                                          const QString & orc_IconPath, const QString & orc_IconPathDark,
                                          const QString & orc_ToolTipContent, const QString & orc_ToolTipHeading)
{
   if (opc_ListWidget != NULL)
   {
      QListWidgetItem * pc_Item;
      QIcon c_Icon;
      opc_ListWidget->addItem(orc_Text);
      pc_Item = opc_ListWidget->item(opc_ListWidget->count() - 1);
      c_Icon = C_SdUtil::h_InitStaticIconSvg(orc_IconPath, opc_ListWidget->iconSize());
      pc_Item->setData(ms32_USER_ROLE_ADDITIONAL_INFORMATION, orc_Text);
      pc_Item->setData(ms32_USER_ROLE_PIXMAP_BRIGHT_MODE, static_cast<QPixmap>(orc_IconPath));
      if (orc_IconPathDark.compare("") == 0)
      {
         pc_Item->setData(ms32_USER_ROLE_PIXMAP_DARK_MODE, static_cast<QPixmap>(orc_IconPath));
      }
      else
      {
         pc_Item->setData(ms32_USER_ROLE_PIXMAP_DARK_MODE, static_cast<QPixmap>(orc_IconPathDark));
      }
      pc_Item->setData(ms32_USER_ROLE_TOOL_TIP_HEADING, orc_ToolTipHeading);
      pc_Item->setData(ms32_USER_ROLE_TOOL_TIP_CONTENT, orc_ToolTipContent);
      pc_Item->setIcon(c_Icon);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append final spacer to layout

   \param[in,out] opc_Layout     Layout to append spacer to
   \param[in,out] opc_ListWidget Last list widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebToolboxUtil::h_AddFinalSpacer(QVBoxLayout * const opc_Layout, QListWidget * const opc_ListWidget)
{
   if ((opc_Layout != NULL) && (opc_ListWidget != NULL))
   {
      QSpacerItem * pc_Spacer;

      // get index of last added list
      int32_t s32_Index = opc_Layout->indexOf(opc_ListWidget);
      ++s32_Index;

      // add the spacer
      opc_Layout->insertSpacing(s32_Index, 10);

      // configure spacer

      pc_Spacer = dynamic_cast<QSpacerItem *>(opc_Layout->itemAt(s32_Index));
      if (pc_Spacer != NULL)
      {
         // without this call the spacer will not work correctly
         pc_Spacer->changeSize(20, 10, QSizePolicy::Expanding);
      }
      opc_Layout->setStretch(s32_Index, 1);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebToolboxUtil::C_SebToolboxUtil(void)
{
}
