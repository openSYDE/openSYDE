//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class (implementation)

   Add and delete base class

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SebUnoAddDeleteBaseCommand.hpp"
#include "C_GiBiArrow.hpp"
#include "C_GiBiBoundary.hpp"
#include "C_GiBiTextElement.hpp"
#include "C_GiBiImageGroup.hpp"

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

   \param[in,out] opc_Scene  Pointer to currently active scene
   \param[in]     orc_Ids    Affected unique IDs
   \param[in]     orc_Text   Command description
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoAddDeleteBaseCommand::C_SebUnoAddDeleteBaseCommand(QGraphicsScene * const opc_Scene,
                                                           const std::vector<uint64_t> & orc_Ids,
                                                           const QString & orc_Text, QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_Ids, orc_Text, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Store items in new structure and save indices in map

   \param[in,out] orc_Storage             Data structure to store data in
   \param[in,out] orc_MapTypeAndIndexToId Map to locate new items
   \param[in]     orc_AllElements         Items to store
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoAddDeleteBaseCommand::m_StoreCommon(C_PuiBsElements & orc_Storage, QMap<C_PuiBsTemporaryDataId,
                                                                                     uint64_t> & orc_MapTypeAndIndexToId,
                                                 const C_PuiBsElements & orc_AllElements) const
{
   std::vector<QGraphicsItem *> c_RelatedItems = this->m_GetSceneItems();
   const C_GiBiArrow * pc_LineArrow;
   const C_GiBiBoundary * pc_Boundary;
   const C_GiBiImageGroup * pc_Image;
   const C_GiBiTextElement * pc_TextElement;
   C_PuiBsDataElement * pc_Data;
   C_GiUnique * pc_Unique;

   for (std::vector<QGraphicsItem *>::iterator c_ItRelatedItem = c_RelatedItems.begin();
        c_ItRelatedItem != c_RelatedItems.end(); ++c_ItRelatedItem)
   {
      pc_Unique = dynamic_cast<C_GiUnique *>(*c_ItRelatedItem);
      if (pc_Unique != NULL)
      {
         const uint64_t u64_CurUniqueId = pc_Unique->GetId();
         pc_Data = dynamic_cast<C_PuiBsDataElement *>(*c_ItRelatedItem);
         if (pc_Data != NULL)
         {
            const int32_t s32_Index = pc_Data->GetIndex();
            if (s32_Index >= 0)
            {
               const uint32_t u32_Index = static_cast<uint32_t>(s32_Index);
               //Save to scene data
               pc_Data->UpdateData();

               //Backup scene data internally
               //Line arrow
               pc_LineArrow = dynamic_cast<C_GiBiArrow *>(*c_ItRelatedItem);
               if (pc_LineArrow != NULL)
               {
                  if (u32_Index < orc_AllElements.c_LineArrows.size())
                  {
                     orc_Storage.c_LineArrows.push_back(orc_AllElements.c_LineArrows[u32_Index]);
                     orc_MapTypeAndIndexToId.insert(C_PuiBsTemporaryDataId(this->m_GetLineArrowType(),
                                                                           orc_Storage.c_LineArrows.size()
                                                                           -
                                                                           1UL),
                                                    u64_CurUniqueId);
                  }
               }
               //Boundary
               pc_Boundary = dynamic_cast<C_GiBiBoundary *>(*c_ItRelatedItem);
               if (pc_Boundary != NULL)
               {
                  if (u32_Index < orc_AllElements.c_Boundaries.size())
                  {
                     orc_Storage.c_Boundaries.push_back(orc_AllElements.c_Boundaries[u32_Index]);
                     orc_MapTypeAndIndexToId.insert(C_PuiBsTemporaryDataId(this->m_GetBoundaryType(),
                                                                           orc_Storage.c_Boundaries.size()
                                                                           -
                                                                           1UL),
                                                    u64_CurUniqueId);
                  }
               }
               //Image
               pc_Image = dynamic_cast<C_GiBiImageGroup *>(*c_ItRelatedItem);
               if (pc_Image != NULL)
               {
                  if (u32_Index < orc_AllElements.c_Images.size())
                  {
                     orc_Storage.c_Images.push_back(orc_AllElements.c_Images[u32_Index]);
                     orc_MapTypeAndIndexToId.insert(C_PuiBsTemporaryDataId(this->m_GetImageType(),
                                                                           orc_Storage.c_Images.size() -
                                                                           1UL),
                                                    u64_CurUniqueId);
                  }
               }
               //Text element
               pc_TextElement = dynamic_cast<C_GiBiTextElement *>(*c_ItRelatedItem);
               if (pc_TextElement != NULL)
               {
                  if (u32_Index < orc_AllElements.c_TextElements.size())
                  {
                     orc_Storage.c_TextElements.push_back(orc_AllElements.c_TextElements[u32_Index]);
                     orc_MapTypeAndIndexToId.insert(C_PuiBsTemporaryDataId(this->m_GetTextElementType(),
                                                                           orc_Storage.c_TextElements.
                                                                           size() - 1UL),
                                                    u64_CurUniqueId);
                  }
               }
            }
         }
      }
   }
}
