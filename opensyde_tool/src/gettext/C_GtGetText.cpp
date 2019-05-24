//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core gettext utilities

   Wrapper for the gettext library.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "C_GtGetText.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "libintl.h" //gettext header

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
bool C_GtGetText::mhq_Initialized = false;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Initialize wrapper

   - define file name for .mo files
   - define base path to place .mo files
   - set encoding of .mo files to UTF-8

   The function will check for an existing base path.
   It will not check for existing .mo file(s).

   As a result the .mo files need to be placed as follows:
   <orc_BasePath>/<language>/LC_MESSAGES/<orc_MoFileName>.mo

   Example:
   After calling ::h_Initialize("./translations", "application")
   The german language file will be searched at:
   ./translations/de/LC_MESSAGES/application.mo

   \param[in]  orc_BasePath    base path for translation files
   \param[in]  orc_MoFileName  file name of .mo files

   \return
   C_NO_ERR    all set up
   C_RANGE     orc_BasePath or orc_MoFileName are empty strings
   C_NOACT     internal error in gettext library
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GtGetText::h_Initialize(const C_SCLString & orc_BasePath, const C_SCLString & orc_MoFileName)
{
   sint32 s32_Return;

   s32_Return = stw_errors::C_NO_ERR;

   if ((orc_BasePath == "") || (orc_MoFileName == ""))
   {
      s32_Return = stw_errors::C_RANGE;
   }
   else
   {
      charn * pcn_Return;
      pcn_Return = bindtextdomain(orc_MoFileName.c_str(), orc_BasePath.c_str());
      if (pcn_Return != NULL)
      {
         //set UTF-8 encoding
         pcn_Return = bind_textdomain_codeset(orc_MoFileName.c_str(), "UTF-8");
      }

      if (pcn_Return != NULL)
      {
         //set desired .mo file as active:
         pcn_Return = textdomain(orc_MoFileName.c_str());
      }
      if (pcn_Return == NULL)
      {
         s32_Return = stw_errors::C_NOACT;
      }
   }
   if (s32_Return == stw_errors::C_NO_ERR)
   {
      mhq_Initialized = true;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Set active language

   Allow application to override active language.
   By default the system environment variable "LANG" determines the used language.

   As parameters use the language code as defined in ISO 639-1 (e.g. "de", "fr").
   Make sure to place the .mo file in the corresponding folder (as configured with ::h_Initialize).

   Caution:
   This function modifies the environment variable "LANG" for the running process.

   The function does not check whether
   - orc_Language is a valid code
   - the resulting .mo file exists

   \param[in]  orc_Language    ISO 639-1 language code

   \return
   C_NO_ERR    language set
   C_NOACT     could not set language
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GtGetText::h_SetLanguage(const C_SCLString & orc_Language)
{
   sint32 s32_Return;

   s32_Return = stw_tgl::TGL_SetEnvironmentVariable("LANG", orc_Language);
   return (s32_Return == -1) ? stw_errors::C_NOACT : stw_errors::C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Read string from .mo file

   Try to read string for configured language from .mo file.
   If the .mo file is present and contains a translation for the requested string a pointer to this translation is
    returned.
   If the .mo file is not present a pointer to the requested message ID is returned (-> serves as default text).
   If this class has not been initialized "C_GtGetText not initialized" will be returned.

   Note: when parsing the texts in the application using the "xgettext" utility pass "--keyword=h_GetText" so this
    function will be properly identified.

   \param[in]  opcn_MessageID    key

   \return
   Pointer to text (see description)
*/
//----------------------------------------------------------------------------------------------------------------------
const charn * C_GtGetText::h_GetText(const charn * const opcn_MessageID)
{
   static const charn hacn_Error[] = "C_GtGetText not initialized";
   const charn * pcn_String;

   if (mhq_Initialized == false)
   {
      pcn_String = hacn_Error;
   }
   else
   {
      pcn_String = gettext(opcn_MessageID);
   }
   return pcn_String;
}
