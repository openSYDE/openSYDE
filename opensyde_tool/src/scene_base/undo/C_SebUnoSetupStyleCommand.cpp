//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Setup style undo command (implementation)

   Setup style undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiBiArrow.hpp"
#include "C_GiBiBoundary.hpp"
#include "C_GiBiTextElement.hpp"
#include "C_SebUnoSetupStyleCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene   Pointer to currently active scene
   \param[in]     orc_Ids     Affected unique IDs
   \param[in]     oq_DarkMode Optional flag if dark mode is active
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoSetupStyleCommand::C_SebUnoSetupStyleCommand(QGraphicsScene * const opc_Scene,
                                                     const std::vector<uint64_t> & orc_Ids, const bool oq_DarkMode,
                                                     QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_Ids, "Change drawing element(s) style", opc_Parent),
   mpc_PreviousState(new C_PuiBsElements()),
   mpc_NextState(new C_PuiBsElements()),
   mq_DarkMode(oq_DarkMode)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoSetupStyleCommand::~C_SebUnoSetupStyleCommand(void)
{
   delete (mpc_PreviousState);
   delete (mpc_NextState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal for registering setup style start
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::InitPrevious(void)
{
   const std::vector<QGraphicsItem *> c_Items = m_GetSceneItems();

   m_CreateMapAndSaveState(c_Items, this->mc_MapIdToTypeAndIndexPrevious, this->mpc_PreviousState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal for registering setup style complete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::InitNext(void)
{
   const std::vector<QGraphicsItem *> c_Items = m_GetSceneItems();

   m_CreateMapAndSaveState(c_Items, this->mc_MapIdToTypeAndIndexNext, this->mpc_NextState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Undo style
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::undo()
{
   m_Restore(this->mc_MapIdToTypeAndIndexPrevious, this->mpc_PreviousState);
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo style
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::redo()
{
   m_Restore(this->mc_MapIdToTypeAndIndexNext, this->mpc_NextState);
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore all style features for all internally stored items from the snapshot using the preserved mapping

   \param[out] orc_MapIdToTypeAndIndex Map for ID to state data entry
   \param[out] opc_Snapshot            Preserved state data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::m_Restore(const QMap<uint64_t, C_PuiBsTemporaryDataId> & orc_MapIdToTypeAndIndex,
                                          const C_PuiBsElements * const opc_Snapshot)
{
   if (opc_Snapshot != NULL)
   {
      std::vector<QGraphicsItem *> c_Items = m_GetSceneItems();
      QMap<uint64_t, C_PuiBsTemporaryDataId>::const_iterator c_Entry;

      for (std::vector<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
           ++c_ItItem)
      {
         C_GiBiArrow * pc_Arrow;
         C_GiBiBoundary * pc_Boundary;
         C_GiBiTextElement * pc_TextElement;
         //Line arrow
         pc_Arrow = dynamic_cast<C_GiBiArrow *>(*c_ItItem);
         if (pc_Arrow != NULL)
         {
            c_Entry = orc_MapIdToTypeAndIndex.find(pc_Arrow->GetId());
            if (c_Entry != orc_MapIdToTypeAndIndex.end())
            {
               if (c_Entry.value().u32_Index < opc_Snapshot->c_LineArrows.size())
               //Content
               {
                  const C_PuiBsLineArrow & rc_UiArrow = opc_Snapshot->c_LineArrows[c_Entry.value().u32_Index];
                  pc_Arrow->SetColor(rc_UiArrow.c_UiColor);
                  pc_Arrow->SetLineType(rc_UiArrow.e_LineType);
                  pc_Arrow->SetWidth(rc_UiArrow.s32_UiWidthPixels);
                  pc_Arrow->SetStartArrowHeadType(rc_UiArrow.e_StartArrowHeadType);
                  pc_Arrow->SetEndArrowHeadType(rc_UiArrow.e_EndArrowHeadType);
                  pc_Arrow->TriggerSigChangedGraphic();
               }
            }
         }
         //Boundary
         pc_Boundary = dynamic_cast<C_GiBiBoundary *>(*c_ItItem);
         if (pc_Boundary != NULL)
         {
            c_Entry = orc_MapIdToTypeAndIndex.find(pc_Boundary->GetId());
            if (c_Entry != orc_MapIdToTypeAndIndex.end())
            {
               if (c_Entry.value().u32_Index < opc_Snapshot->c_Boundaries.size())
               //Content
               {
                  const C_PuiBsBoundary & rc_UiBoundary = opc_Snapshot->c_Boundaries[c_Entry.value().u32_Index];
                  if (this->mq_DarkMode == true)
                  {
                     pc_Boundary->SetBorderColor(rc_UiBoundary.c_UiBorderColorDark);
                     pc_Boundary->SetBackgroundColor(rc_UiBoundary.c_UiBackgroundColorDark);
                  }
                  else
                  {
                     pc_Boundary->SetBorderColor(rc_UiBoundary.c_UiBorderColorBright);
                     pc_Boundary->SetBackgroundColor(rc_UiBoundary.c_UiBackgroundColorBright);
                  }
                  pc_Boundary->SetBorderWidth(rc_UiBoundary.s32_UiBorderWidth);
               }
            }
         }
         //TextElement
         pc_TextElement = dynamic_cast<C_GiBiTextElement *>(*c_ItItem);
         if (pc_TextElement != NULL)
         {
            c_Entry = orc_MapIdToTypeAndIndex.find(pc_TextElement->GetId());
            if (c_Entry != orc_MapIdToTypeAndIndex.end())
            {
               if (c_Entry.value().u32_Index < opc_Snapshot->c_TextElements.size())
               //Content
               {
                  const C_PuiBsTextElement & rc_UiTextElement = opc_Snapshot->c_TextElements[c_Entry.value().u32_Index];
                  QColor c_Color;
                  if (this->mq_DarkMode == true)
                  {
                     c_Color = rc_UiTextElement.c_UiFontColorDark;
                  }
                  else
                  {
                     c_Color = rc_UiTextElement.c_UiFontColorBright;
                  }
                  pc_TextElement->ApplyStyle(rc_UiTextElement.c_UiFontStyle, c_Color);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all style features of the specified items in the snapshot and preserve a mapping for later access

   \param[in]     orc_Items    Items to save style features for
   \param[out]    orc_Map      Map for ID to state data entry
   \param[in,out] opc_Snapshot Preserved state data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::m_CreateMapAndSaveState(const std::vector<QGraphicsItem *> & orc_Items, QMap<uint64_t,
                                                                                                             C_PuiBsTemporaryDataId> & orc_Map,
                                                        C_PuiBsElements * const opc_Snapshot) const
{
   if (opc_Snapshot != NULL)
   {
      C_GiBiArrow * pc_Arrow;
      C_GiBiBoundary * pc_Boundary;
      C_GiBiTextElement * pc_TextElement;
      uint32_t u32_Index;

      for (std::vector<QGraphicsItem *>::const_iterator c_ItItem = orc_Items.begin(); c_ItItem != orc_Items.end();
           ++c_ItItem)
      {
         //Line arrow
         pc_Arrow = dynamic_cast<C_GiBiArrow *>(*c_ItItem);
         if (pc_Arrow != NULL)
         {
            //Map
            u32_Index = opc_Snapshot->c_LineArrows.size();
            orc_Map.insert(pc_Arrow->GetId(),
                           C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eLINE_ARROW),
                                                  u32_Index));
            opc_Snapshot->c_LineArrows.resize(static_cast<std::vector<C_PuiBsLineArrow>::size_type>(u32_Index + 1UL));
            //Content
            {
               C_PuiBsLineArrow & rc_UiArrow = opc_Snapshot->c_LineArrows[u32_Index];
               rc_UiArrow.c_UiColor = pc_Arrow->GetColor();
               rc_UiArrow.s32_UiWidthPixels = pc_Arrow->GetWidth();
               rc_UiArrow.e_StartArrowHeadType = pc_Arrow->GetStartArrowHeadType();
               rc_UiArrow.e_EndArrowHeadType = pc_Arrow->GetEndArrowHeadType();
               rc_UiArrow.e_LineType = pc_Arrow->GetLineType();
            }
         }
         //Boundary
         pc_Boundary = dynamic_cast<C_GiBiBoundary *>(*c_ItItem);
         if (pc_Boundary != NULL)
         {
            //Map
            u32_Index = opc_Snapshot->c_Boundaries.size();
            orc_Map.insert(pc_Boundary->GetId(),
                           C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eBOUNDARY),
                                                  u32_Index));
            opc_Snapshot->c_Boundaries.resize(static_cast<std::vector<C_PuiBsBoundary>::size_type>(u32_Index + 1UL));
            //Content
            {
               C_PuiBsBoundary & rc_UiBoundary = opc_Snapshot->c_Boundaries[u32_Index];
               if (this->mq_DarkMode == true)
               {
                  rc_UiBoundary.c_UiBackgroundColorDark = pc_Boundary->GetBackgroundColor();
                  rc_UiBoundary.c_UiBorderColorDark = pc_Boundary->GetBorderColor();
               }
               else
               {
                  rc_UiBoundary.c_UiBackgroundColorBright = pc_Boundary->GetBackgroundColor();
                  rc_UiBoundary.c_UiBorderColorBright = pc_Boundary->GetBorderColor();
               }
               rc_UiBoundary.s32_UiBorderWidth = pc_Boundary->GetBorderWidth();
            }
         }
         //TextElement
         pc_TextElement = dynamic_cast<C_GiBiTextElement *>(*c_ItItem);
         if (pc_TextElement != NULL)
         {
            //Map
            u32_Index = opc_Snapshot->c_TextElements.size();
            orc_Map.insert(pc_TextElement->GetId(),
                           C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eTEXT_ELEMENT),
                                                  u32_Index));
            opc_Snapshot->c_TextElements.resize(static_cast<std::vector<C_PuiBsTextElement>::size_type>(u32_Index +
                                                                                                        1UL));
            //Content
            {
               C_PuiBsTextElement & rc_UiTextElement = opc_Snapshot->c_TextElements[u32_Index];
               if (this->mq_DarkMode)
               {
                  rc_UiTextElement.c_UiFontColorDark = pc_TextElement->GetFontColor();
               }
               else
               {
                  rc_UiTextElement.c_UiFontColorBright = pc_TextElement->GetFontColor();
               }
               rc_UiTextElement.c_UiFontStyle = pc_TextElement->GetFontStyle();
            }
         }
      }
   }
} //lint !e429  //Never took ownership of input parameter
