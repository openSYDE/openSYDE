//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for bus specific lines (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GiLiBus.h"
#include "C_GtGetText.h"
#include "C_GiSyLineWidget.h"
#include "gitypes.h"
#include "C_PuiSdUtil.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index          Index of data element in system definition
   \param[in]     oru64_ID             Unique ID
   \param[in]     opc_TextElementName  Pointer to text element for showing bus name
   \param[in]     oq_DoErrorCheck      Optional flag to trigger error check directly in constructor
   \param[in]     opc_Points           Points for line
   \param[in]     orq_MiddleLine       Indicator if line(s) should have a different colored central line
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBus::C_GiLiBus(const stw_types::sint32 & ors32_Index, const uint64 & oru64_ID,
                     C_GiTextElementBus * const opc_TextElementName, const bool oq_DoErrorCheck,
                     const std::vector<QPointF> * const opc_Points, const bool & orq_MiddleLine,
                     QGraphicsItem * const opc_Parent) :
   C_GiLiLineGroup(opc_Points, orq_MiddleLine, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eBUS),
   C_GiUnique(oru64_ID),
   mpc_TextElementName(opc_TextElementName)
{
   // Init z order
   this->setZValue(mf64_ZORDER_INIT_BUS);

   //Allow hover events for tool tip hide
   this->setAcceptHoverEvents(true);

   if (opc_Points == NULL)
   {
      const stw_opensyde_gui_logic::C_PuiSdBus * const pc_UIBus = C_PuiSdHandler::h_GetInstance()->GetUIBus(ms32_Index);
      if (pc_UIBus != NULL)
      {
         stw_opensyde_gui_logic::C_PuiSdBus c_UIBus = *pc_UIBus;
         //Safety first
         if (c_UIBus.c_UIInteractionPoints.size() < 2)
         {
            c_UIBus.c_UIInteractionPoints.push_back(QPointF(0.0, 0.0));
            c_UIBus.c_UIInteractionPoints.push_back(QPointF(25.0, 25.0));
         }
         Init(c_UIBus.c_UIInteractionPoints);
         C_PuiSdHandler::h_GetInstance()->SetUIBus(ms32_Index, c_UIBus);
         this->LoadData();
      }
   }

   this->m_UpdateTextElementName();
   if (this->mpc_TextElementName != NULL)
   {
      // initial values?
      if ((this->mpc_TextElementName->pos().x() == 0.0) &&
          (this->mpc_TextElementName->pos().y() == 0.0))
      {
         QPointF c_Pos = this->GetPos();
         // Add a little offset to the text element
         c_Pos.setX(c_Pos.x() + 5.0);
         c_Pos.setY(c_Pos.y() + 20.0);
         this->mpc_TextElementName->setPos(c_Pos);
      }
      connect(this->mpc_TextElementName, &C_GiTextElementBus::destroyed, this, &C_GiLiBus::m_UnregisterTextElement);
   }

   //System definition specific check
   if (oq_DoErrorCheck == true)
   {
      C_GiLiBus::CheckBusForChanges();
   }

   connect(this, &C_GiLiBus::SigSubItemMoves, this, &C_GiLiBus::m_BusWasMoved);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiBus::~C_GiLiBus()
{
   if (this->mpc_TextElementName != NULL)
   {
      disconnect(this->mpc_TextElementName, &C_GiTextElementBus::destroyed, this, &C_GiLiBus::m_UnregisterTextElement);
   }
   //lint -e{1740}  no memory leak because of the parent of mpc_TextElementName and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiLiBus::type() const
{
   return msn_GRAPHICS_ITEM_BUS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_GiLiBus::GetName(void) const
{
   //Translation: Default bus name
   QString c_Name = C_GtGetText::h_GetText("Bus");

   const stw_opensyde_core::C_OSCSystemBus * pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(ms32_Index);

   if (pc_Bus != NULL)
   {
      c_Name = pc_Bus->c_Name.c_str();
   }

   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the CAN bitrate in a prepared string

   \param[in]     oq_WithComma      If true, a comma will be added at the beginning of the string

   \return
   Text with CAN bitrate or empty String in case of an Ethernet bus
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_GiLiBus::GetBitrate(const bool oq_WithComma) const
{
   QString c_Bitrate = "";
   const stw_opensyde_core::C_OSCSystemBus * pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(ms32_Index);

   if ((pc_Bus != NULL) &&
       (pc_Bus->e_Type == C_OSCSystemBus::eCAN))
   {
      if (oq_WithComma == true)
      {
         c_Bitrate = ", ";
      }
      c_Bitrate += "Bitrate: " + QString::number(pc_Bus->u64_BitRate / 1000ULL) + " kbit/s";
   }

   return c_Bitrate;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::LoadData(void)
{
   const C_OSCSystemBus * const pc_OSCBus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(ms32_Index);
   const stw_opensyde_gui_logic::C_PuiSdBus * const pc_UIBus = C_PuiSdHandler::h_GetInstance()->GetUIBus(ms32_Index);

   if (pc_UIBus != NULL)
   {
      this->LoadBasicData(*pc_UIBus);
      this->SetMiddleLineColor(pc_UIBus->c_UIColorMiddleLine);
   }
   //Object name for test
   if (pc_OSCBus != NULL)
   {
      this->setObjectName(QString("Bus: %1").arg(pc_OSCBus->c_Name.c_str()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::UpdateData(void)
{
   const stw_opensyde_gui_logic::C_PuiSdBus * const pc_UIBus = C_PuiSdHandler::h_GetInstance()->GetUIBus(ms32_Index);

   if (pc_UIBus != NULL)
   {
      stw_opensyde_gui_logic::C_PuiSdBus c_UIBus = *pc_UIBus;
      this->UpdateBasicData(c_UIBus);
      c_UIBus.c_UIColorMiddleLine = this->GetMiddleLineColor();
      C_PuiSdHandler::h_GetInstance()->SetUIBus(ms32_Index, c_UIBus);
   }

   this->m_UpdateTextElementName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::DeleteData(void)
{
   C_PuiSdHandler::h_GetInstance()->RemoveBus(ms32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate hint to display
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::GenerateHint(void)
{
   bool q_Entry = false;
   QString c_ToolTip;
   const uint32 u32_BusIndex = static_cast<uint32>(this->GetIndex());
   const C_OSCSystemBus * pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_BusIndex);

   //Translation: 1 = Bus name
   //title
   this->SetDefaultToolTipHeading(this->GetName());

   //comment
   if (pc_Bus->c_Comment.IsEmpty() == false)
   {
      c_ToolTip = pc_Bus->c_Comment.c_str();
      c_ToolTip.append("\n\n");
   }

   if (pc_Bus != NULL)
   {
      c_ToolTip.append(QString("Bus ID: %1").arg(QString::number(pc_Bus->u8_BusID)));

      if (pc_Bus->e_Type == C_OSCSystemBus::eCAN)
      {
         c_ToolTip.append(QString("\nBitrate: %1 kbit/s").arg(QString::number(pc_Bus->u64_BitRate /
                                                                              static_cast<uint64>(1000))));
      }
   }

   c_ToolTip.append(C_GtGetText::h_GetText("\n\nConnected nodes:"));

   for (uint32 u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItNode)
   {
      const C_OSCNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItNode);

      if (pc_Node != NULL)
      {
         for (uint32 u32_ItComInterface = 0; u32_ItComInterface < pc_Node->c_Properties.c_ComInterfaces.size();
              ++u32_ItComInterface)
         {
            const C_OSCNodeComInterfaceSettings & rc_ComInterface =
               pc_Node->c_Properties.c_ComInterfaces[u32_ItComInterface];
            if (rc_ComInterface.q_IsBusConnected == true)
            {
               if (u32_BusIndex == rc_ComInterface.u32_BusIndex)
               {
                  //Translation: 1 = Bus name, 2 = Interface, 3 = Node ID
                  c_ToolTip.append("\n");
                  c_ToolTip.append(QString(C_GtGetText::h_GetText("%1 (Interface: %2, Node ID: %3)")).
                                   arg(pc_Node->c_Properties.c_Name.c_str(), //Node
                                       C_PuiSdUtil::h_GetInterfaceName(rc_ComInterface.e_InterfaceType,
                                                                       rc_ComInterface.u8_InterfaceNumber), //Interface
                                       QString::number(rc_ComInterface.u8_NodeID)));                        //Node
                                                                                                            // ID
                  q_Entry = true;
               }
            }
         }
      }
   }
   if (q_Entry == false)
   {
      //Translation: No bus interface connected
      c_ToolTip.append(QString(C_GtGetText::h_GetText("\n-")));
   }

   this->SetDefaultToolTipContent(c_ToolTip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the style of the provided element

   Warning: Only expected to work if the provided item is of the same type as this element

   \param[in] opc_GuidelineItem Detailed input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::CopyStyle(const QGraphicsItem * const opc_GuidelineItem)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_GiLiBus * const pc_Item = dynamic_cast<const C_GiLiBus * const>(opc_GuidelineItem);

   if (pc_Item != NULL)
   {
      this->ApplyStyle(pc_Item->GetColor(), pc_Item->GetWidth());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::SetZValueCustom(const float64 of64_ZValue)
{
   C_GiLiLineGroup::SetZValueCustom(of64_ZValue);
   //Apply to data
   this->UpdateData();
   //Signal update
   Q_EMIT this->SigChangedZOrder();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item disabled look flag

   \param[in] oq_Disabled Flag if item is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::SetDisabledLook(const bool oq_Disabled)
{
   C_GiLiLineGroup::SetDisabledLook(oq_Disabled);

   if (this->mpc_TextElementName != NULL)
   {
      this->mpc_TextElementName->SetDisabledLook(oq_Disabled);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in]     opc_InputDialog      Dialog with style result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::ApplyStyle(const QColor & orc_LineColor, const stw_types::sintn osn_Width)
{
   this->SetColor(orc_LineColor);
   this->SetWidth(osn_Width);
   this->TriggerSigChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking the bus data for errors and updates the conflict icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::CheckBusForChanges(void)
{
   if (this->mpc_TextElementName != NULL)
   {
      this->mpc_TextElementName->SetErrorState(C_PuiSdHandler::h_GetInstance()->CheckBusConflict(this->ms32_Index));

      //check for new name
      this->mpc_TextElementName->SetText(this->GetName());
      this->mpc_TextElementName->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Flag for displaying interaction points and handling multiselection mode

   false:
   Deactivate interaction points
   Move complete object

   \param[in] orq_ResizeActive Flag if resize mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::SetResizing(const bool & orq_ResizeActive)
{
   C_GiLiLineGroup::SetResizing(orq_ResizeActive);

   if (this->mpc_TextElementName != NULL)
   {
      if (this->me_ActiveResizeMode == eALL)
      {
         this->mpc_TextElementName->setSelected(true);
      }
      else
      {
         this->mpc_TextElementName->setSelected(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the associated bus text element

   \return
   Pointer to the text element
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiTextElementBus * C_GiLiBus::GetTextElementBus(void)
{
   return this->mpc_TextElementName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Hide tool tip

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   Q_EMIT this->SigHideToolTip();

   C_GiLiLineGroup::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hover leave event slot

   Here: Hide tool tip

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   Q_EMIT this->SigHideToolTip();

   C_GiLiLineGroup::hoverLeaveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the position of the text element

   \param[in]     orc_PositionDifference      Dialog with style result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::m_BusWasMoved(const stw_types::sint32 & ors32_LineIndex, const QPointF & orc_PositionDifference)
{
   if ((this->mpc_TextElementName != NULL) &&
       (ors32_LineIndex == 0) &&
       (this->mpc_TextElementName->isSelected() == false))
   {
      // Move the textelement only if it is not selected. If it is selected, the movement will be handled by the
      // item itself.
      this->mpc_TextElementName->moveBy(orc_PositionDifference.x(), orc_PositionDifference.y());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::m_UpdateTextElementName(void)
{
   if (this->mpc_TextElementName != NULL)
   {
      this->mpc_TextElementName->SetText(this->GetName());
      this->mpc_TextElementName->SetBusIndex(this->ms32_Index);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unregister text element for bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiBus::m_UnregisterTextElement(void)
{
   this->mpc_TextElementName = NULL;
}
