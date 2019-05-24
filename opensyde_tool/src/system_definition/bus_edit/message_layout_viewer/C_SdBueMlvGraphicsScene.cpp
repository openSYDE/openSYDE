//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics scene for the message layout viewer (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cstdlib>
#include <ctime> //lint !e829 //this module is specifically for Windows targets; no trouble with unspecified
// behavior expected
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>

#include "stwtypes.h"
#include "constants.h"

#include "TGLUtils.h"
#include "C_SdBueMlvGraphicsScene.h"

#include "C_SdBueMlvBaseItem.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdNodeCanSignal.h"
#include "C_GtGetText.h"
#include "C_OSCCanMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_tgl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::float64 C_SdBueMlvGraphicsScene::mhf64_Space = 2.0;
const stw_types::float64 C_SdBueMlvGraphicsScene::mhf64_ZOrderBelowAllItems = -1.0;
const stw_types::float64 C_SdBueMlvGraphicsScene::mhf64_ZOrderDefault = 0.0;
const stw_types::float64 C_SdBueMlvGraphicsScene::mhf64_ZOrderInteraction = 1.0;
const stw_types::float64 C_SdBueMlvGraphicsScene::mhf64_ZOrderEmptyItem = 10000.0;
const stw_types::float64 C_SdBueMlvGraphicsScene::mhf64_ZOrderHintItem = 10001.0;

