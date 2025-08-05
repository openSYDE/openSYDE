//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to handle imports of KEFEX RAMView .def style projects

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_OscImportRamView.hpp"
#include "C_SdNdeDpImportRamView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load RAMView .def project and import lists to openSYDE Data Pool

   See C_OscImportRamView::h_ImportDataPoolFromRamViewDefProject for basic documentation.
   This function adds pre-setting a C_PuiSdNodeDataPool based on the contents imported by the core class.

   \param[in]     orc_ProjectPath           Full path to .def file
   \param[in,out] orc_DataPool              Loaded Data Pool definition
   \param[out]    orc_GuiDataPool           GUI Data Pool definition matching orc_DataPool
   \param[out]    orc_ImportInformation     Textual information about issues when importing

   \return
   C_NO_ERR     information read
   C_RANGE      orc_ProjectPath::e_Type is not eNMV or eDIAG
   C_RD_WR      critical problem loading project information
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpImportRamView::h_ImportDataPoolFromRamViewDefProject(const C_SclString & orc_ProjectPath,
                                                                      C_OscNodeDataPool & orc_DataPool,
                                                                      C_PuiSdNodeDataPool & orc_GuiDataPool,
                                                                      stw::scl::C_SclStringList & orc_ImportInformation)
{
   const int32_t s32_Result = C_OscImportRamView::h_ImportDataPoolFromRamViewDefProject(orc_ProjectPath, orc_DataPool,
                                                                                        orc_ImportInformation);

   if (s32_Result == C_NO_ERR)
   {
      //set up GUI DP structure corresponding to core structure
      orc_GuiDataPool.c_DataPoolLists.resize(orc_DataPool.c_Lists.size());
      for (uint32_t u32_List = 0U; u32_List < orc_DataPool.c_Lists.size(); u32_List++)
      {
         const uint32_t u32_NumElements = orc_DataPool.c_Lists[u32_List].c_Elements.size();

         orc_GuiDataPool.c_DataPoolLists[u32_List].c_DataPoolListElements.resize(u32_NumElements);

         for (uint32_t u32_Element = 0U; u32_Element < u32_NumElements; u32_Element++)
         {
            C_OscNodeDataPoolListElement & rc_Element = orc_DataPool.c_Lists[u32_List].c_Elements[u32_Element];
            C_SclString c_Comment;

            //Set GUI-only properties:
            //RAMView projects have a clear logic: arrays of sint8 are generally interpreted as strings
            orc_GuiDataPool.c_DataPoolLists[u32_List].c_DataPoolListElements[u32_Element].q_AutoMinMaxActive = false;

            //Try to cope with non-ASCII characters in variable comments as good as possible
            //RAMView projects are basically stored in single-byte ASCII format.
            //So if a character > 127 has somehow made it's way there we cannot be sure which encoding was used.
            //Knowing that RAMView did not allow to save Unicode strings a good guess could be that it's a latin
            // extended ASCII encoding. So for many cases (e.g. most common European special characters)
            // storing the byte value as UTF8 will have the expected results in openSYDE.
            //Approach taken does not have high performance. But this is not considered an issue as importing typically
            // is a single shot procedure.
            for (uint32_t u32_Character = 0; u32_Character < rc_Element.c_Comment.Length(); u32_Character++)
            {
               const uint8_t u8_CharacterValue = static_cast<uint8_t>(rc_Element.c_Comment.c_str()[u32_Character]);
               if (u8_CharacterValue > 127U)
               {
                  //Convert to UTF8 encoding:
                  //We could have values from 0x80 to 0xFF.
                  //This will result in two bytes:
                  //[0] = 0b11000000 + ((value & 0xC0) >> 6)
                  //[1] = 0x10000000 + (value & 0x3F)
                  uint8_t au8_Utf8Value[2];
                  au8_Utf8Value[0] = (0xC0U + ((u8_CharacterValue & 0xC0U) >> 6U));
                  au8_Utf8Value[1] = (0x80U + (u8_CharacterValue & 0x3FU));
                  //hack unsigned values into C_SCLString:
                  c_Comment += static_cast<char_t>(au8_Utf8Value[0]);
                  c_Comment += static_cast<char_t>(au8_Utf8Value[1]);
               }
               else
               {
                  //plain old character ...
                  c_Comment += static_cast<char_t>(u8_CharacterValue);
               }
            }
            rc_Element.c_Comment = c_Comment;
         }
      }
   }

   return s32_Result;
}
