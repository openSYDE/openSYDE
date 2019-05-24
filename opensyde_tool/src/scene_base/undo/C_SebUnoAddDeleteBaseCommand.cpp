//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class (implementation)

   Add and delete base class

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SebUnoAddDeleteBaseCommand.h"
#include "C_GiBiArrow.h"
#include "C_GiBiBoundary.h"
#include "C_GiBiTextElement.h"
#include "C_GiBiImageGroup.h"

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
/*! \brief   Default constructor

   \param[in,out] opc_Scene  Pointer to currently active scene
   \param[in]     orc_IDs    Affected unique IDs
   \param[in]     orc_Text   Command description
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoAddDeleteBaseCommand::C_SebUnoAddDeleteBaseCommand(QGraphicsScene * const opc_Scene,
                                                           const std::vector<uint64> & orc_IDs,
                                                           const QString & orc_Text, QUndoCommand * const opc_Parent) :
   C_SebUnoBaseCommand(opc_Scene, orc_IDs, orc_Text, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store items in new structure and save indices in map

   \param[in,out] orc_Storage             Data structure to store data in
   \param[in,out] orc_MapTypeAndIndexToID Map to locate new items
   \param[in]     orc_AllElements         Items to store
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoAddDeleteBaseCommand::m_StoreCommon(C_PuiBsElements & orc_Storage, QMap<C_PuiBsTemporaryDataID,
                                                                                     uint64> & orc_MapTypeAndIndexToID,
                                                 const C_PuiBsElements & orc_AllElements)
{
   std::vector<QGraphicsItem *> c_RelatedItems = this->m_GetSceneItems();
   C_GiBiArrow * pc_LineArrow;
   C_GiBiBoundary * pc_Boundary;
   C_GiBiImageGroup * pc_Image;
   C_GiBiTextElement * pc_TextElement;
   C_PuiBsDataElement * pc_Data;
   C_GiUnique * pc_Unique;

   for (std::vector<QGraphicsItem *>::iterator c_ItRelatedItem = c_RelatedItems.begin();
        c_ItRelatedItem != c_RelatedItems.end(); ++c_ItRelatedItem)
   {
      //lint -e{740}  no problem because of common base class
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      pc_Unique = dynamic_cast<C_GiUnique *>(*c_ItRelatedItem);
      if (pc_Unique != NULL)
      {
         const uint64 u64_CurUniqueID = pc_Unique->GetID();
         //lint -e{740}  no problem because of common base class
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_Data = dynamic_cast<C_PuiBsDataElement *>(*c_ItRelatedItem);
         if (pc_Data != NULL)
         {
            const sint32 s32_Index = pc_Data->GetIndex();
            if (s32_Index >= 0)
            {
               const uint32 u32_Index = static_cast<uint32>(s32_Index);
               //Save to scene data
               pc_Data->UpdateData();

               //Backup scene data internally
               //Line arrow
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               pc_LineArrow = dynamic_cast<C_GiBiArrow *>(*c_ItRelatedItem);
               if (pc_LineArrow != NULL)
               {
                  if (u32_Index < orc_AllElements.c_LineArrows.size())
                  {
                     orc_Storage.c_LineArrows.push_back(orc_AllElements.c_LineArrows[u32_Index]);
                     orc_MapTypeAndIndexToID.insert(C_PuiBsTemporaryDataID(this->GetLineArrowType(),
                                                                           orc_Storage.c_LineArrows.size()
                                                                           -
                                                                           1UL),
                                                    u64_CurUniqueID);
                  }
               }
               //Boundary
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               pc_Boundary = dynamic_cast<C_GiBiBoundary *>(*c_ItRelatedItem);
               if (pc_Boundary != NULL)
               {
                  if (u32_Index < orc_AllElements.c_Boundaries.size())
                  {
                     orc_Storage.c_Boundaries.push_back(orc_AllElements.c_Boundaries[u32_Index]);
                     orc_MapTypeAndIndexToID.insert(C_PuiBsTemporaryDataID(this->GetBoundaryType(),
                                                                           orc_Storage.c_Boundaries.size()
                                                                           -
                                                                           1UL),
                                                    u64_CurUniqueID);
                  }
               }
               //Image
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               pc_Image = dynamic_cast<C_GiBiImageGroup *>(*c_ItRelatedItem);
               if (pc_Image != NULL)
               {
                  if (u32_Index < orc_AllElements.c_Images.size())
                  {
                     orc_Storage.c_Images.push_back(orc_AllElements.c_Images[u32_Index]);
                     orc_MapTypeAndIndexToID.insert(C_PuiBsTemporaryDataID(this->GetImageType(),
                                                                           orc_Storage.c_Images.size() -
                                                                           1UL),
                                                    u64_CurUniqueID);
                  }
               }
               //Text element
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               pc_TextElement = dynamic_cast<C_GiBiTextElement *>(*c_ItRelatedItem);
               if (pc_TextElement != NULL)
               {
                  if (u32_Index < orc_AllElements.c_TextElements.size())
                  {
                     orc_Storage.c_TextElements.push_back(orc_AllElements.c_TextElements[u32_Index]);
                     orc_MapTypeAndIndexToID.insert(C_PuiBsTemporaryDataID(this->GetTextElementType(),
                                                                           orc_Storage.c_TextElements.
                                                                           size() - 1UL),
                                                    u64_CurUniqueID);
                  }
               }
            }
         }
      }
   }
}
