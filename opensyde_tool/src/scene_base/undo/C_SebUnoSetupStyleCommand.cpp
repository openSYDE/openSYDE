//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Setup style undo command (implementation)

   Setup style undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GiBiArrow.h"
#include "C_GiBiBoundary.h"
#include "C_GiBiTextElement.h"
#include "C_SebUnoSetupStyleCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Scene   Pointer to currently active scene
   \param[in]     orc_IDs     Affected unique IDs
   \param[in]     oq_DarkMode Optional flag if dark mode is active
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoSetupStyleCommand::C_SebUnoSetupStyleCommand(QGraphicsScene * const opc_Scene,
                                                     const std::vector<uint64> & orc_IDs, const bool oq_DarkMode,
                                                     QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, "Change drawing element(s) style", opc_Parent),
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
   std::vector<QGraphicsItem *> c_Items = m_GetSceneItems();
   m_CreateMapAndSaveState(c_Items, this->mc_MapIDToTypeAndIndexPrevious, this->mpc_PreviousState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal for registering setup style complete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::InitNext(void)
{
   std::vector<QGraphicsItem *> c_Items = m_GetSceneItems();
   m_CreateMapAndSaveState(c_Items, this->mc_MapIDToTypeAndIndexNext, this->mpc_NextState);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Undo style
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::undo()
{
   m_Restore(this->mc_MapIDToTypeAndIndexPrevious, this->mpc_PreviousState);
   QUndoCommand::undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Redo style
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::redo()
{
   m_Restore(this->mc_MapIDToTypeAndIndexNext, this->mpc_NextState);
   QUndoCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore all style features for all internally stored items from the snapshot using the preserved mapping

   \param[out] orc_MapIDToTypeAndIndex Map for ID to state data entry
   \param[out] opc_Snapshot            Preserved state data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoSetupStyleCommand::m_Restore(const QMap<uint64, C_PuiBsTemporaryDataID> & orc_MapIDToTypeAndIndex,
                                          const C_PuiBsElements * const opc_Snapshot)
{
   if (opc_Snapshot != NULL)
   {
      std::vector<QGraphicsItem *> c_Items = m_GetSceneItems();
      QMap<uint64, C_PuiBsTemporaryDataID>::const_iterator c_Entry;

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
            c_Entry = orc_MapIDToTypeAndIndex.find(pc_Arrow->GetID());
            if (c_Entry != orc_MapIDToTypeAndIndex.end())
            {
               if (c_Entry.value().u32_Index < opc_Snapshot->c_LineArrows.size())
               //Content
               {
                  const C_PuiBsLineArrow & rc_UIArrow = opc_Snapshot->c_LineArrows[c_Entry.value().u32_Index];
                  pc_Arrow->SetColor(rc_UIArrow.c_UIColor);
                  pc_Arrow->SetLineType(rc_UIArrow.e_LineType);
                  pc_Arrow->SetWidth(rc_UIArrow.s32_UIWidthPixels);
                  pc_Arrow->SetStartArrowHeadType(rc_UIArrow.e_StartArrowHeadType);
                  pc_Arrow->SetEndArrowHeadType(rc_UIArrow.e_EndArrowHeadType);
                  pc_Arrow->TriggerSigChangedGraphic();
               }
            }
         }
         //Boundary
         pc_Boundary = dynamic_cast<C_GiBiBoundary *>(*c_ItItem);
         if (pc_Boundary != NULL)
         {
            c_Entry = orc_MapIDToTypeAndIndex.find(pc_Boundary->GetID());
            if (c_Entry != orc_MapIDToTypeAndIndex.end())
            {
               if (c_Entry.value().u32_Index < opc_Snapshot->c_Boundaries.size())
               //Content
               {
                  const C_PuiBsBoundary & rc_UIBoundary = opc_Snapshot->c_Boundaries[c_Entry.value().u32_Index];
                  if (this->mq_DarkMode == true)
                  {
                     pc_Boundary->SetBorderColor(rc_UIBoundary.c_UIBorderColorDark);
                     pc_Boundary->SetBackgroundColor(rc_UIBoundary.c_UIBackgroundColorDark);
                  }
                  else
                  {
                     pc_Boundary->SetBorderColor(rc_UIBoundary.c_UIBorderColorBright);
                     pc_Boundary->SetBackgroundColor(rc_UIBoundary.c_UIBackgroundColorBright);
                  }
                  pc_Boundary->SetBorderWidth(rc_UIBoundary.s32_UIBorderWidth);
               }
            }
         }
         //TextElement
         pc_TextElement = dynamic_cast<C_GiBiTextElement *>(*c_ItItem);
         if (pc_TextElement != NULL)
         {
            c_Entry = orc_MapIDToTypeAndIndex.find(pc_TextElement->GetID());
            if (c_Entry != orc_MapIDToTypeAndIndex.end())
            {
               if (c_Entry.value().u32_Index < opc_Snapshot->c_TextElements.size())
               //Content
               {
                  const C_PuiBsTextElement & rc_UITextElement = opc_Snapshot->c_TextElements[c_Entry.value().u32_Index];
                  QColor c_Color;
                  if (this->mq_DarkMode == true)
                  {
                     c_Color = rc_UITextElement.c_UIFontColorDark;
                  }
                  else
                  {
                     c_Color = rc_UITextElement.c_UIFontColorBright;
                  }
                  pc_TextElement->ApplyStyle(rc_UITextElement.c_UIFontStyle, c_Color);
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
void C_SebUnoSetupStyleCommand::m_CreateMapAndSaveState(const std::vector<QGraphicsItem *> & orc_Items, QMap<uint64,
                                                                                                             C_PuiBsTemporaryDataID> & orc_Map,
                                                        C_PuiBsElements * const opc_Snapshot) const
{
   if (opc_Snapshot != NULL)
   {
      C_GiBiArrow * pc_Arrow;
      C_GiBiBoundary * pc_Boundary;
      C_GiBiTextElement * pc_TextElement;
      uint32 u32_Index;

      for (std::vector<QGraphicsItem *>::const_iterator c_ItItem = orc_Items.begin(); c_ItItem != orc_Items.end();
           ++c_ItItem)
      {
         //Line arrow
         pc_Arrow = dynamic_cast<C_GiBiArrow *>(*c_ItItem);
         if (pc_Arrow != NULL)
         {
            //Map
            u32_Index = opc_Snapshot->c_LineArrows.size();
            orc_Map.insert(pc_Arrow->GetID(),
                           C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSdDataElement::eLINE_ARROW),
                                                  u32_Index));
            opc_Snapshot->c_LineArrows.resize(static_cast<std::vector<C_PuiBsLineArrow>::size_type>(u32_Index + 1UL));
            //Content
            {
               C_PuiBsLineArrow & rc_UIArrow = opc_Snapshot->c_LineArrows[u32_Index];
               rc_UIArrow.c_UIColor = pc_Arrow->GetColor();
               rc_UIArrow.s32_UIWidthPixels = pc_Arrow->GetWidth();
               rc_UIArrow.e_StartArrowHeadType = pc_Arrow->GetStartArrowHeadType();
               rc_UIArrow.e_EndArrowHeadType = pc_Arrow->GetEndArrowHeadType();
               rc_UIArrow.e_LineType = pc_Arrow->GetLineType();
            }
         }
         //Boundary
         pc_Boundary = dynamic_cast<C_GiBiBoundary *>(*c_ItItem);
         if (pc_Boundary != NULL)
         {
            //Map
            u32_Index = opc_Snapshot->c_Boundaries.size();
            orc_Map.insert(pc_Boundary->GetID(),
                           C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSdDataElement::eBOUNDARY),
                                                  u32_Index));
            opc_Snapshot->c_Boundaries.resize(static_cast<std::vector<C_PuiBsBoundary>::size_type>(u32_Index + 1UL));
            //Content
            {
               C_PuiBsBoundary & rc_UIBoundary = opc_Snapshot->c_Boundaries[u32_Index];
               if (this->mq_DarkMode == true)
               {
                  rc_UIBoundary.c_UIBackgroundColorDark = pc_Boundary->GetBackgroundColor();
                  rc_UIBoundary.c_UIBorderColorDark = pc_Boundary->GetBorderColor();
               }
               else
               {
                  rc_UIBoundary.c_UIBackgroundColorBright = pc_Boundary->GetBackgroundColor();
                  rc_UIBoundary.c_UIBorderColorBright = pc_Boundary->GetBorderColor();
               }
               rc_UIBoundary.s32_UIBorderWidth = pc_Boundary->GetBorderWidth();
            }
         }
         //TextElement
         pc_TextElement = dynamic_cast<C_GiBiTextElement *>(*c_ItItem);
         if (pc_TextElement != NULL)
         {
            //Map
            u32_Index = opc_Snapshot->c_TextElements.size();
            orc_Map.insert(pc_TextElement->GetID(),
                           C_PuiBsTemporaryDataID(static_cast<sint32>(C_PuiSdDataElement::eTEXT_ELEMENT),
                                                  u32_Index));
            opc_Snapshot->c_TextElements.resize(static_cast<std::vector<C_PuiBsTextElement>::size_type>(u32_Index +
                                                                                                        1UL));
            //Content
            {
               C_PuiBsTextElement & rc_UITextElement = opc_Snapshot->c_TextElements[u32_Index];
               if (this->mq_DarkMode)
               {
                  rc_UITextElement.c_UIFontColorDark = pc_TextElement->GetFontColor();
               }
               else
               {
                  rc_UITextElement.c_UIFontColorBright = pc_TextElement->GetFontColor();
               }
               rc_UITextElement.c_UIFontStyle = pc_TextElement->GetFontStyle();
            }
         }
      }
   }
}  //lint !e429  //Never took ownership of input parameter