const C_SdBueMlvSignalManager::C_SignalItemColors C_SdBueMlvGraphicsScene::mhac_SignalsColors[64] =
{
   // First the background color and as second the font color
   {1U, QColor(172, 255, 250), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {2U, QColor(249, 203, 223), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {3U, QColor(244, 154, 193), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {4U, QColor(236, 0, 140), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {5U, QColor(255, 251, 203), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {6U, QColor(251, 200, 180), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {7U, QColor(246, 153, 157), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {8U, QColor(255, 28, 129), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {9U, QColor(185, 229, 251), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {10U, QColor(188, 183, 220), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {11U, QColor(189, 141, 191), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {12U, QColor(189, 27, 141), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {13U, QColor(191, 226, 201), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {14U, QColor(192, 181, 178), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {15U, QColor(192, 140, 156), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {16U, QColor(254, 58, 163), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {17U, QColor(110, 208, 246), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {18U, QColor(126, 168, 217), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {19U, QColor(136, 130, 190), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {20U, QColor(146, 39, 143), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {21U, QColor(123, 204, 199), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {22U, QColor(134, 166, 177), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {23U, QColor(141, 130, 157), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {24U, QColor(211, 74, 162), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {25U, QColor(0, 174, 239), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {26U, QColor(0, 143, 212), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {27U, QColor(0, 114, 188), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {28U, QColor(47, 49, 147), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {29U, QColor(0, 171, 196), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {30U, QColor(0, 142, 176), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {31U, QColor(0, 113, 156), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {32U, QColor(86, 86, 179), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {33U, QColor(255, 247, 154), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {34U, QColor(253, 198, 138), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {35U, QColor(247, 151, 122), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {36U, QColor(237, 20, 91), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {37U, QColor(255, 242, 0), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {38U, QColor(255, 194, 14), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {39U, QColor(248, 148, 30), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {40U, QColor(237, 28, 36), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {41U, QColor(196, 223, 155), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {42U, QColor(195, 179, 139), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {43U, QColor(193, 138, 123), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {44U, QColor(192, 36, 94), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {45U, QColor(203, 219, 43), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {46U, QColor(200, 177, 47), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {47U, QColor(196, 137, 47), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {48U, QColor(193, 39, 45), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {49U, QColor(131, 202, 157), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {50U, QColor(139, 165, 141), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {51U, QColor(144, 129, 125), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {52U, QColor(149, 44, 97), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {53U, QColor(141, 198, 63), mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_COLOR_8},
   {54U, QColor(145, 162, 61), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {55U, QColor(148, 127, 58), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {56U, QColor(151, 46, 52), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {57U, QColor(0, 169, 157), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {58U, QColor(0, 140, 142), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {59U, QColor(0, 112, 127), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {60U, QColor(52, 52, 101), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {61U, QColor(0, 166, 80), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {62U, QColor(0, 138, 75), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {63U, QColor(0, 112, 70), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10},
   {64U, QColor(55, 54, 57), mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_10}
};

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent              Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvGraphicsScene::C_SdBueMlvGraphicsScene(QObject * const opc_Parent) :
   QGraphicsScene(opc_Parent),
   mpc_ContextMenu(NULL),
   mpc_MessageSyncManager(NULL),
   me_Protocol(C_OSCCanProtocol::eLAYER2),
   mc_MessageId(),
   mu16_MaximumCountBits(64U),
   mf64_SingleItemWidth(50.0),
   mf64_SingleItemHeight(50.0),
   mpc_HoveredSignal(NULL),
   mpc_ActualSignal(NULL),
   me_InteractionMode(C_SdBueMlvSignalManager::eIAM_NONE),
   mc_LastMousePos(0.0, 0.0),
   ms32_LastGridIndex(-1),
   mq_SignalChanged(false)
{
   //Init?
   for (uint32 u32_ItFree = 0UL; u32_ItFree < 64; ++u32_ItFree)
   {
      this->maq_SingalsColorsUsed[u32_ItFree] = false;
   }

   this->m_InitBorderItems();
   this->m_InitEmptyItems();
   this->m_InitProtocolItems();

   connect(this, &C_SdBueMlvGraphicsScene::sceneRectChanged, this, &C_SdBueMlvGraphicsScene::m_SceneRectChanged);

   this->m_SetupContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvGraphicsScene::~C_SdBueMlvGraphicsScene(void)
{
   //lint -e{1540}  no memory leak because of the parent of mpc_BorderItemUpperLeft and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   Must be set before using any signal functions

   \param[in,out] opc_Value Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::SetMessageSyncManager(
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mpc_MessageSyncManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new com protocol

   \param[in] ore_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::SetComProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value)
{
   this->me_Protocol = ore_Value;

   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the actual message to work with

   \param[in]  orc_MessageId   Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::SetMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   const C_OSCCanMessage * const pc_Message =
      stw_opensyde_gui_logic::C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);

   tgl_assert(pc_Message != NULL);
   if (pc_Message != NULL)
   {
      uint32 u32_Counter;
      sint32 s32_Counter;

      // save the identification information
      this->mc_MessageId = orc_MessageId;
      if (this->me_Protocol == C_OSCCanProtocol::eECES)
      {
         // special case DLC is always 8, but byte 7 and byte 8 are reserved for message counter and CRC
         this->mu16_MaximumCountBits = 48U;
         this->mapc_ECeSHints[0]->setVisible(true);
         this->mapc_ECeSHints[1]->setVisible(true);
      }
      else
      {
         this->mu16_MaximumCountBits = static_cast<uint16>(pc_Message->u16_Dlc * 8U);
         this->mapc_ECeSHints[0]->setVisible(false);
         this->mapc_ECeSHints[1]->setVisible(false);
      }

      // remove all previous signals
      this->Clear();

      // adapt the backgrounds to the maximum available bit bounded by the DLC
      for (s32_Counter = 0U; s32_Counter < this->mc_VecEmptyItems.size(); ++s32_Counter)
      {
         if (s32_Counter < this->mu16_MaximumCountBits)
         {
            this->mc_VecEmptyItems[s32_Counter]->SetActive(true);
         }
         else
         {
            this->mc_VecEmptyItems[s32_Counter]->SetActive(false);
         }
      }
      for (s32_Counter = 0U; s32_Counter < mc_VecBorderItemsVertical.size(); ++s32_Counter)
      {
         if (s32_Counter < pc_Message->u16_Dlc)
         {
            this->mc_VecBorderItemsVertical[s32_Counter]->SetActive(true);
         }
         else
         {
            this->mc_VecBorderItemsVertical[s32_Counter]->SetActive(false);
         }
      }

      // Deactivate the top header too if DLC is zero
      this->mpc_BorderItemUpperLeft->SetActive(pc_Message->u16_Dlc != 0U);
      for (s32_Counter = 0U; s32_Counter < mc_VecBorderItemsHorizontal.size(); ++s32_Counter)
      {
         this->mc_VecBorderItemsHorizontal[s32_Counter]->SetActive(pc_Message->u16_Dlc != 0U);
      }

      // add the signals
      for (u32_Counter = 0; u32_Counter < pc_Message->c_Signals.size(); ++u32_Counter)
      {
         this->m_AddSignal(u32_Counter);
      }

      this->m_UpdateSignalManager();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select signal

   \param[in] oru32_SignalIndex     Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::SetSignal(const uint32 ou32_SignalIndex)
{
   sint32 s32_Counter;

   for (s32_Counter = 0U; s32_Counter < this->mc_VecSignals.size(); ++s32_Counter)
   {
      const bool q_Selected = (this->mc_VecSignals[s32_Counter]->GetSignalIndex() == ou32_SignalIndex);
      this->mc_VecSignals[s32_Counter]->SetSelected(q_Selected);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all signals
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::Clear(void)
{
   QVector<C_SdBueMlvSignalManager *>::iterator pc_ItOldItem;

   // remove all signals
   while (this->mc_VecSignals.size() > 0)
   {
      pc_ItOldItem = this->mc_VecSignals.begin();
      this->m_RemoveSignal((*pc_ItOldItem)->GetSignalIndex());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check scene if tool tip for current position is necessary

   \param[in] orc_ScenePos Scene position to check for tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::DisplayToolTip(const QPointF & orc_ScenePos)
{
   //Check if item is hovered
   sint32 s32_Counter;
   C_SdBueMlvSignalManager * pc_HoveredItem = NULL;

   for (s32_Counter = 0U; s32_Counter < this->mc_VecSignals.size(); ++s32_Counter)
   {
      if ((this->mc_VecSignals[s32_Counter] != NULL) &&
          (this->mc_VecSignals[s32_Counter]->IsHovered() == true))
      {
         pc_HoveredItem = this->mc_VecSignals[s32_Counter];
         break;
      }
   }

   //Do not reopen tool tip if there already is an active tool tip
   if (pc_HoveredItem != NULL)
   {
      const sint32 s32_GridIndex = this->m_GetGridIndex(orc_ScenePos);

      // Check for multiple signals on one spot
      if ((s32_GridIndex >= 0) &&
          (s32_GridIndex < 64) &&
          (this->mac_SetGridState[s32_GridIndex].size() > 1))
      {
         // More than one signal on the position
         std::set<C_SdBueMlvSignalManager *>::const_iterator c_It;
         QString c_ToolTipContent = "";

         this->mpc_HoveredSignal = pc_HoveredItem;

         for (c_It = this->mac_SetGridState[s32_GridIndex].begin();
              c_It != this->mac_SetGridState[s32_GridIndex].end();
              ++c_It)
         {
            c_ToolTipContent += (*c_It)->GetName() + "\n";
         }

         Q_EMIT this->SigShowToolTip(orc_ScenePos, C_GtGetText::h_GetText("Following signals are overlapping"),
                                     c_ToolTipContent, true);
      }
      else if (this->mpc_HoveredSignal != pc_HoveredItem)
      {
         // No error, show normal tool tip
         //Update tool tip
         pc_HoveredItem->GenerateHint();
         if (pc_HoveredItem->NotEmpty())
         {
            this->mpc_HoveredSignal = pc_HoveredItem;
            Q_EMIT this->SigShowToolTip(orc_ScenePos, pc_HoveredItem->GetCurrentToolTipHeading(),
                                        pc_HoveredItem->GetCurrentToolTipContent(), false);
         }
      }
      else
      {
         // Nothing to do
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Moving and resizing the signal

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   this->mc_LastMousePos = opc_Event->scenePos();
   this->m_SearchClickedItem(this->mc_LastMousePos);

   if (this->mpc_ActualSignal != NULL)
   {
      this->me_InteractionMode = this->mpc_ActualSignal->GetInteractionMode(this->mc_LastMousePos);

      if ((this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_RESIZELEFT_INTEL) ||
          (this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_RESIZERIGHT_INTEL))
      {
         Q_EMIT this->SigChangeCursor(Qt::SizeHorCursor);
      }
      else if (this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_MOVE)
      {
         Q_EMIT this->SigChangeCursor(Qt::SizeAllCursor);
      }
      else
      {
         // nothing to do
      }
   }
   else
   {
      this->me_InteractionMode = C_SdBueMlvSignalManager::eIAM_NONE;
   }

   QGraphicsScene::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   const sint32 s32_ActGridIndex = this->m_GetGridIndex(opc_Event->scenePos());

   if ((s32_ActGridIndex >= 0) &&
       (this->mpc_ActualSignal != NULL) &&
       (s32_ActGridIndex != this->ms32_LastGridIndex))
   {
      const uint16 u16_StartBit = this->mpc_ActualSignal->GetStartBit();
      const uint16 u16_LastBit = this->mpc_ActualSignal->GetLastBit();

      if (this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_MOVE)
      {
         // move an item
         const sint32 s32_IndexDif = s32_ActGridIndex - this->ms32_LastGridIndex;

         if (this->mpc_ActualSignal->MoveSignal(s32_IndexDif) == true)
         {
            this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
         }
      }
      else if (this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_RESIZELEFT_INTEL)
      {
         // item will resized on the "left" side
         if (s32_ActGridIndex >= static_cast<sint32>(u16_StartBit))
         {
            // update the item
            if (this->mpc_ActualSignal->SetLastBit(static_cast<uint16>(s32_ActGridIndex)) == true)
            {
               this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
            }
         }
         else if (s32_ActGridIndex <= static_cast<sint32>(u16_LastBit))
         {
            // the resizing changed to the other direction
            // update the item and change the mode
            if (this->mpc_ActualSignal->SetStartBit(static_cast<uint16>(s32_ActGridIndex)) == true)
            {
               this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
               this->me_InteractionMode = C_SdBueMlvSignalManager::eIAM_RESIZERIGHT_INTEL;
            }
         }
         else
         {
            // nothing to do
         }
      }
      else if (this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_RESIZERIGHT_INTEL)
      {
         // item will resized on the "right" side
         if (s32_ActGridIndex <= static_cast<sint32>(u16_LastBit))
         {
            // update the item
            if (this->mpc_ActualSignal->SetStartBit(static_cast<uint16>(s32_ActGridIndex)) == true)
            {
               this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
            }
         }
         else if (s32_ActGridIndex >= static_cast<sint32>(u16_StartBit))
         {
            // the resizing changed to the other direction
            // update the item and change the mode
            if (this->mpc_ActualSignal->SetLastBit(static_cast<uint16>(s32_ActGridIndex)) == true)
            {
               this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
               this->me_InteractionMode = C_SdBueMlvSignalManager::eIAM_RESIZELEFT_INTEL;
            }
         }
         else
         {
            // nothing to do
         }
      }
      else if (this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_RESIZELEFT_MOTOROLA)
      {
         // item will resized on the "left" side with Motorola format
         const uint16 u16_RowNewStartBit = static_cast<uint16>(s32_ActGridIndex) / 8U;
         const uint16 u16_RowLastBit = u16_LastBit / 8U;

         if ((u16_RowNewStartBit < u16_RowLastBit) ||
             ((s32_ActGridIndex >= static_cast<sint32>(u16_LastBit)) &&
              (u16_RowNewStartBit == u16_RowLastBit)))
         {
            // the start bit is not in the same row as the last bit or
            // special case: the start bit is in the same row and is with
            // Motorola format on a higher index as the last bit
            // update the item
            if (this->mpc_ActualSignal->SetStartBit(static_cast<uint16>(s32_ActGridIndex)) == true)
            {
               this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
            }
         }
         else if ((s32_ActGridIndex <= static_cast<sint32>(u16_StartBit)) &&
                  (u16_RowNewStartBit == u16_RowLastBit))
         {
            // the resizing changed to the other direction
            // update the item and change the mode
            if (this->mpc_ActualSignal->SetLastBit(static_cast<uint16>(s32_ActGridIndex)) == true)
            {
               this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
               this->me_InteractionMode = C_SdBueMlvSignalManager::eIAM_RESIZERIGHT_MOTOROLA;
            }
         }
         else
         {
            // nothing to do
         }
      }
      else if (this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_RESIZERIGHT_MOTOROLA)
      {
         // item will resized on the "right" side with Motorola format
         const uint16 u16_RowNewLastBit = static_cast<uint16>(s32_ActGridIndex) / 8U;
         const uint16 u16_RowStartBit = u16_StartBit / 8U;

         if ((u16_RowNewLastBit > u16_RowStartBit) ||
             ((s32_ActGridIndex <= static_cast<sint32>(u16_StartBit)) &&
              (u16_RowNewLastBit == u16_RowStartBit)))
         {
            // update the item
            if (this->mpc_ActualSignal->SetLastBit(static_cast<uint16>(s32_ActGridIndex)) == true)
            {
               this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
            }
         }
         else if ((s32_ActGridIndex >= static_cast<sint32>(u16_LastBit)) &&
                  (u16_RowNewLastBit == u16_RowStartBit))
         {
            // the resizing changed to the other direction
            // update the item and change the mode
            if (this->mpc_ActualSignal->SetStartBit(static_cast<uint16>(s32_ActGridIndex)) == true)
            {
               this->m_UpdateConcreteSignalManager(this->mpc_ActualSignal);
               this->me_InteractionMode = C_SdBueMlvSignalManager::eIAM_RESIZELEFT_MOTOROLA;
            }
         }
         else
         {
            // nothing to do
         }
      }
      else
      {
         QGraphicsScene::mouseMoveEvent(opc_Event);
      }

      // save the grid index for the next call
      this->ms32_LastGridIndex = s32_ActGridIndex;
   }
   else
   {
      QGraphicsScene::mouseMoveEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if ((this->mq_SignalChanged == true) &&
       (this->mpc_ActualSignal != NULL))
   {
      // signal was changed. save the changes and send the information about the change
      this->mpc_ActualSignal->SaveSignal();
      Q_EMIT this->SigMessageUpdated();
      this->mq_SignalChanged = false;
   }

   if (this->mpc_ActualSignal != NULL)
   {
      Q_EMIT this->SigSignalActivated(this->mpc_ActualSignal->GetSignalIndex());
   }

   // reset the configuration
   this->mpc_ActualSignal = NULL;
   this->me_InteractionMode = C_SdBueMlvSignalManager::eIAM_NONE;
   this->ms32_LastGridIndex = -1;

   Q_EMIT this->SigChangeCursor(Qt::ArrowCursor);

   QGraphicsScene::mouseReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten context menu event

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event)
{
   this->m_OnCustomContextMenuRequested(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_InitBorderItems(void)
{
   uint8 u8_Counter;
   C_SdBueMlvBorderItem * pc_Item;

   // item in the upper left corner
   this->mpc_BorderItemUpperLeft = new C_SdBueMlvBorderItem();
   this->mpc_BorderItemUpperLeft->setPos(0.0, 0.0);
   this->addItem(mpc_BorderItemUpperLeft);

   for (u8_Counter = 0U; u8_Counter < 8U; ++u8_Counter)
   {
      //lint -e{423}  no memory leak because of adding the item to the scene and the Qt memory management
      pc_Item = new C_SdBueMlvBorderItem("Bit ", (u8_Counter));
      this->mc_VecBorderItemsHorizontal.push_back(pc_Item);
      this->addItem(pc_Item);
   }

   for (u8_Counter = 0U; u8_Counter < 8U; ++u8_Counter)
   {
      //lint -e{423}  no memory leak because of adding the item to the scene and the Qt memory management
      pc_Item = new C_SdBueMlvBorderItem("Byte ", u8_Counter);
      this->mc_VecBorderItemsVertical.push_back(pc_Item);
      this->addItem(pc_Item);
   }

   this->m_UpdateBorderItems();

   //lint -e{429}  no memory leak because of the parent of pc_Item by addItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_InitEmptyItems(void)
{
   uint8 u8_Counter;

   // fill the vector
   for (u8_Counter = 0U; u8_Counter < 64U; ++u8_Counter)
   {
      C_SdBueMlvEmptyItem * const pc_Item = new C_SdBueMlvEmptyItem(u8_Counter);
      this->mc_VecEmptyItems.push_back(pc_Item);
      pc_Item->setZValue(C_SdBueMlvGraphicsScene::mhf64_ZOrderEmptyItem);
      this->addItem(pc_Item);

      //lint -e{429}  no memory leak because of the parent of pc_Item by addItem and the Qt memory management
   }

   this->m_UpdateEmptyItems();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_InitProtocolItems(void)
{
   this->mapc_ECeSHints[0] = new C_SdBueMlvBaseItem(QColor(0, 0, 0, 0),
                                                    mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_FONT_REGULAR_16,
                                                    stw_opensyde_gui_logic::C_GtGetText::h_GetText(
                                                       "Reserved by Message Counter"), true);
   this->mapc_ECeSHints[1] = new C_SdBueMlvBaseItem(QColor(0, 0, 0, 0),
                                                    mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_FONT_REGULAR_16,
                                                    stw_opensyde_gui_logic::C_GtGetText::h_GetText(
                                                       "Reserved by Checksum"), true);

   this->mapc_ECeSHints[0]->setZValue(mhf64_ZOrderHintItem);
   this->mapc_ECeSHints[1]->setZValue(mhf64_ZOrderHintItem);

   this->mapc_ECeSHints[0]->setVisible(false);
   this->mapc_ECeSHints[1]->setVisible(false);

   this->addItem(this->mapc_ECeSHints[0]);
   this->addItem(this->mapc_ECeSHints[1]);

   this->m_UpdateProtocolItems();

   //lint -e{429}  no memory leak because of the parent of mapc_ECeSHints by addItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a signal

   \param[in]     ou32_SignalIndex    Index of the signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_AddSignal(const uint32 ou32_SignalIndex)
{
   C_SdBueMlvSignalManager * pc_Item;

   C_SdBueMlvSignalManager::C_SignalItemColors c_ColorConfig;
   const stw_opensyde_gui_logic::C_PuiSdNodeCanSignal * const pc_SignalUiItem =
      stw_opensyde_gui_logic::C_PuiSdHandler::h_GetInstance()->GetUiCanSignal(this->mc_MessageId,
                                                                              ou32_SignalIndex);

   tgl_assert(pc_SignalUiItem != NULL);
   if (pc_SignalUiItem != NULL)
   {
      if (pc_SignalUiItem->u8_ColorIndex == 0U)
      {
         // no concrete color saved. get a new one
         c_ColorConfig = this->m_GetNextNotUsedColors();
      }
      else
      {
         c_ColorConfig = this->m_GetConcreteColors(pc_SignalUiItem->u8_ColorIndex);
      }
   }
   else
   {
      // no concrete color saved. get a new one
      c_ColorConfig = this->m_GetNextNotUsedColors();
   }

   pc_Item = new C_SdBueMlvSignalManager(this->mpc_MessageSyncManager, this->mc_MessageId, this->mu16_MaximumCountBits,
                                         C_SdBueMlvGraphicsScene::mhf64_Space, this);

   this->mc_VecSignals.push_back(pc_Item);
   pc_Item->SetZOrder(C_SdBueMlvGraphicsScene::mhf64_ZOrderDefault);

   connect(pc_Item, &C_SdBueMlvSignalManager::SigAddItem, this, &C_SdBueMlvGraphicsScene::m_AddItemSlot);
   connect(pc_Item, &C_SdBueMlvSignalManager::SigRemoveItem, this, &C_SdBueMlvGraphicsScene::m_RemoveItemSlot);
   connect(pc_Item, &C_SdBueMlvSignalManager::SigChangeCursor, this, &C_SdBueMlvGraphicsScene::m_ChangeCursor);
   connect(pc_Item, &C_SdBueMlvSignalManager::SigHideToolTip, this, &C_SdBueMlvGraphicsScene::m_HandleHideToolTip);

   // set the informations only when the signals are connected
   pc_Item->LoadSignal(ou32_SignalIndex, c_ColorConfig);

   if (pc_SignalUiItem != NULL)
   {
      if (pc_SignalUiItem->u8_ColorIndex == 0U)
      {
         // a new color was used, save the signal
         pc_Item->SaveSignal();
      }
   }

   this->m_AddSignalToGridMapping(pc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_RemoveSignal(const uint32 ou32_SignalIndex)
{
   QVector<C_SdBueMlvSignalManager *>::iterator pc_ItItem;
   C_SdBueMlvSignalManager * pc_Item;
   uint32 u32_Index;

   // search the signal
   for (pc_ItItem = this->mc_VecSignals.begin(); pc_ItItem != this->mc_VecSignals.end(); ++pc_ItItem)
   {
      u32_Index = (*pc_ItItem)->GetSignalIndex();

      if (u32_Index == ou32_SignalIndex)
      {
         // remove the item from the vector
         pc_Item = (*pc_ItItem);
         this->mc_VecSignals.erase(pc_ItItem);

         // remove the color
         this->m_SetColorsUsed(pc_Item->GetColorConfiguration(), false);

         // update the mapping
         this->m_RemoveSignalFromGridMapping(pc_Item);

         disconnect(pc_Item, &C_SdBueMlvSignalManager::SigAddItem, this, &C_SdBueMlvGraphicsScene::m_AddItemSlot);
         disconnect(pc_Item, &C_SdBueMlvSignalManager::SigRemoveItem, this, &C_SdBueMlvGraphicsScene::m_RemoveItemSlot);
         disconnect(pc_Item, &C_SdBueMlvSignalManager::SigChangeCursor, this,
                    &C_SdBueMlvGraphicsScene::SigChangeCursor);
         disconnect(pc_Item, &C_SdBueMlvSignalManager::SigHideToolTip, this,
                    &C_SdBueMlvGraphicsScene::m_HandleHideToolTip);

         // clear the sub items separate
         pc_Item->ClearItems();
         // delete the item
         pc_Item->setParent(NULL);
         delete pc_Item;

         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_UpdateAll(void)
{
   this->m_UpdateBorderItems();
   this->m_UpdateEmptyItems();
   this->m_UpdateSignalManager();
   this->m_UpdateProtocolItems();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_UpdateBorderItems(void)
{
   QVector<C_SdBueMlvBorderItem *>::const_iterator pc_ItItem;
   sint32 s32_Pos = 8;

   this->mpc_BorderItemUpperLeft->SetSize(QSizeF(this->mf64_SingleItemWidth, this->mf64_SingleItemHeight));

   // update the coordinates of all horizontal items
   for (pc_ItItem = this->mc_VecBorderItemsHorizontal.begin();
        pc_ItItem != this->mc_VecBorderItemsHorizontal.end();
        ++pc_ItItem)
   {
      (*pc_ItItem)->SetSize(QSizeF(this->mf64_SingleItemWidth, this->mf64_SingleItemHeight));
      // the first item is at the right side of the scene (Bit 7 - Bit 0)
      (*pc_ItItem)->setPos(static_cast<float64>(s32_Pos) *
                           (this->mf64_SingleItemWidth + C_SdBueMlvGraphicsScene::mhf64_Space), 0.0);

      --s32_Pos;
   }

   // update the coordinates of all vertical items
   // start at one because of border items
   s32_Pos = 1;

   for (pc_ItItem = this->mc_VecBorderItemsVertical.begin();
        pc_ItItem != this->mc_VecBorderItemsVertical.end();
        ++pc_ItItem)
   {
      (*pc_ItItem)->SetSize(QSizeF(this->mf64_SingleItemWidth, this->mf64_SingleItemHeight));
      (*pc_ItItem)->setPos(0.0, static_cast<float64>(s32_Pos) *
                           (this->mf64_SingleItemHeight + C_SdBueMlvGraphicsScene::mhf64_Space));

      ++s32_Pos;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_UpdateEmptyItems(void)
{
   sint32 s32_PosHorizontal;
   sint32 s32_PosVertical;
   sint32 s32_BitPosition;
   float64 f64_PosX;
   float64 f64_PosY;

   // the horizontal "internal" vector
   for (s32_BitPosition = 0; s32_BitPosition < this->mc_VecEmptyItems.size(); ++s32_BitPosition)
   {
      this->mc_VecEmptyItems[s32_BitPosition]->SetSize(QSizeF(this->mf64_SingleItemWidth, this->mf64_SingleItemHeight));

      // counting starts at the right side
      s32_PosHorizontal = 8 - (s32_BitPosition % 8);
      // compensate the border items
      s32_PosVertical = (s32_BitPosition / 8) + 1;

      f64_PosX = static_cast<float64>(s32_PosHorizontal) *
                 (this->mf64_SingleItemWidth + C_SdBueMlvGraphicsScene::mhf64_Space);
      f64_PosY = (static_cast<float64>(s32_PosVertical) *
                  (this->mf64_SingleItemHeight + C_SdBueMlvGraphicsScene::mhf64_Space));

      this->mc_VecEmptyItems[s32_BitPosition]->setPos(f64_PosX, f64_PosY);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_UpdateSignalManager(void)
{
   QVector<C_SdBueMlvSignalManager *>::const_iterator pc_ItItem;

   for (pc_ItItem = this->mc_VecSignals.begin();
        pc_ItItem != this->mc_VecSignals.end();
        ++pc_ItItem)
   {
      (*pc_ItItem)->Update(this->mf64_SingleItemWidth, this->mf64_SingleItemHeight);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_UpdateConcreteSignalManager(C_SdBueMlvSignalManager * const opc_Item)
{
   // update the size of the item
   opc_Item->Update(this->mf64_SingleItemWidth, this->mf64_SingleItemHeight);

   // update the grid mapping
   this->m_UpdateSignalInGridMapping(opc_Item);

   this->mq_SignalChanged = true;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_UpdateProtocolItems(void)
{
   QSizeF c_Size;

   c_Size.setHeight(this->mf64_SingleItemHeight);
   c_Size.setWidth((8.0 * this->mf64_SingleItemWidth) + (7.0 * mhf64_Space));

   // set the size
   if (this->mapc_ECeSHints[0] != NULL)
   {
      this->mapc_ECeSHints[0]->SetSize(c_Size);
      this->mapc_ECeSHints[0]->setPos(this->mc_VecEmptyItems[55]->pos());
   }
   if (this->mapc_ECeSHints[1] != NULL)
   {
      this->mapc_ECeSHints[1]->SetSize(c_Size);
      this->mapc_ECeSHints[1]->setPos(this->mc_VecEmptyItems[63]->pos());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_AddSignalToGridMapping(C_SdBueMlvSignalManager * const opc_Item)
{
   uint16 u16_Counter;
   uint16 u16_GridPos;

   for (u16_Counter = 0; u16_Counter < opc_Item->GetLength(); ++u16_Counter)
   {
      u16_GridPos = opc_Item->GetDataBytesBitPosOfSignalBit(u16_Counter);

      if (u16_GridPos < this->mu16_MaximumCountBits)
      {
         this->mac_SetGridState[u16_GridPos].insert(opc_Item);
         // a signal is on this spot, deactivate the background rectangle of the empty items and
         // show only the number of the bit
         this->mc_VecEmptyItems[u16_GridPos]->SetDrawRectangle(false);

         this->m_CheckGridMappingPositionForError(u16_GridPos);
      }
      else
      {
         // TODO Set error
      }
   }

   // suppression is for function m_AddSignal
   //lint -e{429}  no memory leak because of the parent of pc_Item by addItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_UpdateSignalInGridMapping(C_SdBueMlvSignalManager * const opc_Item)
{
   uint16 u16_Counter;

   std::set<uint16> c_SetGridPositions;
   std::set<uint16>::iterator c_ItSetGridPosition;

   // get all grid positions for this signal. necessary for motorola byte order
   opc_Item->GetDataBytesBitPositionsOfSignal(c_SetGridPositions);

   for (u16_Counter = 0U; u16_Counter < this->mu16_MaximumCountBits; ++u16_Counter)
   {
      c_ItSetGridPosition = c_SetGridPositions.find(u16_Counter);

      // is this position used?
      if (c_ItSetGridPosition != c_SetGridPositions.end())
      {
         // clean up the set
         c_SetGridPositions.erase(c_ItSetGridPosition);

         this->mac_SetGridState[u16_Counter].insert(opc_Item);
         // a signal is on this spot, deactivate the background rectangle of the empty items and
         // show only the number of the bit
         this->mc_VecEmptyItems[u16_Counter]->SetDrawRectangle(false);

         this->m_CheckGridMappingPositionForError(u16_Counter);
      }
      else
      {
         // remove the item from the other positions
         this->m_RemoveSignalFromGridMappingPosition(opc_Item, u16_Counter);
      }
   }

   // set must be empty
   if (c_SetGridPositions.size() > 0)
   {
      // TODO Set error. Must be a range error
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_RemoveSignalFromGridMapping(C_SdBueMlvSignalManager * const opc_Item)
{
   uint16 u16_Counter;

   for (u16_Counter = 0U; u16_Counter < 64U; ++u16_Counter)
   {
      this->m_RemoveSignalFromGridMappingPosition(opc_Item, u16_Counter);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_RemoveSignalFromGridMappingPosition(C_SdBueMlvSignalManager * const opc_Item,
                                                                    const uint16 ou16_Pos)
{
   // search the item
   std::set<C_SdBueMlvSignalManager *>::iterator c_ItItem = this->mac_SetGridState[ou16_Pos].find(opc_Item);

   if (c_ItItem != this->mac_SetGridState[ou16_Pos].end())
   {
      // remove the pointer from the set
      this->mac_SetGridState[ou16_Pos].erase(c_ItItem);

      // are there any other items left?
      if (this->mac_SetGridState[ou16_Pos].size() == 0)
      {
         // show the background of the empty item again
         this->mc_VecEmptyItems[ou16_Pos]->SetDrawRectangle(true);
      }

      this->m_CheckGridMappingPositionForError(ou16_Pos);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_CheckGridMappingPositionForError(const uint16 ou16_Pos)
{
   if (this->mac_SetGridState[ou16_Pos].size() > 1)
   {
      // error state
      this->mc_VecEmptyItems[ou16_Pos]->SetError(true);
   }
   else
   {
      this->mc_VecEmptyItems[ou16_Pos]->SetError(false);

      if (this->mac_SetGridState[ou16_Pos].size() > 0)
      {
         std::set<C_SdBueMlvSignalManager *>::iterator c_ItItem = this->mac_SetGridState[ou16_Pos].begin();
         const C_SdBueMlvSignalManager::C_SignalItemColors c_ColorConf = (*c_ItItem)->GetColorConfiguration();
         this->mc_VecEmptyItems[ou16_Pos]->SetFontColor(c_ColorConf.c_FontColor);
      }
      else
      {
         this->mc_VecEmptyItems[ou16_Pos]->RestoreFontColor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_AddItemSlot(C_SdBueMlvSignalItem * const opc_Item)
{
   this->addItem(opc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_RemoveItemSlot(C_SdBueMlvSignalItem * const opc_Item)
{
   this->removeItem(opc_Item);
}

//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvSignalManager::C_SignalItemColors C_SdBueMlvGraphicsScene::m_GetNextNotUsedColors(void)
{
   sintn sn_ColorCounter;
   uint32 u32_ItFree;

   C_SdBueMlvSignalManager::C_SignalItemColors c_ColorConfig;

   srand(time(NULL));

   for (u32_ItFree = 0UL; u32_ItFree < 64; ++u32_ItFree)
   {
      if (this->maq_SingalsColorsUsed[u32_ItFree] == false)
      {
         break;
      }
   }
   //Don't start searching if there are no more free colors
   tgl_assert(u32_ItFree < 64);
   if (u32_ItFree < 64)
   {
      bool q_ColorFound = false;
      // search a not used color with a random number
      do
      {
         sn_ColorCounter = rand() % 64;
         if (u32_ItFree == 64)
         {
            break;
         }
         if (this->maq_SingalsColorsUsed[sn_ColorCounter] == false)
         {
            c_ColorConfig = C_SdBueMlvGraphicsScene::mhac_SignalsColors[sn_ColorCounter];
            // set the color variant as used
            this->maq_SingalsColorsUsed[sn_ColorCounter] = true;
            q_ColorFound = true;
         }
      }
      while (q_ColorFound == false);
   }

   return c_ColorConfig;
}

//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvSignalManager::C_SignalItemColors C_SdBueMlvGraphicsScene::m_GetConcreteColors(const uint8 ou8_Index)
{
   uint32 u32_ColorCounter;

   C_SdBueMlvSignalManager::C_SignalItemColors c_ColorConfig;

   // search a the color to set the flag
   for (u32_ColorCounter = 0U; u32_ColorCounter < 64U; ++u32_ColorCounter)
   {
      if (ou8_Index == C_SdBueMlvGraphicsScene::mhac_SignalsColors[u32_ColorCounter].u8_Index)
      {
         c_ColorConfig = C_SdBueMlvGraphicsScene::mhac_SignalsColors[u32_ColorCounter];
         // set the color variant as used
         this->maq_SingalsColorsUsed[u32_ColorCounter] = true;
         break;
      }
   }

   return c_ColorConfig;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_SetColorsUsed(const C_SdBueMlvSignalManager::C_SignalItemColors & orc_Colors,
                                              const bool oq_Used)
{
   uint32 u32_ColorCounter;

   // search a the color to set the flag
   for (u32_ColorCounter = 0U; u32_ColorCounter < 64U; ++u32_ColorCounter)
   {
      if (C_SdBueMlvGraphicsScene::mhac_SignalsColors[u32_ColorCounter].u8_Index == orc_Colors.u8_Index)
      {
         this->maq_SingalsColorsUsed[u32_ColorCounter] = oq_Used;
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_SceneRectChanged(void)
{
   // update the size values of a signgle item
   this->mf64_SingleItemWidth = (this->sceneRect().width() - (8.0 * C_SdBueMlvGraphicsScene::mhf64_Space)) / 9.0;
   this->mf64_SingleItemHeight = (this->sceneRect().height() - (8.0 * C_SdBueMlvGraphicsScene::mhf64_Space)) / 9.0;

   // update all items
   this->m_UpdateAll();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_SearchClickedItem(const QPointF & orc_Pos)
{
   const sint32 s32_Counter = this->m_GetGridIndex(orc_Pos);

   this->mpc_ActualSignal = NULL;
   this->ms32_LastGridIndex = -1;

   if (s32_Counter >= 0)
   {
      if (this->mac_SetGridState[s32_Counter].size() > 0)
      {
         std::set<C_SdBueMlvSignalManager *>::iterator c_ItSetGridPosition;
         float64 f64_ZOrderActual;
         float64 f64_ZOrderHighest = mhf64_ZOrderBelowAllItems;

         // search the item with the highest ZOrder
         for (c_ItSetGridPosition = this->mac_SetGridState[s32_Counter].begin();
              c_ItSetGridPosition != this->mac_SetGridState[s32_Counter].end();
              ++c_ItSetGridPosition)
         {
            f64_ZOrderActual = (*c_ItSetGridPosition)->GetZOrder();

            if (f64_ZOrderActual > f64_ZOrderHighest)
            {
               // set the actual item with the highest zorder
               f64_ZOrderHighest = f64_ZOrderActual;
               this->mpc_ActualSignal = (*c_ItSetGridPosition);
            }
         }

         this->ms32_LastGridIndex = s32_Counter;

         // bring the actual element to the top
         this->m_BringActualSignalToTop();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_BringActualSignalToTop(void)
{
   sint32 s32_Counter;

   // reset all items in the first step
   for (s32_Counter = 0; s32_Counter < this->mc_VecSignals.size(); ++s32_Counter)
   {
      this->mc_VecSignals[s32_Counter]->SetZOrder(mhf64_ZOrderDefault);
   }

   // set the actual item on top
   if (this->mpc_ActualSignal != NULL)
   {
      this->mpc_ActualSignal->SetZOrder(mhf64_ZOrderInteraction);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_ChangeCursor(const Qt::CursorShape oe_Cursor)
{
   // forward this signal only if no interaction mode is active
   if (this->me_InteractionMode == C_SdBueMlvSignalManager::eIAM_NONE)
   {
      Q_EMIT this->SigChangeCursor(oe_Cursor);
   }
}

//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueMlvGraphicsScene::m_GetGridIndex(const QPointF & orc_Pos) const
{
   sint32 s32_Counter;
   bool q_Found = false;

   for (s32_Counter = 0; s32_Counter < this->mc_VecEmptyItems.size(); ++s32_Counter)
   {
      QRectF c_Rect(this->mc_VecEmptyItems[s32_Counter]->pos(),
                    this->mc_VecEmptyItems[s32_Counter]->boundingRect().size());

      if (c_Rect.contains(orc_Pos) == true)
      {
         q_Found = true;
         break;
      }
   }

   if (q_Found == false)
   {
      s32_Counter = -1;
   }

   return s32_Counter;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_HandleHideToolTip(void)
{
   this->mpc_HoveredSignal = NULL;
   Q_EMIT this->SigHideToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new stw_opensyde_gui_elements::C_OgeContextMenu();

   this->mpc_Add = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add new Signal"), this,
                                                    &C_SdBueMlvGraphicsScene::m_ActionAdd);

   this->mpc_ContextMenu->addSeparator();

   this->mpc_Cut = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Cut"),
                                                    this, &C_SdBueMlvGraphicsScene::m_ActionCut);
   this->mpc_Copy = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                                     this, &C_SdBueMlvGraphicsScene::m_ActionCopy);
   this->mpc_Paste = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"),
                                                      this, &C_SdBueMlvGraphicsScene::m_ActionPaste);

   this->mpc_ContextMenu->addSeparator();

   this->mpc_Delete = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete"),
                                                       this, &C_SdBueMlvGraphicsScene::m_ActionDelete);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_OnCustomContextMenuRequested(const QGraphicsSceneContextMenuEvent * const opc_Event)
{
   bool q_Enabled = false;

   this->mc_LastMousePos = opc_Event->scenePos();
   this->m_SearchClickedItem(this->mc_LastMousePos);

   if (this->mpc_ActualSignal != NULL)
   {
      // an item was clicked
      q_Enabled = true;
   }

   this->mpc_Copy->setEnabled(q_Enabled);
   this->mpc_Cut->setEnabled(q_Enabled);
   this->mpc_Delete->setEnabled(q_Enabled);

   this->mpc_ContextMenu->popup(opc_Event->screenPos());
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_ActionAdd(void)
{
   sint32 s32_Counter = this->m_GetGridIndex(this->mc_LastMousePos);

   if (s32_Counter < 0)
   {
      s32_Counter = 0;
   }

   Q_EMIT this->SigAddSignal(this->mc_MessageId, static_cast<uint16>(s32_Counter));
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_ActionCopy(void)
{
   if (this->mpc_ActualSignal != NULL)
   {
      Q_EMIT this->SigCopySignal(this->mc_MessageId, this->mpc_ActualSignal->GetSignalIndex());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_ActionPaste(void)
{
   sint32 s32_Counter = this->m_GetGridIndex(this->mc_LastMousePos);

   if (s32_Counter < 0)
   {
      s32_Counter = 0;
   }

   if (this->mc_VecSignals.size() < 64)
   {
      Q_EMIT this->SigPasteSignal(this->mc_MessageId, static_cast<uint16>(s32_Counter));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_ActionCut(void)
{
   if (this->mpc_ActualSignal != NULL)
   {
      Q_EMIT this->SigCutSignal(this->mc_MessageId, this->mpc_ActualSignal->GetSignalIndex());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsScene::m_ActionDelete(void)
{
   if (this->mpc_ActualSignal != NULL)
   {
      Q_EMIT this->SigDeleteSignal(this->mc_MessageId, this->mpc_ActualSignal->GetSignalIndex());
   }
}
